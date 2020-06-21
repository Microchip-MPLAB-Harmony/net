/*******************************************************************************
  USB Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_usb_local.h

  Summary:
    USB driver local declarations and definitions

  Description:
    This file contains the USB driver's local declarations and definitions.
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

#ifndef _DRV_USBFSV1_LOCAL_H
#define _DRV_USBFSV1_LOCAL_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "driver/usb/drv_usb_external_dependencies.h"
#include "driver/usb/usbfsv1/drv_usbfsv1.h"
#include "driver/usb/usbfsv1/src/drv_usbfsv1_variant_mapping.h"
#include "osal/osal.h"


#define COMPILER_WORD_ALIGNED                               __attribute__((__aligned__(4)))

#define DRV_USBFSV1_HOST_MAXIMUM_ENDPOINTS_NUMBER           10
#define _DRV_USBFSV1_HOST_IRP_PER_FRAME_NUMBER              5
#define _DRV_USBFSV1_SW_EP_NUMBER                           _DRV_USBFSV1_HOST_IRP_PER_FRAME_NUMBER
#define DRV_USBFSV1_POST_DETACH_DELAY                       2000

/* Number of Endpoints used */
#define DRV_USBFSV1_ENDPOINT_NUMBER_MASK                    0x0F
#define DRV_USBFSV1_ENDPOINT_DIRECTION_MASK                 0x80

#define DRV_USBFSV1_AUTO_ZLP_ENABLE                         false

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

/***************************************************
 * This is an intermediate flag that is set by
 * the driver to indicate that a ZLP should be sent
 ***************************************************/
#define USB_DEVICE_IRP_FLAG_SEND_ZLP 0x80

/***************************************************
 * This object is used by the driver as IRP place
 * holder along with queuing feature.
 ***************************************************/
typedef struct _USB_DEVICE_IRP_LOCAL
{
    /* Pointer to the data buffer */
    void * data;

    /* Size of the data buffer */
    unsigned int size;

    /* Status of the IRP */
    USB_DEVICE_IRP_STATUS status;

    /* IRP Callback. If this is NULL,
     * then there is no callback generated */
    void (*callback)(struct _USB_DEVICE_IRP * irp);

    /* Request specific flags */
    USB_DEVICE_IRP_FLAG flags;

    /* User data */
    uintptr_t userData;

    /* This points to the next IRP in the queue */
    struct _USB_DEVICE_IRP_LOCAL * next;

    /* This points to the previous IRP in the queue */
    struct _USB_DEVICE_IRP_LOCAL * previous;

    /* Pending bytes in the IRP */
    uint32_t nPendingBytes;

}
USB_DEVICE_IRP_LOCAL;

/************************************************
 * Endpoint state enumeration.
 ************************************************/

typedef enum
{
    DRV_USBFSV1_DEVICE_ENDPOINT_STATE_ENABLED = 0x1,
    DRV_USBFSV1_DEVICE_ENDPOINT_STATE_STALLED = 0x2
}
DRV_USBFSV1_DEVICE_ENDPOINT_STATE;

/************************************************
 * Endpoint data structure. This data structure
 * holds the IRP queue and other flags associated
 * with functioning of the endpoint.
 ************************************************/

typedef struct
{
    /* This is the IRP queue for
     * the endpoint */
    USB_DEVICE_IRP_LOCAL * irpQueue;

    /* Max packet size for the endpoint */
    uint16_t maxPacketSize;

    /* Endpoint type */
    USB_TRANSFER_TYPE endpointType;

    /* Endpoint state bitmap */
    DRV_USBFSV1_DEVICE_ENDPOINT_STATE endpointState;

}
DRV_USBFSV1_DEVICE_ENDPOINT_OBJ;

/*********************************************
 * These IRP states are used internally by the
 * HCD to track completion of a host IRP. This
 * is not the same as the public IRP status
 *********************************************/
