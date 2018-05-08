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
/*******************************************************************************
Copyright © 2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END


#include <stdint.h>
#include <stdbool.h>
#include "system_config.h"

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