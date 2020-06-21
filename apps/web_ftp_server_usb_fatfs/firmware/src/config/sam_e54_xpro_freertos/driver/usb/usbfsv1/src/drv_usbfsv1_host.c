/*******************************************************************************
  USB Host Controller Driver

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usbfsv1_host.c

  Summary:
    USB Host Driver Implementation.

  Description:
    This file implements the Host mode operation of the Full Speed USB Driver.
    This file should be included in the application if USB Host mode operation
    is desired.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
//DOM-IGNORE-END

#include "driver/usb/usbfsv1/src/drv_usbfsv1_local.h"
#include "driver/usb/usbfsv1/drv_usbfsv1.h"
/**
 * \name USB SRAM data containing pipe descriptor table
 * The content of the USB SRAM can be :
 * - modified by USB hardware interface to update pipe status.
 *   Thereby, it is read by software.
 * - modified by USB software to control pipe.
 *   Thereby, it is read by hardware.
 * This data section is volatile.
 */

volatile COMPILER_WORD_ALIGNED usb_descriptor_host_registers_t usb_pipe_table[USB_HOST_PIPES_NUMBER];
volatile COMPILER_WORD_ALIGNED uint8_t usb_ctrl_buffer[1024];

/**
 * \brief Host pipe token types.
 */
enum usb_host_pipe_token 
{
	USB_HOST_PIPE_TOKEN_SETUP = 0x00,
	USB_HOST_PIPE_TOKEN_IN    = 0x01,
	USB_HOST_PIPE_TOKEN_OUT   = 0x02
};

enum usb_host_pipe_type 
{
	USB_HOST_PIPE_TYPE_DISABLED  = 0x00,
	USB_HOST_PIPE_TYPE_CONTROL   = 0x01,
	USB_HOST_PIPE_TYPE_ISO       = 0x02,
	USB_HOST_PIPE_TYPE_BULK      = 0x03,
	USB_HOST_PIPE_TYPE_INTERRUPT = 0x04,
	USB_HOST_PIPE_TYPE_EXTENDED  = 0x05
};

/**********************************************************
 * This structure is a set of pointer to the USBFSV1 driver
 * functions. It is provided to the host and device layer
 * as the interface to the driver.
 * *******************************************************/
DRV_USB_HOST_INTERFACE gDrvUSBFSV1HostInterface =
{
    .open = DRV_USBFSV1_Open,
    .close = DRV_USBFSV1_Close,
    .eventHandlerSet = DRV_USBFSV1_ClientEventCallBackSet,
    .hostIRPSubmit = DRV_USBFSV1_HOST_IRPSubmit,
    .hostIRPCancel = DRV_USBFSV1_HOST_IRPCancel,
    .hostPipeSetup = DRV_USBFSV1_HOST_PipeSetup,
    .hostPipeClose = DRV_USBFSV1_HOST_PipeClose,
    .hostEventsDisable = DRV_USBFSV1_HOST_EventsDisable,
	.endpointToggleClear = DRV_USBFSV1_HOST_EndpointToggleClear,
    .hostEventsEnable = DRV_USBFSV1_HOST_EventsEnable,
    .rootHubInterface.rootHubPortInterface.hubPortReset = DRV_USBFSV1_HOST_ROOT_HUB_PortReset,
    .rootHubInterface.rootHubPortInterface.hubPortSpeedGet = DRV_USBFSV1_HOST_ROOT_HUB_PortSpeedGet,
    .rootHubInterface.rootHubPortInterface.hubPortResetIsComplete = DRV_USBFSV1_HOST_ROOT_HUB_PortResetIsComplete,
    .rootHubInterface.rootHubPortInterface.hubPortSuspend = DRV_USBFSV1_HOST_ROOT_HUB_PortSuspend,
    .rootHubInterface.rootHubPortInterface.hubPortResume = DRV_USBFSV1_HOST_ROOT_HUB_PortResume,
    .rootHubInterface.rootHubMaxCurrentGet = DRV_USBFSV1_HOST_ROOT_HUB_MaximumCurrentGet,
    .rootHubInterface.rootHubPortNumbersGet = DRV_USBFSV1_HOST_ROOT_HUB_PortNumbersGet,
    .rootHubInterface.rootHubSpeedGet = DRV_USBFSV1_HOST_ROOT_HUB_BusSpeedGet,
    .rootHubInterface.rootHubInitialize = DRV_USBFSV1_HOST_ROOT_HUB_Initialize,
    .rootHubInterface.rootHubOperationEnable = DRV_USBFSV1_HOST_ROOT_HUB_OperationEnable,
    .rootHubInterface.rootHubOperationIsEnabled = DRV_USBFSV1_HOST_ROOT_HUB_OperationIsEnabled,
};

/*****************************************************
 * Global Variable used as Pool of pipe objects 
 * that is used by all driver instances.
 *****************************************************/
DRV_USBFSV1_HOST_PIPE_OBJ gDrvUSBHostPipeObj[DRV_USBFSV1_HOST_PIPES_NUMBER];

/****************************************
 * The driver object
 ****************************************/
extern DRV_USBFSV1_OBJ gDrvUSBObj[];

// ****************************************************************************
// ****************************************************************************
// Local Functions
// ****************************************************************************
// ****************************************************************************

void _DRV_USBFSV1_HOST_AttachDetachStateMachine (DRV_USBFSV1_OBJ * hDriver)
{
    /* In the host mode, we perform attach de-bouncing */
   bool interruptWasEnabled;

    switch(hDriver->attachState)
    {
        case DRV_USBFSV1_HOST_ATTACH_STATE_CHECK_FOR_DEVICE_ATTACH:
            /* If no device is attached, then there is nothing to do
             * If device is attached, then move to next state */
            if(hDriver->deviceAttached)
            {
                hDriver->attachState = DRV_USBFSV1_HOST_ATTACH_STATE_DETECTED;
            }

            break;
            
        case DRV_USBFSV1_HOST_ATTACH_STATE_DETECTED:
            /* Disable the driver interrupt as
             * we do not want this section to be interrupted. */
            interruptWasEnabled = SYS_INT_SourceDisable((INT_SOURCE)hDriver->interruptSource);

            if(hDriver->deviceAttached)
            {
                /* Yes the device is still attached. Enumerate
                 * this device. usbHostDeviceInfo is the ID of
                 * this root hub. */
                hDriver->attachedDeviceObjHandle = USB_HOST_DeviceEnumerate(hDriver->usbHostDeviceInfo, 0);
                hDriver->attachState = DRV_USBFSV1_HOST_ATTACH_STATE_READY;
            }
            else
            {
                /* The device is not attached any more. This was a false attach 
                 */
                hDriver->attachState = DRV_USBFSV1_HOST_ATTACH_STATE_CHECK_FOR_DEVICE_ATTACH;
            }

            if(interruptWasEnabled)
            {
                /* Re-enable the interrupt if it was originally
                 * enabled. */
                SYS_INT_SourceEnable((INT_SOURCE)hDriver->interruptSource);
            }
            break;

        case DRV_USBFSV1_HOST_ATTACH_STATE_READY:

            /* De-bouncing is done and device ready. We can check
             * here if the device is detached */
            if(!hDriver->deviceAttached)
            {
                /* Device is not attached */
                hDriver->attachState = DRV_USBFSV1_HOST_ATTACH_STATE_CHECK_FOR_DEVICE_ATTACH;
            }
            break;

        default:
            break;
    }
}

void _DRV_USBFSV1_HOST_ResetStateMachine(DRV_USBFSV1_OBJ * hDriver)
{
    /* Check if reset is needed */
    switch(hDriver->resetState)
    {
        case DRV_USBFSV1_HOST_RESET_STATE_NO_RESET:

            /* No reset signaling is request */
            break;

        case DRV_USBFSV1_HOST_RESET_STATE_START:
            /* Trigger USB Reset */
            hDriver->usbID->HOST.USB_CTRLB |= USB_HOST_CTRLB_BUSRESET_Msk;
            hDriver->resetState = DRV_USBFSV1_HOST_RESET_STATE_WAIT_FOR_COMPLETE;
            break;

        case DRV_USBFSV1_HOST_RESET_STATE_WAIT_FOR_COMPLETE:

            /* Check if the reset has completed */
            if(USB_HOST_CTRLB_BUSRESET_Msk != (hDriver->usbID->HOST.USB_CTRLB & USB_HOST_CTRLB_BUSRESET_Msk))
            {
                /* Reset has completed */
                hDriver->resetState = DRV_USBFSV1_HOST_RESET_STATE_NO_RESET;

                /* Clear the flag */
                hDriver->isResetting = false;

                /* Now that reset is complete, we can find out the
                 * speed of the attached device. */
                switch( (hDriver->usbID->HOST.USB_STATUS & USB_HOST_STATUS_SPEED_Msk)>>USB_HOST_STATUS_SPEED_Pos)
                {
                    case 0x0:
                        hDriver->deviceSpeed = USB_SPEED_FULL;
                        break;
                    case 0x1:
                        hDriver->deviceSpeed = USB_SPEED_LOW;
                        break;    
                    case 0x2:
                        break;    
                    default:
                        break;
    	        }
            }
            break;

        default:
            break;
    }
}


static void _DRV_USBFSV1_HOST_IRPTransmitFIFOLoad
(
    volatile usb_registers_t * usbID, 
    USB_HOST_IRP_LOCAL * irp,
    uint8_t hPipe
)
{
    /* This function will copy data from the irp to the fifo
     * based on the number of bytes that were completed and
     * then trigger the transmit */

    uint32_t count;
    uint32_t pendingBytes;
    uint8_t *data;
    uint8_t *ptrEPData;
    
    /* Load the FIFO */
    pendingBytes = irp->size - irp->completedBytes;
    if(pendingBytes > sizeof(usb_ctrl_buffer))
    {
        count = sizeof(usb_ctrl_buffer);
    }
    else
    {
        count = pendingBytes;
    }
    data = (uint8_t *)((uint8_t *)irp->data + irp->completedBytes);
    ptrEPData = (uint8_t *) &usb_ctrl_buffer[0];

    /* Configure OUT Token */
    /* Be careful with this buffer, the PTYPE must not be set to zero (pipe disable). */
    usbID->HOST.HOST_PIPE[hPipe].USB_PCFG = (usbID->HOST.HOST_PIPE[hPipe].USB_PCFG
                                                  & ~(USB_HOST_PCFG_PTOKEN_Msk))
                                            //    | USB_HOST_PCFG_BK      // 0x1 Dual-bank endpoint
                                                | USB_HOST_PCFG_PTOKEN(USB_HOST_PIPE_TOKEN_OUT);
    /* Copy data to be sent in case the data is in stack */
    for(uint16_t i = 0; i < count; i ++)
    {
	    *ptrEPData++ = *data++;
    }
    usb_pipe_table[hPipe].HOST_DESC_BANK[0].USB_ADDR = (uint32_t) &usb_ctrl_buffer[0];
    
    usb_pipe_table[hPipe].HOST_DESC_BANK[0].USB_PCKSIZE = ((usb_pipe_table[hPipe].HOST_DESC_BANK[0].USB_PCKSIZE) & USB_HOST_PCKSIZE_SIZE_Msk)
                                                | USB_HOST_PCKSIZE_BYTE_COUNT(count)  
                                                | USB_HOST_PCKSIZE_MULTI_PACKET_SIZE(0);

    /* Update the irp with the byte count loaded */
    irp->completedBytes += count;

    /* Enable Pipe out ready interrupt */
    /* The user writes the OUT data to the data buffer pointer by ADDR in the pipe descriptor and allows the */
    /* USB to send the data by writing a one to the PSTATUS.BK0/1RDY. */
    usbID->HOST.HOST_PIPE[hPipe].USB_PSTATUSSET = USB_HOST_PSTATUSSET_BK0RDY_Msk;

    /* Enable transfer complete interrupt */
    usbID->HOST.HOST_PIPE[hPipe].USB_PINTENSET = USB_HOST_PINTENSET_TRCPT0_Msk;  
    /* Unfreeze pipe */
    usbID->HOST.HOST_PIPE[hPipe].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_PFREEZE_Msk;

}/* end of _DRV_USBFSV1_HOST_IRPTransmitFIFOLoad() */

static void _DRV_USBFSV1_HOST_IRPTransmitSetupPacket
(
    volatile usb_registers_t * usbID,
    USB_HOST_IRP_LOCAL * irp
)
{
    uint8_t *ptrEPData;
    uint8_t *data;
    
    /* This function will load the irp setup packet into 
     * pipe0 FIFO and then transmit the setup packet. */

    data = (uint8_t *) irp->setup;
    ptrEPData = (uint8_t *) &usb_ctrl_buffer[0];

    /* Configure Setup Token for Pipe 0 */
    /* Be careful with this buffer, the PTYPE must not be set to zero (pipe disable). */
    usbID->HOST.HOST_PIPE[0].USB_PCFG = (usbID->HOST.HOST_PIPE[0].USB_PCFG 
                            & ~(USB_HOST_PCFG_PTYPE_Msk|USB_HOST_PCFG_PTOKEN_Msk))
                        | USB_HOST_PCFG_PTYPE(USB_HOST_PIPE_TYPE_CONTROL) 
                        | USB_HOST_PCFG_PTOKEN(USB_HOST_PIPE_TOKEN_SETUP);

    /* Copy data to be sent in case the data is in stack */
    for(uint16_t i = 0; i < 8; i ++)
    {
	    *ptrEPData++ = *data++;
    }
    usb_pipe_table[0].HOST_DESC_BANK[0].USB_ADDR = (uint32_t)&usb_ctrl_buffer[0];

    /* SETUP packet size is always 8 */
    usb_pipe_table[0].HOST_DESC_BANK[0].USB_PCKSIZE = ((usb_pipe_table[0].HOST_DESC_BANK[0].USB_PCKSIZE) & USB_HOST_PCKSIZE_SIZE_Msk)
                                                    | USB_HOST_PCKSIZE_BYTE_COUNT(8) 
                                                    | USB_HOST_PCKSIZE_MULTI_PACKET_SIZE(0);

    /* Transmit Setup Interrupt Enable */
    usbID->HOST.HOST_PIPE[0].USB_PINTENSET = USB_HOST_PINTENSET_TXSTP_Msk;
    /* The user writes the OUT data to the data buffer pointer by ADDR in the pipe descriptor and allows the */
    /* USB to send the data by writing a one to the PSTATUS.BK0/1RDY. */
    usbID->HOST.HOST_PIPE[0].USB_PSTATUSSET = USB_HOST_PSTATUSSET_BK0RDY_Msk;
    /* Unfreeze pipe */
    usbID->HOST.HOST_PIPE[0].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_PFREEZE_Msk;

}/* end of _DRV_USBFSV1_HOST_IRPTransmitSetupPacket() */

