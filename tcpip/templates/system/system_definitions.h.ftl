#include "library/tcpip/tcpip.h"
<#if (drvGmac.TCPIP_USE_ETH_MAC)?has_content && (drvGmac.TCPIP_USE_ETH_MAC) == true>
#include "driver/gmac/drv_gmac.h"
</#if>
<#if (drvMiim.DRV_MIIM_USE_DRIVER)?has_content && (drvMiim.DRV_MIIM_USE_DRIVER) == true>
#include "driver/miim/drv_miim.h"
</#if>

