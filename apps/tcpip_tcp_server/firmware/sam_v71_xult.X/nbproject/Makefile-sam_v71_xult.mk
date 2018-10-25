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
ifeq "$(wildcard nbproject/Makefile-local-sam_v71_xult.mk)" "nbproject/Makefile-local-sam_v71_xult.mk"
include nbproject/Makefile-local-sam_v71_xult.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=sam_v71_xult
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/sam_v71_xult.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/sam_v71_xult.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
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
SOURCEFILES_QUOTED_IF_SPACED=../src/main.c ../src/config/sam_v71_xult/initialization.c ../src/config/sam_v71_xult/interrupts.c ../src/config/sam_v71_xult/exceptions.c ../src/config/sam_v71_xult/stdio/xc32_monitor.c ../src/config/sam_v71_xult/peripheral/clk/plib_clk.c ../src/config/sam_v71_xult/peripheral/pio/plib_pio.c ../src/config/sam_v71_xult/peripheral/nvic/plib_nvic.c ../src/config/sam_v71_xult/peripheral/mpu/plib_mpu.c ../src/config/sam_v71_xult/startup.c ../src/config/sam_v71_xult/libc_syscalls.c ../src/config/sam_v71_xult/bsp/bsp.c ../src/config/sam_v71_xult/system/int/src/sys_int.c ../src/app.c ../src/config/sam_v71_xult/tasks.c ../src/config/sam_v71_xult/driver/ethphy/src/dynamic/drv_ethphy.c ../src/config/sam_v71_xult/driver/ethphy/src/dynamic/drv_extphy_ksz8061.c ../src/config/sam_v71_xult/driver/miim/src/dynamic/drv_miim.c ../src/config/sam_v71_xult/driver/gmac/src/dynamic/drv_gmac.c ../src/config/sam_v71_xult/driver/gmac/src/dynamic/drv_gmac_lib.c ../src/config/sam_v71_xult/system/console/src/sys_console.c ../src/config/sam_v71_xult/system/console/src/sys_console_uart.c ../src/config/sam_v71_xult/system/console/src/sys_debug.c ../src/config/sam_v71_xult/system/console/src/sys_command.c ../src/config/sam_v71_xult/system/time/src/sys_time.c ../src/config/sam_v71_xult/peripheral/tc/plib_tc0.c ../src/config/sam_v71_xult/library/tcpip/src/arp.c ../src/config/sam_v71_xult/library/tcpip/src/tcpip_commands.c ../src/config/sam_v71_xult/library/tcpip/src/dhcp.c ../src/config/sam_v71_xult/library/tcpip/src/ipv4.c ../src/config/sam_v71_xult/library/tcpip/src/icmp.c ../src/config/sam_v71_xult/library/tcpip/src/tcpip_heap_alloc.c ../src/config/sam_v71_xult/library/tcpip/src/tcpip_heap_internal.c ../src/config/sam_v71_xult/library/tcpip/src/helpers.c ../src/config/sam_v71_xult/library/tcpip/src/hash_fnv.c ../src/config/sam_v71_xult/library/tcpip/src/oahash.c ../src/config/sam_v71_xult/library/tcpip/src/tcpip_helpers.c ../src/config/sam_v71_xult/library/tcpip/src/tcpip_manager.c ../src/config/sam_v71_xult/library/tcpip/src/tcpip_notify.c ../src/config/sam_v71_xult/library/tcpip/src/tcpip_packet.c ../src/config/sam_v71_xult/system/sys_time_h2_adapter.c ../src/config/sam_v71_xult/library/tcpip/src/tcp.c ../src/config/sam_v71_xult/library/tcpip/src/udp.c ../src/config/sam_v71_xult/peripheral/usart/plib_usart1.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1360937237/main.o ${OBJECTDIR}/_ext/1320227943/initialization.o ${OBJECTDIR}/_ext/1320227943/interrupts.o ${OBJECTDIR}/_ext/1320227943/exceptions.o ${OBJECTDIR}/_ext/1643282145/xc32_monitor.o ${OBJECTDIR}/_ext/1157990217/plib_clk.o ${OBJECTDIR}/_ext/1157977813/plib_pio.o ${OBJECTDIR}/_ext/1537621011/plib_nvic.o ${OBJECTDIR}/_ext/1157980473/plib_mpu.o ${OBJECTDIR}/_ext/1320227943/startup.o ${OBJECTDIR}/_ext/1320227943/libc_syscalls.o ${OBJECTDIR}/_ext/1379310441/bsp.o ${OBJECTDIR}/_ext/663512716/sys_int.o ${OBJECTDIR}/_ext/1360937237/app.o ${OBJECTDIR}/_ext/1320227943/tasks.o ${OBJECTDIR}/_ext/741152148/drv_ethphy.o ${OBJECTDIR}/_ext/741152148/drv_extphy_ksz8061.o ${OBJECTDIR}/_ext/678158500/drv_miim.o ${OBJECTDIR}/_ext/429799316/drv_gmac.o ${OBJECTDIR}/_ext/429799316/drv_gmac_lib.o ${OBJECTDIR}/_ext/568452108/sys_console.o ${OBJECTDIR}/_ext/568452108/sys_console_uart.o ${OBJECTDIR}/_ext/568452108/sys_debug.o ${OBJECTDIR}/_ext/568452108/sys_command.o ${OBJECTDIR}/_ext/789992922/sys_time.o ${OBJECTDIR}/_ext/1561374662/plib_tc0.o ${OBJECTDIR}/_ext/1363032097/arp.o ${OBJECTDIR}/_ext/1363032097/tcpip_commands.o ${OBJECTDIR}/_ext/1363032097/dhcp.o ${OBJECTDIR}/_ext/1363032097/ipv4.o ${OBJECTDIR}/_ext/1363032097/icmp.o ${OBJECTDIR}/_ext/1363032097/tcpip_heap_alloc.o ${OBJECTDIR}/_ext/1363032097/tcpip_heap_internal.o ${OBJECTDIR}/_ext/1363032097/helpers.o ${OBJECTDIR}/_ext/1363032097/hash_fnv.o ${OBJECTDIR}/_ext/1363032097/oahash.o ${OBJECTDIR}/_ext/1363032097/tcpip_helpers.o ${OBJECTDIR}/_ext/1363032097/tcpip_manager.o ${OBJECTDIR}/_ext/1363032097/tcpip_notify.o ${OBJECTDIR}/_ext/1363032097/tcpip_packet.o ${OBJECTDIR}/_ext/592786217/sys_time_h2_adapter.o ${OBJECTDIR}/_ext/1363032097/tcp.o ${OBJECTDIR}/_ext/1363032097/udp.o ${OBJECTDIR}/_ext/415242918/plib_usart1.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1360937237/main.o.d ${OBJECTDIR}/_ext/1320227943/initialization.o.d ${OBJECTDIR}/_ext/1320227943/interrupts.o.d ${OBJECTDIR}/_ext/1320227943/exceptions.o.d ${OBJECTDIR}/_ext/1643282145/xc32_monitor.o.d ${OBJECTDIR}/_ext/1157990217/plib_clk.o.d ${OBJECTDIR}/_ext/1157977813/plib_pio.o.d ${OBJECTDIR}/_ext/1537621011/plib_nvic.o.d ${OBJECTDIR}/_ext/1157980473/plib_mpu.o.d ${OBJECTDIR}/_ext/1320227943/startup.o.d ${OBJECTDIR}/_ext/1320227943/libc_syscalls.o.d ${OBJECTDIR}/_ext/1379310441/bsp.o.d ${OBJECTDIR}/_ext/663512716/sys_int.o.d ${OBJECTDIR}/_ext/1360937237/app.o.d ${OBJECTDIR}/_ext/1320227943/tasks.o.d ${OBJECTDIR}/_ext/741152148/drv_ethphy.o.d ${OBJECTDIR}/_ext/741152148/drv_extphy_ksz8061.o.d ${OBJECTDIR}/_ext/678158500/drv_miim.o.d ${OBJECTDIR}/_ext/429799316/drv_gmac.o.d ${OBJECTDIR}/_ext/429799316/drv_gmac_lib.o.d ${OBJECTDIR}/_ext/568452108/sys_console.o.d ${OBJECTDIR}/_ext/568452108/sys_console_uart.o.d ${OBJECTDIR}/_ext/568452108/sys_debug.o.d ${OBJECTDIR}/_ext/568452108/sys_command.o.d ${OBJECTDIR}/_ext/789992922/sys_time.o.d ${OBJECTDIR}/_ext/1561374662/plib_tc0.o.d ${OBJECTDIR}/_ext/1363032097/arp.o.d ${OBJECTDIR}/_ext/1363032097/tcpip_commands.o.d ${OBJECTDIR}/_ext/1363032097/dhcp.o.d ${OBJECTDIR}/_ext/1363032097/ipv4.o.d ${OBJECTDIR}/_ext/1363032097/icmp.o.d ${OBJECTDIR}/_ext/1363032097/tcpip_heap_alloc.o.d ${OBJECTDIR}/_ext/1363032097/tcpip_heap_internal.o.d ${OBJECTDIR}/_ext/1363032097/helpers.o.d ${OBJECTDIR}/_ext/1363032097/hash_fnv.o.d ${OBJECTDIR}/_ext/1363032097/oahash.o.d ${OBJECTDIR}/_ext/1363032097/tcpip_helpers.o.d ${OBJECTDIR}/_ext/1363032097/tcpip_manager.o.d ${OBJECTDIR}/_ext/1363032097/tcpip_notify.o.d ${OBJECTDIR}/_ext/1363032097/tcpip_packet.o.d ${OBJECTDIR}/_ext/592786217/sys_time_h2_adapter.o.d ${OBJECTDIR}/_ext/1363032097/tcp.o.d ${OBJECTDIR}/_ext/1363032097/udp.o.d ${OBJECTDIR}/_ext/415242918/plib_usart1.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1360937237/main.o ${OBJECTDIR}/_ext/1320227943/initialization.o ${OBJECTDIR}/_ext/1320227943/interrupts.o ${OBJECTDIR}/_ext/1320227943/exceptions.o ${OBJECTDIR}/_ext/1643282145/xc32_monitor.o ${OBJECTDIR}/_ext/1157990217/plib_clk.o ${OBJECTDIR}/_ext/1157977813/plib_pio.o ${OBJECTDIR}/_ext/1537621011/plib_nvic.o ${OBJECTDIR}/_ext/1157980473/plib_mpu.o ${OBJECTDIR}/_ext/1320227943/startup.o ${OBJECTDIR}/_ext/1320227943/libc_syscalls.o ${OBJECTDIR}/_ext/1379310441/bsp.o ${OBJECTDIR}/_ext/663512716/sys_int.o ${OBJECTDIR}/_ext/1360937237/app.o ${OBJECTDIR}/_ext/1320227943/tasks.o ${OBJECTDIR}/_ext/741152148/drv_ethphy.o ${OBJECTDIR}/_ext/741152148/drv_extphy_ksz8061.o ${OBJECTDIR}/_ext/678158500/drv_miim.o ${OBJECTDIR}/_ext/429799316/drv_gmac.o ${OBJECTDIR}/_ext/429799316/drv_gmac_lib.o ${OBJECTDIR}/_ext/568452108/sys_console.o ${OBJECTDIR}/_ext/568452108/sys_console_uart.o ${OBJECTDIR}/_ext/568452108/sys_debug.o ${OBJECTDIR}/_ext/568452108/sys_command.o ${OBJECTDIR}/_ext/789992922/sys_time.o ${OBJECTDIR}/_ext/1561374662/plib_tc0.o ${OBJECTDIR}/_ext/1363032097/arp.o ${OBJECTDIR}/_ext/1363032097/tcpip_commands.o ${OBJECTDIR}/_ext/1363032097/dhcp.o ${OBJECTDIR}/_ext/1363032097/ipv4.o ${OBJECTDIR}/_ext/1363032097/icmp.o ${OBJECTDIR}/_ext/1363032097/tcpip_heap_alloc.o ${OBJECTDIR}/_ext/1363032097/tcpip_heap_internal.o ${OBJECTDIR}/_ext/1363032097/helpers.o ${OBJECTDIR}/_ext/1363032097/hash_fnv.o ${OBJECTDIR}/_ext/1363032097/oahash.o ${OBJECTDIR}/_ext/1363032097/tcpip_helpers.o ${OBJECTDIR}/_ext/1363032097/tcpip_manager.o ${OBJECTDIR}/_ext/1363032097/tcpip_notify.o ${OBJECTDIR}/_ext/1363032097/tcpip_packet.o ${OBJECTDIR}/_ext/592786217/sys_time_h2_adapter.o ${OBJECTDIR}/_ext/1363032097/tcp.o ${OBJECTDIR}/_ext/1363032097/udp.o ${OBJECTDIR}/_ext/415242918/plib_usart1.o