static uint32_t _DRV_USBFSV1_HOST_IRPReceiveFIFOUnload 
(
    volatile usb_registers_t * usbID,
    USB_HOST_IRP_LOCAL * irp,
    uint8_t hPipe,
    bool * pisDMAUsed
)
{
    /* This function will recover the count of the received data/
     * and then unload the pipe FIFO. */

    uint32_t  count;
    uint8_t * data;
    uint8_t * ptrEPData;

    /* Copy the data from the FIFO0 to the application
     * buffer and then update the complete byte count
     * and clear the RX packet ready bit */
    ptrEPData = (uint8_t *) &usb_ctrl_buffer[0];
    
    data = (uint8_t *)((uint8_t *)irp->data + irp->completedBytes);

    /* Get byte count to read data */
    count = usb_pipe_table[hPipe].HOST_DESC_BANK[0].USB_PCKSIZE & USB_HOST_PCKSIZE_BYTE_COUNT_Msk;
    
    if( sizeof(usb_ctrl_buffer) < count )
    {
        count = sizeof(usb_ctrl_buffer);
    }

    for(uint16_t i = 0; i < count; i ++)
    {
	    *data++ = *ptrEPData++;
    }
    irp->completedBytes += count;
    *pisDMAUsed = false;
    
    return (count);
}/* end of _DRV_USBFSV1_HOST_IRPReceiveFIFOUnload() */

// *****************************************************************************
/* Function:
    void _DRV_USBFSV1_HOST_Initialize
    (
        DRV_USBFSV1_OBJ * const pUSBDrvObj,
        const SYS_MODULE_INDEX index
    )

  Summary:
    This function initializes the driver for host mode operation.

  Description:
    Function performs the following tasks:
      - Informs the USB module with SOF threshold in bit times
      - Enables VBUS power and initializes the module in HOST mode
      - Resets the BDT table data structure with init value
      - Configures EP0 register for the specific USB module
      - Enables the USB attach interrupt

  Remarks:
    This is a local function and should not be called directly by the
    application.
*/
void _DRV_USBFSV1_HOST_Initialize
(
    DRV_USBFSV1_OBJ * drvObj, 
    SYS_MODULE_INDEX index
)
{
//   volatile usb_registers_t * usbID = drvObj->usbID;
   
    /* No device attached */
    drvObj->deviceAttached = false;
    /* Initialize the device handle */
    drvObj->attachedDeviceObjHandle = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;
    /* Initialize the host specific members in the driver object */
    drvObj->isResetting     = false;
    drvObj->usbHostDeviceInfo = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;
    drvObj->operationEnabled = false;
    drvObj->hostEndpointTablePtr = (usb_descriptor_host_registers_t *)&usb_pipe_table[0];
    drvObj->usbID->HOST.USB_DESCADD = (uint32_t)drvObj->hostEndpointTablePtr;
    
}/* end of _DRV_USBFSV1_HOST_Initialize() */