typedef enum
{
    DRV_USBFSV1_HOST_IRP_STATE_SETUP_STAGE,
    DRV_USBFSV1_HOST_IRP_STATE_SETUP_TOKEN_SENT,
    DRV_USBFSV1_HOST_IRP_STATE_DATA_STAGE,
    DRV_USBFSV1_HOST_IRP_STATE_DATA_STAGE_SENT,
    DRV_USBFSV1_HOST_IRP_STATE_HANDSHAKE,
    DRV_USBFSV1_HOST_IRP_STATE_HANDSHAKE_SENT,
    DRV_USBFSV1_HOST_IRP_STATE_COMPLETE,
    DRV_USBFSV1_HOST_IRP_STATE_ABORTED,
    DRV_USBFSV1_HOST_IRP_STATE_PROCESSING
}
DRV_USBFSV1_HOST_IRP_STATE;

/*********************************************
 * This is the local USB Host IRP object
 ********************************************/
typedef struct _USB_HOST_IRP_LOCAL
{
    /* Points to the 8 byte setup command
     * packet in case this is a IRP is
     * scheduled on a CONTROL pipe. Should
     * be NULL otherwise */
    void * setup;

    /* Pointer to data buffer */
    void * data;

    /* Size of the data buffer */
    unsigned int size;

    /* Status of the IRP */
    USB_HOST_IRP_STATUS status;

    /* Request specific flags */
    USB_HOST_IRP_FLAG flags;

    /* User data */
    uintptr_t userData;

    /* Pointer to function to be called
     * when IRP is terminated. Can be
     * NULL, in which case the function
     * will not be called. */
    void (*callback)(struct _USB_HOST_IRP * irp);

    /****************************************
     * These members of the IRP should not be
     * modified by client
     ****************************************/

    uint32_t tempSize;
    DRV_USBFSV1_HOST_IRP_STATE tempState;
    uint32_t completedBytes;
    struct _USB_HOST_IRP_LOCAL * next;
    struct _USB_HOST_IRP_LOCAL * previous;
    DRV_USB_HOST_PIPE_HANDLE  pipe;

}
USB_HOST_IRP_LOCAL;

/************************************************
 * This is the Host Pipe Object.
 ************************************************/
typedef struct _DRV_USBFSV1_HOST_PIPE_OBJ
{
    /* This pipe object is in use */
    bool inUse;

    /* Client that owns this pipe */
    DRV_HANDLE hClient;

    /* USB endpoint and direction */
    USB_ENDPOINT endpointAndDirection;

    /* USB Endpoint type */
    USB_TRANSFER_TYPE pipeType;

    /* The IRP queue on this pipe */
    USB_HOST_IRP_LOCAL * irpQueueHead;

    /* The data toggle for this pipe*/
    bool dataToggle;

    /* The NAK counter for the IRP
     * being served on the pipe */

    uint32_t nakCounter;

    /* Pipe endpoint size*/

    unsigned int endpointSize;

    /* The next pipe */
    struct _DRV_USBFSV1_HOST_PIPE_OBJ * next;
    struct _DRV_USBFSV1_HOST_PIPE_OBJ * previous;
    /* Interval in case this
     * is a Isochronous or
     * an interrupt pipe */
    uint8_t bInterval;

    /* The device address */
    uint8_t deviceAddress;

    /* Interval counter */

    uint8_t intervalCounter;

    /* Bandwidth */
    uint8_t bwPerTransaction;

    /* Pipe Speed */
    USB_SPEED speed;

    /* Hub Address */
    uint8_t hubAddress;

    /* Hub Port*/
    uint8_t hubPort;

    /* Host Pipe allocated*/
    uint8_t hostPipeN;
}
DRV_USBFSV1_HOST_PIPE_OBJ;

/***********************************************
 * Possible states for the 1 millisecond timer
 * interrupt task.
 ***********************************************/
