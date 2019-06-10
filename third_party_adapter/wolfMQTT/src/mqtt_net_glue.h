/*******************************************************************************
  wolfMQTT NET Glue Source File

  Summary: 
    wolfMQTT MQTTNET glue file
    
  Description:
    This file provides the MQTTNET glue file for using
    wolfMQTT with the Harmony TCP/IP stack
*******************************************************************************/

/*****************************************************************************
 Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/

#ifndef _MQTT_NET_GLUE_H_
#define _MQTT_NET_GLUE_H_

#ifdef __cplusplus
extern "C" {
#endif

// *****************************************************************************
/*
  Structure:
    _MqttNet

  Summary:
    _MqttNet structure declaration.

  Description:
    Forward declaration of the MqttNet structure defined in wolfmqtt/mqtt_socket.h.
*/
struct _MqttNet;

//*****************************************************************************
/*
  Function:
     int WMQTT_NETGlue_Initialize(struct _MqttNet* net);

  Summary:
    Intializes the MQTTNET glue layer

  Description:
    Performs the initialization of the MQTTNET glue layer.
    This layer is used by wolfMQTT to connect to the network

  Precondition:
    None

  Parameters:
    net     - pointer to a MqttNet structure to be initialized.
              This data structure usually belongs to the wolfMQTT application.

  Returns:
    - MQTT_CODE_SUCCESS - if initialization was successful
    - MQTT_CODE_ERROR_MEMORY - if initialization failed because of memory allocation failure.

  Remarks:
    See wolfmqtt/mqtt_types.h for the wolfMQTT response codes

    This function initializes a MqttNet data structure that should be passed as a parameter
    to the wolfMQTT::MqttClient_Init() API call.
    The MqttClient_Init() performs the initialization of the wolfMQTT library for use.
 */

int WMQTT_NETGlue_Initialize(struct _MqttNet* net);

//*****************************************************************************
/*
  Function:
     int WMQTT_NETGlue_SNInitialize(struct _MqttNet* net);

  Summary:
    Intializes the MQTTNET glue layer

  Description:
    Performs the initialization of the MQTTNET glue layer for a Sensor Networks (SN) context.
    This layer is used by wolfMQTT to connect to the network

  Precondition:
    None

  Parameters:
    net     - pointer to a MqttNet structure to be initialized.
              This data structure usually belongs to the wolfMQTT application.

  Returns:
    - MQTT_CODE_SUCCESS - if initialization was successful
    - MQTT_CODE_ERROR_MEMORY - if initialization failed because of memory allocation failure.

  Remarks:
    See wolfmqtt/mqtt_types.h for the wolfMQTT response codes

    This function initializes a MqttNet data structure that should be passed as a parameter
    to the wolfMQTT::MqttClient_Init() API call.
    The MqttClient_Init() performs the initialization of the wolfMQTT library for use.

    The WOLFMQTT_SN symbol needs to be defined to use the wolfMQTT for Sensor Networks

    The SN connection does not have DTLS support.

 */

int WMQTT_NETGlue_SNInitialize(struct _MqttNet* net);

//*****************************************************************************
/*
  Function:
     int WMQTT_NETGlue_Deinitialize(struct _MqttNet* net);

  Summary:
    Deintializes the MQTTNET glue layer

  Description:
    Performs the deinitialization of the MQTTNET glue layer.
    This layer is deinitialized once the wolfMQTT network connection is closed

  Precondition:
    WMQTT_NETGlue_Initialize/WMQTT_NETGlue_SNInitialize should have been called
    Valid MqttNet pointer


  Parameters:
    net     - pointer to a MqttNet structure to be deinitialized.
              This data structure is the one used for WMQTT_NETGlue_Initialize/WMQTT_NETGlue_SNInitialize.

  Returns:
    - MQTT_CODE_SUCCESS - deinitialization was successful

  Remarks:
    See wolfmqtt/mqtt_types.h for the wolfMQTT response codes

    This function deinitializes the MqttNet structure that was passed as a parameter
    to the wolfMQTT::MqttClient_Init() API call.
    The network layer is closed at this moment.
 */
int WMQTT_NETGlue_Deinitialize(struct _MqttNet* net);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _MQTT_NET_GLUE_H_ */