// *****************************************************************************
/* Function:
    USB_ERROR DRV_USBFSV1_HOST_IRPSubmit
    (
        DRV_USBFSV1_HOST_PIPE_HANDLE  hPipe,
        USB_HOST_IRP * pinputIRP
    )

  Summary:
    Submits an IRP on a pipe.
    
  Description:
    This function submits an IRP on the specified pipe. The IRP will be added to
    the queue and will be processed in turn. The data will be transferred on the
    bus based on the USB bus scheduling rules. When the IRP has been processed,
    the callback function specified in the IRP will be called. The IRP status
    will be updated to reflect the completion status of the IRP. 

  Remarks:
    See drv_usbfs.h for usage information.
*/
USB_ERROR DRV_USBFSV1_HOST_IRPSubmit
(
    DRV_USBFSV1_HOST_PIPE_HANDLE  hPipe, 
    USB_HOST_IRP * inputIRP
)
{
    USB_HOST_IRP_LOCAL * irpIterator = NULL;
    DRV_USBFSV1_HOST_TRANSFER_GROUP * controlTransferGroup = NULL;
    bool interruptWasEnabled = false;
    uint32_t direction = 0;
    uint8_t hostPipe = 0;

    USB_HOST_IRP_LOCAL * irp = (USB_HOST_IRP_LOCAL *)inputIRP;
    DRV_USBFSV1_HOST_PIPE_OBJ * pipe = (DRV_USBFSV1_HOST_PIPE_OBJ *)(hPipe);
    DRV_USBFSV1_OBJ * hDriver = NULL;
    volatile usb_registers_t * usbID;
    USB_ERROR returnValue = USB_ERROR_PARAMETER_INVALID;

    if((pipe == NULL) || (hPipe == (DRV_USBFSV1_HOST_PIPE_HANDLE_INVALID)))
    {
        /* This means an invalid pipe was specified.  Return with an error */
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Pipe handle is not valid in DRV_USBFSV1_HOST_IRPSubmit()");
    }
    else
    {
        hDriver = (DRV_USBFSV1_OBJ *)(pipe->hClient);
        usbID = hDriver->usbID;
        controlTransferGroup = &hDriver->controlTransferGroup;

        /* Assign owner pipe */
        irp->pipe = hPipe;
        irp->status = USB_HOST_IRP_STATUS_PENDING;
        irp->tempState = DRV_USBFSV1_HOST_IRP_STATE_PROCESSING;
        hostPipe = pipe->hostPipeN;
        direction = (pipe->endpointAndDirection & 0x80) >> 7;

        /* We need to disable interrupts was the queue state does not change
         * asynchronously */

        if(!hDriver->isInInterruptContext)
        {
            if(OSAL_MUTEX_Lock(&(hDriver->mutexID), OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
            {
                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Mutex lock failed");
                returnValue = USB_ERROR_OSAL_FUNCTION;
            }
            else
            {
                interruptWasEnabled = SYS_INT_SourceDisable((INT_SOURCE)hDriver->interruptSource);
            }
        }

        if(USB_ERROR_OSAL_FUNCTION != returnValue)
        {
            /* This needs to be done for all irp irrespective of type or if
             * there IRP is immediately processed */

            irp->next = NULL;
            irp->completedBytes = 0;
            irp->status = USB_HOST_IRP_STATUS_PENDING;

            if(pipe->irpQueueHead == NULL)
            {
                /* This means that there are no IRPs on this pipe. We can add
                 * this IRP directly */

                irp->previous = NULL;
                pipe->irpQueueHead = irp;

                if(pipe->pipeType == USB_TRANSFER_TYPE_CONTROL)
                {
                    /* Set the initial stage of the IRP */
                    irp->tempState = DRV_USBFSV1_HOST_IRP_STATE_SETUP_STAGE;

                    /* We need to update the flags parameter of the IRP to
                     * indicate the direction of the control transfer. */

                    if(*((uint8_t*)(irp->setup)) & 0x80)
                    {
                        /* This means the data stage moves from device to host.
                         * Set bit 15 of the flags parameter */

                        irp->flags |= 0x80;
                    }
                    else
                    {
                        /* This means the data stage moves from host to device.
                         * Clear bit 15 of the flags parameter. */

                        irp->flags &= 0x7F;
                    }

                    /* We need to check if the endpoint 0 is free and if so then
                     * start processing the IRP */

                    if(controlTransferGroup->currentIRP == NULL)
                    {
                        /* This means that no IRPs are being processed
                         * So we should start the IRP processing. Else
                         * the IRP processing will start in interrupt.
                         * We start by copying the setup command */

                        controlTransferGroup->currentIRP = irp;
                        controlTransferGroup->currentPipe = pipe;
                        irp->status = USB_HOST_IRP_STATUS_IN_PROGRESS;

                        /* Send the setup packet to device */
                        _DRV_USBFSV1_HOST_IRPTransmitSetupPacket(usbID, irp);
                    }
                }
                else
                {
                    /* For non control transfers, if this is the first irp in
                     * the queue, then we can start the irp */

                    irp->status = USB_HOST_IRP_STATUS_IN_PROGRESS;

                    if(USB_DATA_DIRECTION_HOST_TO_DEVICE == direction)
                    {
                        /* Data is moving from host to device. We need to copy
                         * data into the FIFO and then and set the TX request
                         * bit. If the IRP size is greater than endpoint size
                         * then we must packetize. */

                        _DRV_USBFSV1_HOST_IRPTransmitFIFOLoad(usbID, irp, hostPipe);
                    }
                    else
                    {
                        /* IN Packets */
                        /* Data is moving from device to host
                         * We need to set the Rx Packet Request bit */
                        /* The USB will perform IN requests as long as the pipe is not frozen by the user. */
                        /* The generation of IN requests starts when the pipe is unfrozen (PSTATUS.PFREEZE is set to zero). */
                        /* Clear Transfer Complete 0 Interrupt Flag */
                        usbID->HOST.HOST_PIPE[hostPipe].USB_PINTFLAG = USB_HOST_PINTFLAG_TRCPT0_Msk;
                        usb_pipe_table[hostPipe].HOST_DESC_BANK[0].USB_ADDR = (uint32_t)&usb_ctrl_buffer[0];
                        /* MULTI_PACKET_SIZE: For IN pipes, MULTI_PACKET_SIZE holds the total number of bytes sent. 
                         * MULTI_PACKET_SIZE should be written to zero when setting up a new transfer. */
                        /* BYTE_COUNT: These bits define the 14-bit value that contains number of bytes sent in 
                         * the last OUT or SETUP transaction for an OUT pipe, or of the number of bytes to be received
                         * in the next IN transaction for an input pipe. */
                        /* Set Multi_Packet_Size to transfer more data than wMaxPacketSize. So multiple IN will be performed if needed. */
                        usb_pipe_table[hostPipe].HOST_DESC_BANK[0].USB_PCKSIZE = ((usb_pipe_table[hostPipe].HOST_DESC_BANK[0].USB_PCKSIZE) & USB_HOST_PCKSIZE_SIZE_Msk)
                                                                        | USB_HOST_PCKSIZE_BYTE_COUNT(0) 
                                                                        | USB_HOST_PCKSIZE_MULTI_PACKET_SIZE(irp->size);

                        /* This means the data stage moves from device to host.
                         * So the host would have to send an IN token.  */
                        /* Configure pipe for IN token */
                        /* Be careful with this buffer, the PTYPE must not be set to zero (pipe disable). */
                        usbID->HOST.HOST_PIPE[hostPipe].USB_PCFG = (usbID->HOST.HOST_PIPE[hostPipe].USB_PCFG 
                                                & ~(USB_HOST_PCFG_PTYPE_Msk|USB_HOST_PCFG_PTOKEN_Msk))
                                            | USB_HOST_PCFG_PTYPE(USB_HOST_PIPE_TYPE_BULK) 
                                 //           | USB_HOST_PCFG_BK      // 0x1 Dual-bank endpoint
                                            | USB_HOST_PCFG_PTOKEN(USB_HOST_PIPE_TOKEN_IN);

                        /* The host indicates it is able to receive data from the device by clearing the Bank 0/1 Ready bit in PSTATUS
                         * which means that the memory for the bank is available for new USB transfer */
                        usbID->HOST.HOST_PIPE[hostPipe].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_BK0RDY_Msk;
                        /* Enable transfer complete interrupt */
                        usbID->HOST.HOST_PIPE[hostPipe].USB_PINTENSET = USB_HOST_PINTENSET_TRCPT0_Msk;
                        /* Unfreeze pipe */
                        usbID->HOST.HOST_PIPE[hostPipe].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_PFREEZE_Msk;
                    }
                }
            }
            else
            {
                /* We need to add the irp to the last irp in the pipe queue
                 * (which is a linked list) */
                irpIterator = pipe->irpQueueHead;

                /* Find the last IRP in the linked list*/
                while(irpIterator->next != 0)
                {
                    irpIterator = irpIterator->next;
                }

                /* Add the item to the last irp in the linked list */
                irpIterator->next = irp;
                irp->previous = irpIterator;
            }

            if(!hDriver->isInInterruptContext)
            {
                if(interruptWasEnabled)
                {
                    SYS_INT_SourceEnable((INT_SOURCE)hDriver->interruptSource);
                }

                if(OSAL_MUTEX_Unlock(&hDriver->mutexID) != OSAL_RESULT_TRUE)
                {
                    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nUSBFS Driver: Mutex unlock failed in DRV_USBFSV1_HOST_IRPSubmit()");
                }
            }

            returnValue = USB_ERROR_NONE;
        }
    }
    
    return (returnValue);
}/* end of DRV_USBFSV1_HOST_IRPSubmit() */

// *****************************************************************************
/* Function:
    void DRV_USBFSV1_HOST_IRPCancel(USB_HOST_IRP * pinputIRP)

  Summary:
    Cancels the specified IRP.
    
  Description:
    This function attempts to cancel the specified IRP. If the IRP is queued and
    its processing has not started, it will be cancelled successfully. If the
    IRP in progress, the ongoing transaction will be allowed to complete. 

  Remarks:
    See drv_usbfs.h for usage information.
*/
void DRV_USBFSV1_HOST_IRPCancel
(
    USB_HOST_IRP * inputIRP
)
{
    /* This function cancels an IRP */

    USB_HOST_IRP_LOCAL * irp = (USB_HOST_IRP_LOCAL *) inputIRP;
    DRV_USBFSV1_OBJ * hDriver;
    DRV_USBFSV1_HOST_PIPE_OBJ * pipe;
    bool interruptWasEnabled = false;

    if(irp->pipe == DRV_USBFSV1_HOST_PIPE_HANDLE_INVALID)
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Invalid pipe");
    }
    else if(irp->status <= USB_HOST_IRP_STATUS_COMPLETED_SHORT)
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: IRP is not pending or in progress");

    }
    else
    {
        pipe = (DRV_USBFSV1_HOST_PIPE_OBJ *)irp->pipe;
        hDriver = (DRV_USBFSV1_OBJ *) pipe->hClient;

        if(!hDriver->isInInterruptContext)
        {
            if(OSAL_MUTEX_Lock(&(hDriver->mutexID), OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
            {
                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Mutex lock failed");
            }
            interruptWasEnabled = SYS_INT_SourceDisable((INT_SOURCE)hDriver->interruptSource);
        }

        if(irp->previous == NULL)
        {
                /* This means this was the first irp in the queue. Update the pipe
                 * queue head directly */

            pipe->irpQueueHead = irp->next;
            if(irp->next != NULL)
            {
                irp->next->previous = NULL;
            }
        }
        else
        {
                /* Remove the IRP from the linked list */
            irp->previous->next = irp->next;

            if(irp->next != NULL)
            {
                /* This applies if this is not the last
                 * irp in the linked list */
                irp->next->previous = irp->previous;
            }
        }

        if(irp->status == USB_HOST_IRP_STATUS_IN_PROGRESS)
        {
            /* If the irp is already in progress then
             * we set the temporary state. This will get
             * caught in _DRV_USBFSV1_HOST_ControlXferProcess()
             * and _DRV_USBFSV1_HOST_NonControlIRPProcess() functions. */

            irp->tempState = DRV_USBFSV1_HOST_IRP_STATE_ABORTED;
        }
        else
        {
            irp->status = USB_HOST_IRP_STATUS_ABORTED;
            if(irp->callback != NULL)
            {
                irp->callback((USB_HOST_IRP *)irp);
            }
        }

        if(!hDriver->isInInterruptContext)
        {
            if(interruptWasEnabled)
            {
                SYS_INT_SourceEnable((INT_SOURCE)hDriver->interruptSource);
            }

            if(OSAL_MUTEX_Unlock(&hDriver->mutexID) != OSAL_RESULT_TRUE)
            {
                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Mutex unlock failed");
            }
        }
    }
}/* end of DRV_USBFSV1_HOST_IRPCancel() */

// *****************************************************************************
/* Function:
     void DRV_USBFSV1_HOST_PipeClose(DRV_USBFSV1_HOST_PIPE_HANDLE pipeHandle)

  Summary:
    Closes an open pipe.
	
  Description:
    This function closes an open pipe. Any IRPs scheduled on the pipe will be
    aborted and IRP callback functions will be called with the status as
    DRV_USB_HOST_IRP_STATE_ABORTED. The pipe handle will become invalid and the
    pipe will not accept IRPs.

  Remarks:
    See drv_usbfs.h for usage information.
*/
void DRV_USBFSV1_HOST_PipeClose
(
    DRV_USBFSV1_HOST_PIPE_HANDLE pipeHandle
)
{
    /* This function closes an open pipe */

    bool interruptWasEnabled = false;

    DRV_USBFSV1_OBJ * hDriver;
    USB_HOST_IRP_LOCAL  * irp;
    DRV_USBFSV1_HOST_PIPE_OBJ       * pipe;
    DRV_USBFSV1_HOST_TRANSFER_GROUP * transferGroup;
    DRV_USBFSV1_HOST_ENDPOINT_OBJ   * endpointObj;
    volatile usb_registers_t * usbID;

    /* Make sure we have a valid pipe */
    if( ( pipeHandle == 0 )  || pipeHandle == DRV_USBFSV1_HOST_PIPE_HANDLE_INVALID)
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Invalid pipe handle");
    }
    else
    {
        pipe = (DRV_USBFSV1_HOST_PIPE_OBJ*) pipeHandle;

        /* Make sure that we are working with a pipe in use */
        if(pipe->inUse != true)
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Pipe is not in use");
        }
        else
        {
            hDriver = (DRV_USBFSV1_OBJ *)pipe->hClient;
            usbID = hDriver->usbID;

            /* Disable the interrupt */

            if(!hDriver->isInInterruptContext)
            {
                if(OSAL_MUTEX_Lock(&hDriver->mutexID, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
                {
                    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Mutex lock failed");
                }
                interruptWasEnabled = SYS_INT_SourceDisable((INT_SOURCE)hDriver->interruptSource);
            }

            if(USB_TRANSFER_TYPE_CONTROL == pipe->pipeType)
            {
                transferGroup = &hDriver->controlTransferGroup;

                if(pipe->previous == NULL)
                {
                    /* The previous pipe could be null if this was the first pipe in the
                     * transfer group */

                    transferGroup->pipe = pipe->next;
                    if(pipe->next != NULL)
                    {
                        pipe->next->previous = NULL;
                    }
                }
                else
                {
                    /* Remove this pipe from the linked list */

                    pipe->previous->next = pipe->next;
                    if(pipe->next != NULL)
                    {
                        pipe->next->previous = pipe->previous;
                    }
                }

                if(transferGroup->nPipes != 0)
                {
                    /* Reduce the count only if its not zero already */

                    transferGroup->nPipes --;
                }
            }
            else
            {
                /* Non control transfer pipes are not stored as groups.  We deallocate
                 * the endpoint object that this pipe used */

                endpointObj = &hDriver->hostEndpointTable[pipe->hostPipeN];
                endpointObj->endpoint.inUse = false;
                endpointObj->endpoint.pipe = NULL;
            }

            /* Clear all Pipe Interrupt enable bits and flags  */
            usbID->HOST.HOST_PIPE[pipe->hostPipeN].USB_PINTFLAG = USB_HOST_PINTFLAG_Msk;
            usbID->HOST.HOST_PIPE[pipe->hostPipeN].USB_PINTENCLR = USB_HOST_PINTENCLR_Msk;
            usbID->HOST.HOST_PIPE[pipe->hostPipeN].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_Msk;

            /* Now we invoke the call back for each IRP in this pipe and say that it is
             * aborted.  If the IRP is in progress, then that IRP will be actually
             * aborted on the next SOF This will allow the USB module to complete any
             * transaction that was in progress. */

            irp = (USB_HOST_IRP_LOCAL *)pipe->irpQueueHead;
            while(irp != NULL)
            {
                irp->pipe = DRV_USBFSV1_HOST_PIPE_HANDLE_INVALID;

                if(irp->status == USB_HOST_IRP_STATUS_IN_PROGRESS)
                {
                    /* If the IRP is in progress, then we set the temp IRP state. This
                     * will be caught in the _DRV_USBFSV1_HOST_NonControlTransferProcess() and
                     * _DRV_USBFSV1_HOST_ControlTransferProcess() functions */

                    irp->status = USB_HOST_IRP_STATUS_ABORTED;

                    if(irp->callback != NULL)
                    {
                        irp->callback((USB_HOST_IRP*)irp);
                    }
                    irp->tempState = DRV_USBFSV1_HOST_IRP_STATE_ABORTED;
                }
                else
                {
                    /* IRP is pending */
                    irp->status = USB_HOST_IRP_STATUS_ABORTED;

                    if(irp->callback != NULL)
                    {
                        irp->callback((USB_HOST_IRP*)irp);
                    }
                }
                irp = irp->next;
            }

            /* Now we return the pipe back to the driver */
            pipe->inUse = false;

            /* Enable the interrupts */
            if(!hDriver->isInInterruptContext)
            {
                if(interruptWasEnabled)
                {
                    SYS_INT_SourceEnable((INT_SOURCE)hDriver->interruptSource);
                }

                if(OSAL_MUTEX_Unlock(&hDriver->mutexID) != OSAL_RESULT_TRUE)
                {
                    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Mutex unlock failed");
                }
            }
        }
    }
}/* end of DRV_USBFSV1_HOST_PipeClose() */

// *****************************************************************************
/* Function:
    DRV_USBFSV1_HOST_PIPE_HANDLE DRV_USBFSV1_HOST_PipeSetup
    (
        DRV_HANDLE handle,
        uint8_t deviceAddress,
        USB_ENDPOINT endpointAndDirection,
        uint8_t hubAddress,
        uint8_t hubPort,
        USB_TRANSFER_TYPE pipeType,
        uint8_t bInterval,
        uint16_t wMaxPacketSize,
        USB_SPEED speed
    )

  Summary:
    Open a pipe with the specified attributes.
	
  Description:
    This function opens a communication pipe between the Host and the device
    endpoint. The transfer type and other attributes are specified through the
    function parameters. The driver does not check for available bus bandwidth,
    which should be done by the application (the USB Host Layer in this case)

  Remarks:
    See drv_usbfs.h for usage information.
*/
DRV_USBFSV1_HOST_PIPE_HANDLE DRV_USBFSV1_HOST_PipeSetup
(
    DRV_HANDLE client,
    uint8_t deviceAddress,
    USB_ENDPOINT endpointAndDirection,
    uint8_t hubAddress,
    uint8_t hubPort,
    USB_TRANSFER_TYPE pipeType,
    uint8_t bInterval,
    uint16_t wMaxPacketSize,
    USB_SPEED speed
)
{
    uint32_t pipeIter = 0;
    bool epFound = false;
    uint8_t bufferSize = 0;  
    uint16_t defaultEndpointSize = 8;                /* Default size of Endpoint */

    DRV_USBFSV1_OBJ * hDriver;
    DRV_USBFSV1_HOST_PIPE_OBJ * pipe = NULL;
    DRV_USBFSV1_HOST_PIPE_HANDLE pipeHandle = DRV_USBFSV1_HOST_PIPE_HANDLE_INVALID;

    if((client == DRV_HANDLE_INVALID) || (((DRV_HANDLE *)client) == NULL))
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Invalid client handle");
    }

    else if((speed == USB_SPEED_LOW) || (speed == USB_SPEED_FULL) || (speed == USB_SPEED_HIGH))
    {
        if(pipeType != USB_TRANSFER_TYPE_CONTROL)
        {
            if(wMaxPacketSize < 8)
            {
                wMaxPacketSize = 8;
            }
        }

        if((wMaxPacketSize < 8) ||(wMaxPacketSize > 4096))
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Invalid pipe endpoint size");
        }
        else
        {

            hDriver = (DRV_USBFSV1_OBJ *)client;

            if(OSAL_MUTEX_Lock(&hDriver->mutexID, OSAL_WAIT_FOREVER) != OSAL_RESULT_TRUE)
            {
                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Mutex lock failed");
            }
            else
            {
                if(pipeType == USB_TRANSFER_TYPE_CONTROL)
                {
                    /* Set pipeIter to zero to indicate that we must use pipe 0
                     * for control transfers. We also add the control transfer pipe
                     * to the control transfer group. */
                    pipeIter = 0;

                    if (wMaxPacketSize >= 8)
                    {
                        /* Find upper 2 power number of endpointSize */
                        if(wMaxPacketSize)
                        {
                            while (defaultEndpointSize < wMaxPacketSize)
                            {
                                bufferSize++;
                                defaultEndpointSize <<= 1;
                            }
                        }
        
                        /* Pipe is enabled and configured as CONTROL */
                        hDriver->usbID->HOST.HOST_PIPE[pipeIter].USB_PCFG = USB_HOST_PCFG_PTYPE(USB_HOST_PIPE_TYPE_CONTROL);
                        /* SETUP CTRL, single bank */
                        hDriver->usbID->HOST.HOST_PIPE[pipeIter].USB_PCFG |= USB_HOST_PCFG_PTOKEN( USB_HOST_PIPE_TOKEN_SETUP );
                        /* Interval for the Bulk-Out/Ping Transaction */ 
                        hDriver->usbID->HOST.HOST_PIPE[pipeIter].USB_BINTERVAL |= USB_HOST_BINTERVAL_BITINTERVAL(bInterval);

                        memset((uint8_t *)&usb_pipe_table[pipeIter], 0, sizeof(usb_pipe_table[0]));

                        hDriver->hostEndpointTablePtr[pipeIter].HOST_DESC_BANK[0].USB_CTRL_PIPE = USB_HOST_CTRL_PIPE_PDADDR(deviceAddress) |
	                                                                     USB_HOST_CTRL_PIPE_PEPNUM(pipeIter);

                        hDriver->hostEndpointTablePtr[pipeIter].HOST_DESC_BANK[0].USB_PCKSIZE |= USB_DEVICE_PCKSIZE_SIZE(bufferSize);

                        /* Always enable stall and error interrupts of control endpoint */
                        /* Enable Stall Interrupt */
                        hDriver->usbID->HOST.HOST_PIPE[pipeIter].USB_PINTENSET = USB_HOST_PINTENSET_STALL_Msk;
                        /* Enable Pipe Error Interrupt */
                        hDriver->usbID->HOST.HOST_PIPE[pipeIter].USB_PINTENSET = USB_HOST_PINTENSET_PERR_Msk;

                        /* Enable transfer complete interrupt */
                        hDriver->usbID->HOST.HOST_PIPE[pipeIter].USB_PINTENSET = USB_HOST_PINTENSET_TRCPT0_Msk;

                        epFound = true;
                        pipe = &gDrvUSBHostPipeObj[pipeIter];
                    }
                }
                else
                {
                    /* Pipe allocation for non-control transfer */
                    for(pipeIter = 1; pipeIter < 10; pipeIter ++)
                    {
                        if((false == gDrvUSBHostPipeObj[pipeIter].inUse))
                        {
                            /* This means we have found a free pipe object */
                            /* Reset Pipe */
                            hDriver->usbID->HOST.HOST_PIPE[pipeIter].USB_PCFG = 0;

                            /* Allocate the Pipe Memory */
                            if( pipeType == USB_TRANSFER_TYPE_ISOCHRONOUS) 
                            {
                                /* Pipe is enabled and configured as iso */
                                SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "\r\nUSB Host Layer: Pipe[%d] is enabled and configured as iso", (int)pipeIter);
                                hDriver->usbID->HOST.HOST_PIPE[pipeIter].USB_PCFG = USB_HOST_PCFG_PTYPE(USB_HOST_PIPE_TYPE_ISO);
                            }
                            else if( pipeType == USB_TRANSFER_TYPE_BULK) 
                            {
                                /* Pipe is enabled and configured as bulk */
                                SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "\r\nUSB Host Layer: Pipe[%d] is enabled and configured as bulk", (int)pipeIter);
                                hDriver->usbID->HOST.HOST_PIPE[pipeIter].USB_PCFG = USB_HOST_PCFG_PTYPE(USB_HOST_PIPE_TYPE_BULK);
                            }
                            else if( pipeType == USB_TRANSFER_TYPE_INTERRUPT) 
                            {
                                /* Pipe is enabled and configured as interrupt */
                                SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "\r\nUSB Host Layer: Pipe[%d] is enabled and configured as interrupt", (int)pipeIter);
                                hDriver->usbID->HOST.HOST_PIPE[pipeIter].USB_PCFG = USB_HOST_PCFG_PTYPE(USB_HOST_PIPE_TYPE_INTERRUPT);
                            }
                            else
                            {
                                /* Error */
                                SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nError");
                            }

                            if( (endpointAndDirection & 0x80) == 0x80 )
                            {
                                /* device to host: IN */
                                SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, " IN");
                                /* Be careful with this buffer, the PTYPE must not be set to zero (pipe disable). */
                                hDriver->usbID->HOST.HOST_PIPE[pipeIter].USB_PCFG = (hDriver->usbID->HOST.HOST_PIPE[pipeIter].USB_PCFG
                                                                                   & ~(USB_HOST_PCFG_PTOKEN_Msk))
                                                                                | USB_HOST_PCFG_PTOKEN(USB_HOST_PIPE_TOKEN_IN);
                            }
                            else
                            {
                                /* device to host: OUT */
                                SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, " OUT");
                                /* Be careful with this buffer, the PTYPE must not be set to zero (pipe disable). */
                                hDriver->usbID->HOST.HOST_PIPE[pipeIter].USB_PCFG = (hDriver->usbID->HOST.HOST_PIPE[pipeIter].USB_PCFG
                                                                                   & ~(USB_HOST_PCFG_PTOKEN_Msk))
                                                                                | USB_HOST_PCFG_PTOKEN(USB_HOST_PIPE_TOKEN_OUT);
                                /* Interval for the Bulk-Out/Ping Transaction */
                                hDriver->usbID->HOST.HOST_PIPE[pipeIter].USB_BINTERVAL &= ~USB_HOST_BINTERVAL_BITINTERVAL_Msk;
                                hDriver->usbID->HOST.HOST_PIPE[pipeIter].USB_BINTERVAL |= USB_HOST_BINTERVAL_BITINTERVAL(bInterval);
                            }
 
                            /* Configure address of Pipe */
                            memset((uint8_t *)&usb_pipe_table[pipeIter], 0, sizeof(usb_pipe_table[0]));

                            if (wMaxPacketSize == 1023)
                            {
                                /* 1023 Byte in FS mode */
                                usb_pipe_table[pipeIter].HOST_DESC_BANK[0].USB_PCKSIZE =  USB_HOST_PCKSIZE_SIZE(7);
                            } 
                            else 
                            {
                                usb_pipe_table[pipeIter].HOST_DESC_BANK[0].USB_PCKSIZE = USB_HOST_PCKSIZE_SIZE(32 - clz(((uint32_t)min(max(wMaxPacketSize, 8), 1024) << 1) - 1) - 1 - 3);
                            }

                            /* Pipe and endpoint can be different. */
                            usb_pipe_table[pipeIter].HOST_DESC_BANK[0].USB_CTRL_PIPE = USB_HOST_CTRL_PIPE_PDADDR(deviceAddress) |
                                                                                     USB_HOST_CTRL_PIPE_PEPNUM(endpointAndDirection&(~0x80));

                            /* Enable Stall Interrupt */
                            hDriver->usbID->HOST.HOST_PIPE[pipeIter].USB_PINTENSET = USB_HOST_PINTENSET_STALL_Msk;

                            /* Enable Pipe Error Interrupt */
                            hDriver->usbID->HOST.HOST_PIPE[pipeIter].USB_PINTENSET = USB_HOST_PINTENSET_PERR_Msk;
                            /* isochronous only: hDriver->usbID->HOST.HOST_PIPE[pipeIter].USB_PINTENSET = USB_HOST_PINTENSET_TRFAIL; */

                            /* Enable transfer complete interrupt */
                            hDriver->usbID->HOST.HOST_PIPE[pipeIter].USB_PINTENSET = USB_HOST_PINTENSET_TRCPT0_Msk;

                            epFound = true;
                            pipe = &gDrvUSBHostPipeObj[pipeIter];
                            hDriver->hostEndpointTable[pipeIter].endpoint.inUse = true;
                            hDriver->hostEndpointTable[pipeIter].endpoint.pipe = pipe;                
                            break;
                        }                            
                    } /* for */            
                }    

                if(!epFound)
                {
                    /* This means we could not find a spare endpoint for this
                     * non control transfer. */
                    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\033[31m\r\nDRV USBFSV1: Could not allocate endpoint\033[39m");
                }
                if(OSAL_MUTEX_Unlock(&hDriver->mutexID) != OSAL_RESULT_TRUE)
                {
                    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\033[31m\r\nDRV USBFSV1: Mutex unlock failed\033[39m");
                }
            }
    
            if (pipe != NULL)
            {
                /* Setup the pipe object */
                pipe->inUse         = true;
                pipe->deviceAddress = deviceAddress;
                pipe->irpQueueHead  = NULL;
                pipe->bInterval     = bInterval;
                pipe->speed         = speed;
                pipe->hubAddress    = hubAddress;
                pipe->hubPort       = hubPort;
                pipe->pipeType      = pipeType;
                pipe->hClient       = client;
                pipe->endpointSize  = wMaxPacketSize;
                pipe->intervalCounter = bInterval;
                pipe->hostPipeN     = pipeIter;
                pipe->endpointAndDirection   = endpointAndDirection;
                pipeHandle = (DRV_USBFSV1_HOST_PIPE_HANDLE)pipe;
            }
        }
    }
    return(pipeHandle);

}/* end of DRV_USBFSV1_HOST_PipeSetup() */