typedef enum
{
    /* Nothing to be done */
    DRV_USBFSV1_ONE_MILLISECOND_TASK_STATE_IDLE,

    /* Task is attach debouncing */
    DRV_USBFSV1_ONE_MILLISECOND_TASK_STATE_ATTACH_DEBOUNCING,

    /* Task is detach debouncing */
    DRV_USBFSV1_ONE_MILLISECOND_TASK_STATE_POST_DETACH_DELAY,

    /* Providing reset delay */
    DRV_USBFSV1_ONE_MILLISECOND_TASK_STATE_RESETTING_DELAY

} DRV_USBFSV1_ONE_MILLISECOND_STATE_TASK_STATE;

/************************************************
 * This is the Host SW EP Object.
 ************************************************/
typedef struct _DRV_USBFSV1_HOST_SW_EP
{
    bool tobeDone;
    USB_TRANSFER_TYPE transferType;
    USB_HOST_IRP_LOCAL * pIRP;
}
DRV_USBFSV1_HOST_SW_EP;

/*********************************************
 * Host Transfer Group. This data structures
 * contains all pipes belonging to one transfer
 * type.
 *********************************************/

typedef struct _DRV_USBFSV1_HOST_TRANSFER_GROUP
{
    /* The first pipe in this transfer
     * group */
    DRV_USBFSV1_HOST_PIPE_OBJ * pipe;

    /* The current pipe being serviced
     * in this transfer group */
    DRV_USBFSV1_HOST_PIPE_OBJ * currentPipe;

    /* The current IRP being serviced
     * in the pipe */
    void * currentIRP;

    /* Total number of pipes in this
     * transfer group */
    int nPipes;
}
DRV_USBFSV1_HOST_TRANSFER_GROUP;

/********************************************
 * This enumeration list the possible status
 * value of a token once it has completed.
 ********************************************/

typedef enum
{
    USB_TRANSACTION_ACK             = 0x2,
    USB_TRANSACTION_NAK             = 0xA,
    USB_TRANSACTION_STALL           = 0xE,
    USB_TRANSACTION_DATA0           = 0x3,
    USB_TRANSACTION_DATA1           = 0xB,
    USB_TRANSACTION_BUS_TIME_OUT    = 0x0,
    USB_TRANSACTION_DATA_ERROR      = 0xF

}
DRV_USBFSV1_TRANSACTION_RESULT;

/***********************************************
 * USB Driver flags. Binary flags needed to
 * track different states of the USB driver.
 ***********************************************/
typedef enum
{

    /* Driver Host Mode operation has been enabled */
    DRV_USBFSV1_FLAG_HOST_MODE_ENABLED = /*DOM-IGNORE-BEGIN*/0x10/*DOM-IGNORE-END*/,


} DRV_USBFSV1_FLAGS;

/**************************************
 * Root Hub parameters
 **************************************/

typedef struct
{
    /* Pointer to the port over current detect function */
    DRV_USBFSV1_ROOT_HUB_PORT_OVER_CURRENT_DETECT portOverCurrentDetect;

    /* Pointer to the port indication function */
    DRV_USBFSV1_ROOT_HUB_PORT_INDICATION portIndication;

    /* Pointer to the port enable function */
    DRV_USBFSV1_ROOT_HUB_PORT_POWER_ENABLE portPowerEnable;

    /* Total current available */
    uint32_t rootHubAvailableCurrent;
}
DRV_USBFSV1_HOST_ROOT_HUB_INFO;

/**********************************************
 * Host Endpoint Object.
 *********************************************/

typedef struct
{
    /* Indicates this endpoint is in use */
    bool inUse;
    DRV_USBFSV1_HOST_PIPE_OBJ * pipe;

}_DRV_USBFSV1_HOST_ENDPOINT;


typedef struct
{
    /* A combination of two structures for
     * each direction. */

    _DRV_USBFSV1_HOST_ENDPOINT endpoint;

}DRV_USBFSV1_HOST_ENDPOINT_OBJ;


/*********************************************
 * Host Mode Device Attach Detach State
 ********************************************/