# Source Files
SOURCEFILES=../src/main.c ../src/config/sam_v71_xult/initialization.c ../src/config/sam_v71_xult/interrupts.c ../src/config/sam_v71_xult/exceptions.c ../src/config/sam_v71_xult/stdio/xc32_monitor.c ../src/config/sam_v71_xult/peripheral/clk/plib_clk.c ../src/config/sam_v71_xult/peripheral/pio/plib_pio.c ../src/config/sam_v71_xult/peripheral/nvic/plib_nvic.c ../src/config/sam_v71_xult/peripheral/mpu/plib_mpu.c ../src/config/sam_v71_xult/startup.c ../src/config/sam_v71_xult/libc_syscalls.c ../src/config/sam_v71_xult/bsp/bsp.c ../src/config/sam_v71_xult/system/int/src/sys_int.c ../src/app.c ../src/config/sam_v71_xult/tasks.c ../src/config/sam_v71_xult/driver/ethphy/src/dynamic/drv_ethphy.c ../src/config/sam_v71_xult/driver/ethphy/src/dynamic/drv_extphy_ksz8061.c ../src/config/sam_v71_xult/driver/miim/src/dynamic/drv_miim.c ../src/config/sam_v71_xult/driver/gmac/src/dynamic/drv_gmac.c ../src/config/sam_v71_xult/driver/gmac/src/dynamic/drv_gmac_lib.c ../src/config/sam_v71_xult/system/console/src/sys_console.c ../src/config/sam_v71_xult/system/console/src/sys_console_uart.c ../src/config/sam_v71_xult/system/console/src/sys_debug.c ../src/config/sam_v71_xult/system/console/src/sys_command.c ../src/config/sam_v71_xult/system/time/src/sys_time.c ../src/config/sam_v71_xult/peripheral/tc/plib_tc0.c ../src/config/sam_v71_xult/library/tcpip/src/arp.c ../src/config/sam_v71_xult/library/tcpip/src/tcpip_commands.c ../src/config/sam_v71_xult/library/tcpip/src/dhcp.c ../src/config/sam_v71_xult/library/tcpip/src/ipv4.c ../src/config/sam_v71_xult/library/tcpip/src/icmp.c ../src/config/sam_v71_xult/library/tcpip/src/tcpip_heap_alloc.c ../src/config/sam_v71_xult/library/tcpip/src/tcpip_heap_internal.c ../src/config/sam_v71_xult/library/tcpip/src/helpers.c ../src/config/sam_v71_xult/library/tcpip/src/hash_fnv.c ../src/config/sam_v71_xult/library/tcpip/src/oahash.c ../src/config/sam_v71_xult/library/tcpip/src/tcpip_helpers.c ../src/config/sam_v71_xult/library/tcpip/src/tcpip_manager.c ../src/config/sam_v71_xult/library/tcpip/src/tcpip_notify.c ../src/config/sam_v71_xult/library/tcpip/src/tcpip_packet.c ../src/config/sam_v71_xult/system/sys_time_h2_adapter.c ../src/config/sam_v71_xult/library/tcpip/src/tcp.c ../src/config/sam_v71_xult/library/tcpip/src/udp.c ../src/config/sam_v71_xult/peripheral/usart/plib_usart1.c


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
	${MAKE}  -f nbproject/Makefile-sam_v71_xult.mk dist/${CND_CONF}/${IMAGE_TYPE}/sam_v71_xult.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=ATSAMV71Q21B
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
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1320227943/initialization.o: ../src/config/sam_v71_xult/initialization.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/initialization.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/initialization.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1320227943/initialization.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1320227943/initialization.o.d" -o ${OBJECTDIR}/_ext/1320227943/initialization.o ../src/config/sam_v71_xult/initialization.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1320227943/interrupts.o: ../src/config/sam_v71_xult/interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/interrupts.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/interrupts.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1320227943/interrupts.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1320227943/interrupts.o.d" -o ${OBJECTDIR}/_ext/1320227943/interrupts.o ../src/config/sam_v71_xult/interrupts.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1320227943/exceptions.o: ../src/config/sam_v71_xult/exceptions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/exceptions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/exceptions.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1320227943/exceptions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1320227943/exceptions.o.d" -o ${OBJECTDIR}/_ext/1320227943/exceptions.o ../src/config/sam_v71_xult/exceptions.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1643282145/xc32_monitor.o: ../src/config/sam_v71_xult/stdio/xc32_monitor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1643282145" 
	@${RM} ${OBJECTDIR}/_ext/1643282145/xc32_monitor.o.d 
	@${RM} ${OBJECTDIR}/_ext/1643282145/xc32_monitor.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1643282145/xc32_monitor.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1643282145/xc32_monitor.o.d" -o ${OBJECTDIR}/_ext/1643282145/xc32_monitor.o ../src/config/sam_v71_xult/stdio/xc32_monitor.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1157990217/plib_clk.o: ../src/config/sam_v71_xult/peripheral/clk/plib_clk.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1157990217" 
	@${RM} ${OBJECTDIR}/_ext/1157990217/plib_clk.o.d 
	@${RM} ${OBJECTDIR}/_ext/1157990217/plib_clk.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1157990217/plib_clk.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1157990217/plib_clk.o.d" -o ${OBJECTDIR}/_ext/1157990217/plib_clk.o ../src/config/sam_v71_xult/peripheral/clk/plib_clk.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1157977813/plib_pio.o: ../src/config/sam_v71_xult/peripheral/pio/plib_pio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1157977813" 
	@${RM} ${OBJECTDIR}/_ext/1157977813/plib_pio.o.d 
	@${RM} ${OBJECTDIR}/_ext/1157977813/plib_pio.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1157977813/plib_pio.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1157977813/plib_pio.o.d" -o ${OBJECTDIR}/_ext/1157977813/plib_pio.o ../src/config/sam_v71_xult/peripheral/pio/plib_pio.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1537621011/plib_nvic.o: ../src/config/sam_v71_xult/peripheral/nvic/plib_nvic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1537621011" 
	@${RM} ${OBJECTDIR}/_ext/1537621011/plib_nvic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1537621011/plib_nvic.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1537621011/plib_nvic.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1537621011/plib_nvic.o.d" -o ${OBJECTDIR}/_ext/1537621011/plib_nvic.o ../src/config/sam_v71_xult/peripheral/nvic/plib_nvic.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1157980473/plib_mpu.o: ../src/config/sam_v71_xult/peripheral/mpu/plib_mpu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1157980473" 
	@${RM} ${OBJECTDIR}/_ext/1157980473/plib_mpu.o.d 
	@${RM} ${OBJECTDIR}/_ext/1157980473/plib_mpu.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1157980473/plib_mpu.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1157980473/plib_mpu.o.d" -o ${OBJECTDIR}/_ext/1157980473/plib_mpu.o ../src/config/sam_v71_xult/peripheral/mpu/plib_mpu.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1320227943/startup.o: ../src/config/sam_v71_xult/startup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/startup.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/startup.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1320227943/startup.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1320227943/startup.o.d" -o ${OBJECTDIR}/_ext/1320227943/startup.o ../src/config/sam_v71_xult/startup.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1320227943/libc_syscalls.o: ../src/config/sam_v71_xult/libc_syscalls.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/libc_syscalls.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/libc_syscalls.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1320227943/libc_syscalls.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1320227943/libc_syscalls.o.d" -o ${OBJECTDIR}/_ext/1320227943/libc_syscalls.o ../src/config/sam_v71_xult/libc_syscalls.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1379310441/bsp.o: ../src/config/sam_v71_xult/bsp/bsp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1379310441" 
	@${RM} ${OBJECTDIR}/_ext/1379310441/bsp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1379310441/bsp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1379310441/bsp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1379310441/bsp.o.d" -o ${OBJECTDIR}/_ext/1379310441/bsp.o ../src/config/sam_v71_xult/bsp/bsp.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/663512716/sys_int.o: ../src/config/sam_v71_xult/system/int/src/sys_int.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/663512716" 
	@${RM} ${OBJECTDIR}/_ext/663512716/sys_int.o.d 
	@${RM} ${OBJECTDIR}/_ext/663512716/sys_int.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/663512716/sys_int.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/663512716/sys_int.o.d" -o ${OBJECTDIR}/_ext/663512716/sys_int.o ../src/config/sam_v71_xult/system/int/src/sys_int.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/app.o: ../src/app.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/app.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1360937237/app.o.d" -o ${OBJECTDIR}/_ext/1360937237/app.o ../src/app.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1320227943/tasks.o: ../src/config/sam_v71_xult/tasks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/tasks.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1320227943/tasks.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1320227943/tasks.o.d" -o ${OBJECTDIR}/_ext/1320227943/tasks.o ../src/config/sam_v71_xult/tasks.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/741152148/drv_ethphy.o: ../src/config/sam_v71_xult/driver/ethphy/src/dynamic/drv_ethphy.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/741152148" 
	@${RM} ${OBJECTDIR}/_ext/741152148/drv_ethphy.o.d 
	@${RM} ${OBJECTDIR}/_ext/741152148/drv_ethphy.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/741152148/drv_ethphy.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/741152148/drv_ethphy.o.d" -o ${OBJECTDIR}/_ext/741152148/drv_ethphy.o ../src/config/sam_v71_xult/driver/ethphy/src/dynamic/drv_ethphy.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/741152148/drv_extphy_ksz8061.o: ../src/config/sam_v71_xult/driver/ethphy/src/dynamic/drv_extphy_ksz8061.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/741152148" 
	@${RM} ${OBJECTDIR}/_ext/741152148/drv_extphy_ksz8061.o.d 
	@${RM} ${OBJECTDIR}/_ext/741152148/drv_extphy_ksz8061.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/741152148/drv_extphy_ksz8061.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/741152148/drv_extphy_ksz8061.o.d" -o ${OBJECTDIR}/_ext/741152148/drv_extphy_ksz8061.o ../src/config/sam_v71_xult/driver/ethphy/src/dynamic/drv_extphy_ksz8061.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/678158500/drv_miim.o: ../src/config/sam_v71_xult/driver/miim/src/dynamic/drv_miim.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/678158500" 
	@${RM} ${OBJECTDIR}/_ext/678158500/drv_miim.o.d 
	@${RM} ${OBJECTDIR}/_ext/678158500/drv_miim.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/678158500/drv_miim.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/678158500/drv_miim.o.d" -o ${OBJECTDIR}/_ext/678158500/drv_miim.o ../src/config/sam_v71_xult/driver/miim/src/dynamic/drv_miim.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/429799316/drv_gmac.o: ../src/config/sam_v71_xult/driver/gmac/src/dynamic/drv_gmac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/429799316" 
	@${RM} ${OBJECTDIR}/_ext/429799316/drv_gmac.o.d 
	@${RM} ${OBJECTDIR}/_ext/429799316/drv_gmac.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/429799316/drv_gmac.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/429799316/drv_gmac.o.d" -o ${OBJECTDIR}/_ext/429799316/drv_gmac.o ../src/config/sam_v71_xult/driver/gmac/src/dynamic/drv_gmac.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/429799316/drv_gmac_lib.o: ../src/config/sam_v71_xult/driver/gmac/src/dynamic/drv_gmac_lib.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/429799316" 
	@${RM} ${OBJECTDIR}/_ext/429799316/drv_gmac_lib.o.d 
	@${RM} ${OBJECTDIR}/_ext/429799316/drv_gmac_lib.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/429799316/drv_gmac_lib.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/429799316/drv_gmac_lib.o.d" -o ${OBJECTDIR}/_ext/429799316/drv_gmac_lib.o ../src/config/sam_v71_xult/driver/gmac/src/dynamic/drv_gmac_lib.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/568452108/sys_console.o: ../src/config/sam_v71_xult/system/console/src/sys_console.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/568452108" 
	@${RM} ${OBJECTDIR}/_ext/568452108/sys_console.o.d 
	@${RM} ${OBJECTDIR}/_ext/568452108/sys_console.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/568452108/sys_console.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/568452108/sys_console.o.d" -o ${OBJECTDIR}/_ext/568452108/sys_console.o ../src/config/sam_v71_xult/system/console/src/sys_console.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/568452108/sys_console_uart.o: ../src/config/sam_v71_xult/system/console/src/sys_console_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/568452108" 
	@${RM} ${OBJECTDIR}/_ext/568452108/sys_console_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/568452108/sys_console_uart.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/568452108/sys_console_uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/568452108/sys_console_uart.o.d" -o ${OBJECTDIR}/_ext/568452108/sys_console_uart.o ../src/config/sam_v71_xult/system/console/src/sys_console_uart.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/568452108/sys_debug.o: ../src/config/sam_v71_xult/system/console/src/sys_debug.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/568452108" 
	@${RM} ${OBJECTDIR}/_ext/568452108/sys_debug.o.d 
	@${RM} ${OBJECTDIR}/_ext/568452108/sys_debug.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/568452108/sys_debug.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/568452108/sys_debug.o.d" -o ${OBJECTDIR}/_ext/568452108/sys_debug.o ../src/config/sam_v71_xult/system/console/src/sys_debug.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/568452108/sys_command.o: ../src/config/sam_v71_xult/system/console/src/sys_command.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/568452108" 
	@${RM} ${OBJECTDIR}/_ext/568452108/sys_command.o.d 
	@${RM} ${OBJECTDIR}/_ext/568452108/sys_command.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/568452108/sys_command.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/568452108/sys_command.o.d" -o ${OBJECTDIR}/_ext/568452108/sys_command.o ../src/config/sam_v71_xult/system/console/src/sys_command.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/789992922/sys_time.o: ../src/config/sam_v71_xult/system/time/src/sys_time.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/789992922" 
	@${RM} ${OBJECTDIR}/_ext/789992922/sys_time.o.d 
	@${RM} ${OBJECTDIR}/_ext/789992922/sys_time.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/789992922/sys_time.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/789992922/sys_time.o.d" -o ${OBJECTDIR}/_ext/789992922/sys_time.o ../src/config/sam_v71_xult/system/time/src/sys_time.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1561374662/plib_tc0.o: ../src/config/sam_v71_xult/peripheral/tc/plib_tc0.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1561374662" 
	@${RM} ${OBJECTDIR}/_ext/1561374662/plib_tc0.o.d 
	@${RM} ${OBJECTDIR}/_ext/1561374662/plib_tc0.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1561374662/plib_tc0.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1561374662/plib_tc0.o.d" -o ${OBJECTDIR}/_ext/1561374662/plib_tc0.o ../src/config/sam_v71_xult/peripheral/tc/plib_tc0.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/arp.o: ../src/config/sam_v71_xult/library/tcpip/src/arp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/arp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/arp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/arp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/arp.o.d" -o ${OBJECTDIR}/_ext/1363032097/arp.o ../src/config/sam_v71_xult/library/tcpip/src/arp.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/tcpip_commands.o: ../src/config/sam_v71_xult/library/tcpip/src/tcpip_commands.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_commands.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_commands.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/tcpip_commands.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/tcpip_commands.o.d" -o ${OBJECTDIR}/_ext/1363032097/tcpip_commands.o ../src/config/sam_v71_xult/library/tcpip/src/tcpip_commands.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/dhcp.o: ../src/config/sam_v71_xult/library/tcpip/src/dhcp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/dhcp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/dhcp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/dhcp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/dhcp.o.d" -o ${OBJECTDIR}/_ext/1363032097/dhcp.o ../src/config/sam_v71_xult/library/tcpip/src/dhcp.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/ipv4.o: ../src/config/sam_v71_xult/library/tcpip/src/ipv4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/ipv4.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/ipv4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/ipv4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/ipv4.o.d" -o ${OBJECTDIR}/_ext/1363032097/ipv4.o ../src/config/sam_v71_xult/library/tcpip/src/ipv4.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/icmp.o: ../src/config/sam_v71_xult/library/tcpip/src/icmp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/icmp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/icmp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/icmp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/icmp.o.d" -o ${OBJECTDIR}/_ext/1363032097/icmp.o ../src/config/sam_v71_xult/library/tcpip/src/icmp.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/tcpip_heap_alloc.o: ../src/config/sam_v71_xult/library/tcpip/src/tcpip_heap_alloc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_heap_alloc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_heap_alloc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/tcpip_heap_alloc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/tcpip_heap_alloc.o.d" -o ${OBJECTDIR}/_ext/1363032097/tcpip_heap_alloc.o ../src/config/sam_v71_xult/library/tcpip/src/tcpip_heap_alloc.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/tcpip_heap_internal.o: ../src/config/sam_v71_xult/library/tcpip/src/tcpip_heap_internal.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_heap_internal.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_heap_internal.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/tcpip_heap_internal.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/tcpip_heap_internal.o.d" -o ${OBJECTDIR}/_ext/1363032097/tcpip_heap_internal.o ../src/config/sam_v71_xult/library/tcpip/src/tcpip_heap_internal.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/helpers.o: ../src/config/sam_v71_xult/library/tcpip/src/helpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/helpers.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/helpers.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/helpers.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/helpers.o.d" -o ${OBJECTDIR}/_ext/1363032097/helpers.o ../src/config/sam_v71_xult/library/tcpip/src/helpers.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/hash_fnv.o: ../src/config/sam_v71_xult/library/tcpip/src/hash_fnv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/hash_fnv.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/hash_fnv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/hash_fnv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/hash_fnv.o.d" -o ${OBJECTDIR}/_ext/1363032097/hash_fnv.o ../src/config/sam_v71_xult/library/tcpip/src/hash_fnv.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/oahash.o: ../src/config/sam_v71_xult/library/tcpip/src/oahash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/oahash.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/oahash.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/oahash.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/oahash.o.d" -o ${OBJECTDIR}/_ext/1363032097/oahash.o ../src/config/sam_v71_xult/library/tcpip/src/oahash.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/tcpip_helpers.o: ../src/config/sam_v71_xult/library/tcpip/src/tcpip_helpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_helpers.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_helpers.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/tcpip_helpers.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/tcpip_helpers.o.d" -o ${OBJECTDIR}/_ext/1363032097/tcpip_helpers.o ../src/config/sam_v71_xult/library/tcpip/src/tcpip_helpers.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/tcpip_manager.o: ../src/config/sam_v71_xult/library/tcpip/src/tcpip_manager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_manager.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_manager.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/tcpip_manager.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/tcpip_manager.o.d" -o ${OBJECTDIR}/_ext/1363032097/tcpip_manager.o ../src/config/sam_v71_xult/library/tcpip/src/tcpip_manager.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/tcpip_notify.o: ../src/config/sam_v71_xult/library/tcpip/src/tcpip_notify.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_notify.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_notify.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/tcpip_notify.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/tcpip_notify.o.d" -o ${OBJECTDIR}/_ext/1363032097/tcpip_notify.o ../src/config/sam_v71_xult/library/tcpip/src/tcpip_notify.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/tcpip_packet.o: ../src/config/sam_v71_xult/library/tcpip/src/tcpip_packet.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_packet.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_packet.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/tcpip_packet.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/tcpip_packet.o.d" -o ${OBJECTDIR}/_ext/1363032097/tcpip_packet.o ../src/config/sam_v71_xult/library/tcpip/src/tcpip_packet.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/592786217/sys_time_h2_adapter.o: ../src/config/sam_v71_xult/system/sys_time_h2_adapter.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/592786217" 
	@${RM} ${OBJECTDIR}/_ext/592786217/sys_time_h2_adapter.o.d 
	@${RM} ${OBJECTDIR}/_ext/592786217/sys_time_h2_adapter.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592786217/sys_time_h2_adapter.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/592786217/sys_time_h2_adapter.o.d" -o ${OBJECTDIR}/_ext/592786217/sys_time_h2_adapter.o ../src/config/sam_v71_xult/system/sys_time_h2_adapter.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/tcp.o: ../src/config/sam_v71_xult/library/tcpip/src/tcp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/tcp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/tcp.o.d" -o ${OBJECTDIR}/_ext/1363032097/tcp.o ../src/config/sam_v71_xult/library/tcpip/src/tcp.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/udp.o: ../src/config/sam_v71_xult/library/tcpip/src/udp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/udp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/udp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/udp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/udp.o.d" -o ${OBJECTDIR}/_ext/1363032097/udp.o ../src/config/sam_v71_xult/library/tcpip/src/udp.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/415242918/plib_usart1.o: ../src/config/sam_v71_xult/peripheral/usart/plib_usart1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/415242918" 
	@${RM} ${OBJECTDIR}/_ext/415242918/plib_usart1.o.d 
	@${RM} ${OBJECTDIR}/_ext/415242918/plib_usart1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/415242918/plib_usart1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/415242918/plib_usart1.o.d" -o ${OBJECTDIR}/_ext/415242918/plib_usart1.o ../src/config/sam_v71_xult/peripheral/usart/plib_usart1.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