// *****************************************************************************
/* Function:
    void _DRV_USBFSV1_HOST_ControlTransferProcess
    (
        DRV_USBFSV1_OBJ * hDriver
    )

  Summary:
    Control Transfer Process.
	
  Description:
    This function is called every time there is an endpoint 0 interrupt.
    This means that a stage of the current control IRP has been completed.
    This function is called from an interrupt context

  Remarks:
   
*/

static void _DRV_USBFSV1_HOST_ControlTransferProcess(DRV_USBFSV1_OBJ * hDriver)
{
    /* This function is called every time there is an endpoint 0
     * interrupt. This means that a stage of the current control IRP has been
     * completed. This function is called from an interrupt context */

    USB_HOST_IRP_LOCAL * irp;
    DRV_USBFSV1_HOST_PIPE_OBJ * pipe, * iterPipe;
    DRV_USBFSV1_HOST_TRANSFER_GROUP * transferGroup;
    bool endIRP = false;
    uint32_t count, i;
    bool foundIRP = false;
    bool isDmaUsed = false;
    volatile usb_registers_t * usbID;
    transferGroup = &hDriver->controlTransferGroup;
    usbID = hDriver->usbID;

    /* First check if the IRP was aborted */
    irp = transferGroup->currentIRP;
    pipe = transferGroup->currentPipe;

    /* If current IRP is null, or current pipe is null then we have unknown
     * failure. We just quit.  Nothing we can do.*/
    if((irp == NULL) || (pipe == NULL) ||
            (pipe == (DRV_USBFSV1_HOST_PIPE_OBJ *)DRV_USBFSV1_HOST_PIPE_HANDLE_INVALID))
    {
        /* Unknown error */
        SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "\033[31m\r\n Unknown error USB\033[39m");
        return;
    }

    /* If here means, we have a valid IRP and pipe.  Check the status register.
     * The IRP could have been aborted. This would be known in the temp state.
     */
    if(irp->tempState == DRV_USBFSV1_HOST_IRP_STATE_ABORTED)
    {
        /* This means the application has aborted this
         IRP.*/
        SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "\033[31m\r\nabort\033[39m");
        endIRP = true;
        irp->status = USB_HOST_IRP_STATUS_ABORTED;
    }
    else if ( 0 != (usbID->HOST.HOST_PIPE[0].USB_PINTFLAG & USB_HOST_PINTFLAG_STALL_Msk))
    {
        /* This means the packet was stalled. Set the error status and then
         * clear the stall bit */
        SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "\033[31m\r\nError: pipe number: 0, USB_HOST_PINTFLAG_STALL\033[39m");

        endIRP = true;
        irp->status = USB_HOST_IRP_STATUS_ERROR_STALL;
        /* Clear the STALL Interrupt Flag. */
        hDriver->usbID->HOST.HOST_PIPE[0].USB_PINTFLAG = USB_HOST_PINTFLAG_STALL_Msk;
        /* Reset DATA Toggle */
        usbID->HOST.HOST_PIPE[0].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_DTGL_Msk;
    }
    /* Pipe Error Interrupt Flag */
    else if(USB_HOST_PINTFLAG_PERR_Msk == (usbID->HOST.HOST_PIPE[0].USB_PINTFLAG & USB_HOST_PINTFLAG_PERR_Msk))
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "\033[31m\r\n USB_HOST_PINTFLAG_PERR\033[39m");
        /* This means there was a pipe error. Set the error status and then
		 * clear the error bits */
        endIRP = true;
        irp->status = USB_HOST_IRP_STATUS_ERROR_DATA;
        /* Ack all errors */
        usbID->HOST.HOST_PIPE[0].USB_PINTFLAG = USB_HOST_PINTFLAG_PERR_Msk;
    }
    else
    {
        /* This means there was no error. We should check the current state of
         * the IRP and then proceed accordingly */

        switch(irp->tempState)
        {
             case DRV_USBFSV1_HOST_IRP_STATE_SETUP_STAGE:
                if ( 0 != (usbID->HOST.HOST_PIPE[0].USB_PINTFLAG & USB_HOST_PINTFLAG_TXSTP_Msk))
                {
                    /* SETUP packet sent */
                    /* Freeze Pipe */
                    usbID->HOST.HOST_PIPE[0].USB_PSTATUSSET = USB_HOST_PSTATUSSET_PFREEZE_Msk;
                    /* Clear Transmitted Setup Interrupt Flag */
                    usbID->HOST.HOST_PIPE[0].USB_PINTFLAG = USB_HOST_PINTFLAG_TXSTP_Msk;
                    /* Clear Tx Setup Ready Interrupt */
                    usbID->HOST.HOST_PIPE[0].USB_PINTENCLR = USB_HOST_PINTENCLR_TXSTP_Msk;
                }
                else
                {
                    return;
                }
 
               /* We got an interrupt after this stage. This means that the
                * setup stage has completed.  We need to check if a data stage
                * is required and then start the data stage. */

               /* Data Stage */
               if((irp->data == NULL) || (irp->size == 0))
               {
                   /* This means there is no data stage. We can proceed to the
                    * handshake stage. In a case where there is not a data stage,
                    * we need to send an IN token to the device */
                    irp->tempState = DRV_USBFSV1_HOST_IRP_STATE_HANDSHAKE_SENT;
                    /* Configure pipe for IN token */
                    /* Be careful with this buffer, the PTYPE must not be set to zero (pipe disable). */
                    usbID->HOST.HOST_PIPE[0].USB_PCFG = (usbID->HOST.HOST_PIPE[0].USB_PCFG
                                                       & ~(USB_HOST_PCFG_PTYPE_Msk|USB_HOST_PCFG_PTOKEN_Msk))
                                                    | USB_HOST_PCFG_PTYPE(USB_HOST_PIPE_TYPE_CONTROL)
                                                    | USB_HOST_PCFG_PTOKEN(USB_HOST_PIPE_TOKEN_IN);

                    /* The host indicates it is able to receive data from the device by clearing the Bank 0/1 Ready bit in PSTATUS
                     * which means that the memory for the bank is available for new USB transfer */
                    usbID->HOST.HOST_PIPE[0].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_BK0RDY_Msk;
                    /* Unfreeze pipe */
                    usbID->HOST.HOST_PIPE[0].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_PFREEZE_Msk;
                }
                else
                {
                    /* This means that a data stage is required. We can find out
                     * the direction of the data stage by investigating the flags
                     * parameter of the  IRP. */
                    irp->tempState = DRV_USBFSV1_HOST_IRP_STATE_DATA_STAGE_SENT;
                    if(irp->flags & 0x80)
                    {
                       /* This means the data stage moves from device to 
                        * host. Set bit 15 of the flags parameter */
                        /* Start IN requests */
                        /* Clear Transfer Complete 0 Interrupt Flag */
                        usbID->HOST.HOST_PIPE[0].USB_PINTFLAG = USB_HOST_PINTFLAG_TRCPT0_Msk;
                        /* Enable transfer complete interrupt */
                        usbID->HOST.HOST_PIPE[0].USB_PINTENSET = USB_HOST_PINTENSET_TRCPT0_Msk;
                        usb_pipe_table[0].HOST_DESC_BANK[0].USB_ADDR = (uint32_t)&usb_ctrl_buffer[0];
                        /* MULTI_PACKET_SIZE: For IN pipes, MULTI_PACKET_SIZE holds the total number of bytes sent. 
                         * MULTI_PACKET_SIZE should be written to zero when setting up a new transfer. */
                        /* BYTE_COUNT: These bits define the 14-bit value that contains number of bytes sent in 
                         * the last OUT or SETUP transaction for an OUT pipe, or of the number of bytes to be received
                         * in the next IN transaction for an input pipe. */
                        /* Set Multi_Packet_Size to transfer more data than wMaxPacketSize. So multiple IN will be performed if needed. */
                        usb_pipe_table[0].HOST_DESC_BANK[0].USB_PCKSIZE = ((usb_pipe_table[0].HOST_DESC_BANK[0].USB_PCKSIZE) & USB_HOST_PCKSIZE_SIZE_Msk)
                                                                        | USB_HOST_PCKSIZE_BYTE_COUNT(0) 
                                                                        | USB_HOST_PCKSIZE_MULTI_PACKET_SIZE(irp->size);

                        /* This means the data stage moves from device to host.
                         * So the host would have to send an IN token.  */
                        /* Configure pipe for IN token */
                        /* Be careful with this buffer, the PTYPE must not be set to zero (pipe disable). */
                        usbID->HOST.HOST_PIPE[0].USB_PCFG = (usbID->HOST.HOST_PIPE[0].USB_PCFG 
                                               & ~(USB_HOST_PCFG_PTYPE_Msk|USB_HOST_PCFG_PTOKEN_Msk))
                                            | USB_HOST_PCFG_PTYPE(USB_HOST_PIPE_TYPE_CONTROL) 
                                            | USB_HOST_PCFG_PTOKEN(USB_HOST_PIPE_TOKEN_IN);

                        /* The host indicates it is able to receive data from the device by clearing the Bank 0/1 Ready bit in PSTATUS
                         * which means that the memory for the bank is available for new USB transfer */
                        usbID->HOST.HOST_PIPE[0].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_BK0RDY_Msk;

                        /* Unfreeze pipe */
                        usbID->HOST.HOST_PIPE[0].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_PFREEZE_Msk;
                    }
                    else
                    {
                       /* This means the data stage moves from host to
                        * device. Clear bit 15 of the flags parameter. */

                        /* This function loads the fifo and sends the packet. The
                        * completed bytes field in the IRP will be updated. */
                       _DRV_USBFSV1_HOST_IRPTransmitFIFOLoad(usbID, irp, 0);
                   }
               }
               break;

           case DRV_USBFSV1_HOST_IRP_STATE_DATA_STAGE_SENT:

                if ( 0 != (usbID->HOST.HOST_PIPE[0].USB_PINTFLAG & USB_HOST_PINTFLAG_TRCPT0_Msk))
                {
                    /* Freeze a pipe */
                    usbID->HOST.HOST_PIPE[0].USB_PSTATUSSET = USB_HOST_PSTATUSSET_PFREEZE_Msk; 
                   
                    if( usbID->HOST.HOST_PIPE[0].USB_PCFG & USB_HOST_PCFG_PTOKEN(USB_HOST_PIPE_TOKEN_IN))
                    {
                        /* IN packet received */
                        /* Clear Transfer Complete 0 Interrupt Flag */
                        usbID->HOST.HOST_PIPE[0].USB_PINTFLAG = USB_HOST_PINTFLAG_TRCPT0_Msk;
                        /* In case of low USB speed and with a high CPU frequency,
                         * an ACK from host can be always running on USB line
                         * then wait end of ACK on IN pipe */
                        if (((usbID->HOST.USB_STATUS & USB_HOST_STATUS_SPEED_Msk)>>USB_HOST_STATUS_SPEED_Pos) == 0x1) /* 0x1: Low-speed mode */
                        {    
                            // while(!usbID->USBFS_FSTPIPIMR[0].PFREEZE);
                            //while( (usbID->HOST.HOST_PIPE[0].USB_PINTFLAG & USB_HOST_PINTFLAG_TRCPT0) == USB_HOST_PINTFLAG_TRCPT0 );
                        }    
                    }
                    else 
                    {
                        /* OUT packet sent */
                        /* Set Data Toggle */
                        usbID->HOST.HOST_PIPE[0].USB_PSTATUSSET = USB_HOST_PSTATUSSET_DTGL_Msk;
                        /* Clear Transfer Complete 0 Interrupt Flag */
                        usbID->HOST.HOST_PIPE[0].USB_PINTFLAG = USB_HOST_PINTFLAG_TRCPT0_Msk;
                        /* Clear Transmitted Setup Interrupt Flag */
                        usbID->HOST.HOST_PIPE[0].USB_PINTFLAG = USB_HOST_PINTFLAG_TXSTP_Msk;
                    }
                }
	
                /* We reach here after an interrupt which means that a data stage
                * interaction was completed. Find out what was the direction the
                * data stage */

                if(irp->flags & 0x80)
                {
                    /* This means the data was moving from device to host. We got
                     * an interrupt, which means we have received data. Start by
                     * checking how much data we received from the device */

                    count = _DRV_USBFSV1_HOST_IRPReceiveFIFOUnload(usbID, irp, 0, &isDmaUsed);
                        
                    if((count < pipe->endpointSize) ||
                            (irp->completedBytes >= irp->size))
                    {
                        /* Status Stage */
                        /* This means that we either received a short packet or
                         * we received the amount of data that we needed. We
                         * should move to the handshake stage. */
                        irp->tempState = DRV_USBFSV1_HOST_IRP_STATE_HANDSHAKE_SENT;

                        /* Configure pipe for OUT token */
                        /* Be careful with this buffer, the PTYPE must not be set to zero (pipe disable). */
                        hDriver->usbID->HOST.HOST_PIPE[0].USB_PCFG = (usbID->HOST.HOST_PIPE[0].USB_PCFG
                                                            & ~(USB_HOST_PCFG_PTYPE_Msk|USB_HOST_PCFG_PTOKEN_Msk))
                                                        | USB_HOST_PCFG_PTYPE(USB_HOST_PIPE_TYPE_CONTROL)
                                                        | USB_HOST_PCFG_PTOKEN(USB_HOST_PIPE_TOKEN_OUT);

                        usb_pipe_table[0].HOST_DESC_BANK[0].USB_ADDR = (uint32_t)&usb_ctrl_buffer[0];
                        usb_pipe_table[0].HOST_DESC_BANK[0].USB_PCKSIZE = ((usb_pipe_table[0].HOST_DESC_BANK[0].USB_PCKSIZE) & USB_HOST_PCKSIZE_SIZE_Msk)
                                                                        | USB_HOST_PCKSIZE_BYTE_COUNT(0)   /* ZLP */
                                                                        | USB_HOST_PCKSIZE_MULTI_PACKET_SIZE(0);
                        /* Set Data Toggle */
                        usbID->HOST.HOST_PIPE[0].USB_PSTATUSSET = USB_HOST_PSTATUSSET_DTGL_Msk;
                        /* The user writes the OUT data to the data buffer pointer by ADDR in the pipe descriptor and allows the */
                        /* USB to send the data by writing a one to the PSTATUS.BK0/1RDY. */
                        usbID->HOST.HOST_PIPE[0].USB_PSTATUSSET = USB_HOST_PSTATUSSET_BK0RDY_Msk;
                        
                        /* Enable transfer complete interrupt */
                        usbID->HOST.HOST_PIPE[0].USB_PINTENSET = USB_HOST_PINTENSET_TRCPT0_Msk;  
                        /* Unfreeze pipe */
                        usbID->HOST.HOST_PIPE[0].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_PFREEZE_Msk;
                   }
                   else
                   {
                        /* This means this is a multi stage control read
                         * transfer. Issue another IN token */
                        /* Configure pipe for IN token */
                        /* Be careful with this buffer, the PTYPE must not be set to zero (pipe disable). */
                        hDriver->usbID->HOST.HOST_PIPE[0].USB_PCFG = (usbID->HOST.HOST_PIPE[0].USB_PCFG
                                                           & ~(USB_HOST_PCFG_PTOKEN_Msk))
                                                        | USB_HOST_PCFG_PTOKEN(USB_HOST_PIPE_TOKEN_IN);

                        /* The host indicates it is able to receive data from the device by clearing the Bank 0/1 Ready bit in PSTATUS
                         * which means that the memory for the bank is available for new USB transfer */
                        usbID->HOST.HOST_PIPE[0].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_BK0RDY_Msk;
                        /* Unfreeze pipe */
                        usbID->HOST.HOST_PIPE[0].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_PFREEZE_Msk;
                   }
               }
               else
               {
                   /* Data stage was moving from host to device.  Check if we
                    * need to send more data */

                   if(irp->completedBytes < irp->size)
                   {
                        _DRV_USBFSV1_HOST_IRPTransmitFIFOLoad(usbID, irp, 0);
                   }
                   else
                   {
                        /* We can move to the status stage */
                        irp->tempState = DRV_USBFSV1_HOST_IRP_STATE_HANDSHAKE_SENT;
                        /* Be careful with this buffer, the PTYPE must not be set to zero (pipe disable). */
                        hDriver->usbID->HOST.HOST_PIPE[0].USB_PCFG = (usbID->HOST.HOST_PIPE[0].USB_PCFG
                                                                       & ~(USB_HOST_PCFG_PTOKEN_Msk))
                                                                    | USB_HOST_PCFG_PTOKEN(USB_HOST_PIPE_TOKEN_IN);

                        /* The host indicates it is able to receive data from the device by clearing the Bank 0/1 Ready bit in PSTATUS
                         * which means that the memory for the bank is available for new USB transfer */
                        usbID->HOST.HOST_PIPE[0].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_BK0RDY_Msk;
                        /* Unfreeze pipe */
                        usbID->HOST.HOST_PIPE[0].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_PFREEZE_Msk;
                   }
               }
               break;

           case DRV_USBFSV1_HOST_IRP_STATE_HANDSHAKE_SENT:

               /* If we have reached here, it means that status stage has
                * completed. Check the direction of the data stage, update the
                * irp status flag and then end the irp. */
                if ( USB_HOST_PINTFLAG_TRCPT0_Msk == (usbID->HOST.HOST_PIPE[0].USB_PINTFLAG & USB_HOST_PINTFLAG_TRCPT0_Msk))
                {
                    /* Freeze a pipe */
                    usbID->HOST.HOST_PIPE[0].USB_PSTATUSSET = USB_HOST_PSTATUSSET_PFREEZE_Msk;
                    /* IN packet received */
                    /* In case of low USB speed and with a high CPU frequency,
                     * an ACK from host can be always running on USB line
                     * then wait end of ACK on IN pipe */
                    if (((usbID->HOST.USB_STATUS & USB_HOST_STATUS_SPEED_Msk)>>USB_HOST_STATUS_SPEED_Pos) == 0x1) /* 0x1: Low-speed mode */
                    {    
                        // while(!usbID->USBFS_FSTPIPIMR[0].PFREEZE);
                        //while( (usbID->HOST.HOST_PIPE[0].USB_PINTFLAG & USB_HOST_PINTFLAG_TRCPT0) != USB_HOST_PINTFLAG_TRCPT0 );
                    }                     
                    /* Clear Transfer Complete 0 Interrupt Flag */
                    usbID->HOST.HOST_PIPE[0].USB_PINTFLAG = USB_HOST_PINTFLAG_TRCPT0_Msk;
                }
                else if ( 0 != (usbID->HOST.HOST_PIPE[0].USB_PINTFLAG & USB_HOST_PINTFLAG_TXSTP_Msk))
                {
                    /* Freeze a pipe */
                    usbID->HOST.HOST_PIPE[0].USB_PSTATUSSET = USB_HOST_PSTATUSSET_PFREEZE_Msk;
                    /* OUT packet sent */
                    /* Clear Transmitted Setup Interrupt Flag */
                    usbID->HOST.HOST_PIPE[0].USB_PINTFLAG = USB_HOST_PINTFLAG_TXSTP_Msk;
                }
                else
                {
                    return;
                }
                
               irp->status = USB_HOST_IRP_STATUS_COMPLETED;
               
               if(irp->flags & 0x80)
               {
                  
                   /* This means the data stage moved from device to host. We
                    * need to check the number of bytes the host sent. If it was
                    * less than expected, then irp status should say so */

                   if(irp->completedBytes < irp->size)
                   {
                       irp->size = irp->completedBytes;
                       irp->status = USB_HOST_IRP_STATUS_COMPLETED_SHORT;
                   }
               }
               else
               {
                   /* We need to clear the Status Packet bit and
                    * the Rx packet ready bit */
               }
               endIRP = true;
               break;

           default:
               break;
       }
    }

    if(endIRP)
    {
        /* This means this IRP needs to be terminated and new one started. */

        if(irp->callback != NULL)
        {
            irp->callback((USB_HOST_IRP *)irp);
        }

        /* Expire the IRP */

        pipe->irpQueueHead = irp->next;

        /* Now we need to check if any more IRPs are in this group are pending.
         * We start searching from the current pipe onwards. If we don't find
         * another pipe with an IRP, we should land back on the current pipe and
         * check if we have a IRP to process */

        iterPipe = transferGroup->currentPipe->next;
        for(i = 0; i < transferGroup->nPipes; i ++)
        {
            if(iterPipe == NULL)
            {
                /* We have reached the end of the pipe group. Rewind the pipe
                 * iterator to the start of the pipe group. */

                iterPipe = transferGroup->pipe;
            }

            /* This means that we have a valid pipe.  Now check if there is irp
             * to be processed. */

            if(iterPipe->irpQueueHead != NULL)
            {
                foundIRP = true;
                transferGroup->currentPipe = iterPipe;
                transferGroup->currentIRP = iterPipe->irpQueueHead;
                break;
            }

            iterPipe = iterPipe->next;
        }

        if(foundIRP)
        {
            /* This means we have found another IRP to process. We must load the
             * endpoint. */
            
            irp = transferGroup->currentIRP;
            pipe = transferGroup->currentPipe;
            irp->status = USB_HOST_IRP_STATUS_IN_PROGRESS;
            irp->tempState = DRV_USBFSV1_HOST_IRP_STATE_SETUP_STAGE;

            /* We need to update the flags parameter of the IRP to indicate the
             * direction of the control transfer. */

            if(*((uint8_t*)(irp->setup)) & 0x80)
            {
                /* This means the data stage moves from device to host. Set bit
                 * 15 of the flags parameter */
               irp->flags |= 0x80;
            }
            else
            {
                /* This means the data stage moves from host to device. Clear
                 * bit 15 of the flags parameter. */
                irp->flags &= 0x7F;
            }

            /* Send the setup packet to the device */
            _DRV_USBFSV1_HOST_IRPTransmitSetupPacket(usbID, irp);
        }
        else
        {
            /* This means we don t have an IRP. Set the current IRP and current
             * pipe to NULL to indicate that we don t have any active IRP */

            transferGroup->currentPipe = NULL;
            transferGroup->currentIRP = NULL;
        }
    }

    return;
}/* end of _DRV_USBFSV1_HOST_ControlTransferProcess() */

