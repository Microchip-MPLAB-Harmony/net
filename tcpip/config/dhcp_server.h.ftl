<#--
/*******************************************************************************
  DHCP server Freemarker Template File

  Company:
    Microchip Technology Inc.

  File Name:
    dhcps.h.ftl

  Summary:
    DHCP server Freemarker Template File

  Description:

*******************************************************************************/
-->

<#----------------------------------------------------------------------------
 Copyright (C) 2014-2018 Microchip Technology Inc. and its subsidiaries.

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
----------------------------------------------------------------------------->

<#if TCPIP_STACK_USE_DHCP_SERVER_V2 == true>
/*** DHCP Server v2 Configuration ***/
#define TCPIP_STACK_USE_DHCP_SERVER_V2
#define TCPIP_DHCPS_MAX_INTERFACES          ${TCPIP_DHCPS_MAX_INTERFACES}
#define TCPIP_DHCPS_MAX_LEASES              ${TCPIP_DHCPS_MAX_LEASES}
#define TCPIP_DHCPS_ICMP_PROBES             ${TCPIP_DHCPS_ICMP_PROBES}
#define TCPIP_DHCPS_CONFLICT_ATTEMPTS       ${TCPIP_DHCPS_CONFLICT_ATTEMPTS}
#define TCPIP_DHCPS_TASK_PROCESS_RATE       ${TCPIP_DHCPS_TASK_PROCESS_RATE}
#define TCPIP_DHCPS_CLIENT_ID_MAX_SIZE      ${TCPIP_DHCPS_CLIENT_ID_MAX_SIZE}
#define TCPIP_DHCPS_ICMP_ECHO_DATA_SIZE     ${TCPIP_DHCPS_ICMP_ECHO_DATA_SIZE}
#define TCPIP_DHCPS_ICMP_ECHO_RETRIES       ${TCPIP_DHCPS_ICMP_ECHO_RETRIES}

#define TCPIP_DHCPS_OPTION_ROUTER_VALUES        ${TCPIP_DHCPS_OPTION_ROUTER_VALUES}
#define TCPIP_DHCPS_OPTION_DNS_VALUES           ${TCPIP_DHCPS_OPTION_DNS_VALUES}
#define TCPIP_DHCPS_OPTION_TIME_SERVER_VALUES   ${TCPIP_DHCPS_OPTION_TIME_SERVER_VALUES}
#define TCPIP_DHCPS_OPTION_NAME_SERVER_VALUES   ${TCPIP_DHCPS_OPTION_NAME_SERVER_VALUES}
#define TCPIP_DHCPS_OPTION_NTP_SERVER_VALUES    ${TCPIP_DHCPS_OPTION_NTP_SERVER_VALUES}
#define TCPIP_DHCPS_OPTION_T1_T2_SUPPRESS       ${TCPIP_DHCPS_OPTION_T1_T2_SUPPRESS?c}

#define TCPIP_DHCPS_MAX_EVENT_REGISTRATIONS     ${TCPIP_DHCPS_MAX_EVENT_REGISTRATIONS}
#define TCPIP_DHCPS_REPORT_ERROR_EVENT          ${TCPIP_DHCPS_REPORT_ERROR_EVENT?c}
#define TCPIP_DHCPS_REPORT_CLIENT_EVENT         ${TCPIP_DHCPS_REPORT_CLIENT_EVENT?c}
#define TCPIP_DHCPS_ENABLE_STATISTICS           ${TCPIP_DHCPS_ENABLE_STATISTICS?c}
#define TCPIP_DHCPS_DYNAMIC_DB_ACCESS           ${TCPIP_DHCPS_DYNAMIC_DB_ACCESS?c}
#define TCPIP_DHCPS_MULTI_THREADED_ACCESS       ${TCPIP_DHCPS_MULTI_THREADED_ACCESS?c}

