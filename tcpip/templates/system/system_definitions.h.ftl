#include "library/tcpip/tcpip.h"
<#if (drvSamv71Gmac.TCPIP_USE_ETH_MAC)?has_content && (drvSamv71Gmac.TCPIP_USE_ETH_MAC) == true>
#include "driver/gmac/drv_gmac.h"
</#if>
<#if (drvMiim.DRV_MIIM_USE_DRIVER)?has_content && (drvMiim.DRV_MIIM_USE_DRIVER) == true>
#include "driver/miim/drv_miim.h"
</#if>