// *****************************************************************************
/* Function:
    void _DRV_USBFSV1_HOST_NonControlTransferProcess
    (
        DRV_USBFSV1_OBJ * hDriver
        uint8_t hostPipe
   )

  Summary:
    Non Control Transfer Process.
	
  Description:
    This function processes non-zero endpoint transfers which
    could be any of bulk, interrupt and isochronous transfers

  Remarks:
*/
static void _DRV_USBFSV1_HOST_NonControlTransferProcess
(
    DRV_USBFSV1_OBJ * hDriver,
    uint8_t hostPipe
)
{
    /* This function processes non-zero endpoint transfers which
     * could be any of bulk, interrupt and isochronous transfers */

    DRV_USBFSV1_HOST_ENDPOINT_OBJ * endpointTable;
    DRV_USBFSV1_HOST_PIPE_OBJ * pipe;
    USB_HOST_IRP_LOCAL * irp;
    volatile usb_registers_t * usbID;
    bool endIRP = false;
    bool isDmaUsed = false;
    bool endIRPOut = false;
    uint32_t count;

    endpointTable = &(hDriver->hostEndpointTable[hostPipe]);
    usbID = hDriver->usbID;
    pipe = endpointTable->endpoint.pipe; 

    if((!endpointTable->endpoint.inUse) ||
       (pipe == NULL) ||
       (pipe == (DRV_USBFSV1_HOST_PIPE_OBJ *)(DRV_USBFSV1_HOST_PIPE_HANDLE_INVALID)))
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nPipe close");
        /* This means the pipe was closed. We don't do anything */
        return;
    }

    if(pipe->irpQueueHead == NULL)
    {
        return;
    }

    irp = pipe->irpQueueHead;
	
    /* We got an interrupt for data moving from host to to device. Check if
     * there were any transmission errors. Then check if there is any more
     * data to be sent in the IRP. If we are done, then end the IRP and
     * start a new one. */

    /* Check if the IRP was aborted */
    if(irp->tempState == DRV_USBFSV1_HOST_IRP_STATE_ABORTED)
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nabort");
        endIRP = true;
        irp->status = USB_HOST_IRP_STATUS_ABORTED;
    }
    else if ( USB_HOST_PINTFLAG_STALL_Msk == (usbID->HOST.HOST_PIPE[hostPipe].USB_PINTFLAG & USB_HOST_PINTFLAG_STALL_Msk))
    {
        /* This means the packet was stalled. Set the error status and then
         * clear the stall bit */

        endIRP = true;
        irp->status = USB_HOST_IRP_STATUS_ERROR_STALL;
        /* Clear Stall Interrupt */
        hDriver->usbID->HOST.HOST_PIPE[hostPipe].USB_PINTFLAG = USB_HOST_PINTFLAG_STALL_Msk;

        /* Reset DATA Toggle */
        usbID->HOST.HOST_PIPE[hostPipe].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_DTGL_Msk;
    }
    else if(USB_HOST_PINTFLAG_PERR_Msk == (usbID->HOST.HOST_PIPE[hostPipe].USB_PINTFLAG & USB_HOST_PINTFLAG_PERR_Msk))
    {
        /* This means there was an bus error. The packet was tried three
         * times and was not successfully processed */

        endIRP = true;
        irp->status = USB_HOST_IRP_STATUS_ERROR_DATA;
        /* Ack all errors */
        usbID->HOST.HOST_PIPE[hostPipe].USB_PINTFLAG = USB_HOST_PINTFLAG_PERR_Msk;
    }
    else if ( USB_HOST_PINTFLAG_TXSTP_Msk == (usbID->HOST.HOST_PIPE[hostPipe].USB_PINTFLAG & USB_HOST_PINTFLAG_TXSTP_Msk))
    {
        /* This means this transaction completed successfully.  We should
         * check if there are any spare bytes remaining to be sent and then
         * send it */
            
        /* Clear Transmitted Setup Interrupt Flag */
        usbID->HOST.HOST_PIPE[hostPipe].USB_PINTFLAG = USB_HOST_PINTFLAG_TXSTP_Msk;

        /* Reset data toggle for DATA */
        hDriver->usbID->HOST.HOST_PIPE[hostPipe].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_DTGL_Msk;

        if(irp->completedBytes >= irp->size)
        {
            endIRP = true;
		    endIRPOut = true;
            irp->status = USB_HOST_IRP_STATUS_COMPLETED;
        }
        else
        {
            /* This means we have more data to send */
            endIRP = false;

            /* This function will load the next packet for this IRP into the
             * endpoint FIFO and then transmit it. */
            _DRV_USBFSV1_HOST_IRPTransmitFIFOLoad(usbID, irp, hostPipe);
        }
    }
    else if ( USB_HOST_PINTFLAG_TRCPT1_Msk == (usbID->HOST.HOST_PIPE[hostPipe].USB_PINTFLAG & USB_HOST_PINTFLAG_TRCPT1_Msk))
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: USB_HOST_PINTFLAG_TRCPT1");
    }        
    else if ( USB_HOST_PINTFLAG_TRCPT0_Msk == (usbID->HOST.HOST_PIPE[hostPipe].USB_PINTFLAG & USB_HOST_PINTFLAG_TRCPT0_Msk))
    {
        /* Clear Transfer Complete 0 Interrupt Flag */
        usbID->HOST.HOST_PIPE[hostPipe].USB_PINTFLAG = USB_HOST_PINTFLAG_TRCPT0_Msk;

        /* In case of low USB speed and with a high CPU frequency,
         * an ACK from host can be always running on USB line
         * then wait end of ACK on IN pipe */
        if (((usbID->HOST.USB_STATUS & USB_HOST_STATUS_SPEED_Msk)>>USB_HOST_STATUS_SPEED_Pos) == 0x1) /* 0x1: Low-speed mode */
        {    
            // while(!usbID->USBFS_FSTPIPIMR[hostPipe].PFREEZE);
            //while( (usbID->HOST.HOST_PIPE[hostPipe].USB_PINTFLAG & USB_HOST_PINTFLAG_TRCPT0) == USB_HOST_PINTFLAG_TRCPT0 );
        }  

        if( usbID->HOST.HOST_PIPE[hostPipe].USB_PCFG & USB_HOST_PCFG_PTOKEN(USB_HOST_PIPE_TOKEN_IN))
        {
            /* IN management */
            /* This means that data was received without errors. */
            count = _DRV_USBFSV1_HOST_IRPReceiveFIFOUnload(usbID, irp, hostPipe, &isDmaUsed);

            if(isDmaUsed == false)
            {
                if((count < pipe->endpointSize) ||
                        (irp->completedBytes >= irp->size))
                {
                    endIRP = true;
                    irp->status = USB_HOST_IRP_STATUS_COMPLETED;

                    if(irp->completedBytes < irp->size)
                    {
                        /* This means the device ended the transfer and and we
                         * have a short transfer */
                        irp->status = USB_HOST_IRP_STATUS_COMPLETED_SHORT;
                    }

                    /* Update the actual amount of data received */
                    irp->size = irp->completedBytes;
                }
                else
                {
                    /* This means we have more data to send We request another
                     * packet */

                    endIRP = false;
                    /* The host indicates it is able to receive data from the device by clearing the Bank 0/1 Ready bit in PSTATUS
                     * which means that the memory for the bank is available for new USB transfer */
                    usbID->HOST.HOST_PIPE[hostPipe].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_BK0RDY_Msk;
                    /* Unfreeze pipe */
                    usbID->HOST.HOST_PIPE[hostPipe].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_PFREEZE_Msk;
                }
            }
            else
            {
                /* DMA has been used. Do not end the IRP here.
                 * It will be done in DMA ISR handler */
                endIRP = false;
            }

        }
        else            
        {
            /* OUT packet sent */
            /* Freeze Pipe */
            usbID->HOST.HOST_PIPE[hostPipe].USB_PSTATUSSET = USB_HOST_PSTATUSSET_PFREEZE_Msk;
            endIRP = true;

            irp->status = USB_HOST_IRP_STATUS_COMPLETED;

            if(irp->completedBytes < irp->size)
            {
                /* This means the device ended the transfer and and we
                 * have a short transfer */
                irp->status = USB_HOST_IRP_STATUS_COMPLETED_SHORT;
            }

            /* Update the actual amount of data received */
            irp->size = irp->completedBytes;
        }
    }
    else
    {
	    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Unknown Pipe Interrupt");
    }
		

    if(endIRP)
    {
        /* This means we need to end the IRP */
        pipe->irpQueueHead = irp->next;
        if(irp->callback)
        {
            /* Invoke the call back*/
            irp->callback((USB_HOST_IRP *)irp);
        }
        irp = pipe->irpQueueHead;
        if((irp != NULL) && (!(irp->status == USB_HOST_IRP_STATUS_IN_PROGRESS)) && (endIRPOut != false) )
        {
            /* We do have another IRP to process. */
            irp->status = USB_HOST_IRP_STATUS_IN_PROGRESS;
            /* Clear Transmitted Setup Interrupt Flag */
            usbID->HOST.HOST_PIPE[hostPipe].USB_PINTFLAG = USB_HOST_PINTFLAG_TXSTP_Msk;
            _DRV_USBFSV1_HOST_IRPTransmitFIFOLoad(usbID, irp, hostPipe);
        }

        /* A IRP could have been submitted in the callback. If that is the
         * case and the IRP status would indicate that it already in
         * progress. If the IRP in the queue head is not in progress then we
         * should initiate the transaction */

        if((irp != NULL) && (!(irp->status == USB_HOST_IRP_STATUS_IN_PROGRESS)) && (endIRPOut == false) )
        {
            irp->status = USB_HOST_IRP_STATUS_IN_PROGRESS;  
            /* We do have another IRP to process. Request for an IN packet. */
            /* The host indicates it is able to receive data from the device by clearing the Bank 0/1 Ready bit in PSTATUS
             * which means that the memory for the bank is available for new USB transfer */
            usbID->HOST.HOST_PIPE[hostPipe].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_BK0RDY_Msk;
            /* Unfreeze pipe */
            usbID->HOST.HOST_PIPE[hostPipe].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_PFREEZE_Msk;
        }
    }
}/* end of _DRV_USBFSV1_HOST_NonControlTransferProcess() */

