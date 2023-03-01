/*******************************************************************************
  wolfMQTT NET Glue Source File

  Summary: 
    wolfMQTT MQTTNET glue file
    
  Description:
    This file provides the MQTTNET glue file for using
    wolfMQTT with the Harmony TCP/IP stack
*******************************************************************************/

/*
Copyright (C) 2019-2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

The software and documentation is provided by microchip and its contributors
"as is" and any express, implied or statutory warranties, including, but not
limited to, the implied warranties of merchantability, fitness for a particular
purpose and non-infringement of third party intellectual property rights are
disclaimed to the fullest extent permitted by law. In no event shall microchip
or its contributors be liable for any direct, indirect, incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement
of substitute goods or services; loss of use, data, or profits; or business
interruption) however caused and on any theory of liability, whether in contract,
strict liability, or tort (including negligence or otherwise) arising in any way
out of the use of the software and documentation, even if advised of the
possibility of such damage.

Except as expressly permitted hereunder and subject to the applicable license terms
for any third-party software incorporated in the software and any applicable open
source software license terms, no license or other rights, whether express or
implied, are granted under any patent or other intellectual property rights of
Microchip or any third party.
*/

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