typedef enum
{
    /* No device is attached */
    DRV_USBFSV1_HOST_ATTACH_STATE_CHECK_FOR_DEVICE_ATTACH = 0,

    /* Waiting for debounce delay */
    DRV_USBFSV1_HOST_ATTACH_STATE_DETECTED,

    /* Debouncing is complete. Device is attached */
    DRV_USBFSV1_HOST_ATTACH_STATE_READY,

} DRV_USBFSV1_HOST_ATTACH_STATE;

/*********************************************
 * Host Mode Device Reset state
 *********************************************/
typedef enum
{
    /* No Reset in progress */
    DRV_USBFSV1_HOST_RESET_STATE_NO_RESET = 0,

    /* Start the reset signalling */
    DRV_USBFSV1_HOST_RESET_STATE_START,

    /* Check if reset duration is done and stop reset */
    DRV_USBFSV1_HOST_RESET_STATE_WAIT_FOR_COMPLETE,

    DRV_USBFSV1_HOST_RESET_STATE_COMPLETE,
    DRV_USBFSV1_HOST_RESET_STATE_COMPLETE2

} DRV_USBFSV1_HOST_RESET_STATE;



typedef enum
{
    DRV_USBFSV1_DEVICE_EP0_STATE_EXPECTING_SETUP_FROM_HOST,
    DRV_USBFSV1_DEVICE_EP0_STATE_WAITING_FOR_SETUP_IRP_FROM_CLIENT,

    DRV_USBFSV1_DEVICE_EP0_STATE_EXPECTING_RX_DATA_STAGE_FROM_HOST,
	DRV_USBFSV1_DEVICE_EP0_STATE_WAITING_FOR_RX_DATA_IRP_FROM_CLIENT,
    DRV_USBFSV1_DEVICE_EP0_STATE_WAITING_FOR_RX_STATUS_IRP_FROM_CLIENT,
    DRV_USBFSV1_DEVICE_EP0_STATE_WAITING_FOR_RX_STATUS_COMPLETE,

    DRV_USBFSV1_DEVICE_EP0_STATE_WAITING_FOR_TX_DATA_IRP_FROM_CLIENT,
    DRV_USBFSV1_DEVICE_EP0_STATE_WAITING_FOR_TX_STATUS_IRP_FROM_CLIENT,
	DRV_USBFSV1_DEVICE_EP0_STATE_WAITING_FOR_TX_STATUS_COMPLETE,
	DRV_USBFSV1_DEVICE_EP0_STATE_TX_DATA_STAGE_IN_PROGRESS
}
DRV_USBFSV1_DEVICE_EP0_STATE;

/***********************************************
 * Driver object structure. One object per
 * hardware instance
 **********************************************/