<#list 0 ..(TCPIP_DHCPS_MAX_INTERFACES -1) as i >
    <#if (.vars["TCPIP_DHCPS_IDX" + i] != false)>
        <#lt>#define TCPIP_DHCPS_INTERFACE_INDEX_IDX${i}        ${.vars["TCPIP_DHCPS_INTERFACE_INDEX_IDX" + i]}
        <#lt>#define TCPIP_DHCPS_MAX_LEASE_NUM_IDX${i}          ${.vars["TCPIP_DHCPS_MAX_LEASE_NUM_IDX" + i]}
        <#lt>#define TCPIP_DHCPS_LEASEDURATION_DFLT_IDX${i}     ${.vars["TCPIP_DHCPS_LEASEDURATION_DFLT_IDX" + i]}
        <#lt>#define TCPIP_DHCPS_SERVER_IP_ADDRESS_IDX${i}      "${.vars["TCPIP_DHCPS_SERVER_IP_ADDRESS_IDX" + i]}"
        <#lt>#define TCPIP_DHCPS_START_IP_ADDR_IDX${i}          "${.vars["TCPIP_DHCPS_START_IP_ADDR_IDX" + i]}"
        <#lt>#define TCPIP_DHCPS_MASK_PREFIX_NUM_IDX${i}      	${.vars["TCPIP_DHCPS_MASK_PREFIX_NUM_IDX" + i]}
        <#lt>#define TCPIP_DHCPS_ROUTER_IP_ADDR_IDX${i}         "${.vars["TCPIP_DHCPS_ROUTER_IP_ADDR_IDX" + i]}"
        <#lt>#define TCPIP_DHCPS_DNS_IP_ADDR_IDX${i}            "${.vars["TCPIP_DHCPS_DNS_IP_ADDR_IDX" + i]}"
        <#lt>#define TCPIP_DHCPS_TIMESERVER_IP_ADDR_IDX${i}     "${.vars["TCPIP_DHCPS_TIMESERVER_IP_ADDR_IDX" + i]}"
        <#lt>#define TCPIP_DHCPS_NAMESERVER_IP_ADDR_IDX${i}     "${.vars["TCPIP_DHCPS_NAMESERVER_IP_ADDR_IDX" + i]}"
        <#lt>#define TCPIP_DHCPS_NTPSERVER_IP_ADDR_IDX${i}      "${.vars["TCPIP_DHCPS_NTPSERVER_IP_ADDR_IDX" + i]}"
        <#lt>#define TCPIP_DHCPS_CONFIG_FLAG_IDX${i}            \
        <#if (.vars["TCPIP_DHCPS_START_DISABLE_IDX" + i] != false)>
        <#lt>                                                TCPIP_DHCPS_CONFIG_FLAG_DISABLED \
        </#if>
        <#if (.vars["TCPIP_DHCPS_DELETE_LEASE_INFO_IDX" + i] != false)>
        <#lt>                                                TCPIP_DHCPS_CONFIG_FLAG_DELETE_OLD \
        </#if>
        <#if (.vars["TCPIP_DHCPS_CONFLICT_DETECT_DISABLE_IDX" + i] != false)>
        <#lt>                                                TCPIP_DHCPS_CONFIG_FLAG_NO_CONFLICT_DETECT \
        </#if>
        <#if (.vars["TCPIP_DHCPS_LEASE_EXT_DISABLE_IDX" + i] != false)>
        <#lt>                                                TCPIP_DHCPS_CONFIG_FLAG_NO_LEASE_EXTEND \
        </#if>
        <#if (.vars["TCPIP_DHCPS_KEEP_INFO_UNREQ_IDX" + i] != false)>
        <#lt>                                                TCPIP_DHCPS_CONFIG_FLAG_KEEP_UNREQ_OFFERS \
        </#if>
        <#if (.vars["TCPIP_DHCPS_PROB_FAIL_ABORT_IDX" + i] != false)>
        <#lt>                                                TCPIP_DHCPS_CONFIG_FLAG_ABORT_IF_PROBE_FAILED \
        </#if>
        <#lt>                                                0
        
        <#lt>#define TCPIP_DHCPS_LEASEDURATION_MIN_IDX${i}      ${.vars["TCPIP_DHCPS_LEASEDURATION_MIN_IDX" + i]}
        <#lt>#define TCPIP_DHCPS_LEASEDURATION_MAX_IDX${i}      ${.vars["TCPIP_DHCPS_LEASEDURATION_MAX_IDX" + i]}
        <#lt>#define TCPIP_DHCPS_UNREQ_TMO_IDX${i}              ${.vars["TCPIP_DHCPS_UNREQ_TMO_IDX" + i]}
        
        <#lt>#define TCPIP_DHCPS_T1RENEW_MULT_FACT_IDX${i}      ${.vars["TCPIP_DHCPS_T1RENEW_MULT_FACT_IDX" + i]}
        <#lt>#define TCPIP_DHCPS_T1RENEW_DIV_FACT_IDX${i}       ${.vars["TCPIP_DHCPS_T1RENEW_DIV_FACT_IDX" + i]}
        <#lt>#define TCPIP_DHCPS_T2REBIND_MULT_FACT_IDX${i}     ${.vars["TCPIP_DHCPS_T2REBIND_MULT_FACT_IDX" + i]}
        <#lt>#define TCPIP_DHCPS_T2REBIND_DIV_FACT_IDX${i}      ${.vars["TCPIP_DHCPS_T2REBIND_DIV_FACT_IDX" + i]}
        
    </#if>
</#list>



</#if>

<#--
/*******************************************************************************
 End of File
*/
-->