// *****************************************************************************
/* Function:
    void _DRV_USBFSV1_HOST_Tasks_ISR(DRV_USBFSV1_OBJ * pUSBDrvObj)

  Summary:
    Dynamic implementation of _DRV_USBFSV1_HOST_Tasks_ISR function.

  Description:
    This is the dynamic implementation of _DRV_USBFSV1_HOST_Tasks_ISR internal
    function. Function is an interrupt handler which does necessary processing
    based on the interrupt.

  Remarks:
    This is local function and should not be called directly by the client.

*/
/* Interrupt handler */
void _DRV_USBFSV1_HOST_Tasks_ISR(DRV_USBFSV1_OBJ * hDriver)
{
    uint32_t isr_status; 
    uint8_t intPipe;

    isr_status = hDriver->usbID->HOST.USB_INTFLAG;
    
    /* Device Disconnect */
    if ((USB_HOST_INTFLAG_DDISC_Msk == (isr_status & USB_HOST_INTFLAG_DDISC_Msk)) &&
        (USB_HOST_INTENSET_DDISC_Msk == (hDriver->usbID->HOST.USB_INTENSET & USB_HOST_INTENSET_DDISC_Msk)))
    {
        /* Manage Device Disconnection Interrupt 
         */
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, "\r\nDRV USBFSV1: Interrupt: DDISC");

        /* Stop reset signal, in case of disconnection during reset */
        hDriver->usbID->HOST.USB_CTRLB &= ~USB_HOST_CTRLB_BUSRESET_Msk;

        /* Disable SOF */
        hDriver->usbID->HOST.USB_CTRLB &=~USB_HOST_CTRLB_SOFE_Msk;
        /* Clear Device Disconnection Interrupt */
        hDriver->usbID->HOST.USB_INTFLAG = USB_HOST_INTFLAG_DDISC_Msk;
        /* Disable wakeup/resumes interrupts,
         * in case of disconnection during suspend mode */
        hDriver->usbID->HOST.USB_INTENCLR = USB_HOST_INTENCLR_WAKEUP_Msk 
                                          | USB_HOST_INTENCLR_DNRSM_Msk 
                                          | USB_HOST_INTENCLR_UPRSM_Msk;
    	/* Clear Wake-up Interrupt */
        hDriver->usbID->HOST.USB_INTFLAG = USB_HOST_INTFLAG_WAKEUP_Msk;
        /* Enable Host Wakeup Interrupt */
        hDriver->usbID->HOST.USB_INTENSET = USB_HOST_INTENSET_WAKEUP_Msk;

        if (hDriver->usbID->HOST.USB_INTFLAG & USB_HOST_INTENSET_DCONN_Msk) 
        {
            /* Both connection and disconnection interrupts are set
             * and to sort this out the connection flag is cleared
             * at cost of loss the connection interrupt
             * Clear Connection Interrupt */
            hDriver->usbID->HOST.USB_INTFLAG = USB_HOST_INTFLAG_DDISC_Msk;
        }
        hDriver->deviceAttached = false;
        if(hDriver->attachedDeviceObjHandle != USB_HOST_DEVICE_OBJ_HANDLE_INVALID)
        {
            /* Ask the host layer to de-enumerate this device. The de-enumeration
             * must be done in the interrupt context. */
            USB_HOST_DeviceDenumerate (hDriver->attachedDeviceObjHandle);
        }
        hDriver->attachedDeviceObjHandle = USB_HOST_DEVICE_OBJ_HANDLE_INVALID;        
    }
    else if((USB_HOST_INTFLAG_DCONN_Msk == (isr_status & USB_HOST_INTFLAG_DCONN_Msk)) 
         && (USB_HOST_INTENSET_DCONN_Msk == (hDriver->usbID->HOST.USB_INTENSET & USB_HOST_INTENSET_DCONN_Msk)))
    {
        /* Manage Device Connection Interrupt
         */
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, " DCONN");  
        /* This notifies the USB HOST that USB operations can be started. */
        hDriver->usbID->HOST.USB_CTRLB = USB_HOST_CTRLB_VBUSOK_Msk;
        /* Clear Connection Interrupt */
        hDriver->usbID->HOST.USB_INTFLAG = USB_HOST_INTFLAG_DCONN_Msk;
        /* Enable Disconnection Interrupt */
        hDriver->usbID->HOST.USB_INTENSET = USB_HOST_INTENSET_DDISC_Msk;
     	hDriver->deviceAttached = true;
    }
    /* If Wakeup interrupt is enabled and triggered and the usb is in disconnected state */
    else if (((USB_HOST_INTFLAG_WAKEUP_Msk  == (isr_status & USB_HOST_INTFLAG_WAKEUP_Msk )) 
          && (USB_HOST_INTENSET_WAKEUP_Msk == (hDriver->usbID->HOST.USB_INTENSET & USB_HOST_INTENSET_WAKEUP_Msk)))
          && (hDriver->deviceAttached == false))
    {
    	/* Manage Host Wakeup Interrupt. This interrupt is generated 
         * even if the clock is frozen
         */
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, " WAKEUP");
        /* Disable Wake-up Interrupt */
        hDriver->usbID->HOST.USB_INTENCLR = USB_HOST_INTENCLR_WAKEUP_Msk | USB_HOST_INTENCLR_UPRSM_Msk | USB_HOST_INTENCLR_DNRSM_Msk;
    }
    else if (((USB_HOST_INTFLAG_WAKEUP_Msk | USB_HOST_INTFLAG_UPRSM_Msk) 
                == ((USB_HOST_INTFLAG_WAKEUP_Msk | USB_HOST_INTFLAG_UPRSM_Msk) & isr_status)) 
          && (USB_HOST_INTENSET_WAKEUP_Msk == (hDriver->usbID->HOST.USB_INTENSET & USB_HOST_INTENSET_WAKEUP_Msk)))
    {  
        /* Wake up */

        /* This notifies the USB HOST that USB operations can be started. */
        hDriver->usbID->HOST.USB_CTRLB = USB_HOST_CTRLB_VBUSOK_Msk;
        /* Disable Wake-up Interrupt / resumes */
        hDriver->usbID->HOST.USB_INTENCLR = USB_HOST_INTENCLR_WAKEUP_Msk
                                          | USB_HOST_INTENCLR_UPRSM_Msk
                                          | USB_HOST_INTENCLR_DNRSM_Msk;
        /* Enable SOF */
        hDriver->usbID->HOST.USB_CTRLB |= USB_HOST_CTRLB_SOFE_Msk;
    }
    /* Bus reset sent */
    else if ((USB_HOST_INTFLAG_RST_Msk == (hDriver->usbID->HOST.USB_INTFLAG & USB_HOST_INTFLAG_RST_Msk)) &&
             (USB_HOST_INTENSET_RST_Msk == (hDriver->usbID->HOST.USB_INTENSET & USB_HOST_INTENSET_RST_Msk)))
    {
        /* Manage USB Reset Sent Interrupt
         */
        SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, " Bus reset sent");
        /* Clear USB Reset Sent Interrupt */
        hDriver->usbID->HOST.USB_INTFLAG = USB_HOST_INTFLAG_RST_Msk;
        /* Disable USB Reset Sent Interrupt */
        hDriver->usbID->HOST.USB_INTENCLR = USB_HOST_INTENCLR_RST_Msk;
        /* Clear the flag */
        hDriver->isResetting = false;
        /* Now that reset is complete, we can find out the
         * speed of the attached device. */
        switch ((hDriver->usbID->HOST.USB_STATUS & USB_HOST_STATUS_SPEED_Msk) >> USB_HOST_STATUS_SPEED_Pos)
        {
            case 0x0:
                hDriver->deviceSpeed = USB_SPEED_FULL;
                SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, " USB_SPEED_FULL");
            break;
            case 0x1:
                hDriver->deviceSpeed = USB_SPEED_LOW;
                SYS_DEBUG_MESSAGE(SYS_ERROR_DEBUG, " USB_SPEED_LOW");
            break;

            default:
                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Unknown Speed at Reset");
            break;
        }
    }
    /* Just ignore unexpected interrupts */
    else if( isr_status != 0 )
    {
        /* Wake Up Interrupt Flag */
        hDriver->usbID->HOST.USB_INTFLAG = USB_HOST_INTFLAG_WAKEUP_Msk;
        /* Host Start-of-Frame Interrupt Flag */
        hDriver->usbID->HOST.USB_INTFLAG = USB_HOST_INTFLAG_HSOF_Msk;
    }

    if(hDriver->deviceAttached == true)
    {
        /* Manage pipe interrupts */
        for(intPipe = 0; intPipe < HOST_PIPE_NUMBER; intPipe++)
        {
            if((hDriver->usbID->HOST.USB_PINTSMRY & (0x01 << intPipe)) == 0x0000)
            {
                continue;
            }
            if (hDriver->usbID->HOST.HOST_PIPE[intPipe].USB_PINTFLAG & USB_HOST_PINTFLAG_PERR_Msk)
            {
                SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "\033[31m");
                SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "\r\nPERR: pipe number: %d, ", intPipe);
                SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "Number of error(s): %d, ", (usb_pipe_table[intPipe].HOST_DESC_BANK[0].USB_STATUS_PIPE & USB_HOST_STATUS_PIPE_ERCNT_Msk)>>USB_HOST_STATUS_PIPE_ERCNT_Pos);
                if( usb_pipe_table[intPipe].HOST_DESC_BANK[0].USB_STATUS_PIPE & USB_HOST_STATUS_PIPE_CRC16ER_Msk )
                {
                   SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "A CRC 16 error has been detected.");
                }
                if( usb_pipe_table[intPipe].HOST_DESC_BANK[0].USB_STATUS_PIPE & USB_HOST_STATUS_PIPE_TOUTER_Msk )
                {
                   SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "A Time Out error has been detected.");
                }
                if( usb_pipe_table[intPipe].HOST_DESC_BANK[0].USB_STATUS_PIPE & USB_HOST_STATUS_PIPE_PIDER_Msk )
                {
                   SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "A PID error has been detected.");
                }
                if( usb_pipe_table[intPipe].HOST_DESC_BANK[0].USB_STATUS_PIPE & USB_HOST_STATUS_PIPE_DAPIDER_Msk )
                {
                   SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "A Data PID error has been detected.");
                }
                if( usb_pipe_table[intPipe].HOST_DESC_BANK[0].USB_STATUS_PIPE & USB_HOST_STATUS_PIPE_DTGLER_Msk )
                {
                   SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "Data Toggle Error detected.");
                }
                SYS_DEBUG_MESSAGE(SYS_ERROR_ERROR, "\033[39m");
                hDriver->usbID->HOST.HOST_PIPE[intPipe].USB_PINTFLAG = USB_HOST_PINTFLAG_PERR_Msk;
            }
            else if (hDriver->usbID->HOST.HOST_PIPE[intPipe].USB_PINTFLAG & USB_HOST_PINTFLAG_TRFAIL_Msk)
            {
                /* Only for isochronous */
                if( (hDriver->usbID->HOST.HOST_PIPE[intPipe].USB_PCFG & USB_HOST_PCFG_PTOKEN_Msk) == USB_HOST_PCFG_PTYPE(USB_HOST_PIPE_TYPE_ISO))
                {
                   SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "\r\n\033[31mTRFAIL pipe [%d]\033[39m", intPipe);
                }
                hDriver->usbID->HOST.HOST_PIPE[intPipe].USB_PINTFLAG = USB_HOST_PINTFLAG_TRFAIL_Msk;
            }
            else if (intPipe == 0)
            {
                /* Manage control pipe */
                _DRV_USBFSV1_HOST_ControlTransferProcess(hDriver);
            }
            else
            {
                /* Manage Non-control pipe */
                _DRV_USBFSV1_HOST_NonControlTransferProcess(hDriver, intPipe);
            }            
        }
    }
}/* end of _DRV_USBFSV1_HOST_Tasks_ISR() */

