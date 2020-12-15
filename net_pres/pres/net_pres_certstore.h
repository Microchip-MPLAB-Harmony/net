/*******************************************************************************
MPLAB Harmony Networking Presentation Certificate Storage header file

  Company:
    Microchip Technology Inc.
    
  Filename:
    net_pres_certstore.h
    
  Summary:
    This file describes the standard interface for the certificate store
    
  Description:
   This file describes the interface that the presentation layer uses to access
   the certificate store.  The MHC can generate a read-only, Flash-based 
   certificate store, or stubs for these functions.  If a more complex certificate
   store is desired, for instance storing multiple certificates or being able to 
   update certificates, the implementation may be provided by the end user.

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2015-2018 Microchip Technology Inc. and its subsidiaries.

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


//DOM-IGNORE-END


#include <stdint.h>
#include <stdbool.h>
#include "configuration.h"

#include "net_pres.h"
#ifndef _NET_PRES_CERTSTORE_H_
#define _NET_PRES_CERTSTORE_H_

#ifdef __cplusplus
extern "C" {
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines - System Level
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Get CA Certificates function

  Summary:
    This function gets the CA certificates from the store,
	<p><b>Implementation:</b> Dynamic</p>
    
  Description:
    This function is used by client connections to retrieve the Certificate 
	Authority certificates that are used to validate signatures on server 
	certificates.
    
  Preconditions:
    None.

  Parameters:
    certPtr   - A pointer to the CA certificates
    certSize  - The size of the certificates
    certIndex - Most likely '0', but this parameter is provided to select 
	            a different set of CA certificates
    
    Returns:
    - true  - Indicates success
    - false - Indicates failure
	
*/    

bool NET_PRES_CertStoreGetCACerts(const uint8_t ** certPtr, int32_t * certSize, 
                                  uint8_t certIndex);
								  
// *****************************************************************************
/* Get Device X509 TLS Parameters function

  Summary:
    This function gets the Device X09 TLS Parameters from the store,
  <p><b>Implementation:</b> Dynamic</p>
    
  Description:
    This function is used by client connections to retrieve TLS parameters   
  (Device Certificate and Device Private Key) that are used for mutual 
  authentication.
    
  Preconditions:
    None.

  Parameters:
    certPtr    - A pointer to the device certificates
    certSize   - The size of the device certificates
    pvtKeyPtr  - A pointer to the device's private key
    pvtKeySize - The size of the device's private key
    certIndex  - Most likely '0', but this parameter is provided to select 
                 a different set of TLS parameters
    
    Returns:
    - true  - Indicates success
    - false - Indicates failure
  
*/    

bool NET_PRES_CertStoreGetDeviceTlsParams(const uint8_t ** certPtr, 
          int32_t * certSize, const uint8_t ** pvtKeyPtr, int32_t * pvtKeySize, 
          uint8_t certIndex);

// *****************************************************************************
/* Get Server Certificate and Key function

  Summary:
    This function gets a server certificate and key from the certificate store.
	<p><b>Implementation:</b> Dynamic</p>
    
  Description:
    This function is used by server connections to retrieve their certificate 
	and private key. Multiple server certificates can be stored in the certificate 
	store, for example one for a Web server and one for a mail server.
    
  Preconditions:
    None.

  Parameters:
    serverCertPtr  - A pointer to the server certificate
    serverCertSize - The size of the server certificate
    serverKeyPtr   - A pointer to the server private key
    serverKeySize  - The size of the server private key
    certIndex      - Most likely '0', but this parameter is provided to select a 
	                 different server certificate
    
  Returns:
    - true  - Indicates success
    - false - Indicates failure
*/
bool NET_PRES_CertStoreGetServerCert(const uint8_t ** serverCertPtr, int32_t * serverCertSize, 
                      const uint8_t ** serverKeyPtr, int32_t * serverKeySize, uint8_t certIndex);

#ifdef __cplusplus
}
#endif


#endif //_NET_PRES_CERTSTORE_H_