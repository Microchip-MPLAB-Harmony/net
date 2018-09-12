#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-sam_e70_xult.mk)" "nbproject/Makefile-local-sam_e70_xult.mk"
include nbproject/Makefile-local-sam_e70_xult.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=sam_e70_xult
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/berkeley_udp_client.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/berkeley_udp_client.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS

else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../src/app_commands.c ../src/main.c ../src/config/sam_e70_xult/initialization.c ../src/config/sam_e70_xult/interrupts.c ../src/config/sam_e70_xult/exceptions.c ../src/config/sam_e70_xult/stdio/xc32_monitor.c ../src/config/sam_e70_xult/peripheral/clk/plib_clk.c ../src/config/sam_e70_xult/peripheral/pio/plib_pio.c ../src/config/sam_e70_xult/peripheral/nvic/plib_nvic.c ../src/config/sam_e70_xult/peripheral/mpu/plib_mpu.c ../src/config/sam_e70_xult/startup.c ../src/config/sam_e70_xult/libc_syscalls.c ../src/config/sam_e70_xult/system/int/src/sys_int.c ../src/app.c ../src/config/sam_e70_xult/tasks.c ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_ethphy.c ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_extphy_smsc8740.c ../src/config/sam_e70_xult/driver/miim/src/dynamic/drv_miim.c ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac.c ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac_lib.c ../src/config/sam_e70_xult/net/pres/src/net_pres.c ../src/config/sam_e70_xult/net/pres/net_pres_enc_glue.c ../src/config/sam_e70_xult/system/console/src/sys_console.c ../src/config/sam_e70_xult/system/console/src/sys_console_uart.c ../src/config/sam_e70_xult/system/console/src/sys_debug.c ../src/config/sam_e70_xult/system/console/src/sys_command.c ../src/config/sam_e70_xult/system/time/src/sys_time.c ../src/config/sam_e70_xult/peripheral/tc/plib_tc0.c ../src/config/sam_e70_xult/library/tcpip/src/arp.c ../src/config/sam_e70_xult/library/tcpip/src/berkeley_api.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_commands.c ../src/config/sam_e70_xult/library/tcpip/src/dhcp.c ../src/config/sam_e70_xult/library/tcpip/src/dns.c ../src/config/sam_e70_xult/library/tcpip/src/ipv4.c ../src/config/sam_e70_xult/library/tcpip/src/icmp.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_alloc.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_internal.c ../src/config/sam_e70_xult/library/tcpip/src/helpers.c ../src/config/sam_e70_xult/library/tcpip/src/hash_fnv.c ../src/config/sam_e70_xult/library/tcpip/src/oahash.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_helpers.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_manager.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_notify.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_packet.c ../src/config/sam_e70_xult/system/sys_time_h2_adapter.c ../src/config/sam_e70_xult/library/tcpip/src/tcp.c ../src/config/sam_e70_xult/library/tcpip/src/udp.c ../src/config/sam_e70_xult/peripheral/usart/plib_usart1.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1360937237/app_commands.o ${OBJECTDIR}/_ext/1360937237/main.o ${OBJECTDIR}/_ext/1728594169/initialization.o ${OBJECTDIR}/_ext/1728594169/interrupts.o ${OBJECTDIR}/_ext/1728594169/exceptions.o ${OBJECTDIR}/_ext/1518761459/xc32_monitor.o ${OBJECTDIR}/_ext/1055500827/plib_clk.o ${OBJECTDIR}/_ext/1055488423/plib_pio.o ${OBJECTDIR}/_ext/1639550079/plib_nvic.o ${OBJECTDIR}/_ext/1055491083/plib_mpu.o ${OBJECTDIR}/_ext/1728594169/startup.o ${OBJECTDIR}/_ext/1728594169/libc_syscalls.o ${OBJECTDIR}/_ext/766002106/sys_int.o ${OBJECTDIR}/_ext/1360937237/app.o ${OBJECTDIR}/_ext/1728594169/tasks.o ${OBJECTDIR}/_ext/1015456894/drv_ethphy.o ${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o ${OBJECTDIR}/_ext/478869302/drv_miim.o ${OBJECTDIR}/_ext/629088514/drv_gmac.o ${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o ${OBJECTDIR}/_ext/581080307/net_pres.o ${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o ${OBJECTDIR}/_ext/1953779166/sys_console.o ${OBJECTDIR}/_ext/1953779166/sys_console_uart.o ${OBJECTDIR}/_ext/1953779166/sys_debug.o ${OBJECTDIR}/_ext/1953779166/sys_command.o ${OBJECTDIR}/_ext/327803284/sys_time.o ${OBJECTDIR}/_ext/865331892/plib_tc0.o ${OBJECTDIR}/_ext/902702131/arp.o ${OBJECTDIR}/_ext/902702131/berkeley_api.o ${OBJECTDIR}/_ext/902702131/tcpip_commands.o ${OBJECTDIR}/_ext/902702131/dhcp.o ${OBJECTDIR}/_ext/902702131/dns.o ${OBJECTDIR}/_ext/902702131/ipv4.o ${OBJECTDIR}/_ext/902702131/icmp.o ${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o ${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o ${OBJECTDIR}/_ext/902702131/helpers.o ${OBJECTDIR}/_ext/902702131/hash_fnv.o ${OBJECTDIR}/_ext/902702131/oahash.o ${OBJECTDIR}/_ext/902702131/tcpip_helpers.o ${OBJECTDIR}/_ext/902702131/tcpip_manager.o ${OBJECTDIR}/_ext/902702131/tcpip_notify.o ${OBJECTDIR}/_ext/902702131/tcpip_packet.o ${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o ${OBJECTDIR}/_ext/902702131/tcp.o ${OBJECTDIR}/_ext/902702131/udp.o ${OBJECTDIR}/_ext/707186936/plib_usart1.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1360937237/app_commands.o.d ${OBJECTDIR}/_ext/1360937237/main.o.d ${OBJECTDIR}/_ext/1728594169/initialization.o.d ${OBJECTDIR}/_ext/1728594169/interrupts.o.d ${OBJECTDIR}/_ext/1728594169/exceptions.o.d ${OBJECTDIR}/_ext/1518761459/xc32_monitor.o.d ${OBJECTDIR}/_ext/1055500827/plib_clk.o.d ${OBJECTDIR}/_ext/1055488423/plib_pio.o.d ${OBJECTDIR}/_ext/1639550079/plib_nvic.o.d ${OBJECTDIR}/_ext/1055491083/plib_mpu.o.d ${OBJECTDIR}/_ext/1728594169/startup.o.d ${OBJECTDIR}/_ext/1728594169/libc_syscalls.o.d ${OBJECTDIR}/_ext/766002106/sys_int.o.d ${OBJECTDIR}/_ext/1360937237/app.o.d ${OBJECTDIR}/_ext/1728594169/tasks.o.d ${OBJECTDIR}/_ext/1015456894/drv_ethphy.o.d ${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o.d ${OBJECTDIR}/_ext/478869302/drv_miim.o.d ${OBJECTDIR}/_ext/629088514/drv_gmac.o.d ${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o.d ${OBJECTDIR}/_ext/581080307/net_pres.o.d ${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o.d ${OBJECTDIR}/_ext/1953779166/sys_console.o.d ${OBJECTDIR}/_ext/1953779166/sys_console_uart.o.d ${OBJECTDIR}/_ext/1953779166/sys_debug.o.d ${OBJECTDIR}/_ext/1953779166/sys_command.o.d ${OBJECTDIR}/_ext/327803284/sys_time.o.d ${OBJECTDIR}/_ext/865331892/plib_tc0.o.d ${OBJECTDIR}/_ext/902702131/arp.o.d ${OBJECTDIR}/_ext/902702131/berkeley_api.o.d ${OBJECTDIR}/_ext/902702131/tcpip_commands.o.d ${OBJECTDIR}/_ext/902702131/dhcp.o.d ${OBJECTDIR}/_ext/902702131/dns.o.d ${OBJECTDIR}/_ext/902702131/ipv4.o.d ${OBJECTDIR}/_ext/902702131/icmp.o.d ${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o.d ${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o.d ${OBJECTDIR}/_ext/902702131/helpers.o.d ${OBJECTDIR}/_ext/902702131/hash_fnv.o.d ${OBJECTDIR}/_ext/902702131/oahash.o.d ${OBJECTDIR}/_ext/902702131/tcpip_helpers.o.d ${OBJECTDIR}/_ext/902702131/tcpip_manager.o.d ${OBJECTDIR}/_ext/902702131/tcpip_notify.o.d ${OBJECTDIR}/_ext/902702131/tcpip_packet.o.d ${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o.d ${OBJECTDIR}/_ext/902702131/tcp.o.d ${OBJECTDIR}/_ext/902702131/udp.o.d ${OBJECTDIR}/_ext/707186936/plib_usart1.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1360937237/app_commands.o ${OBJECTDIR}/_ext/1360937237/main.o ${OBJECTDIR}/_ext/1728594169/initialization.o ${OBJECTDIR}/_ext/1728594169/interrupts.o ${OBJECTDIR}/_ext/1728594169/exceptions.o ${OBJECTDIR}/_ext/1518761459/xc32_monitor.o ${OBJECTDIR}/_ext/1055500827/plib_clk.o ${OBJECTDIR}/_ext/1055488423/plib_pio.o ${OBJECTDIR}/_ext/1639550079/plib_nvic.o ${OBJECTDIR}/_ext/1055491083/plib_mpu.o ${OBJECTDIR}/_ext/1728594169/startup.o ${OBJECTDIR}/_ext/1728594169/libc_syscalls.o ${OBJECTDIR}/_ext/766002106/sys_int.o ${OBJECTDIR}/_ext/1360937237/app.o ${OBJECTDIR}/_ext/1728594169/tasks.o ${OBJECTDIR}/_ext/1015456894/drv_ethphy.o ${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o ${OBJECTDIR}/_ext/478869302/drv_miim.o ${OBJECTDIR}/_ext/629088514/drv_gmac.o ${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o ${OBJECTDIR}/_ext/581080307/net_pres.o ${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o ${OBJECTDIR}/_ext/1953779166/sys_console.o ${OBJECTDIR}/_ext/1953779166/sys_console_uart.o ${OBJECTDIR}/_ext/1953779166/sys_debug.o ${OBJECTDIR}/_ext/1953779166/sys_command.o ${OBJECTDIR}/_ext/327803284/sys_time.o ${OBJECTDIR}/_ext/865331892/plib_tc0.o ${OBJECTDIR}/_ext/902702131/arp.o ${OBJECTDIR}/_ext/902702131/berkeley_api.o ${OBJECTDIR}/_ext/902702131/tcpip_commands.o ${OBJECTDIR}/_ext/902702131/dhcp.o ${OBJECTDIR}/_ext/902702131/dns.o ${OBJECTDIR}/_ext/902702131/ipv4.o ${OBJECTDIR}/_ext/902702131/icmp.o ${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o ${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o ${OBJECTDIR}/_ext/902702131/helpers.o ${OBJECTDIR}/_ext/902702131/hash_fnv.o ${OBJECTDIR}/_ext/902702131/oahash.o ${OBJECTDIR}/_ext/902702131/tcpip_helpers.o ${OBJECTDIR}/_ext/902702131/tcpip_manager.o ${OBJECTDIR}/_ext/902702131/tcpip_notify.o ${OBJECTDIR}/_ext/902702131/tcpip_packet.o ${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o ${OBJECTDIR}/_ext/902702131/tcp.o ${OBJECTDIR}/_ext/902702131/udp.o ${OBJECTDIR}/_ext/707186936/plib_usart1.o

# Source Files
SOURCEFILES=../src/app_commands.c ../src/main.c ../src/config/sam_e70_xult/initialization.c ../src/config/sam_e70_xult/interrupts.c ../src/config/sam_e70_xult/exceptions.c ../src/config/sam_e70_xult/stdio/xc32_monitor.c ../src/config/sam_e70_xult/peripheral/clk/plib_clk.c ../src/config/sam_e70_xult/peripheral/pio/plib_pio.c ../src/config/sam_e70_xult/peripheral/nvic/plib_nvic.c ../src/config/sam_e70_xult/peripheral/mpu/plib_mpu.c ../src/config/sam_e70_xult/startup.c ../src/config/sam_e70_xult/libc_syscalls.c ../src/config/sam_e70_xult/system/int/src/sys_int.c ../src/app.c ../src/config/sam_e70_xult/tasks.c ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_ethphy.c ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_extphy_smsc8740.c ../src/config/sam_e70_xult/driver/miim/src/dynamic/drv_miim.c ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac.c ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac_lib.c ../src/config/sam_e70_xult/net/pres/src/net_pres.c ../src/config/sam_e70_xult/net/pres/net_pres_enc_glue.c ../src/config/sam_e70_xult/system/console/src/sys_console.c ../src/config/sam_e70_xult/system/console/src/sys_console_uart.c ../src/config/sam_e70_xult/system/console/src/sys_debug.c ../src/config/sam_e70_xult/system/console/src/sys_command.c ../src/config/sam_e70_xult/system/time/src/sys_time.c ../src/config/sam_e70_xult/peripheral/tc/plib_tc0.c ../src/config/sam_e70_xult/library/tcpip/src/arp.c ../src/config/sam_e70_xult/library/tcpip/src/berkeley_api.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_commands.c ../src/config/sam_e70_xult/library/tcpip/src/dhcp.c ../src/config/sam_e70_xult/library/tcpip/src/dns.c ../src/config/sam_e70_xult/library/tcpip/src/ipv4.c ../src/config/sam_e70_xult/library/tcpip/src/icmp.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_alloc.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_internal.c ../src/config/sam_e70_xult/library/tcpip/src/helpers.c ../src/config/sam_e70_xult/library/tcpip/src/hash_fnv.c ../src/config/sam_e70_xult/library/tcpip/src/oahash.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_helpers.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_manager.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_notify.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_packet.c ../src/config/sam_e70_xult/system/sys_time_h2_adapter.c ../src/config/sam_e70_xult/library/tcpip/src/tcp.c ../src/config/sam_e70_xult/library/tcpip/src/udp.c ../src/config/sam_e70_xult/peripheral/usart/plib_usart1.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-sam_e70_xult.mk dist/${CND_CONF}/${IMAGE_TYPE}/berkeley_udp_client.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=ATSAME70Q21B
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1360937237/app_commands.o: ../src/app_commands.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app_commands.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app_commands.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/app_commands.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1360937237/app_commands.o.d" -o ${OBJECTDIR}/_ext/1360937237/app_commands.o ../src/app_commands.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/initialization.o: ../src/config/sam_e70_xult/initialization.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/initialization.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/initialization.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/initialization.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/initialization.o.d" -o ${OBJECTDIR}/_ext/1728594169/initialization.o ../src/config/sam_e70_xult/initialization.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/interrupts.o: ../src/config/sam_e70_xult/interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/interrupts.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/interrupts.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/interrupts.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/interrupts.o.d" -o ${OBJECTDIR}/_ext/1728594169/interrupts.o ../src/config/sam_e70_xult/interrupts.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/exceptions.o: ../src/config/sam_e70_xult/exceptions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/exceptions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/exceptions.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/exceptions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/exceptions.o.d" -o ${OBJECTDIR}/_ext/1728594169/exceptions.o ../src/config/sam_e70_xult/exceptions.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1518761459/xc32_monitor.o: ../src/config/sam_e70_xult/stdio/xc32_monitor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1518761459" 
	@${RM} ${OBJECTDIR}/_ext/1518761459/xc32_monitor.o.d 
	@${RM} ${OBJECTDIR}/_ext/1518761459/xc32_monitor.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1518761459/xc32_monitor.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1518761459/xc32_monitor.o.d" -o ${OBJECTDIR}/_ext/1518761459/xc32_monitor.o ../src/config/sam_e70_xult/stdio/xc32_monitor.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1055500827/plib_clk.o: ../src/config/sam_e70_xult/peripheral/clk/plib_clk.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1055500827" 
	@${RM} ${OBJECTDIR}/_ext/1055500827/plib_clk.o.d 
	@${RM} ${OBJECTDIR}/_ext/1055500827/plib_clk.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055500827/plib_clk.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1055500827/plib_clk.o.d" -o ${OBJECTDIR}/_ext/1055500827/plib_clk.o ../src/config/sam_e70_xult/peripheral/clk/plib_clk.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1055488423/plib_pio.o: ../src/config/sam_e70_xult/peripheral/pio/plib_pio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1055488423" 
	@${RM} ${OBJECTDIR}/_ext/1055488423/plib_pio.o.d 
	@${RM} ${OBJECTDIR}/_ext/1055488423/plib_pio.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055488423/plib_pio.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1055488423/plib_pio.o.d" -o ${OBJECTDIR}/_ext/1055488423/plib_pio.o ../src/config/sam_e70_xult/peripheral/pio/plib_pio.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1639550079/plib_nvic.o: ../src/config/sam_e70_xult/peripheral/nvic/plib_nvic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1639550079" 
	@${RM} ${OBJECTDIR}/_ext/1639550079/plib_nvic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1639550079/plib_nvic.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1639550079/plib_nvic.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1639550079/plib_nvic.o.d" -o ${OBJECTDIR}/_ext/1639550079/plib_nvic.o ../src/config/sam_e70_xult/peripheral/nvic/plib_nvic.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1055491083/plib_mpu.o: ../src/config/sam_e70_xult/peripheral/mpu/plib_mpu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1055491083" 
	@${RM} ${OBJECTDIR}/_ext/1055491083/plib_mpu.o.d 
	@${RM} ${OBJECTDIR}/_ext/1055491083/plib_mpu.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055491083/plib_mpu.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1055491083/plib_mpu.o.d" -o ${OBJECTDIR}/_ext/1055491083/plib_mpu.o ../src/config/sam_e70_xult/peripheral/mpu/plib_mpu.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/startup.o: ../src/config/sam_e70_xult/startup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/startup.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/startup.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/startup.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/startup.o.d" -o ${OBJECTDIR}/_ext/1728594169/startup.o ../src/config/sam_e70_xult/startup.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/libc_syscalls.o: ../src/config/sam_e70_xult/libc_syscalls.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/libc_syscalls.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/libc_syscalls.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/libc_syscalls.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/libc_syscalls.o.d" -o ${OBJECTDIR}/_ext/1728594169/libc_syscalls.o ../src/config/sam_e70_xult/libc_syscalls.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/766002106/sys_int.o: ../src/config/sam_e70_xult/system/int/src/sys_int.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/766002106" 
	@${RM} ${OBJECTDIR}/_ext/766002106/sys_int.o.d 
	@${RM} ${OBJECTDIR}/_ext/766002106/sys_int.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/766002106/sys_int.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/766002106/sys_int.o.d" -o ${OBJECTDIR}/_ext/766002106/sys_int.o ../src/config/sam_e70_xult/system/int/src/sys_int.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/app.o: ../src/app.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/app.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1360937237/app.o.d" -o ${OBJECTDIR}/_ext/1360937237/app.o ../src/app.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/tasks.o: ../src/config/sam_e70_xult/tasks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/tasks.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/tasks.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/tasks.o.d" -o ${OBJECTDIR}/_ext/1728594169/tasks.o ../src/config/sam_e70_xult/tasks.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1015456894/drv_ethphy.o: ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_ethphy.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1015456894" 
	@${RM} ${OBJECTDIR}/_ext/1015456894/drv_ethphy.o.d 
	@${RM} ${OBJECTDIR}/_ext/1015456894/drv_ethphy.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1015456894/drv_ethphy.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1015456894/drv_ethphy.o.d" -o ${OBJECTDIR}/_ext/1015456894/drv_ethphy.o ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_ethphy.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o: ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_extphy_smsc8740.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1015456894" 
	@${RM} ${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o.d 
	@${RM} ${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o.d" -o ${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_extphy_smsc8740.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/478869302/drv_miim.o: ../src/config/sam_e70_xult/driver/miim/src/dynamic/drv_miim.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/478869302" 
	@${RM} ${OBJECTDIR}/_ext/478869302/drv_miim.o.d 
	@${RM} ${OBJECTDIR}/_ext/478869302/drv_miim.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/478869302/drv_miim.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/478869302/drv_miim.o.d" -o ${OBJECTDIR}/_ext/478869302/drv_miim.o ../src/config/sam_e70_xult/driver/miim/src/dynamic/drv_miim.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/629088514/drv_gmac.o: ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/629088514" 
	@${RM} ${OBJECTDIR}/_ext/629088514/drv_gmac.o.d 
	@${RM} ${OBJECTDIR}/_ext/629088514/drv_gmac.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/629088514/drv_gmac.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/629088514/drv_gmac.o.d" -o ${OBJECTDIR}/_ext/629088514/drv_gmac.o ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o: ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac_lib.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/629088514" 
	@${RM} ${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o.d 
	@${RM} ${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o.d" -o ${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac_lib.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/581080307/net_pres.o: ../src/config/sam_e70_xult/net/pres/src/net_pres.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/581080307" 
	@${RM} ${OBJECTDIR}/_ext/581080307/net_pres.o.d 
	@${RM} ${OBJECTDIR}/_ext/581080307/net_pres.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581080307/net_pres.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/581080307/net_pres.o.d" -o ${OBJECTDIR}/_ext/581080307/net_pres.o ../src/config/sam_e70_xult/net/pres/src/net_pres.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o: ../src/config/sam_e70_xult/net/pres/net_pres_enc_glue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2090003384" 
	@${RM} ${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o.d 
	@${RM} ${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o.d" -o ${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o ../src/config/sam_e70_xult/net/pres/net_pres_enc_glue.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1953779166/sys_console.o: ../src/config/sam_e70_xult/system/console/src/sys_console.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1953779166" 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_console.o.d 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_console.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1953779166/sys_console.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1953779166/sys_console.o.d" -o ${OBJECTDIR}/_ext/1953779166/sys_console.o ../src/config/sam_e70_xult/system/console/src/sys_console.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1953779166/sys_console_uart.o: ../src/config/sam_e70_xult/system/console/src/sys_console_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1953779166" 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_console_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_console_uart.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1953779166/sys_console_uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1953779166/sys_console_uart.o.d" -o ${OBJECTDIR}/_ext/1953779166/sys_console_uart.o ../src/config/sam_e70_xult/system/console/src/sys_console_uart.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1953779166/sys_debug.o: ../src/config/sam_e70_xult/system/console/src/sys_debug.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1953779166" 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_debug.o.d 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_debug.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1953779166/sys_debug.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1953779166/sys_debug.o.d" -o ${OBJECTDIR}/_ext/1953779166/sys_debug.o ../src/config/sam_e70_xult/system/console/src/sys_debug.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1953779166/sys_command.o: ../src/config/sam_e70_xult/system/console/src/sys_command.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1953779166" 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_command.o.d 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_command.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1953779166/sys_command.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1953779166/sys_command.o.d" -o ${OBJECTDIR}/_ext/1953779166/sys_command.o ../src/config/sam_e70_xult/system/console/src/sys_command.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/327803284/sys_time.o: ../src/config/sam_e70_xult/system/time/src/sys_time.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/327803284" 
	@${RM} ${OBJECTDIR}/_ext/327803284/sys_time.o.d 
	@${RM} ${OBJECTDIR}/_ext/327803284/sys_time.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/327803284/sys_time.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/327803284/sys_time.o.d" -o ${OBJECTDIR}/_ext/327803284/sys_time.o ../src/config/sam_e70_xult/system/time/src/sys_time.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/865331892/plib_tc0.o: ../src/config/sam_e70_xult/peripheral/tc/plib_tc0.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/865331892" 
	@${RM} ${OBJECTDIR}/_ext/865331892/plib_tc0.o.d 
	@${RM} ${OBJECTDIR}/_ext/865331892/plib_tc0.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/865331892/plib_tc0.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/865331892/plib_tc0.o.d" -o ${OBJECTDIR}/_ext/865331892/plib_tc0.o ../src/config/sam_e70_xult/peripheral/tc/plib_tc0.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/arp.o: ../src/config/sam_e70_xult/library/tcpip/src/arp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/arp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/arp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/arp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/arp.o.d" -o ${OBJECTDIR}/_ext/902702131/arp.o ../src/config/sam_e70_xult/library/tcpip/src/arp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/berkeley_api.o: ../src/config/sam_e70_xult/library/tcpip/src/berkeley_api.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/berkeley_api.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/berkeley_api.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/berkeley_api.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/berkeley_api.o.d" -o ${OBJECTDIR}/_ext/902702131/berkeley_api.o ../src/config/sam_e70_xult/library/tcpip/src/berkeley_api.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_commands.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_commands.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_commands.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_commands.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_commands.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_commands.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_commands.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_commands.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/dhcp.o: ../src/config/sam_e70_xult/library/tcpip/src/dhcp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/dhcp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/dhcp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/dhcp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/dhcp.o.d" -o ${OBJECTDIR}/_ext/902702131/dhcp.o ../src/config/sam_e70_xult/library/tcpip/src/dhcp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/dns.o: ../src/config/sam_e70_xult/library/tcpip/src/dns.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/dns.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/dns.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/dns.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/dns.o.d" -o ${OBJECTDIR}/_ext/902702131/dns.o ../src/config/sam_e70_xult/library/tcpip/src/dns.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/ipv4.o: ../src/config/sam_e70_xult/library/tcpip/src/ipv4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/ipv4.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/ipv4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/ipv4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/ipv4.o.d" -o ${OBJECTDIR}/_ext/902702131/ipv4.o ../src/config/sam_e70_xult/library/tcpip/src/ipv4.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/icmp.o: ../src/config/sam_e70_xult/library/tcpip/src/icmp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/icmp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/icmp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/icmp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/icmp.o.d" -o ${OBJECTDIR}/_ext/902702131/icmp.o ../src/config/sam_e70_xult/library/tcpip/src/icmp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_alloc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_alloc.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_internal.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_internal.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/helpers.o: ../src/config/sam_e70_xult/library/tcpip/src/helpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/helpers.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/helpers.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/helpers.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/helpers.o.d" -o ${OBJECTDIR}/_ext/902702131/helpers.o ../src/config/sam_e70_xult/library/tcpip/src/helpers.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/hash_fnv.o: ../src/config/sam_e70_xult/library/tcpip/src/hash_fnv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/hash_fnv.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/hash_fnv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/hash_fnv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/hash_fnv.o.d" -o ${OBJECTDIR}/_ext/902702131/hash_fnv.o ../src/config/sam_e70_xult/library/tcpip/src/hash_fnv.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/oahash.o: ../src/config/sam_e70_xult/library/tcpip/src/oahash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/oahash.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/oahash.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/oahash.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/oahash.o.d" -o ${OBJECTDIR}/_ext/902702131/oahash.o ../src/config/sam_e70_xult/library/tcpip/src/oahash.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_helpers.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_helpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_helpers.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_helpers.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_helpers.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_helpers.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_helpers.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_helpers.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_manager.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_manager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_manager.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_manager.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_manager.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_manager.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_manager.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_manager.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_notify.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_notify.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_notify.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_notify.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_notify.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_notify.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_notify.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_notify.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_packet.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_packet.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_packet.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_packet.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_packet.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_packet.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_packet.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_packet.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o: ../src/config/sam_e70_xult/system/sys_time_h2_adapter.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/157960187" 
	@${RM} ${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o.d 
	@${RM} ${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o.d" -o ${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o ../src/config/sam_e70_xult/system/sys_time_h2_adapter.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcp.o: ../src/config/sam_e70_xult/library/tcpip/src/tcp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcp.o.d" -o ${OBJECTDIR}/_ext/902702131/tcp.o ../src/config/sam_e70_xult/library/tcpip/src/tcp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/udp.o: ../src/config/sam_e70_xult/library/tcpip/src/udp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/udp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/udp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/udp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/udp.o.d" -o ${OBJECTDIR}/_ext/902702131/udp.o ../src/config/sam_e70_xult/library/tcpip/src/udp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/707186936/plib_usart1.o: ../src/config/sam_e70_xult/peripheral/usart/plib_usart1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/707186936" 
	@${RM} ${OBJECTDIR}/_ext/707186936/plib_usart1.o.d 
	@${RM} ${OBJECTDIR}/_ext/707186936/plib_usart1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/707186936/plib_usart1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -DSimulator=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/707186936/plib_usart1.o.d" -o ${OBJECTDIR}/_ext/707186936/plib_usart1.o ../src/config/sam_e70_xult/peripheral/usart/plib_usart1.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
else
${OBJECTDIR}/_ext/1360937237/app_commands.o: ../src/app_commands.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app_commands.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app_commands.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/app_commands.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1360937237/app_commands.o.d" -o ${OBJECTDIR}/_ext/1360937237/app_commands.o ../src/app_commands.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/initialization.o: ../src/config/sam_e70_xult/initialization.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/initialization.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/initialization.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/initialization.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/initialization.o.d" -o ${OBJECTDIR}/_ext/1728594169/initialization.o ../src/config/sam_e70_xult/initialization.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/interrupts.o: ../src/config/sam_e70_xult/interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/interrupts.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/interrupts.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/interrupts.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/interrupts.o.d" -o ${OBJECTDIR}/_ext/1728594169/interrupts.o ../src/config/sam_e70_xult/interrupts.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/exceptions.o: ../src/config/sam_e70_xult/exceptions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/exceptions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/exceptions.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/exceptions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/exceptions.o.d" -o ${OBJECTDIR}/_ext/1728594169/exceptions.o ../src/config/sam_e70_xult/exceptions.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1518761459/xc32_monitor.o: ../src/config/sam_e70_xult/stdio/xc32_monitor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1518761459" 
	@${RM} ${OBJECTDIR}/_ext/1518761459/xc32_monitor.o.d 
	@${RM} ${OBJECTDIR}/_ext/1518761459/xc32_monitor.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1518761459/xc32_monitor.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1518761459/xc32_monitor.o.d" -o ${OBJECTDIR}/_ext/1518761459/xc32_monitor.o ../src/config/sam_e70_xult/stdio/xc32_monitor.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1055500827/plib_clk.o: ../src/config/sam_e70_xult/peripheral/clk/plib_clk.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1055500827" 
	@${RM} ${OBJECTDIR}/_ext/1055500827/plib_clk.o.d 
	@${RM} ${OBJECTDIR}/_ext/1055500827/plib_clk.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055500827/plib_clk.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1055500827/plib_clk.o.d" -o ${OBJECTDIR}/_ext/1055500827/plib_clk.o ../src/config/sam_e70_xult/peripheral/clk/plib_clk.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1055488423/plib_pio.o: ../src/config/sam_e70_xult/peripheral/pio/plib_pio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1055488423" 
	@${RM} ${OBJECTDIR}/_ext/1055488423/plib_pio.o.d 
	@${RM} ${OBJECTDIR}/_ext/1055488423/plib_pio.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055488423/plib_pio.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1055488423/plib_pio.o.d" -o ${OBJECTDIR}/_ext/1055488423/plib_pio.o ../src/config/sam_e70_xult/peripheral/pio/plib_pio.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1639550079/plib_nvic.o: ../src/config/sam_e70_xult/peripheral/nvic/plib_nvic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1639550079" 
	@${RM} ${OBJECTDIR}/_ext/1639550079/plib_nvic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1639550079/plib_nvic.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1639550079/plib_nvic.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1639550079/plib_nvic.o.d" -o ${OBJECTDIR}/_ext/1639550079/plib_nvic.o ../src/config/sam_e70_xult/peripheral/nvic/plib_nvic.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1055491083/plib_mpu.o: ../src/config/sam_e70_xult/peripheral/mpu/plib_mpu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1055491083" 
	@${RM} ${OBJECTDIR}/_ext/1055491083/plib_mpu.o.d 
	@${RM} ${OBJECTDIR}/_ext/1055491083/plib_mpu.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055491083/plib_mpu.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1055491083/plib_mpu.o.d" -o ${OBJECTDIR}/_ext/1055491083/plib_mpu.o ../src/config/sam_e70_xult/peripheral/mpu/plib_mpu.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/startup.o: ../src/config/sam_e70_xult/startup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/startup.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/startup.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/startup.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/startup.o.d" -o ${OBJECTDIR}/_ext/1728594169/startup.o ../src/config/sam_e70_xult/startup.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/libc_syscalls.o: ../src/config/sam_e70_xult/libc_syscalls.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/libc_syscalls.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/libc_syscalls.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/libc_syscalls.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/libc_syscalls.o.d" -o ${OBJECTDIR}/_ext/1728594169/libc_syscalls.o ../src/config/sam_e70_xult/libc_syscalls.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/766002106/sys_int.o: ../src/config/sam_e70_xult/system/int/src/sys_int.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/766002106" 
	@${RM} ${OBJECTDIR}/_ext/766002106/sys_int.o.d 
	@${RM} ${OBJECTDIR}/_ext/766002106/sys_int.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/766002106/sys_int.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/766002106/sys_int.o.d" -o ${OBJECTDIR}/_ext/766002106/sys_int.o ../src/config/sam_e70_xult/system/int/src/sys_int.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/app.o: ../src/app.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/app.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1360937237/app.o.d" -o ${OBJECTDIR}/_ext/1360937237/app.o ../src/app.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/tasks.o: ../src/config/sam_e70_xult/tasks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/tasks.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/tasks.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/tasks.o.d" -o ${OBJECTDIR}/_ext/1728594169/tasks.o ../src/config/sam_e70_xult/tasks.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1015456894/drv_ethphy.o: ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_ethphy.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1015456894" 
	@${RM} ${OBJECTDIR}/_ext/1015456894/drv_ethphy.o.d 
	@${RM} ${OBJECTDIR}/_ext/1015456894/drv_ethphy.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1015456894/drv_ethphy.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1015456894/drv_ethphy.o.d" -o ${OBJECTDIR}/_ext/1015456894/drv_ethphy.o ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_ethphy.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o: ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_extphy_smsc8740.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1015456894" 
	@${RM} ${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o.d 
	@${RM} ${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o.d" -o ${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_extphy_smsc8740.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/478869302/drv_miim.o: ../src/config/sam_e70_xult/driver/miim/src/dynamic/drv_miim.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/478869302" 
	@${RM} ${OBJECTDIR}/_ext/478869302/drv_miim.o.d 
	@${RM} ${OBJECTDIR}/_ext/478869302/drv_miim.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/478869302/drv_miim.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/478869302/drv_miim.o.d" -o ${OBJECTDIR}/_ext/478869302/drv_miim.o ../src/config/sam_e70_xult/driver/miim/src/dynamic/drv_miim.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/629088514/drv_gmac.o: ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/629088514" 
	@${RM} ${OBJECTDIR}/_ext/629088514/drv_gmac.o.d 
	@${RM} ${OBJECTDIR}/_ext/629088514/drv_gmac.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/629088514/drv_gmac.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/629088514/drv_gmac.o.d" -o ${OBJECTDIR}/_ext/629088514/drv_gmac.o ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o: ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac_lib.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/629088514" 
	@${RM} ${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o.d 
	@${RM} ${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o.d" -o ${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac_lib.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/581080307/net_pres.o: ../src/config/sam_e70_xult/net/pres/src/net_pres.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/581080307" 
	@${RM} ${OBJECTDIR}/_ext/581080307/net_pres.o.d 
	@${RM} ${OBJECTDIR}/_ext/581080307/net_pres.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581080307/net_pres.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/581080307/net_pres.o.d" -o ${OBJECTDIR}/_ext/581080307/net_pres.o ../src/config/sam_e70_xult/net/pres/src/net_pres.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o: ../src/config/sam_e70_xult/net/pres/net_pres_enc_glue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2090003384" 
	@${RM} ${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o.d 
	@${RM} ${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o.d" -o ${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o ../src/config/sam_e70_xult/net/pres/net_pres_enc_glue.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1953779166/sys_console.o: ../src/config/sam_e70_xult/system/console/src/sys_console.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1953779166" 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_console.o.d 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_console.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1953779166/sys_console.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1953779166/sys_console.o.d" -o ${OBJECTDIR}/_ext/1953779166/sys_console.o ../src/config/sam_e70_xult/system/console/src/sys_console.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1953779166/sys_console_uart.o: ../src/config/sam_e70_xult/system/console/src/sys_console_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1953779166" 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_console_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_console_uart.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1953779166/sys_console_uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1953779166/sys_console_uart.o.d" -o ${OBJECTDIR}/_ext/1953779166/sys_console_uart.o ../src/config/sam_e70_xult/system/console/src/sys_console_uart.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1953779166/sys_debug.o: ../src/config/sam_e70_xult/system/console/src/sys_debug.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1953779166" 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_debug.o.d 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_debug.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1953779166/sys_debug.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1953779166/sys_debug.o.d" -o ${OBJECTDIR}/_ext/1953779166/sys_debug.o ../src/config/sam_e70_xult/system/console/src/sys_debug.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1953779166/sys_command.o: ../src/config/sam_e70_xult/system/console/src/sys_command.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1953779166" 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_command.o.d 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_command.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1953779166/sys_command.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/1953779166/sys_command.o.d" -o ${OBJECTDIR}/_ext/1953779166/sys_command.o ../src/config/sam_e70_xult/system/console/src/sys_command.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/327803284/sys_time.o: ../src/config/sam_e70_xult/system/time/src/sys_time.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/327803284" 
	@${RM} ${OBJECTDIR}/_ext/327803284/sys_time.o.d 
	@${RM} ${OBJECTDIR}/_ext/327803284/sys_time.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/327803284/sys_time.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/327803284/sys_time.o.d" -o ${OBJECTDIR}/_ext/327803284/sys_time.o ../src/config/sam_e70_xult/system/time/src/sys_time.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/865331892/plib_tc0.o: ../src/config/sam_e70_xult/peripheral/tc/plib_tc0.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/865331892" 
	@${RM} ${OBJECTDIR}/_ext/865331892/plib_tc0.o.d 
	@${RM} ${OBJECTDIR}/_ext/865331892/plib_tc0.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/865331892/plib_tc0.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/865331892/plib_tc0.o.d" -o ${OBJECTDIR}/_ext/865331892/plib_tc0.o ../src/config/sam_e70_xult/peripheral/tc/plib_tc0.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/arp.o: ../src/config/sam_e70_xult/library/tcpip/src/arp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/arp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/arp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/arp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/arp.o.d" -o ${OBJECTDIR}/_ext/902702131/arp.o ../src/config/sam_e70_xult/library/tcpip/src/arp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/berkeley_api.o: ../src/config/sam_e70_xult/library/tcpip/src/berkeley_api.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/berkeley_api.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/berkeley_api.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/berkeley_api.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/berkeley_api.o.d" -o ${OBJECTDIR}/_ext/902702131/berkeley_api.o ../src/config/sam_e70_xult/library/tcpip/src/berkeley_api.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_commands.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_commands.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_commands.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_commands.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_commands.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_commands.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_commands.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_commands.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/dhcp.o: ../src/config/sam_e70_xult/library/tcpip/src/dhcp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/dhcp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/dhcp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/dhcp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/dhcp.o.d" -o ${OBJECTDIR}/_ext/902702131/dhcp.o ../src/config/sam_e70_xult/library/tcpip/src/dhcp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/dns.o: ../src/config/sam_e70_xult/library/tcpip/src/dns.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/dns.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/dns.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/dns.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/dns.o.d" -o ${OBJECTDIR}/_ext/902702131/dns.o ../src/config/sam_e70_xult/library/tcpip/src/dns.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/ipv4.o: ../src/config/sam_e70_xult/library/tcpip/src/ipv4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/ipv4.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/ipv4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/ipv4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/ipv4.o.d" -o ${OBJECTDIR}/_ext/902702131/ipv4.o ../src/config/sam_e70_xult/library/tcpip/src/ipv4.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/icmp.o: ../src/config/sam_e70_xult/library/tcpip/src/icmp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/icmp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/icmp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/icmp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/icmp.o.d" -o ${OBJECTDIR}/_ext/902702131/icmp.o ../src/config/sam_e70_xult/library/tcpip/src/icmp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_alloc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_alloc.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_internal.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_internal.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/helpers.o: ../src/config/sam_e70_xult/library/tcpip/src/helpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/helpers.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/helpers.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/helpers.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/helpers.o.d" -o ${OBJECTDIR}/_ext/902702131/helpers.o ../src/config/sam_e70_xult/library/tcpip/src/helpers.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/hash_fnv.o: ../src/config/sam_e70_xult/library/tcpip/src/hash_fnv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/hash_fnv.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/hash_fnv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/hash_fnv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/hash_fnv.o.d" -o ${OBJECTDIR}/_ext/902702131/hash_fnv.o ../src/config/sam_e70_xult/library/tcpip/src/hash_fnv.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/oahash.o: ../src/config/sam_e70_xult/library/tcpip/src/oahash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/oahash.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/oahash.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/oahash.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/oahash.o.d" -o ${OBJECTDIR}/_ext/902702131/oahash.o ../src/config/sam_e70_xult/library/tcpip/src/oahash.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_helpers.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_helpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_helpers.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_helpers.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_helpers.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_helpers.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_helpers.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_helpers.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_manager.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_manager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_manager.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_manager.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_manager.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_manager.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_manager.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_manager.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_notify.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_notify.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_notify.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_notify.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_notify.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_notify.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_notify.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_notify.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_packet.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_packet.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_packet.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_packet.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_packet.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_packet.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_packet.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_packet.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o: ../src/config/sam_e70_xult/system/sys_time_h2_adapter.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/157960187" 
	@${RM} ${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o.d 
	@${RM} ${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o.d" -o ${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o ../src/config/sam_e70_xult/system/sys_time_h2_adapter.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcp.o: ../src/config/sam_e70_xult/library/tcpip/src/tcp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcp.o.d" -o ${OBJECTDIR}/_ext/902702131/tcp.o ../src/config/sam_e70_xult/library/tcpip/src/tcp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/udp.o: ../src/config/sam_e70_xult/library/tcpip/src/udp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/udp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/udp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/udp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/udp.o.d" -o ${OBJECTDIR}/_ext/902702131/udp.o ../src/config/sam_e70_xult/library/tcpip/src/udp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/707186936/plib_usart1.o: ../src/config/sam_e70_xult/peripheral/usart/plib_usart1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/707186936" 
	@${RM} ${OBJECTDIR}/_ext/707186936/plib_usart1.o.d 
	@${RM} ${OBJECTDIR}/_ext/707186936/plib_usart1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/707186936/plib_usart1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Wall -MMD -MF "${OBJECTDIR}/_ext/707186936/plib_usart1.o.d" -o ${OBJECTDIR}/_ext/707186936/plib_usart1.o ../src/config/sam_e70_xult/peripheral/usart/plib_usart1.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/berkeley_udp_client.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -g  -DSimulator=1 -mprocessor=$(MP_PROCESSOR_OPTION) -mno-device-startup-code -o dist/${CND_CONF}/${IMAGE_TYPE}/berkeley_udp_client.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D=__DEBUG_D,--defsym=Simulator=1,--defsym=_min_heap_size=44960,--gc-sections,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/berkeley_udp_client.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION) -mno-device-startup-code -o dist/${CND_CONF}/${IMAGE_TYPE}/berkeley_udp_client.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=44960,--gc-sections,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml
	${MP_CC_DIR}\\xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/berkeley_udp_client.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/sam_e70_xult
	${RM} -r dist/sam_e70_xult

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