void DRV_USBFSV1_HOST_StartOfFrameControl(DRV_HANDLE client, bool control)
{
    /* At the point this function does not do any thing.
     * The Start of frame signaling in this HCD is controlled
     * automatically by the module. */
}/* end of DRV_USBFSV1_HOST_StartOfFrameControl() */

USB_SPEED DRV_USBFSV1_HOST_DeviceCurrentSpeedGet(DRV_HANDLE client)
{
    /* This function returns the current device speed */
    DRV_USBFSV1_OBJ * hDriver;

    if((client == DRV_HANDLE_INVALID) || (((DRV_USBFSV1_OBJ *)client) == NULL))
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Invalid client");
    }

    hDriver = (DRV_USBFSV1_OBJ *)client;
    return(hDriver->deviceSpeed);

}/* end of DRV_USBFSV1_HOST_DeviceCurrentSpeedGet() */

// ****************************************************************************
/* Function:
    bool DRV_USBFSV1_HOST_EventDisable
    (
        DRV_HANDLE handle
    );
    
  Summary:
    Disables host mode events.
    
  Description:
    This function disables the host mode events. This function is called by the
    Host Layer when it wants to execute code atomically. 
    
  Remarks:
    Refer to drv_usbfs.h for usage information.
*/

bool DRV_USBFSV1_HOST_EventsDisable
(
    DRV_HANDLE handle
)
{
    DRV_USBFSV1_OBJ * pUSBDrvObj;
    bool result = false;

    if((DRV_HANDLE_INVALID != handle) && (0 != handle))
    {
        pUSBDrvObj = (DRV_USBFSV1_OBJ *)(handle);
        result = SYS_INT_SourceDisable((INT_SOURCE)pUSBDrvObj->interruptSource);
    }

    return(result);
}