else
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1320227943/initialization.o: ../src/config/sam_v71_xult/initialization.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/initialization.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/initialization.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1320227943/initialization.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1320227943/initialization.o.d" -o ${OBJECTDIR}/_ext/1320227943/initialization.o ../src/config/sam_v71_xult/initialization.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1320227943/interrupts.o: ../src/config/sam_v71_xult/interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/interrupts.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/interrupts.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1320227943/interrupts.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1320227943/interrupts.o.d" -o ${OBJECTDIR}/_ext/1320227943/interrupts.o ../src/config/sam_v71_xult/interrupts.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1320227943/exceptions.o: ../src/config/sam_v71_xult/exceptions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/exceptions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/exceptions.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1320227943/exceptions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1320227943/exceptions.o.d" -o ${OBJECTDIR}/_ext/1320227943/exceptions.o ../src/config/sam_v71_xult/exceptions.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1643282145/xc32_monitor.o: ../src/config/sam_v71_xult/stdio/xc32_monitor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1643282145" 
	@${RM} ${OBJECTDIR}/_ext/1643282145/xc32_monitor.o.d 
	@${RM} ${OBJECTDIR}/_ext/1643282145/xc32_monitor.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1643282145/xc32_monitor.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1643282145/xc32_monitor.o.d" -o ${OBJECTDIR}/_ext/1643282145/xc32_monitor.o ../src/config/sam_v71_xult/stdio/xc32_monitor.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1157990217/plib_clk.o: ../src/config/sam_v71_xult/peripheral/clk/plib_clk.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1157990217" 
	@${RM} ${OBJECTDIR}/_ext/1157990217/plib_clk.o.d 
	@${RM} ${OBJECTDIR}/_ext/1157990217/plib_clk.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1157990217/plib_clk.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1157990217/plib_clk.o.d" -o ${OBJECTDIR}/_ext/1157990217/plib_clk.o ../src/config/sam_v71_xult/peripheral/clk/plib_clk.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1157977813/plib_pio.o: ../src/config/sam_v71_xult/peripheral/pio/plib_pio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1157977813" 
	@${RM} ${OBJECTDIR}/_ext/1157977813/plib_pio.o.d 
	@${RM} ${OBJECTDIR}/_ext/1157977813/plib_pio.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1157977813/plib_pio.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1157977813/plib_pio.o.d" -o ${OBJECTDIR}/_ext/1157977813/plib_pio.o ../src/config/sam_v71_xult/peripheral/pio/plib_pio.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1537621011/plib_nvic.o: ../src/config/sam_v71_xult/peripheral/nvic/plib_nvic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1537621011" 
	@${RM} ${OBJECTDIR}/_ext/1537621011/plib_nvic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1537621011/plib_nvic.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1537621011/plib_nvic.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1537621011/plib_nvic.o.d" -o ${OBJECTDIR}/_ext/1537621011/plib_nvic.o ../src/config/sam_v71_xult/peripheral/nvic/plib_nvic.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1157980473/plib_mpu.o: ../src/config/sam_v71_xult/peripheral/mpu/plib_mpu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1157980473" 
	@${RM} ${OBJECTDIR}/_ext/1157980473/plib_mpu.o.d 
	@${RM} ${OBJECTDIR}/_ext/1157980473/plib_mpu.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1157980473/plib_mpu.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1157980473/plib_mpu.o.d" -o ${OBJECTDIR}/_ext/1157980473/plib_mpu.o ../src/config/sam_v71_xult/peripheral/mpu/plib_mpu.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1320227943/startup.o: ../src/config/sam_v71_xult/startup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/startup.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/startup.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1320227943/startup.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1320227943/startup.o.d" -o ${OBJECTDIR}/_ext/1320227943/startup.o ../src/config/sam_v71_xult/startup.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1320227943/libc_syscalls.o: ../src/config/sam_v71_xult/libc_syscalls.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/libc_syscalls.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/libc_syscalls.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1320227943/libc_syscalls.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1320227943/libc_syscalls.o.d" -o ${OBJECTDIR}/_ext/1320227943/libc_syscalls.o ../src/config/sam_v71_xult/libc_syscalls.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1379310441/bsp.o: ../src/config/sam_v71_xult/bsp/bsp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1379310441" 
	@${RM} ${OBJECTDIR}/_ext/1379310441/bsp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1379310441/bsp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1379310441/bsp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1379310441/bsp.o.d" -o ${OBJECTDIR}/_ext/1379310441/bsp.o ../src/config/sam_v71_xult/bsp/bsp.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/663512716/sys_int.o: ../src/config/sam_v71_xult/system/int/src/sys_int.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/663512716" 
	@${RM} ${OBJECTDIR}/_ext/663512716/sys_int.o.d 
	@${RM} ${OBJECTDIR}/_ext/663512716/sys_int.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/663512716/sys_int.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/663512716/sys_int.o.d" -o ${OBJECTDIR}/_ext/663512716/sys_int.o ../src/config/sam_v71_xult/system/int/src/sys_int.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/app.o: ../src/app.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/app.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1360937237/app.o.d" -o ${OBJECTDIR}/_ext/1360937237/app.o ../src/app.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1320227943/tasks.o: ../src/config/sam_v71_xult/tasks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/tasks.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1320227943/tasks.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1320227943/tasks.o.d" -o ${OBJECTDIR}/_ext/1320227943/tasks.o ../src/config/sam_v71_xult/tasks.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/741152148/drv_ethphy.o: ../src/config/sam_v71_xult/driver/ethphy/src/dynamic/drv_ethphy.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/741152148" 
	@${RM} ${OBJECTDIR}/_ext/741152148/drv_ethphy.o.d 
	@${RM} ${OBJECTDIR}/_ext/741152148/drv_ethphy.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/741152148/drv_ethphy.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/741152148/drv_ethphy.o.d" -o ${OBJECTDIR}/_ext/741152148/drv_ethphy.o ../src/config/sam_v71_xult/driver/ethphy/src/dynamic/drv_ethphy.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/741152148/drv_extphy_ksz8061.o: ../src/config/sam_v71_xult/driver/ethphy/src/dynamic/drv_extphy_ksz8061.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/741152148" 
	@${RM} ${OBJECTDIR}/_ext/741152148/drv_extphy_ksz8061.o.d 
	@${RM} ${OBJECTDIR}/_ext/741152148/drv_extphy_ksz8061.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/741152148/drv_extphy_ksz8061.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/741152148/drv_extphy_ksz8061.o.d" -o ${OBJECTDIR}/_ext/741152148/drv_extphy_ksz8061.o ../src/config/sam_v71_xult/driver/ethphy/src/dynamic/drv_extphy_ksz8061.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/678158500/drv_miim.o: ../src/config/sam_v71_xult/driver/miim/src/dynamic/drv_miim.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/678158500" 
	@${RM} ${OBJECTDIR}/_ext/678158500/drv_miim.o.d 
	@${RM} ${OBJECTDIR}/_ext/678158500/drv_miim.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/678158500/drv_miim.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/678158500/drv_miim.o.d" -o ${OBJECTDIR}/_ext/678158500/drv_miim.o ../src/config/sam_v71_xult/driver/miim/src/dynamic/drv_miim.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/429799316/drv_gmac.o: ../src/config/sam_v71_xult/driver/gmac/src/dynamic/drv_gmac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/429799316" 
	@${RM} ${OBJECTDIR}/_ext/429799316/drv_gmac.o.d 
	@${RM} ${OBJECTDIR}/_ext/429799316/drv_gmac.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/429799316/drv_gmac.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/429799316/drv_gmac.o.d" -o ${OBJECTDIR}/_ext/429799316/drv_gmac.o ../src/config/sam_v71_xult/driver/gmac/src/dynamic/drv_gmac.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/429799316/drv_gmac_lib.o: ../src/config/sam_v71_xult/driver/gmac/src/dynamic/drv_gmac_lib.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/429799316" 
	@${RM} ${OBJECTDIR}/_ext/429799316/drv_gmac_lib.o.d 
	@${RM} ${OBJECTDIR}/_ext/429799316/drv_gmac_lib.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/429799316/drv_gmac_lib.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/429799316/drv_gmac_lib.o.d" -o ${OBJECTDIR}/_ext/429799316/drv_gmac_lib.o ../src/config/sam_v71_xult/driver/gmac/src/dynamic/drv_gmac_lib.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/568452108/sys_console.o: ../src/config/sam_v71_xult/system/console/src/sys_console.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/568452108" 
	@${RM} ${OBJECTDIR}/_ext/568452108/sys_console.o.d 
	@${RM} ${OBJECTDIR}/_ext/568452108/sys_console.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/568452108/sys_console.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/568452108/sys_console.o.d" -o ${OBJECTDIR}/_ext/568452108/sys_console.o ../src/config/sam_v71_xult/system/console/src/sys_console.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/568452108/sys_console_uart.o: ../src/config/sam_v71_xult/system/console/src/sys_console_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/568452108" 
	@${RM} ${OBJECTDIR}/_ext/568452108/sys_console_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/568452108/sys_console_uart.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/568452108/sys_console_uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/568452108/sys_console_uart.o.d" -o ${OBJECTDIR}/_ext/568452108/sys_console_uart.o ../src/config/sam_v71_xult/system/console/src/sys_console_uart.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/568452108/sys_debug.o: ../src/config/sam_v71_xult/system/console/src/sys_debug.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/568452108" 
	@${RM} ${OBJECTDIR}/_ext/568452108/sys_debug.o.d 
	@${RM} ${OBJECTDIR}/_ext/568452108/sys_debug.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/568452108/sys_debug.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/568452108/sys_debug.o.d" -o ${OBJECTDIR}/_ext/568452108/sys_debug.o ../src/config/sam_v71_xult/system/console/src/sys_debug.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/568452108/sys_command.o: ../src/config/sam_v71_xult/system/console/src/sys_command.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/568452108" 
	@${RM} ${OBJECTDIR}/_ext/568452108/sys_command.o.d 
	@${RM} ${OBJECTDIR}/_ext/568452108/sys_command.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/568452108/sys_command.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/568452108/sys_command.o.d" -o ${OBJECTDIR}/_ext/568452108/sys_command.o ../src/config/sam_v71_xult/system/console/src/sys_command.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/789992922/sys_time.o: ../src/config/sam_v71_xult/system/time/src/sys_time.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/789992922" 
	@${RM} ${OBJECTDIR}/_ext/789992922/sys_time.o.d 
	@${RM} ${OBJECTDIR}/_ext/789992922/sys_time.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/789992922/sys_time.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/789992922/sys_time.o.d" -o ${OBJECTDIR}/_ext/789992922/sys_time.o ../src/config/sam_v71_xult/system/time/src/sys_time.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1561374662/plib_tc0.o: ../src/config/sam_v71_xult/peripheral/tc/plib_tc0.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1561374662" 
	@${RM} ${OBJECTDIR}/_ext/1561374662/plib_tc0.o.d 
	@${RM} ${OBJECTDIR}/_ext/1561374662/plib_tc0.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1561374662/plib_tc0.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1561374662/plib_tc0.o.d" -o ${OBJECTDIR}/_ext/1561374662/plib_tc0.o ../src/config/sam_v71_xult/peripheral/tc/plib_tc0.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/arp.o: ../src/config/sam_v71_xult/library/tcpip/src/arp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/arp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/arp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/arp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/arp.o.d" -o ${OBJECTDIR}/_ext/1363032097/arp.o ../src/config/sam_v71_xult/library/tcpip/src/arp.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/tcpip_commands.o: ../src/config/sam_v71_xult/library/tcpip/src/tcpip_commands.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_commands.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_commands.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/tcpip_commands.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/tcpip_commands.o.d" -o ${OBJECTDIR}/_ext/1363032097/tcpip_commands.o ../src/config/sam_v71_xult/library/tcpip/src/tcpip_commands.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/dhcp.o: ../src/config/sam_v71_xult/library/tcpip/src/dhcp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/dhcp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/dhcp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/dhcp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/dhcp.o.d" -o ${OBJECTDIR}/_ext/1363032097/dhcp.o ../src/config/sam_v71_xult/library/tcpip/src/dhcp.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/ipv4.o: ../src/config/sam_v71_xult/library/tcpip/src/ipv4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/ipv4.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/ipv4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/ipv4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/ipv4.o.d" -o ${OBJECTDIR}/_ext/1363032097/ipv4.o ../src/config/sam_v71_xult/library/tcpip/src/ipv4.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/icmp.o: ../src/config/sam_v71_xult/library/tcpip/src/icmp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/icmp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/icmp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/icmp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/icmp.o.d" -o ${OBJECTDIR}/_ext/1363032097/icmp.o ../src/config/sam_v71_xult/library/tcpip/src/icmp.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/tcpip_heap_alloc.o: ../src/config/sam_v71_xult/library/tcpip/src/tcpip_heap_alloc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_heap_alloc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_heap_alloc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/tcpip_heap_alloc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/tcpip_heap_alloc.o.d" -o ${OBJECTDIR}/_ext/1363032097/tcpip_heap_alloc.o ../src/config/sam_v71_xult/library/tcpip/src/tcpip_heap_alloc.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/tcpip_heap_internal.o: ../src/config/sam_v71_xult/library/tcpip/src/tcpip_heap_internal.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_heap_internal.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_heap_internal.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/tcpip_heap_internal.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/tcpip_heap_internal.o.d" -o ${OBJECTDIR}/_ext/1363032097/tcpip_heap_internal.o ../src/config/sam_v71_xult/library/tcpip/src/tcpip_heap_internal.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/helpers.o: ../src/config/sam_v71_xult/library/tcpip/src/helpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/helpers.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/helpers.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/helpers.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/helpers.o.d" -o ${OBJECTDIR}/_ext/1363032097/helpers.o ../src/config/sam_v71_xult/library/tcpip/src/helpers.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/hash_fnv.o: ../src/config/sam_v71_xult/library/tcpip/src/hash_fnv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/hash_fnv.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/hash_fnv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/hash_fnv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/hash_fnv.o.d" -o ${OBJECTDIR}/_ext/1363032097/hash_fnv.o ../src/config/sam_v71_xult/library/tcpip/src/hash_fnv.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/oahash.o: ../src/config/sam_v71_xult/library/tcpip/src/oahash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/oahash.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/oahash.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/oahash.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/oahash.o.d" -o ${OBJECTDIR}/_ext/1363032097/oahash.o ../src/config/sam_v71_xult/library/tcpip/src/oahash.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/tcpip_helpers.o: ../src/config/sam_v71_xult/library/tcpip/src/tcpip_helpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_helpers.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_helpers.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/tcpip_helpers.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/tcpip_helpers.o.d" -o ${OBJECTDIR}/_ext/1363032097/tcpip_helpers.o ../src/config/sam_v71_xult/library/tcpip/src/tcpip_helpers.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/tcpip_manager.o: ../src/config/sam_v71_xult/library/tcpip/src/tcpip_manager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_manager.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_manager.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/tcpip_manager.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/tcpip_manager.o.d" -o ${OBJECTDIR}/_ext/1363032097/tcpip_manager.o ../src/config/sam_v71_xult/library/tcpip/src/tcpip_manager.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/tcpip_notify.o: ../src/config/sam_v71_xult/library/tcpip/src/tcpip_notify.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_notify.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_notify.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/tcpip_notify.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/tcpip_notify.o.d" -o ${OBJECTDIR}/_ext/1363032097/tcpip_notify.o ../src/config/sam_v71_xult/library/tcpip/src/tcpip_notify.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/tcpip_packet.o: ../src/config/sam_v71_xult/library/tcpip/src/tcpip_packet.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_packet.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcpip_packet.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/tcpip_packet.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/tcpip_packet.o.d" -o ${OBJECTDIR}/_ext/1363032097/tcpip_packet.o ../src/config/sam_v71_xult/library/tcpip/src/tcpip_packet.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/592786217/sys_time_h2_adapter.o: ../src/config/sam_v71_xult/system/sys_time_h2_adapter.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/592786217" 
	@${RM} ${OBJECTDIR}/_ext/592786217/sys_time_h2_adapter.o.d 
	@${RM} ${OBJECTDIR}/_ext/592786217/sys_time_h2_adapter.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592786217/sys_time_h2_adapter.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/592786217/sys_time_h2_adapter.o.d" -o ${OBJECTDIR}/_ext/592786217/sys_time_h2_adapter.o ../src/config/sam_v71_xult/system/sys_time_h2_adapter.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/tcp.o: ../src/config/sam_v71_xult/library/tcpip/src/tcp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/tcp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/tcp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/tcp.o.d" -o ${OBJECTDIR}/_ext/1363032097/tcp.o ../src/config/sam_v71_xult/library/tcpip/src/tcp.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1363032097/udp.o: ../src/config/sam_v71_xult/library/tcpip/src/udp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1363032097" 
	@${RM} ${OBJECTDIR}/_ext/1363032097/udp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1363032097/udp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1363032097/udp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1363032097/udp.o.d" -o ${OBJECTDIR}/_ext/1363032097/udp.o ../src/config/sam_v71_xult/library/tcpip/src/udp.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/415242918/plib_usart1.o: ../src/config/sam_v71_xult/peripheral/usart/plib_usart1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/415242918" 
	@${RM} ${OBJECTDIR}/_ext/415242918/plib_usart1.o.d 
	@${RM} ${OBJECTDIR}/_ext/415242918/plib_usart1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/415242918/plib_usart1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -I"../src/config/sam_v71_xult/library" -I"../src/config/sam_v71_xult/library/tcpip/src" -I"../src/config/sam_v71_xult/library/tcpip/src/common" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/415242918/plib_usart1.o.d" -o ${OBJECTDIR}/_ext/415242918/plib_usart1.o ../src/config/sam_v71_xult/peripheral/usart/plib_usart1.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/sam_v71_xult.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -g  -D__MPLAB_DEBUGGER_SIMULATOR=1 -mprocessor=$(MP_PROCESSOR_OPTION) -mno-device-startup-code -o dist/${CND_CONF}/${IMAGE_TYPE}/sam_v71_xult.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D=__DEBUG_D,--defsym=__MPLAB_DEBUGGER_SIMULATOR=1,--defsym=_min_heap_size=44960,--gc-sections,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/sam_v71_xult.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION) -mno-device-startup-code -o dist/${CND_CONF}/${IMAGE_TYPE}/sam_v71_xult.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=44960,--gc-sections,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml
	${MP_CC_DIR}\\xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/sam_v71_xult.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/sam_v71_xult
	${RM} -r dist/sam_v71_xult

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