typedef struct _DRV_USBFSV1_OBJ_STRUCT
{
    /* Indicates this object is in use */
    bool inUse;

    /* Set if the driver is opened */
    bool isOpened;

    /* Set if device if D+ pull up is enabled. */
    bool deviceAttached;

    /* The object is current in an interrupt context */
    bool isInInterruptContext;

    /* Sent session invalid event to the client */
    bool sessionInvalidEventSent;

    /* Set if valid VBUS was detected in device mode */
    bool vbusIsValid;

    /* Set if device if D+ pull up is enabled. */
    bool isAttached;

    /* Set if the device is suspended */
    bool isSuspended;

    /* Set if device if D+ pull up is enabled. */
    bool operationEnabled;

    /* Current operating mode of the driver */
    uint8_t operationMode;

    /* Interrupt source for USB module */
    uint8_t interruptSource;

    /* Interrupt source for USB module */
    uint8_t interruptSource1;

    /* Interrupt source for USB module */
    uint8_t interruptSource2;

    /* Interrupt source for USB module */
    uint8_t interruptSource3;

    /* Mutex create function */
    OSAL_MUTEX_DECLARE(mutexID);

    /* Pointer to the endpoint 0 Buffers */
    uint8_t * endpoint0BufferPtr[DEVICE_DESC_BANK_NUMBER];

    /* Next Ping Pong state */
    uint32_t rxEndpointsNextPingPong;
    uint32_t txEndpointsNextPingPong;

    /* Status of this driver instance */
    SYS_STATUS status;

    /* Contains the EPO state */
    DRV_USBFSV1_DEVICE_EP0_STATE endpoint0State;

    /* The USB peripheral associated with
     * the object */
    usb_registers_t * usbID;

    /* Attach state of the device */
    DRV_USBFSV1_HOST_ATTACH_STATE attachState;

    /* Pointer to the endpoint table */
    DRV_USBFSV1_HOST_ENDPOINT_OBJ hostEndpointTable[DRV_USBFSV1_HOST_MAXIMUM_ENDPOINTS_NUMBER];

    /* Root Hub Port 0 attached device speed in host mode
     * In device mode, the speed at which the device attached */
    USB_SPEED deviceSpeed;

    /* Client data that will be returned at callback */
    uintptr_t  hClientArg;

    /* Call back function for this client */
    DRV_USB_EVENT_CALLBACK  pEventCallBack;

    /* Current VBUS level when running in device mode */
    DRV_USB_VBUS_LEVEL vbusLevel;

    /* Callback to determine the Vbus level */
    DRV_USBFSV1_VBUS_COMPARATOR vbusComparator;

    /* This is array of device endpoint objects pointers */
    DRV_USBFSV1_DEVICE_ENDPOINT_OBJ * deviceEndpointObj[DRV_USBFSV1_ENDPOINTS_NUMBER];

    /* Pointer to the endpoint table */
    DRV_USBFSV1_ENDPOINT_DESC_TABLE endpointDescriptorTable[DRV_USBFSV1_ENDPOINTS_NUMBER];

    /* Driver flags to indicate different things */
    DRV_USBFSV1_FLAGS driverFlags;

    /* Transfer Groups */
    DRV_USBFSV1_HOST_TRANSFER_GROUP controlTransferGroup;

    /* This is to track the reset state */
    DRV_USBFSV1_HOST_RESET_STATE resetState;

    /* This counts the reset signal duration */
    DRV_USBFSV1_HOST_ROOT_HUB_INFO rootHubInfo;

    /* This counts the reset signal duration */
    _DRV_USBFSV1_FOR_HOST(uint32_t, resetDuration);

    /* The SWEPBuffer index */
    _DRV_USBFSV1_FOR_HOST(uint8_t, numSWEpEntry);

    /* Placeholder for bandwidth consumed in frame */
    _DRV_USBFSV1_FOR_HOST(uint8_t, globalBWConsumed);

    /* Variable used SW Endpoint objects that is used by this HW instances for
     * USB transfer scheduling */
    _DRV_USBFSV1_FOR_HOST(DRV_USBFSV1_HOST_SW_EP, drvUSBHostSWEp[_DRV_USBFSV1_SW_EP_NUMBER]);

    /* This is needed to track if the host is generating reset signal */
    _DRV_USBFSV1_FOR_HOST(bool, isResetting);

    /* This counts the attach detach debounce interval*/
    _DRV_USBFSV1_FOR_HOST(uint32_t, attachDebounceCounter);

    /* This is the post detach delay counter */
    _DRV_USBFSV1_FOR_HOST(uint32_t, detachDebounceCounter);

    /* This flag is true if an attach de-bounce count is in progress */
    _DRV_USBFSV1_FOR_HOST(bool, isAttachDebouncing);

    /* This flag is true if an detach de-bounce count is in progress */
    _DRV_USBFSV1_FOR_HOST(bool, isDetachDebouncing);

    /* This flag is true if an detach event has come and device de-enumeration
     * operation is in progress  */
    _DRV_USBFSV1_FOR_HOST(bool, isDeviceDeenumerating);

    /* This is the pointer to host Pipe descriptor table */
    _DRV_USBFSV1_FOR_HOST(usb_descriptor_host_registers_t *, hostEndpointTablePtr);

    /* The parent UHD assigned by the host */
    _DRV_USBFSV1_FOR_HOST(USB_HOST_DEVICE_OBJ_HANDLE, usbHostDeviceInfo);

    /* The UHD of the device attached to port assigned by the host */
    _DRV_USBFSV1_FOR_HOST(USB_HOST_DEVICE_OBJ_HANDLE, attachedDeviceObjHandle);

} DRV_USBFSV1_OBJ;