// ****************************************************************************
/* Function:
    void DRV_USBFSV1_HOST_EventsDisable
    (
        DRV_HANDLE handle
        bool eventRestoreContext
    );
    
  Summary:
    Restores the events to the specified the original value.
    
  Description:
    This function will restore the enable disable state of the events.
    eventRestoreContext should be equal to the value returned by the
    DRV_USBFSV1_HOST_EventsDisable() function.
    
  Remarks:
    Refer to drv_usbfs.h for usage information.
*/
void DRV_USBFSV1_HOST_EventsEnable
(
    DRV_HANDLE handle, 
    bool eventContext
)
{
    DRV_USBFSV1_OBJ * pUSBDrvObj;
   
    if((DRV_HANDLE_INVALID != handle) && (0 != handle))
    {
        pUSBDrvObj = (DRV_USBFSV1_OBJ *)(handle);
        if(false == eventContext)
        {
            SYS_INT_SourceDisable((INT_SOURCE)pUSBDrvObj->interruptSource);
        }
        else
        {
            SYS_INT_SourceEnable((INT_SOURCE)pUSBDrvObj->interruptSource);
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Root Hub Driver Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void DRV_USBFSV1_HOST_ROOT_HUB_OperationEnable(DRV_HANDLE handle, bool enable)

  Summary:
    Dynamic implementation of DRV_USBFSV1_HOST_ROOT_HUB_OperationEnable() function.

  Description:
    This function enables the Root Hub Operation. When enabled the root hub will
    supply power to the port thus enabling device attach detection.

  Remarks:
    Refer to drv_usbfs.h for usage information.
*/
void DRV_USBFSV1_HOST_ROOT_HUB_OperationEnable(DRV_HANDLE handle, bool enable)
{
    DRV_USBFSV1_OBJ * pUSBDrvObj = (DRV_USBFSV1_OBJ *)handle;
    usb_registers_t * usbID = pUSBDrvObj->usbID;

    /* Check if the handle is valid or opened */
    if((handle == DRV_HANDLE_INVALID) || (!(pUSBDrvObj->isOpened)))
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Bad Client or client closed");
    }
    else
    {
        if(false == enable)
        {	
             /* If the root hub operation is disable, we disable detach and
             * attached event and switch off the port power. */
	
            SYS_INT_SourceStatusClear((INT_SOURCE)pUSBDrvObj->interruptSource);
            pUSBDrvObj->operationEnabled = false;
			
			/* Disable USB port by turning off VBUS */ 
			if(pUSBDrvObj->rootHubInfo.portPowerEnable != NULL)
            {   
               pUSBDrvObj->rootHubInfo.portPowerEnable(0 /* Port 0 */, false); 
            }

        }
        else
        {
			/* Enable USB port */ 
			if(pUSBDrvObj->rootHubInfo.portPowerEnable != NULL)
			{
				pUSBDrvObj->rootHubInfo.portPowerEnable(0 /* Port 0 */, true);
			}
			
			/* The USB Global interrupt and USB module is already enabled at
             * this point. We enable the attach interrupt to detect attach
             */
            pUSBDrvObj->operationEnabled = true;
            /* Enable Device Connection Interrupt */
            usbID->HOST.USB_INTENSET = USB_HOST_INTENSET_DDISC_Msk;
            usbID->HOST.USB_INTFLAG = USB_HOST_INTFLAG_DCONN_Msk | USB_HOST_INTFLAG_WAKEUP_Msk | USB_HOST_INTFLAG_DNRSM_Msk | USB_HOST_INTFLAG_UPRSM_Msk;
            usbID->HOST.USB_INTENSET = USB_HOST_INTENSET_DCONN_Msk | USB_HOST_INTENSET_WAKEUP_Msk | USB_HOST_INTENSET_DNRSM_Msk | USB_HOST_INTENSET_UPRSM_Msk;
            /* Requests VBus activation */
            usbID->HOST.USB_CTRLB |= USB_HOST_CTRLB_VBUSOK_Msk;
        }
    }
}

// *****************************************************************************
/* Function:
    bool DRV_USBFSV1_HOST_ROOT_HUB_OperationIsEnabled(DRV_HANDLE handle)

  Summary:
    Dynamic implementation of DRV_USBFSV1_HOST_OperationIsEnabled function.

  Description:
    Function returns true if the root hub enable operation has completed.

  Remarks:
    Refer to drv_usbfs.h for usage information.

*/
bool DRV_USBFSV1_HOST_ROOT_HUB_OperationIsEnabled(DRV_HANDLE handle)
{
    DRV_USBFSV1_OBJ * hDriver;
    if((handle == DRV_HANDLE_INVALID) || (((DRV_USBFSV1_OBJ *)handle) == NULL))
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Invalid client");
    }
    hDriver = (DRV_USBFSV1_OBJ *)handle;
    return(hDriver->operationEnabled);

}/* end of DRV_USBFSV1_HOST_OperationIsEnabled() */

// ****************************************************************************
/* Function:
    void DRV_USBFSV1_HOST_ROOT_HUB_Initialize
    (
        DRV_HANDLE handle,
        USB_HOST_DEVICE_OBJ_HANDLE usbHostDeviceInfo,
    )

  Summary:
    This function instantiates the root hub driver.

  Description:
    This function initializes the root hub driver. It is called by the host
    layer at the time of processing root hub devices. The host layer assigns a
    USB_HOST_DEVICE_OBJ_HANDLE reference to this root hub driver. This
    identifies the relationship between the root hub and the host layer.

  Remarks:
    None.
*/

void DRV_USBFSV1_HOST_ROOT_HUB_Initialize
(
    DRV_HANDLE handle,
    USB_HOST_DEVICE_OBJ_HANDLE usbHostDeviceInfo
)
{
    DRV_USBFSV1_OBJ * pUSBDrvObj = (DRV_USBFSV1_OBJ *)handle;

    memset((uint8_t *)&usb_pipe_table[0], 0, sizeof(usb_pipe_table[0]));

    if(DRV_HANDLE_INVALID == handle)
    {
        /* Driver handle is not valid */
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Bad Client or client closed");
    }
    else if(!(pUSBDrvObj->isOpened))
    {
        /* Driver has not been opened. Handle could be stale */
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Bad Client or client closed");
    }
    else
    {
        pUSBDrvObj->usbHostDeviceInfo = usbHostDeviceInfo;
    }
}

// ****************************************************************************
/* Function:
    uint8_t DRV_USBFSV1_HOST_ROOT_HUB_PortNumbersGet(DRV_HANDLE handle);

  Summary:
    Returns the number of ports this root hub contains.

  Description:
    This function returns the number of ports that this root hub contains.

  Remarks:
    None.
*/

uint8_t DRV_USBFSV1_HOST_ROOT_HUB_PortNumbersGet(DRV_HANDLE handle)
{
    DRV_USBFSV1_OBJ * pUSBDrvObj = (DRV_USBFSV1_OBJ *)handle;
    uint8_t result = 0;

    if(DRV_HANDLE_INVALID == handle)
    {
        /* Driver handle is not valid */
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Bad Client or client closed");
    }
    else if(!(pUSBDrvObj->isOpened))
    {
        /* Driver has not been opened. Handle could be stale */
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Bad Client or client closed");
    }
    else
    {
        result = 1;
    }

    return(result);
}

// ****************************************************************************
/* Function:
    uint32_t DRV_USBFSV1_HOST_ROOT_HUB_MaximumCurrentGet(DRV_HANDLE);

  Summary:
    Returns the maximum amount of current that this root hub can provide on the
    bus.

  Description:
    This function returns the maximum amount of current that this root hub can
    provide on the bus.

  Remarks:
    Refer to drv_usbfs.h for usage information.
*/

uint32_t DRV_USBFSV1_HOST_ROOT_HUB_MaximumCurrentGet(DRV_HANDLE handle)
{
    DRV_USBFSV1_OBJ * pUSBDrvObj = (DRV_USBFSV1_OBJ *)handle;
    uint32_t result = 0;

    if(DRV_HANDLE_INVALID == handle)
    {
        /* Driver handle is not valid */
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Bad Client or client closed");
    }
    else if(!(pUSBDrvObj->isOpened))
    {
        /* Driver has not been opened. Handle could be stale */
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Bad Client or client closed");
    }
    else
    {
        result = pUSBDrvObj->rootHubInfo.rootHubAvailableCurrent;
    }

    return(result);
}

// ****************************************************************************
/* Function:
    USB_SPEED DRV_USBFSV1_HOST_ROOT_HUB_BusSpeedGet(DRV_HANDLE handle);

  Summary:
    Returns the speed at which the bus to which this root hub is connected is
    operating.

  Description:
    This function returns the speed at which the bus to which this root hub is
    connected is operating.

 Remarks:
    None.
*/

USB_SPEED DRV_USBFSV1_HOST_ROOT_HUB_BusSpeedGet(DRV_HANDLE handle)
{
    DRV_USBFSV1_OBJ * pUSBDrvObj = (DRV_USBFSV1_OBJ *)handle;
    USB_SPEED speed = USB_SPEED_ERROR;

    if(DRV_HANDLE_INVALID == handle)
    {
        /* Driver handle is not valid */
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Bad Client or client closed");

    }
    else if(!(pUSBDrvObj->isOpened))
    {
        /* Driver has not been opened. Handle could be stale */
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Bad Client or client closed");
    }
    else
    {
        /* Return the bus speed. This is speed at which the root hub is
         * operating. */
        speed = pUSBDrvObj->deviceSpeed;
    }

    return(speed);
}

// ****************************************************************************
/* Function:
    void DRV_USBFSV1_ROOT_HUB_PortResume(DRV_HANDLE handle, uint8_t port );

  Summary:
    Resumes the specified root hub port.

  Description:
    This function resumes the root hub. The resume duration is defined by
    DRV_USBFSV1_ROOT_HUB_RESUME_DURATION. The status of the resume signaling can
    be checked using the DRV_USBFSV1_ROOT_HUB_PortResumeIsComplete() function.

  Remarks:
    The root hub on this particular hardware only contains one port - port 0.
*/

USB_ERROR DRV_USBFSV1_HOST_ROOT_HUB_PortResume(DRV_HANDLE handle, uint8_t port)
{
    /* The functionality is yet to be implemented. */
    return(USB_ERROR_NONE);
}

// ****************************************************************************
/* Function:
    void DRV_USBFSV1_ROOT_HUB_PortSuspend(DRV_HANDLE handle, uint8_t port );

  Summary:
    Suspends the specified root hub port.

  Description:
    This function suspends the root hub port.

  Remarks:
    The root hub on this particular hardware only contains one port - port 0.
*/

USB_ERROR DRV_USBFSV1_HOST_ROOT_HUB_PortSuspend(DRV_HANDLE handle, uint8_t port)
{
    /* The functionality is yet to be implemented. */
    return (USB_ERROR_NONE);
}

// ****************************************************************************
/* Function:
    void DRV_USBFSV1_ROOT_HUB_PortResetIsComplete
    (
        DRV_HANDLE handle,
        uint8_t port
    );

  Summary:
    Returns true if the root hub has completed the port reset operation.

  Description:
    This function returns true if the port reset operation has completed. It
    should be called after the DRV_USB_HOST_ROOT_HUB_PortReset() function to
    check if the reset operation has completed.

  Remarks:
    Refer to drv_usbfs.h for usage information.
 */

bool DRV_USBFSV1_HOST_ROOT_HUB_PortResetIsComplete
(
    DRV_HANDLE handle,
    uint8_t port
)
{
    DRV_USBFSV1_OBJ * pUSBDrvObj = (DRV_USBFSV1_OBJ *)handle;
    bool result = true;

    if(DRV_HANDLE_INVALID == handle)
    {
        /* Driver handle is not valid */
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Bad Client or client closed");
    }
    else if(!(pUSBDrvObj->isOpened))
    {
        /* Driver has not been opened. Handle could be stale */
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Bad Client or client closed");
    }
    else
    {
        /* Return false if the driver is still resetting*/
        result = (pUSBDrvObj->isResetting) ? false : true;
    }

    return(result);
}

// ****************************************************************************
/* Function:
    void DRV_USBFSV1_ROOT_HUB_PortReset(DRV_HANDLE handle, uint8_t port );
    
  Summary:
    Resets the specified root hub port.
    
  Description:
    This function resets the root hub port. The reset duration is defined by
    DRV_USBFSV1_ROOT_HUB_RESET_DURATION. The status of the reset signaling can be
    checked using the DRV_USBFSV1_ROOT_HUB_PortResetIsComplete() function.
    
  Remarks:
    Refer to drv_usbfs.h for usage information.
*/

USB_ERROR DRV_USBFSV1_HOST_ROOT_HUB_PortReset(DRV_HANDLE handle, uint8_t port)
{
    DRV_USBFSV1_OBJ * pUSBDrvObj = (DRV_USBFSV1_OBJ *)handle;
    USB_ERROR result = USB_ERROR_NONE;

    if(DRV_HANDLE_INVALID == handle)
    {
        /* Driver handle is not valid */
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Bad Client or client closed");
        result = USB_ERROR_PARAMETER_INVALID;
    }
    else if(!(pUSBDrvObj->isOpened))
    {
        /* Driver has not been opened. Handle could be stale */
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Bad Client or client closed");
        result = USB_ERROR_PARAMETER_INVALID;
    }
    else if(pUSBDrvObj->isResetting)
    {
        /* This means a reset is already in progress. Lets not do anything. */
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Reset already in progress");
    }
    else
    {
        /* Start the reset signal. Set the driver flag to indicate the reset
         * signal is in progress. Start generating the reset signal.
         */
        pUSBDrvObj->isResetting = true;
        pUSBDrvObj->resetState = DRV_USBFSV1_HOST_RESET_STATE_START;
        /* Enable Reset sent interrupt */
        pUSBDrvObj->usbID->HOST.USB_INTENSET = USB_HOST_INTENSET_RST_Msk;
        /* Start Reset */
        pUSBDrvObj->usbID->HOST.USB_CTRLB |= USB_HOST_CTRLB_BUSRESET_Msk;
    }

    return(result);
}    

// ****************************************************************************
/* Function:
    USB_SPEED DRV_USBFSV1_HOST_ROOT_HUB_PortSpeedGet
    (
        DRV_HANDLE handle,
        uint8_t port
    );

  Summary:
    Returns the speed of at which the port is operating.

  Description:
    This function returns the speed at which the port is operating.

  Remarks:
    Refer to drv_usbfs.h for usage information.
*/

USB_SPEED DRV_USBFSV1_HOST_ROOT_HUB_PortSpeedGet(DRV_HANDLE handle, uint8_t port)
{
    DRV_USBFSV1_OBJ * pUSBDrvObj = (DRV_USBFSV1_OBJ *)handle;
    USB_SPEED speed = USB_SPEED_ERROR;

   if(DRV_HANDLE_INVALID == handle)
    {
        /* Driver handle is not valid */
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Bad Client or client closed");
    }
    else if(!(pUSBDrvObj->isOpened))
    {
        /* Driver has not been opened. Handle could be stale */
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBFSV1: Bad Client or client closed");
    }
    else
    {
        /* The driver will not check if a device is connected. It is assumed
         * that the client has issued a port reset before calling this function
         */
        speed = pUSBDrvObj->deviceSpeed;
    }

    return(speed);
}

// ****************************************************************************
/* Function:
    void DRV_USBFSV1_HOST_EndpointToggleClear
    (
        DRV_HANDLE client,
        USB_ENDPOINT endpointAndDirection
    )

  Summary:
    Facilitates in resetting of endpoint data toggle to 0 for Non Control
    endpoints.

  Description:
    Facilitates in resetting of endpoint data toggle to 0 for Non Control
    endpoints.

  Remarks:
    Refer to drv_usbfsv1.h for usage information.
*/

void DRV_USBFSV1_HOST_EndpointToggleClear
(
    DRV_HANDLE client,
    USB_ENDPOINT endpointAndDirection
)
{
    /* Start of local variables */
    DRV_USBFSV1_OBJ * hDriver;
 
    uint8_t pipeCount = 0;
    hDriver = (DRV_USBFSV1_OBJ *)client;
   
    /* End of local variables */

    if((client == DRV_HANDLE_INVALID) || (((DRV_USBFSV1_OBJ *)client) == NULL))
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "\r\nDRV USBHSV1: Invalid client handle");
    }
    else
    {
        for(pipeCount = 0; pipeCount < DRV_USBFSV1_HOST_PIPES_NUMBER; pipeCount++)
        {
            /* Check for free pipe object */
            if((client == gDrvUSBHostPipeObj[pipeCount].hClient) && (true == gDrvUSBHostPipeObj[pipeCount].inUse) &&
                    (gDrvUSBHostPipeObj[pipeCount].endpointAndDirection == endpointAndDirection))
                
            {
                /* Found the pipe for this endpoint and direction */
                /* Reset DATA Toggle */
                hDriver->usbID->HOST.HOST_PIPE[pipeCount].USB_PSTATUSCLR = USB_HOST_PSTATUSCLR_DTGL_Msk;
               
                break;
            }
        }
        if(DRV_USBFSV1_HOST_PIPES_NUMBER == pipeCount)
        {
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "Invalid endpoint");
        }
    }
} /* End of DRV_USBFSV1_HOST_EndpointToggleClear() */