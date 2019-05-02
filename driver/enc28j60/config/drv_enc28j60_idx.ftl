<#----------------------------------------------------------------------------
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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
---------------------------------------------------------------------------->

config DRV_ENC28J60_INSTANCES_NUMBER_GT_${INSTANCE+1}
    bool
    depends on DRV_ENC28J60_USE_DRIVER
<#if INSTANCE != 0>
	default n if DRV_ENC28J60_INSTANCES_NUMBER_GT_${INSTANCE} = n     
</#if>	
	default n if DRV_ENC28J60_INSTANCES_NUMBER = ${INSTANCE+1}
	default y
	
config DRV_ENC28J60_IDX${INSTANCE}
    depends on DRV_ENC28J60_USE_DRIVER 
<#if INSTANCE != 0>
	             && DRV_SPI_INSTANCES_NUMBER_GT_${INSTANCE}
</#if>
    bool "enc28j60 Driver Instance ${INSTANCE}"
    default y
    
ifblock DRV_ENC28J60_IDX${INSTANCE}

config DRV_ENC28J60_BUS_MODE_IDX${INSTANCE}
	depends on DRV_ENC28J60_IDX${INSTANCE}
	string "Interface Bus"
	range DRV_ENC28J60_BUS_ENUM
	default "SPI"

config DRV_ENC28J60_MAC_TX_DESCRIPTORS_IDX${INSTANCE}
    depends on DRV_ENC28J60_IDX${INSTANCE}
    int "Number of the TX Descriptors to be Created"
    default 2
    
config DRV_ENC28J60_MAC_RX_DESCRIPTORS_IDX${INSTANCE}
    depends on DRV_ENC28J60_IDX${INSTANCE}
    int "Number of the RX Descriptors to be Created"
    default 2

config DRV_ENC28J60_MAX_RX_BUFFER_IDX${INSTANCE}
    depends on DRV_ENC28J60_IDX${INSTANCE}
    int "Size of an RX Buffer"
    default 1536
    
config DRV_ENC28J60_SPI_DRV_INDEX_IDX${INSTANCE}
    depends on DRV_ENC28J60_IDX${INSTANCE}
	depends on DRV_ENC28J60_BUS_MODE_IDX${INSTANCE} = "SPI"
    int "SPI Driver Instance Number"
    default 0
    
config DRV_ENC28J60_SPI_BPS_IDX${INSTANCE}
    depends on DRV_ENC28J60_IDX${INSTANCE}
	depends on DRV_ENC28J60_BUS_MODE_IDX${INSTANCE} = "SPI"
    int "SPI BPS"
    default 14000000
    
config DRV_ENC28J60_SPI_SS_PORT_MODULE_IDX${INSTANCE}
    depends on DRV_ENC28J60_IDX${INSTANCE}
	depends on DRV_ENC28J60_BUS_MODE_IDX${INSTANCE} = "SPI"
    string "enc28j60 SPI Slave Select Port Module"
    range PORTS_MODULE_ID
    default "PORTS_ID_0"
    
config DRV_ENC28J60_SPI_SS_PORT_CHANNEL_IDX${INSTANCE}
    depends on DRV_ENC28J60_IDX${INSTANCE}
	depends on DRV_ENC28J60_BUS_MODE_IDX${INSTANCE} = "SPI"
    string "enc28j60 SPI Slave Select Port Channel"
    range PORTS_CHANNEL
    default "PORT_CHANNEL_D"

config DRV_ENC28J60_SPI_SS_PORT_PIN_IDX${INSTANCE}
    depends on DRV_ENC28J60_IDX${INSTANCE}
	depends on DRV_ENC28J60_BUS_MODE_IDX${INSTANCE} = "SPI"
    string "enc28j60 SPI Slave Select Port Pin"
    range PORTS_BIT_POS
    default "PORTS_BIT_POS_9"
	
config DRV_ENC28J60_RX_BUFFER_SIZE_IDX${INSTANCE}
	depends on DRV_ENC28J60_IDX${INSTANCE}
	int "RX Buffer Size (Must be even)"
	default 16384
	
config DRV_ENC28J60_MAX_FRAME_SIZE_IDX${INSTANCE}
	depends on DRV_ENC28J60_IDX${INSTANCE}
	int "Maximum Frame Length"
	default 1536
	
config DRV_ENC28J60_MAX_USE_AUTOMATIC_FLOW_CONTROL_IDX${INSTANCE}
	depends on DRV_ENC28J60_IDX${INSTANCE}
	bool "Use Automatic Flow Control"
	default y
	


endif