/**************************************
 * Local functions.
 *************************************/

void _DRV_USBFSV1_DEVICE_Initialize(DRV_USBFSV1_OBJ * drvObj, SYS_MODULE_INDEX index);
void _DRV_USBFSV1_DEVICE_Tasks_ISR(DRV_USBFSV1_OBJ * hDriver);
void _DRV_USBFSV1_HOST_Initialize
(
    DRV_USBFSV1_OBJ * const pusbdrvObj,
    const SYS_MODULE_INDEX index
);
void _DRV_USBFSV1_HOST_Tasks_ISR(DRV_USBFSV1_OBJ * pusbdrvObj);

void _DRV_USBFSV1_HOST_AttachDetachStateMachine (DRV_USBFSV1_OBJ * hDriver);

bool _DRV_USBFSV1_HOST_TransferSchedule
(
    DRV_USBFSV1_OBJ * pusbdrvObj,
    DRV_USBFSV1_TRANSACTION_RESULT lastResult,
    unsigned int transactionSize,
    bool frameExpiry
);

void _DRV_USBFSV1_SendTokenToAddress
(
	usb_registers_t * usbID,
    uint8_t address,
    uint16_t pid,
    uint8_t endpoint,
    bool isLowSpeed
);

bool _DRV_USBFSV1_HOST_NonControlIRPProcess
(
    DRV_USBFSV1_OBJ * pusbdrvObj,
    USB_HOST_IRP_LOCAL * pirp,
    DRV_USBFSV1_TRANSACTION_RESULT lastTransactionResult,
    int lastTransactionsize
);

bool _DRV_USBFSV1_HOST_ControlXferProcess
(
    DRV_USBFSV1_OBJ * pusbdrvObj,
    USB_HOST_IRP_LOCAL * pirp,
    DRV_USBFSV1_TRANSACTION_RESULT deviceResponse,
    unsigned int deviceResponseSize
);

void _DRV_USBFSV1_HOST_Calculate_Control_BW
(
    DRV_USBFSV1_OBJ * pusbdrvObj,
    DRV_USBFSV1_HOST_TRANSFER_GROUP * ptransferGroup,
    USB_HOST_IRP_LOCAL * pcontrolIRP
);

bool _DRV_USBFSV1_HOST_Calculate_NonControl_BW
(
    DRV_USBFSV1_OBJ * pusbdrvObj,
    DRV_USBFSV1_HOST_TRANSFER_GROUP * ptransferGroup,
    USB_HOST_IRP_LOCAL * ptransferIRP,
    USB_TRANSFER_TYPE transferType,
    uint8_t numSWEpEntry
);

void _DRV_USBFSV1_HOST_NonControl_Send_Token
(
    USB_HOST_IRP_LOCAL * pirp,
    DRV_USBFSV1_OBJ *pusbdrvObj,
    DRV_USBFSV1_HOST_PIPE_OBJ *pipe,
    bool isLowSpeed
);

void _DRV_USBFSV1_HOST_ControlSendToken
(
    USB_HOST_IRP_LOCAL * pirp,
    DRV_USBFSV1_OBJ *pusbdrvObj,
    DRV_USBFSV1_HOST_PIPE_OBJ *pipe,
    uint8_t endpoint,
    uint8_t deviceAddress,
    usb_registers_t * usbID,
    bool isLowSpeed
);


#endif
