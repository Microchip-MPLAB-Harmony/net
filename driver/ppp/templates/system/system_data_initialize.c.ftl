<#--
Copyright (C) 2023, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
-->
/*** PPP MAC Initialization Data ***/

const SERIAL_HDLC_OBJECT PPP_UART_HDLC =
{
    .read = ${DRV_PPP_UART_TYPE}_Read,
    .readCount = ${DRV_PPP_UART_TYPE}_ReadCountGet,
    .write = ${DRV_PPP_UART_TYPE}_Write,
    .writeFreeSpace = ${DRV_PPP_UART_TYPE}_WriteFreeBufferCountGet,
};



const TCPIP_MODULE_MAC_PPP_CONFIG tcpip_PPP_InitData =
{
    .restartTmo         = TCPIP_PPP_RESTART_TMO,
    .maxTerminate       = TCPIP_PPP_MAX_TERM,
    .maxConfigure       = TCPIP_PPP_MAX_CONFIGURE,
    .maxFailure         = TCPIP_PPP_MAX_FAILURE,
    .nRxDedicatedBuffers = TCPIP_PPP_RX_DEDICATED_BUFFERS,
    .nRxInitBuffers     = TCPIP_PPP_RX_INIT_BUFFERS,
    .rxLowThreshold     = TCPIP_PPP_RX_LOW_THRESHOLD,
    .rxLowFill          = TCPIP_PPP_RX_LOW_FILL,
    .lcpConfigFlags     = TCPIP_LCP_CONF_FLAGS,
    .ipcpConfigFlags    = TCPIP_IPCP_CONF_FLAGS,
    .configFlags        = TCPIP_PPP_CONF_FLAGS,
    .mru                = TCPIP_PPP_MAX_RECEIVE_UNIT,
    .echoTmo            = TCPIP_PPP_ECHO_TMO,
    .lclIpv4Addr        = TCPIP_PPP_LOCAL_IPV4_ADDRESS,
    .peerIpv4Addr       = TCPIP_PPP_PEER_IPV4_ADDRESS,
    .magic_no_func      = DRV_PPP_MAGIC_CALLBACK,
    .rxAccm             = TCPIP_PPP_RX_ACCM,
    .pHdlcObj           = &DRV_PPP_HDLC_DRIVER_OBJECT,
    .hdlcProcSize       = TCPIP_HDLC_PROC_BUFFER_SIZE,
    .hdlcPeekSize       = TCPIP_HDLC_PEEK_BUFFER_SIZE,
    .hdlcIx             = TCPIP_HDLC_OBJECT_INDEX,
    .hdlcSerialObj      = &PPP_UART_HDLC,
};

