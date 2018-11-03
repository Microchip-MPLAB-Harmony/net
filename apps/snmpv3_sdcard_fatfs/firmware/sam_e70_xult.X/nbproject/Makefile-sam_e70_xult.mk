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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/sam_e70_xult.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/sam_e70_xult.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
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
SOURCEFILES_QUOTED_IF_SPACED=../src/main.c ../src/config/sam_e70_xult/initialization.c ../src/config/sam_e70_xult/interrupts.c ../src/config/sam_e70_xult/stdio/xc32_monitor.c ../src/config/sam_e70_xult/exceptions.c ../src/config/sam_e70_xult/peripheral/clk/plib_clk.c ../src/config/sam_e70_xult/peripheral/pio/plib_pio.c ../src/config/sam_e70_xult/peripheral/nvic/plib_nvic.c ../src/config/sam_e70_xult/peripheral/mpu/plib_mpu.c ../src/config/sam_e70_xult/peripheral/xdmac/plib_xdmac.c ../src/config/sam_e70_xult/startup.c ../src/config/sam_e70_xult/libc_syscalls.c ../src/config/sam_e70_xult/bsp/bsp.c ../src/config/sam_e70_xult/system/int/src/sys_int.c ../src/app.c ../src/config/sam_e70_xult/tasks.c ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_ethphy.c ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_extphy_smsc8740.c ../src/config/sam_e70_xult/driver/miim/src/dynamic/drv_miim.c ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac.c ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac_lib.c ../src/config/sam_e70_xult/driver/sdhc/src/drv_sdhc_file_system.c ../src/config/sam_e70_xult/driver/sdhc/src/drv_sdhc.c ../src/config/sam_e70_xult/driver/sdhc/src/drv_sdhc_host.c ../src/config/sam_e70_xult/crypto/src/arc4.c ../src/config/sam_e70_xult/crypto/src/asm.c ../src/config/sam_e70_xult/crypto/src/asn.c ../src/config/sam_e70_xult/crypto/src/async.c ../src/config/sam_e70_xult/crypto/src/chacha.c ../src/config/sam_e70_xult/crypto/src/chacha20_poly1305.c ../src/config/sam_e70_xult/crypto/src/cmac.c ../src/config/sam_e70_xult/crypto/src/coding.c ../src/config/sam_e70_xult/crypto/src/compress.c ../src/config/sam_e70_xult/crypto/src/crypto.c ../src/config/sam_e70_xult/crypto/src/curve25519.c ../src/config/sam_e70_xult/crypto/src/des3.c ../src/config/sam_e70_xult/crypto/src/dh.c ../src/config/sam_e70_xult/crypto/src/dsa.c ../src/config/sam_e70_xult/crypto/src/ecc.c ../src/config/sam_e70_xult/crypto/src/ecc_fp.c ../src/config/sam_e70_xult/crypto/src/ed25519.c ../src/config/sam_e70_xult/crypto/src/error.c ../src/config/sam_e70_xult/crypto/src/fe_low_mem.c ../src/config/sam_e70_xult/crypto/src/fe_operations.c ../src/config/sam_e70_xult/crypto/src/ge_low_mem.c ../src/config/sam_e70_xult/crypto/src/ge_operations.c ../src/config/sam_e70_xult/crypto/src/hash.c ../src/config/sam_e70_xult/crypto/src/hc128.c ../src/config/sam_e70_xult/crypto/src/hmac.c ../src/config/sam_e70_xult/crypto/src/idea.c ../src/config/sam_e70_xult/crypto/src/integer.c ../src/config/sam_e70_xult/crypto/src/logging.c ../src/config/sam_e70_xult/crypto/src/md2.c ../src/config/sam_e70_xult/crypto/src/md4.c ../src/config/sam_e70_xult/crypto/src/md5.c ../src/config/sam_e70_xult/crypto/src/memory.c ../src/config/sam_e70_xult/crypto/src/misc.c ../src/config/sam_e70_xult/crypto/src/pkcs12.c ../src/config/sam_e70_xult/crypto/src/pkcs7.c ../src/config/sam_e70_xult/crypto/src/poly1305.c ../src/config/sam_e70_xult/crypto/src/pwdbased.c ../src/config/sam_e70_xult/crypto/src/rabbit.c ../src/config/sam_e70_xult/crypto/src/ripemd.c ../src/config/sam_e70_xult/crypto/src/rsa.c ../src/config/sam_e70_xult/crypto/src/sha3.c ../src/config/sam_e70_xult/crypto/src/sha512.c ../src/config/sam_e70_xult/crypto/src/signature.c ../src/config/sam_e70_xult/crypto/src/srp.c ../src/config/sam_e70_xult/crypto/src/tfm.c ../src/config/sam_e70_xult/crypto/src/wc_encrypt.c ../src/config/sam_e70_xult/crypto/src/wc_port.c ../src/config/sam_e70_xult/crypto/src/wolfevent.c ../src/config/sam_e70_xult/crypto/src/wolfmath.c ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/adler32.c ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/crc32.c ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/deflate.c ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/inflate.c ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/trees.c ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/zutil.c ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/inftrees.c ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/inffast.c ../src/config/sam_e70_xult/crypto/src/aes.c ../src/config/sam_e70_xult/crypto/src/sha.c ../src/config/sam_e70_xult/crypto/src/sha256.c ../src/config/sam_e70_xult/crypto/src/random.c ../src/config/sam_e70_xult/crypto/src/random_same70.c ../src/config/sam_e70_xult/net/pres/src/net_pres.c ../src/config/sam_e70_xult/net/pres/net_pres_enc_glue.c ../src/config/sam_e70_xult/system/console/src/sys_console.c ../src/config/sam_e70_xult/system/console/src/sys_console_uart.c ../src/config/sam_e70_xult/system/console/src/sys_debug.c ../src/config/sam_e70_xult/system/console/src/sys_command.c ../src/config/sam_e70_xult/system/fs/src/sys_fs.c ../src/config/sam_e70_xult/system/fs/src/sys_fs_media_manager.c ../src/config/sam_e70_xult/system/fs/fat_fs/src/ff.c ../src/config/sam_e70_xult/system/fs/fat_fs/src/diskio.c ../src/config/sam_e70_xult/system/fs/mpfs/src/mpfs.c ../src/config/sam_e70_xult/system/time/src/sys_time.c ../src/config/sam_e70_xult/peripheral/tc/plib_tc0.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_announce.c ../src/config/sam_e70_xult/library/tcpip/src/arp.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_commands.c ../src/config/sam_e70_xult/library/tcpip/src/dhcp.c ../src/config/sam_e70_xult/library/tcpip/src/dns.c ../src/config/sam_e70_xult/library/tcpip/src/http.c ../src/config/sam_e70_xult/app/http_print.c ../src/config/sam_e70_xult/app/custom_http_app.c ../src/config/sam_e70_xult/library/tcpip/src/ipv4.c ../src/config/sam_e70_xult/library/tcpip/src/ipv6.c ../src/config/sam_e70_xult/library/tcpip/src/icmpv6.c ../src/config/sam_e70_xult/library/tcpip/src/icmp.c ../src/config/sam_e70_xult/library/tcpip/src/nbns.c ../src/config/sam_e70_xult/library/tcpip/src/ndp.c ../src/config/sam_e70_xult/library/tcpip/src/smtpc.c ../src/config/sam_e70_xult/library/tcpip/src/snmp.c ../src/config/sam_e70_xult/custom_snmp_app.c ../src/config/sam_e70_xult/library/tcpip/src/snmpv3.c ../src/config/sam_e70_xult/library/tcpip/src/snmpv3_usm.c ../src/config/sam_e70_xult/library/tcpip/src/sntp.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_alloc.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_internal.c ../src/config/sam_e70_xult/library/tcpip/src/helpers.c ../src/config/sam_e70_xult/library/tcpip/src/hash_fnv.c ../src/config/sam_e70_xult/library/tcpip/src/oahash.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_helpers.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_manager.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_notify.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_packet.c ../src/config/sam_e70_xult/system/sys_time_h2_adapter.c ../src/config/sam_e70_xult/library/tcpip/src/common/sys_fs_wrapper.c ../src/config/sam_e70_xult/library/tcpip/src/tcp.c ../src/config/sam_e70_xult/library/tcpip/src/udp.c ../src/config/sam_e70_xult/peripheral/usart/plib_usart1.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1360937237/main.o ${OBJECTDIR}/_ext/1728594169/initialization.o ${OBJECTDIR}/_ext/1728594169/interrupts.o ${OBJECTDIR}/_ext/1518761459/xc32_monitor.o ${OBJECTDIR}/_ext/1728594169/exceptions.o ${OBJECTDIR}/_ext/1055500827/plib_clk.o ${OBJECTDIR}/_ext/1055488423/plib_pio.o ${OBJECTDIR}/_ext/1639550079/plib_nvic.o ${OBJECTDIR}/_ext/1055491083/plib_mpu.o ${OBJECTDIR}/_ext/704852250/plib_xdmac.o ${OBJECTDIR}/_ext/1728594169/startup.o ${OBJECTDIR}/_ext/1728594169/libc_syscalls.o ${OBJECTDIR}/_ext/917764137/bsp.o ${OBJECTDIR}/_ext/766002106/sys_int.o ${OBJECTDIR}/_ext/1360937237/app.o ${OBJECTDIR}/_ext/1728594169/tasks.o ${OBJECTDIR}/_ext/1015456894/drv_ethphy.o ${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o ${OBJECTDIR}/_ext/478869302/drv_miim.o ${OBJECTDIR}/_ext/629088514/drv_gmac.o ${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o ${OBJECTDIR}/_ext/2043027118/drv_sdhc_file_system.o ${OBJECTDIR}/_ext/2043027118/drv_sdhc.o ${OBJECTDIR}/_ext/2043027118/drv_sdhc_host.o ${OBJECTDIR}/_ext/675556972/arc4.o ${OBJECTDIR}/_ext/675556972/asm.o ${OBJECTDIR}/_ext/675556972/asn.o ${OBJECTDIR}/_ext/675556972/async.o ${OBJECTDIR}/_ext/675556972/chacha.o ${OBJECTDIR}/_ext/675556972/chacha20_poly1305.o ${OBJECTDIR}/_ext/675556972/cmac.o ${OBJECTDIR}/_ext/675556972/coding.o ${OBJECTDIR}/_ext/675556972/compress.o ${OBJECTDIR}/_ext/675556972/crypto.o ${OBJECTDIR}/_ext/675556972/curve25519.o ${OBJECTDIR}/_ext/675556972/des3.o ${OBJECTDIR}/_ext/675556972/dh.o ${OBJECTDIR}/_ext/675556972/dsa.o ${OBJECTDIR}/_ext/675556972/ecc.o ${OBJECTDIR}/_ext/675556972/ecc_fp.o ${OBJECTDIR}/_ext/675556972/ed25519.o ${OBJECTDIR}/_ext/675556972/error.o ${OBJECTDIR}/_ext/675556972/fe_low_mem.o ${OBJECTDIR}/_ext/675556972/fe_operations.o ${OBJECTDIR}/_ext/675556972/ge_low_mem.o ${OBJECTDIR}/_ext/675556972/ge_operations.o ${OBJECTDIR}/_ext/675556972/hash.o ${OBJECTDIR}/_ext/675556972/hc128.o ${OBJECTDIR}/_ext/675556972/hmac.o ${OBJECTDIR}/_ext/675556972/idea.o ${OBJECTDIR}/_ext/675556972/integer.o ${OBJECTDIR}/_ext/675556972/logging.o ${OBJECTDIR}/_ext/675556972/md2.o ${OBJECTDIR}/_ext/675556972/md4.o ${OBJECTDIR}/_ext/675556972/md5.o ${OBJECTDIR}/_ext/675556972/memory.o ${OBJECTDIR}/_ext/675556972/misc.o ${OBJECTDIR}/_ext/675556972/pkcs12.o ${OBJECTDIR}/_ext/675556972/pkcs7.o ${OBJECTDIR}/_ext/675556972/poly1305.o ${OBJECTDIR}/_ext/675556972/pwdbased.o ${OBJECTDIR}/_ext/675556972/rabbit.o ${OBJECTDIR}/_ext/675556972/ripemd.o ${OBJECTDIR}/_ext/675556972/rsa.o ${OBJECTDIR}/_ext/675556972/sha3.o ${OBJECTDIR}/_ext/675556972/sha512.o ${OBJECTDIR}/_ext/675556972/signature.o ${OBJECTDIR}/_ext/675556972/srp.o ${OBJECTDIR}/_ext/675556972/tfm.o ${OBJECTDIR}/_ext/675556972/wc_encrypt.o ${OBJECTDIR}/_ext/675556972/wc_port.o ${OBJECTDIR}/_ext/675556972/wolfevent.o ${OBJECTDIR}/_ext/675556972/wolfmath.o ${OBJECTDIR}/_ext/1758509375/adler32.o ${OBJECTDIR}/_ext/1758509375/crc32.o ${OBJECTDIR}/_ext/1758509375/deflate.o ${OBJECTDIR}/_ext/1758509375/inflate.o ${OBJECTDIR}/_ext/1758509375/trees.o ${OBJECTDIR}/_ext/1758509375/zutil.o ${OBJECTDIR}/_ext/1758509375/inftrees.o ${OBJECTDIR}/_ext/1758509375/inffast.o ${OBJECTDIR}/_ext/675556972/aes.o ${OBJECTDIR}/_ext/675556972/sha.o ${OBJECTDIR}/_ext/675556972/sha256.o ${OBJECTDIR}/_ext/675556972/random.o ${OBJECTDIR}/_ext/675556972/random_same70.o ${OBJECTDIR}/_ext/581080307/net_pres.o ${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o ${OBJECTDIR}/_ext/1953779166/sys_console.o ${OBJECTDIR}/_ext/1953779166/sys_console_uart.o ${OBJECTDIR}/_ext/1953779166/sys_debug.o ${OBJECTDIR}/_ext/1953779166/sys_command.o ${OBJECTDIR}/_ext/1305476372/sys_fs.o ${OBJECTDIR}/_ext/1305476372/sys_fs_media_manager.o ${OBJECTDIR}/_ext/111743104/ff.o ${OBJECTDIR}/_ext/111743104/diskio.o ${OBJECTDIR}/_ext/1521459453/mpfs.o ${OBJECTDIR}/_ext/327803284/sys_time.o ${OBJECTDIR}/_ext/865331892/plib_tc0.o ${OBJECTDIR}/_ext/902702131/tcpip_announce.o ${OBJECTDIR}/_ext/902702131/arp.o ${OBJECTDIR}/_ext/902702131/tcpip_commands.o ${OBJECTDIR}/_ext/902702131/dhcp.o ${OBJECTDIR}/_ext/902702131/dns.o ${OBJECTDIR}/_ext/902702131/http.o ${OBJECTDIR}/_ext/917763083/http_print.o ${OBJECTDIR}/_ext/917763083/custom_http_app.o ${OBJECTDIR}/_ext/902702131/ipv4.o ${OBJECTDIR}/_ext/902702131/ipv6.o ${OBJECTDIR}/_ext/902702131/icmpv6.o ${OBJECTDIR}/_ext/902702131/icmp.o ${OBJECTDIR}/_ext/902702131/nbns.o ${OBJECTDIR}/_ext/902702131/ndp.o ${OBJECTDIR}/_ext/902702131/smtpc.o ${OBJECTDIR}/_ext/902702131/snmp.o ${OBJECTDIR}/_ext/1728594169/custom_snmp_app.o ${OBJECTDIR}/_ext/902702131/snmpv3.o ${OBJECTDIR}/_ext/902702131/snmpv3_usm.o ${OBJECTDIR}/_ext/902702131/sntp.o ${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o ${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o ${OBJECTDIR}/_ext/902702131/helpers.o ${OBJECTDIR}/_ext/902702131/hash_fnv.o ${OBJECTDIR}/_ext/902702131/oahash.o ${OBJECTDIR}/_ext/902702131/tcpip_helpers.o ${OBJECTDIR}/_ext/902702131/tcpip_manager.o ${OBJECTDIR}/_ext/902702131/tcpip_notify.o ${OBJECTDIR}/_ext/902702131/tcpip_packet.o ${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o ${OBJECTDIR}/_ext/136402759/sys_fs_wrapper.o ${OBJECTDIR}/_ext/902702131/tcp.o ${OBJECTDIR}/_ext/902702131/udp.o ${OBJECTDIR}/_ext/707186936/plib_usart1.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1360937237/main.o.d ${OBJECTDIR}/_ext/1728594169/initialization.o.d ${OBJECTDIR}/_ext/1728594169/interrupts.o.d ${OBJECTDIR}/_ext/1518761459/xc32_monitor.o.d ${OBJECTDIR}/_ext/1728594169/exceptions.o.d ${OBJECTDIR}/_ext/1055500827/plib_clk.o.d ${OBJECTDIR}/_ext/1055488423/plib_pio.o.d ${OBJECTDIR}/_ext/1639550079/plib_nvic.o.d ${OBJECTDIR}/_ext/1055491083/plib_mpu.o.d ${OBJECTDIR}/_ext/704852250/plib_xdmac.o.d ${OBJECTDIR}/_ext/1728594169/startup.o.d ${OBJECTDIR}/_ext/1728594169/libc_syscalls.o.d ${OBJECTDIR}/_ext/917764137/bsp.o.d ${OBJECTDIR}/_ext/766002106/sys_int.o.d ${OBJECTDIR}/_ext/1360937237/app.o.d ${OBJECTDIR}/_ext/1728594169/tasks.o.d ${OBJECTDIR}/_ext/1015456894/drv_ethphy.o.d ${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o.d ${OBJECTDIR}/_ext/478869302/drv_miim.o.d ${OBJECTDIR}/_ext/629088514/drv_gmac.o.d ${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o.d ${OBJECTDIR}/_ext/2043027118/drv_sdhc_file_system.o.d ${OBJECTDIR}/_ext/2043027118/drv_sdhc.o.d ${OBJECTDIR}/_ext/2043027118/drv_sdhc_host.o.d ${OBJECTDIR}/_ext/675556972/arc4.o.d ${OBJECTDIR}/_ext/675556972/asm.o.d ${OBJECTDIR}/_ext/675556972/asn.o.d ${OBJECTDIR}/_ext/675556972/async.o.d ${OBJECTDIR}/_ext/675556972/chacha.o.d ${OBJECTDIR}/_ext/675556972/chacha20_poly1305.o.d ${OBJECTDIR}/_ext/675556972/cmac.o.d ${OBJECTDIR}/_ext/675556972/coding.o.d ${OBJECTDIR}/_ext/675556972/compress.o.d ${OBJECTDIR}/_ext/675556972/crypto.o.d ${OBJECTDIR}/_ext/675556972/curve25519.o.d ${OBJECTDIR}/_ext/675556972/des3.o.d ${OBJECTDIR}/_ext/675556972/dh.o.d ${OBJECTDIR}/_ext/675556972/dsa.o.d ${OBJECTDIR}/_ext/675556972/ecc.o.d ${OBJECTDIR}/_ext/675556972/ecc_fp.o.d ${OBJECTDIR}/_ext/675556972/ed25519.o.d ${OBJECTDIR}/_ext/675556972/error.o.d ${OBJECTDIR}/_ext/675556972/fe_low_mem.o.d ${OBJECTDIR}/_ext/675556972/fe_operations.o.d ${OBJECTDIR}/_ext/675556972/ge_low_mem.o.d ${OBJECTDIR}/_ext/675556972/ge_operations.o.d ${OBJECTDIR}/_ext/675556972/hash.o.d ${OBJECTDIR}/_ext/675556972/hc128.o.d ${OBJECTDIR}/_ext/675556972/hmac.o.d ${OBJECTDIR}/_ext/675556972/idea.o.d ${OBJECTDIR}/_ext/675556972/integer.o.d ${OBJECTDIR}/_ext/675556972/logging.o.d ${OBJECTDIR}/_ext/675556972/md2.o.d ${OBJECTDIR}/_ext/675556972/md4.o.d ${OBJECTDIR}/_ext/675556972/md5.o.d ${OBJECTDIR}/_ext/675556972/memory.o.d ${OBJECTDIR}/_ext/675556972/misc.o.d ${OBJECTDIR}/_ext/675556972/pkcs12.o.d ${OBJECTDIR}/_ext/675556972/pkcs7.o.d ${OBJECTDIR}/_ext/675556972/poly1305.o.d ${OBJECTDIR}/_ext/675556972/pwdbased.o.d ${OBJECTDIR}/_ext/675556972/rabbit.o.d ${OBJECTDIR}/_ext/675556972/ripemd.o.d ${OBJECTDIR}/_ext/675556972/rsa.o.d ${OBJECTDIR}/_ext/675556972/sha3.o.d ${OBJECTDIR}/_ext/675556972/sha512.o.d ${OBJECTDIR}/_ext/675556972/signature.o.d ${OBJECTDIR}/_ext/675556972/srp.o.d ${OBJECTDIR}/_ext/675556972/tfm.o.d ${OBJECTDIR}/_ext/675556972/wc_encrypt.o.d ${OBJECTDIR}/_ext/675556972/wc_port.o.d ${OBJECTDIR}/_ext/675556972/wolfevent.o.d ${OBJECTDIR}/_ext/675556972/wolfmath.o.d ${OBJECTDIR}/_ext/1758509375/adler32.o.d ${OBJECTDIR}/_ext/1758509375/crc32.o.d ${OBJECTDIR}/_ext/1758509375/deflate.o.d ${OBJECTDIR}/_ext/1758509375/inflate.o.d ${OBJECTDIR}/_ext/1758509375/trees.o.d ${OBJECTDIR}/_ext/1758509375/zutil.o.d ${OBJECTDIR}/_ext/1758509375/inftrees.o.d ${OBJECTDIR}/_ext/1758509375/inffast.o.d ${OBJECTDIR}/_ext/675556972/aes.o.d ${OBJECTDIR}/_ext/675556972/sha.o.d ${OBJECTDIR}/_ext/675556972/sha256.o.d ${OBJECTDIR}/_ext/675556972/random.o.d ${OBJECTDIR}/_ext/675556972/random_same70.o.d ${OBJECTDIR}/_ext/581080307/net_pres.o.d ${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o.d ${OBJECTDIR}/_ext/1953779166/sys_console.o.d ${OBJECTDIR}/_ext/1953779166/sys_console_uart.o.d ${OBJECTDIR}/_ext/1953779166/sys_debug.o.d ${OBJECTDIR}/_ext/1953779166/sys_command.o.d ${OBJECTDIR}/_ext/1305476372/sys_fs.o.d ${OBJECTDIR}/_ext/1305476372/sys_fs_media_manager.o.d ${OBJECTDIR}/_ext/111743104/ff.o.d ${OBJECTDIR}/_ext/111743104/diskio.o.d ${OBJECTDIR}/_ext/1521459453/mpfs.o.d ${OBJECTDIR}/_ext/327803284/sys_time.o.d ${OBJECTDIR}/_ext/865331892/plib_tc0.o.d ${OBJECTDIR}/_ext/902702131/tcpip_announce.o.d ${OBJECTDIR}/_ext/902702131/arp.o.d ${OBJECTDIR}/_ext/902702131/tcpip_commands.o.d ${OBJECTDIR}/_ext/902702131/dhcp.o.d ${OBJECTDIR}/_ext/902702131/dns.o.d ${OBJECTDIR}/_ext/902702131/http.o.d ${OBJECTDIR}/_ext/917763083/http_print.o.d ${OBJECTDIR}/_ext/917763083/custom_http_app.o.d ${OBJECTDIR}/_ext/902702131/ipv4.o.d ${OBJECTDIR}/_ext/902702131/ipv6.o.d ${OBJECTDIR}/_ext/902702131/icmpv6.o.d ${OBJECTDIR}/_ext/902702131/icmp.o.d ${OBJECTDIR}/_ext/902702131/nbns.o.d ${OBJECTDIR}/_ext/902702131/ndp.o.d ${OBJECTDIR}/_ext/902702131/smtpc.o.d ${OBJECTDIR}/_ext/902702131/snmp.o.d ${OBJECTDIR}/_ext/1728594169/custom_snmp_app.o.d ${OBJECTDIR}/_ext/902702131/snmpv3.o.d ${OBJECTDIR}/_ext/902702131/snmpv3_usm.o.d ${OBJECTDIR}/_ext/902702131/sntp.o.d ${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o.d ${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o.d ${OBJECTDIR}/_ext/902702131/helpers.o.d ${OBJECTDIR}/_ext/902702131/hash_fnv.o.d ${OBJECTDIR}/_ext/902702131/oahash.o.d ${OBJECTDIR}/_ext/902702131/tcpip_helpers.o.d ${OBJECTDIR}/_ext/902702131/tcpip_manager.o.d ${OBJECTDIR}/_ext/902702131/tcpip_notify.o.d ${OBJECTDIR}/_ext/902702131/tcpip_packet.o.d ${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o.d ${OBJECTDIR}/_ext/136402759/sys_fs_wrapper.o.d ${OBJECTDIR}/_ext/902702131/tcp.o.d ${OBJECTDIR}/_ext/902702131/udp.o.d ${OBJECTDIR}/_ext/707186936/plib_usart1.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1360937237/main.o ${OBJECTDIR}/_ext/1728594169/initialization.o ${OBJECTDIR}/_ext/1728594169/interrupts.o ${OBJECTDIR}/_ext/1518761459/xc32_monitor.o ${OBJECTDIR}/_ext/1728594169/exceptions.o ${OBJECTDIR}/_ext/1055500827/plib_clk.o ${OBJECTDIR}/_ext/1055488423/plib_pio.o ${OBJECTDIR}/_ext/1639550079/plib_nvic.o ${OBJECTDIR}/_ext/1055491083/plib_mpu.o ${OBJECTDIR}/_ext/704852250/plib_xdmac.o ${OBJECTDIR}/_ext/1728594169/startup.o ${OBJECTDIR}/_ext/1728594169/libc_syscalls.o ${OBJECTDIR}/_ext/917764137/bsp.o ${OBJECTDIR}/_ext/766002106/sys_int.o ${OBJECTDIR}/_ext/1360937237/app.o ${OBJECTDIR}/_ext/1728594169/tasks.o ${OBJECTDIR}/_ext/1015456894/drv_ethphy.o ${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o ${OBJECTDIR}/_ext/478869302/drv_miim.o ${OBJECTDIR}/_ext/629088514/drv_gmac.o ${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o ${OBJECTDIR}/_ext/2043027118/drv_sdhc_file_system.o ${OBJECTDIR}/_ext/2043027118/drv_sdhc.o ${OBJECTDIR}/_ext/2043027118/drv_sdhc_host.o ${OBJECTDIR}/_ext/675556972/arc4.o ${OBJECTDIR}/_ext/675556972/asm.o ${OBJECTDIR}/_ext/675556972/asn.o ${OBJECTDIR}/_ext/675556972/async.o ${OBJECTDIR}/_ext/675556972/chacha.o ${OBJECTDIR}/_ext/675556972/chacha20_poly1305.o ${OBJECTDIR}/_ext/675556972/cmac.o ${OBJECTDIR}/_ext/675556972/coding.o ${OBJECTDIR}/_ext/675556972/compress.o ${OBJECTDIR}/_ext/675556972/crypto.o ${OBJECTDIR}/_ext/675556972/curve25519.o ${OBJECTDIR}/_ext/675556972/des3.o ${OBJECTDIR}/_ext/675556972/dh.o ${OBJECTDIR}/_ext/675556972/dsa.o ${OBJECTDIR}/_ext/675556972/ecc.o ${OBJECTDIR}/_ext/675556972/ecc_fp.o ${OBJECTDIR}/_ext/675556972/ed25519.o ${OBJECTDIR}/_ext/675556972/error.o ${OBJECTDIR}/_ext/675556972/fe_low_mem.o ${OBJECTDIR}/_ext/675556972/fe_operations.o ${OBJECTDIR}/_ext/675556972/ge_low_mem.o ${OBJECTDIR}/_ext/675556972/ge_operations.o ${OBJECTDIR}/_ext/675556972/hash.o ${OBJECTDIR}/_ext/675556972/hc128.o ${OBJECTDIR}/_ext/675556972/hmac.o ${OBJECTDIR}/_ext/675556972/idea.o ${OBJECTDIR}/_ext/675556972/integer.o ${OBJECTDIR}/_ext/675556972/logging.o ${OBJECTDIR}/_ext/675556972/md2.o ${OBJECTDIR}/_ext/675556972/md4.o ${OBJECTDIR}/_ext/675556972/md5.o ${OBJECTDIR}/_ext/675556972/memory.o ${OBJECTDIR}/_ext/675556972/misc.o ${OBJECTDIR}/_ext/675556972/pkcs12.o ${OBJECTDIR}/_ext/675556972/pkcs7.o ${OBJECTDIR}/_ext/675556972/poly1305.o ${OBJECTDIR}/_ext/675556972/pwdbased.o ${OBJECTDIR}/_ext/675556972/rabbit.o ${OBJECTDIR}/_ext/675556972/ripemd.o ${OBJECTDIR}/_ext/675556972/rsa.o ${OBJECTDIR}/_ext/675556972/sha3.o ${OBJECTDIR}/_ext/675556972/sha512.o ${OBJECTDIR}/_ext/675556972/signature.o ${OBJECTDIR}/_ext/675556972/srp.o ${OBJECTDIR}/_ext/675556972/tfm.o ${OBJECTDIR}/_ext/675556972/wc_encrypt.o ${OBJECTDIR}/_ext/675556972/wc_port.o ${OBJECTDIR}/_ext/675556972/wolfevent.o ${OBJECTDIR}/_ext/675556972/wolfmath.o ${OBJECTDIR}/_ext/1758509375/adler32.o ${OBJECTDIR}/_ext/1758509375/crc32.o ${OBJECTDIR}/_ext/1758509375/deflate.o ${OBJECTDIR}/_ext/1758509375/inflate.o ${OBJECTDIR}/_ext/1758509375/trees.o ${OBJECTDIR}/_ext/1758509375/zutil.o ${OBJECTDIR}/_ext/1758509375/inftrees.o ${OBJECTDIR}/_ext/1758509375/inffast.o ${OBJECTDIR}/_ext/675556972/aes.o ${OBJECTDIR}/_ext/675556972/sha.o ${OBJECTDIR}/_ext/675556972/sha256.o ${OBJECTDIR}/_ext/675556972/random.o ${OBJECTDIR}/_ext/675556972/random_same70.o ${OBJECTDIR}/_ext/581080307/net_pres.o ${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o ${OBJECTDIR}/_ext/1953779166/sys_console.o ${OBJECTDIR}/_ext/1953779166/sys_console_uart.o ${OBJECTDIR}/_ext/1953779166/sys_debug.o ${OBJECTDIR}/_ext/1953779166/sys_command.o ${OBJECTDIR}/_ext/1305476372/sys_fs.o ${OBJECTDIR}/_ext/1305476372/sys_fs_media_manager.o ${OBJECTDIR}/_ext/111743104/ff.o ${OBJECTDIR}/_ext/111743104/diskio.o ${OBJECTDIR}/_ext/1521459453/mpfs.o ${OBJECTDIR}/_ext/327803284/sys_time.o ${OBJECTDIR}/_ext/865331892/plib_tc0.o ${OBJECTDIR}/_ext/902702131/tcpip_announce.o ${OBJECTDIR}/_ext/902702131/arp.o ${OBJECTDIR}/_ext/902702131/tcpip_commands.o ${OBJECTDIR}/_ext/902702131/dhcp.o ${OBJECTDIR}/_ext/902702131/dns.o ${OBJECTDIR}/_ext/902702131/http.o ${OBJECTDIR}/_ext/917763083/http_print.o ${OBJECTDIR}/_ext/917763083/custom_http_app.o ${OBJECTDIR}/_ext/902702131/ipv4.o ${OBJECTDIR}/_ext/902702131/ipv6.o ${OBJECTDIR}/_ext/902702131/icmpv6.o ${OBJECTDIR}/_ext/902702131/icmp.o ${OBJECTDIR}/_ext/902702131/nbns.o ${OBJECTDIR}/_ext/902702131/ndp.o ${OBJECTDIR}/_ext/902702131/smtpc.o ${OBJECTDIR}/_ext/902702131/snmp.o ${OBJECTDIR}/_ext/1728594169/custom_snmp_app.o ${OBJECTDIR}/_ext/902702131/snmpv3.o ${OBJECTDIR}/_ext/902702131/snmpv3_usm.o ${OBJECTDIR}/_ext/902702131/sntp.o ${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o ${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o ${OBJECTDIR}/_ext/902702131/helpers.o ${OBJECTDIR}/_ext/902702131/hash_fnv.o ${OBJECTDIR}/_ext/902702131/oahash.o ${OBJECTDIR}/_ext/902702131/tcpip_helpers.o ${OBJECTDIR}/_ext/902702131/tcpip_manager.o ${OBJECTDIR}/_ext/902702131/tcpip_notify.o ${OBJECTDIR}/_ext/902702131/tcpip_packet.o ${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o ${OBJECTDIR}/_ext/136402759/sys_fs_wrapper.o ${OBJECTDIR}/_ext/902702131/tcp.o ${OBJECTDIR}/_ext/902702131/udp.o ${OBJECTDIR}/_ext/707186936/plib_usart1.o

# Source Files
SOURCEFILES=../src/main.c ../src/config/sam_e70_xult/initialization.c ../src/config/sam_e70_xult/interrupts.c ../src/config/sam_e70_xult/stdio/xc32_monitor.c ../src/config/sam_e70_xult/exceptions.c ../src/config/sam_e70_xult/peripheral/clk/plib_clk.c ../src/config/sam_e70_xult/peripheral/pio/plib_pio.c ../src/config/sam_e70_xult/peripheral/nvic/plib_nvic.c ../src/config/sam_e70_xult/peripheral/mpu/plib_mpu.c ../src/config/sam_e70_xult/peripheral/xdmac/plib_xdmac.c ../src/config/sam_e70_xult/startup.c ../src/config/sam_e70_xult/libc_syscalls.c ../src/config/sam_e70_xult/bsp/bsp.c ../src/config/sam_e70_xult/system/int/src/sys_int.c ../src/app.c ../src/config/sam_e70_xult/tasks.c ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_ethphy.c ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_extphy_smsc8740.c ../src/config/sam_e70_xult/driver/miim/src/dynamic/drv_miim.c ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac.c ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac_lib.c ../src/config/sam_e70_xult/driver/sdhc/src/drv_sdhc_file_system.c ../src/config/sam_e70_xult/driver/sdhc/src/drv_sdhc.c ../src/config/sam_e70_xult/driver/sdhc/src/drv_sdhc_host.c ../src/config/sam_e70_xult/crypto/src/arc4.c ../src/config/sam_e70_xult/crypto/src/asm.c ../src/config/sam_e70_xult/crypto/src/asn.c ../src/config/sam_e70_xult/crypto/src/async.c ../src/config/sam_e70_xult/crypto/src/chacha.c ../src/config/sam_e70_xult/crypto/src/chacha20_poly1305.c ../src/config/sam_e70_xult/crypto/src/cmac.c ../src/config/sam_e70_xult/crypto/src/coding.c ../src/config/sam_e70_xult/crypto/src/compress.c ../src/config/sam_e70_xult/crypto/src/crypto.c ../src/config/sam_e70_xult/crypto/src/curve25519.c ../src/config/sam_e70_xult/crypto/src/des3.c ../src/config/sam_e70_xult/crypto/src/dh.c ../src/config/sam_e70_xult/crypto/src/dsa.c ../src/config/sam_e70_xult/crypto/src/ecc.c ../src/config/sam_e70_xult/crypto/src/ecc_fp.c ../src/config/sam_e70_xult/crypto/src/ed25519.c ../src/config/sam_e70_xult/crypto/src/error.c ../src/config/sam_e70_xult/crypto/src/fe_low_mem.c ../src/config/sam_e70_xult/crypto/src/fe_operations.c ../src/config/sam_e70_xult/crypto/src/ge_low_mem.c ../src/config/sam_e70_xult/crypto/src/ge_operations.c ../src/config/sam_e70_xult/crypto/src/hash.c ../src/config/sam_e70_xult/crypto/src/hc128.c ../src/config/sam_e70_xult/crypto/src/hmac.c ../src/config/sam_e70_xult/crypto/src/idea.c ../src/config/sam_e70_xult/crypto/src/integer.c ../src/config/sam_e70_xult/crypto/src/logging.c ../src/config/sam_e70_xult/crypto/src/md2.c ../src/config/sam_e70_xult/crypto/src/md4.c ../src/config/sam_e70_xult/crypto/src/md5.c ../src/config/sam_e70_xult/crypto/src/memory.c ../src/config/sam_e70_xult/crypto/src/misc.c ../src/config/sam_e70_xult/crypto/src/pkcs12.c ../src/config/sam_e70_xult/crypto/src/pkcs7.c ../src/config/sam_e70_xult/crypto/src/poly1305.c ../src/config/sam_e70_xult/crypto/src/pwdbased.c ../src/config/sam_e70_xult/crypto/src/rabbit.c ../src/config/sam_e70_xult/crypto/src/ripemd.c ../src/config/sam_e70_xult/crypto/src/rsa.c ../src/config/sam_e70_xult/crypto/src/sha3.c ../src/config/sam_e70_xult/crypto/src/sha512.c ../src/config/sam_e70_xult/crypto/src/signature.c ../src/config/sam_e70_xult/crypto/src/srp.c ../src/config/sam_e70_xult/crypto/src/tfm.c ../src/config/sam_e70_xult/crypto/src/wc_encrypt.c ../src/config/sam_e70_xult/crypto/src/wc_port.c ../src/config/sam_e70_xult/crypto/src/wolfevent.c ../src/config/sam_e70_xult/crypto/src/wolfmath.c ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/adler32.c ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/crc32.c ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/deflate.c ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/inflate.c ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/trees.c ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/zutil.c ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/inftrees.c ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/inffast.c ../src/config/sam_e70_xult/crypto/src/aes.c ../src/config/sam_e70_xult/crypto/src/sha.c ../src/config/sam_e70_xult/crypto/src/sha256.c ../src/config/sam_e70_xult/crypto/src/random.c ../src/config/sam_e70_xult/crypto/src/random_same70.c ../src/config/sam_e70_xult/net/pres/src/net_pres.c ../src/config/sam_e70_xult/net/pres/net_pres_enc_glue.c ../src/config/sam_e70_xult/system/console/src/sys_console.c ../src/config/sam_e70_xult/system/console/src/sys_console_uart.c ../src/config/sam_e70_xult/system/console/src/sys_debug.c ../src/config/sam_e70_xult/system/console/src/sys_command.c ../src/config/sam_e70_xult/system/fs/src/sys_fs.c ../src/config/sam_e70_xult/system/fs/src/sys_fs_media_manager.c ../src/config/sam_e70_xult/system/fs/fat_fs/src/ff.c ../src/config/sam_e70_xult/system/fs/fat_fs/src/diskio.c ../src/config/sam_e70_xult/system/fs/mpfs/src/mpfs.c ../src/config/sam_e70_xult/system/time/src/sys_time.c ../src/config/sam_e70_xult/peripheral/tc/plib_tc0.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_announce.c ../src/config/sam_e70_xult/library/tcpip/src/arp.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_commands.c ../src/config/sam_e70_xult/library/tcpip/src/dhcp.c ../src/config/sam_e70_xult/library/tcpip/src/dns.c ../src/config/sam_e70_xult/library/tcpip/src/http.c ../src/config/sam_e70_xult/app/http_print.c ../src/config/sam_e70_xult/app/custom_http_app.c ../src/config/sam_e70_xult/library/tcpip/src/ipv4.c ../src/config/sam_e70_xult/library/tcpip/src/ipv6.c ../src/config/sam_e70_xult/library/tcpip/src/icmpv6.c ../src/config/sam_e70_xult/library/tcpip/src/icmp.c ../src/config/sam_e70_xult/library/tcpip/src/nbns.c ../src/config/sam_e70_xult/library/tcpip/src/ndp.c ../src/config/sam_e70_xult/library/tcpip/src/smtpc.c ../src/config/sam_e70_xult/library/tcpip/src/snmp.c ../src/config/sam_e70_xult/custom_snmp_app.c ../src/config/sam_e70_xult/library/tcpip/src/snmpv3.c ../src/config/sam_e70_xult/library/tcpip/src/snmpv3_usm.c ../src/config/sam_e70_xult/library/tcpip/src/sntp.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_alloc.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_internal.c ../src/config/sam_e70_xult/library/tcpip/src/helpers.c ../src/config/sam_e70_xult/library/tcpip/src/hash_fnv.c ../src/config/sam_e70_xult/library/tcpip/src/oahash.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_helpers.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_manager.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_notify.c ../src/config/sam_e70_xult/library/tcpip/src/tcpip_packet.c ../src/config/sam_e70_xult/system/sys_time_h2_adapter.c ../src/config/sam_e70_xult/library/tcpip/src/common/sys_fs_wrapper.c ../src/config/sam_e70_xult/library/tcpip/src/tcp.c ../src/config/sam_e70_xult/library/tcpip/src/udp.c ../src/config/sam_e70_xult/peripheral/usart/plib_usart1.c


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
	${MAKE}  -f nbproject/Makefile-sam_e70_xult.mk dist/${CND_CONF}/${IMAGE_TYPE}/sam_e70_xult.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

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
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/initialization.o: ../src/config/sam_e70_xult/initialization.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/initialization.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/initialization.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/initialization.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/initialization.o.d" -o ${OBJECTDIR}/_ext/1728594169/initialization.o ../src/config/sam_e70_xult/initialization.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/interrupts.o: ../src/config/sam_e70_xult/interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/interrupts.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/interrupts.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/interrupts.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/interrupts.o.d" -o ${OBJECTDIR}/_ext/1728594169/interrupts.o ../src/config/sam_e70_xult/interrupts.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1518761459/xc32_monitor.o: ../src/config/sam_e70_xult/stdio/xc32_monitor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1518761459" 
	@${RM} ${OBJECTDIR}/_ext/1518761459/xc32_monitor.o.d 
	@${RM} ${OBJECTDIR}/_ext/1518761459/xc32_monitor.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1518761459/xc32_monitor.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1518761459/xc32_monitor.o.d" -o ${OBJECTDIR}/_ext/1518761459/xc32_monitor.o ../src/config/sam_e70_xult/stdio/xc32_monitor.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/exceptions.o: ../src/config/sam_e70_xult/exceptions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/exceptions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/exceptions.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/exceptions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/exceptions.o.d" -o ${OBJECTDIR}/_ext/1728594169/exceptions.o ../src/config/sam_e70_xult/exceptions.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1055500827/plib_clk.o: ../src/config/sam_e70_xult/peripheral/clk/plib_clk.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1055500827" 
	@${RM} ${OBJECTDIR}/_ext/1055500827/plib_clk.o.d 
	@${RM} ${OBJECTDIR}/_ext/1055500827/plib_clk.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055500827/plib_clk.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1055500827/plib_clk.o.d" -o ${OBJECTDIR}/_ext/1055500827/plib_clk.o ../src/config/sam_e70_xult/peripheral/clk/plib_clk.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1055488423/plib_pio.o: ../src/config/sam_e70_xult/peripheral/pio/plib_pio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1055488423" 
	@${RM} ${OBJECTDIR}/_ext/1055488423/plib_pio.o.d 
	@${RM} ${OBJECTDIR}/_ext/1055488423/plib_pio.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055488423/plib_pio.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1055488423/plib_pio.o.d" -o ${OBJECTDIR}/_ext/1055488423/plib_pio.o ../src/config/sam_e70_xult/peripheral/pio/plib_pio.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1639550079/plib_nvic.o: ../src/config/sam_e70_xult/peripheral/nvic/plib_nvic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1639550079" 
	@${RM} ${OBJECTDIR}/_ext/1639550079/plib_nvic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1639550079/plib_nvic.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1639550079/plib_nvic.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1639550079/plib_nvic.o.d" -o ${OBJECTDIR}/_ext/1639550079/plib_nvic.o ../src/config/sam_e70_xult/peripheral/nvic/plib_nvic.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1055491083/plib_mpu.o: ../src/config/sam_e70_xult/peripheral/mpu/plib_mpu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1055491083" 
	@${RM} ${OBJECTDIR}/_ext/1055491083/plib_mpu.o.d 
	@${RM} ${OBJECTDIR}/_ext/1055491083/plib_mpu.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055491083/plib_mpu.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1055491083/plib_mpu.o.d" -o ${OBJECTDIR}/_ext/1055491083/plib_mpu.o ../src/config/sam_e70_xult/peripheral/mpu/plib_mpu.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/704852250/plib_xdmac.o: ../src/config/sam_e70_xult/peripheral/xdmac/plib_xdmac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/704852250" 
	@${RM} ${OBJECTDIR}/_ext/704852250/plib_xdmac.o.d 
	@${RM} ${OBJECTDIR}/_ext/704852250/plib_xdmac.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/704852250/plib_xdmac.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/704852250/plib_xdmac.o.d" -o ${OBJECTDIR}/_ext/704852250/plib_xdmac.o ../src/config/sam_e70_xult/peripheral/xdmac/plib_xdmac.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/startup.o: ../src/config/sam_e70_xult/startup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/startup.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/startup.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/startup.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/startup.o.d" -o ${OBJECTDIR}/_ext/1728594169/startup.o ../src/config/sam_e70_xult/startup.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/libc_syscalls.o: ../src/config/sam_e70_xult/libc_syscalls.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/libc_syscalls.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/libc_syscalls.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/libc_syscalls.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/libc_syscalls.o.d" -o ${OBJECTDIR}/_ext/1728594169/libc_syscalls.o ../src/config/sam_e70_xult/libc_syscalls.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/917764137/bsp.o: ../src/config/sam_e70_xult/bsp/bsp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/917764137" 
	@${RM} ${OBJECTDIR}/_ext/917764137/bsp.o.d 
	@${RM} ${OBJECTDIR}/_ext/917764137/bsp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/917764137/bsp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/917764137/bsp.o.d" -o ${OBJECTDIR}/_ext/917764137/bsp.o ../src/config/sam_e70_xult/bsp/bsp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/766002106/sys_int.o: ../src/config/sam_e70_xult/system/int/src/sys_int.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/766002106" 
	@${RM} ${OBJECTDIR}/_ext/766002106/sys_int.o.d 
	@${RM} ${OBJECTDIR}/_ext/766002106/sys_int.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/766002106/sys_int.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/766002106/sys_int.o.d" -o ${OBJECTDIR}/_ext/766002106/sys_int.o ../src/config/sam_e70_xult/system/int/src/sys_int.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/app.o: ../src/app.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/app.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1360937237/app.o.d" -o ${OBJECTDIR}/_ext/1360937237/app.o ../src/app.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/tasks.o: ../src/config/sam_e70_xult/tasks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/tasks.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/tasks.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/tasks.o.d" -o ${OBJECTDIR}/_ext/1728594169/tasks.o ../src/config/sam_e70_xult/tasks.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1015456894/drv_ethphy.o: ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_ethphy.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1015456894" 
	@${RM} ${OBJECTDIR}/_ext/1015456894/drv_ethphy.o.d 
	@${RM} ${OBJECTDIR}/_ext/1015456894/drv_ethphy.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1015456894/drv_ethphy.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1015456894/drv_ethphy.o.d" -o ${OBJECTDIR}/_ext/1015456894/drv_ethphy.o ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_ethphy.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o: ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_extphy_smsc8740.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1015456894" 
	@${RM} ${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o.d 
	@${RM} ${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o.d" -o ${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_extphy_smsc8740.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/478869302/drv_miim.o: ../src/config/sam_e70_xult/driver/miim/src/dynamic/drv_miim.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/478869302" 
	@${RM} ${OBJECTDIR}/_ext/478869302/drv_miim.o.d 
	@${RM} ${OBJECTDIR}/_ext/478869302/drv_miim.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/478869302/drv_miim.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/478869302/drv_miim.o.d" -o ${OBJECTDIR}/_ext/478869302/drv_miim.o ../src/config/sam_e70_xult/driver/miim/src/dynamic/drv_miim.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/629088514/drv_gmac.o: ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/629088514" 
	@${RM} ${OBJECTDIR}/_ext/629088514/drv_gmac.o.d 
	@${RM} ${OBJECTDIR}/_ext/629088514/drv_gmac.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/629088514/drv_gmac.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/629088514/drv_gmac.o.d" -o ${OBJECTDIR}/_ext/629088514/drv_gmac.o ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o: ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac_lib.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/629088514" 
	@${RM} ${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o.d 
	@${RM} ${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o.d" -o ${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac_lib.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2043027118/drv_sdhc_file_system.o: ../src/config/sam_e70_xult/driver/sdhc/src/drv_sdhc_file_system.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2043027118" 
	@${RM} ${OBJECTDIR}/_ext/2043027118/drv_sdhc_file_system.o.d 
	@${RM} ${OBJECTDIR}/_ext/2043027118/drv_sdhc_file_system.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2043027118/drv_sdhc_file_system.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/2043027118/drv_sdhc_file_system.o.d" -o ${OBJECTDIR}/_ext/2043027118/drv_sdhc_file_system.o ../src/config/sam_e70_xult/driver/sdhc/src/drv_sdhc_file_system.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2043027118/drv_sdhc.o: ../src/config/sam_e70_xult/driver/sdhc/src/drv_sdhc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2043027118" 
	@${RM} ${OBJECTDIR}/_ext/2043027118/drv_sdhc.o.d 
	@${RM} ${OBJECTDIR}/_ext/2043027118/drv_sdhc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2043027118/drv_sdhc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/2043027118/drv_sdhc.o.d" -o ${OBJECTDIR}/_ext/2043027118/drv_sdhc.o ../src/config/sam_e70_xult/driver/sdhc/src/drv_sdhc.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2043027118/drv_sdhc_host.o: ../src/config/sam_e70_xult/driver/sdhc/src/drv_sdhc_host.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2043027118" 
	@${RM} ${OBJECTDIR}/_ext/2043027118/drv_sdhc_host.o.d 
	@${RM} ${OBJECTDIR}/_ext/2043027118/drv_sdhc_host.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2043027118/drv_sdhc_host.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/2043027118/drv_sdhc_host.o.d" -o ${OBJECTDIR}/_ext/2043027118/drv_sdhc_host.o ../src/config/sam_e70_xult/driver/sdhc/src/drv_sdhc_host.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/arc4.o: ../src/config/sam_e70_xult/crypto/src/arc4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/arc4.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/arc4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/arc4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/arc4.o.d" -o ${OBJECTDIR}/_ext/675556972/arc4.o ../src/config/sam_e70_xult/crypto/src/arc4.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/asm.o: ../src/config/sam_e70_xult/crypto/src/asm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/asm.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/asm.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/asm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/asm.o.d" -o ${OBJECTDIR}/_ext/675556972/asm.o ../src/config/sam_e70_xult/crypto/src/asm.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/asn.o: ../src/config/sam_e70_xult/crypto/src/asn.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/asn.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/asn.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/asn.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/asn.o.d" -o ${OBJECTDIR}/_ext/675556972/asn.o ../src/config/sam_e70_xult/crypto/src/asn.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/async.o: ../src/config/sam_e70_xult/crypto/src/async.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/async.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/async.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/async.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/async.o.d" -o ${OBJECTDIR}/_ext/675556972/async.o ../src/config/sam_e70_xult/crypto/src/async.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/chacha.o: ../src/config/sam_e70_xult/crypto/src/chacha.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/chacha.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/chacha.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/chacha.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/chacha.o.d" -o ${OBJECTDIR}/_ext/675556972/chacha.o ../src/config/sam_e70_xult/crypto/src/chacha.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/chacha20_poly1305.o: ../src/config/sam_e70_xult/crypto/src/chacha20_poly1305.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/chacha20_poly1305.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/chacha20_poly1305.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/chacha20_poly1305.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/chacha20_poly1305.o.d" -o ${OBJECTDIR}/_ext/675556972/chacha20_poly1305.o ../src/config/sam_e70_xult/crypto/src/chacha20_poly1305.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/cmac.o: ../src/config/sam_e70_xult/crypto/src/cmac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/cmac.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/cmac.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/cmac.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/cmac.o.d" -o ${OBJECTDIR}/_ext/675556972/cmac.o ../src/config/sam_e70_xult/crypto/src/cmac.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/coding.o: ../src/config/sam_e70_xult/crypto/src/coding.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/coding.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/coding.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/coding.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/coding.o.d" -o ${OBJECTDIR}/_ext/675556972/coding.o ../src/config/sam_e70_xult/crypto/src/coding.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/compress.o: ../src/config/sam_e70_xult/crypto/src/compress.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/compress.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/compress.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/compress.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/compress.o.d" -o ${OBJECTDIR}/_ext/675556972/compress.o ../src/config/sam_e70_xult/crypto/src/compress.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/crypto.o: ../src/config/sam_e70_xult/crypto/src/crypto.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/crypto.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/crypto.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/crypto.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/crypto.o.d" -o ${OBJECTDIR}/_ext/675556972/crypto.o ../src/config/sam_e70_xult/crypto/src/crypto.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/curve25519.o: ../src/config/sam_e70_xult/crypto/src/curve25519.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/curve25519.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/curve25519.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/curve25519.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/curve25519.o.d" -o ${OBJECTDIR}/_ext/675556972/curve25519.o ../src/config/sam_e70_xult/crypto/src/curve25519.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/des3.o: ../src/config/sam_e70_xult/crypto/src/des3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/des3.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/des3.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/des3.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/des3.o.d" -o ${OBJECTDIR}/_ext/675556972/des3.o ../src/config/sam_e70_xult/crypto/src/des3.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/dh.o: ../src/config/sam_e70_xult/crypto/src/dh.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/dh.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/dh.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/dh.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/dh.o.d" -o ${OBJECTDIR}/_ext/675556972/dh.o ../src/config/sam_e70_xult/crypto/src/dh.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/dsa.o: ../src/config/sam_e70_xult/crypto/src/dsa.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/dsa.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/dsa.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/dsa.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/dsa.o.d" -o ${OBJECTDIR}/_ext/675556972/dsa.o ../src/config/sam_e70_xult/crypto/src/dsa.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/ecc.o: ../src/config/sam_e70_xult/crypto/src/ecc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/ecc.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/ecc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/ecc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/ecc.o.d" -o ${OBJECTDIR}/_ext/675556972/ecc.o ../src/config/sam_e70_xult/crypto/src/ecc.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/ecc_fp.o: ../src/config/sam_e70_xult/crypto/src/ecc_fp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/ecc_fp.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/ecc_fp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/ecc_fp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/ecc_fp.o.d" -o ${OBJECTDIR}/_ext/675556972/ecc_fp.o ../src/config/sam_e70_xult/crypto/src/ecc_fp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/ed25519.o: ../src/config/sam_e70_xult/crypto/src/ed25519.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/ed25519.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/ed25519.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/ed25519.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/ed25519.o.d" -o ${OBJECTDIR}/_ext/675556972/ed25519.o ../src/config/sam_e70_xult/crypto/src/ed25519.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/error.o: ../src/config/sam_e70_xult/crypto/src/error.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/error.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/error.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/error.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/error.o.d" -o ${OBJECTDIR}/_ext/675556972/error.o ../src/config/sam_e70_xult/crypto/src/error.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/fe_low_mem.o: ../src/config/sam_e70_xult/crypto/src/fe_low_mem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/fe_low_mem.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/fe_low_mem.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/fe_low_mem.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/fe_low_mem.o.d" -o ${OBJECTDIR}/_ext/675556972/fe_low_mem.o ../src/config/sam_e70_xult/crypto/src/fe_low_mem.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/fe_operations.o: ../src/config/sam_e70_xult/crypto/src/fe_operations.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/fe_operations.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/fe_operations.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/fe_operations.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/fe_operations.o.d" -o ${OBJECTDIR}/_ext/675556972/fe_operations.o ../src/config/sam_e70_xult/crypto/src/fe_operations.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/ge_low_mem.o: ../src/config/sam_e70_xult/crypto/src/ge_low_mem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/ge_low_mem.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/ge_low_mem.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/ge_low_mem.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/ge_low_mem.o.d" -o ${OBJECTDIR}/_ext/675556972/ge_low_mem.o ../src/config/sam_e70_xult/crypto/src/ge_low_mem.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/ge_operations.o: ../src/config/sam_e70_xult/crypto/src/ge_operations.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/ge_operations.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/ge_operations.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/ge_operations.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/ge_operations.o.d" -o ${OBJECTDIR}/_ext/675556972/ge_operations.o ../src/config/sam_e70_xult/crypto/src/ge_operations.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/hash.o: ../src/config/sam_e70_xult/crypto/src/hash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/hash.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/hash.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/hash.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/hash.o.d" -o ${OBJECTDIR}/_ext/675556972/hash.o ../src/config/sam_e70_xult/crypto/src/hash.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/hc128.o: ../src/config/sam_e70_xult/crypto/src/hc128.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/hc128.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/hc128.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/hc128.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/hc128.o.d" -o ${OBJECTDIR}/_ext/675556972/hc128.o ../src/config/sam_e70_xult/crypto/src/hc128.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/hmac.o: ../src/config/sam_e70_xult/crypto/src/hmac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/hmac.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/hmac.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/hmac.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/hmac.o.d" -o ${OBJECTDIR}/_ext/675556972/hmac.o ../src/config/sam_e70_xult/crypto/src/hmac.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/idea.o: ../src/config/sam_e70_xult/crypto/src/idea.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/idea.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/idea.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/idea.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/idea.o.d" -o ${OBJECTDIR}/_ext/675556972/idea.o ../src/config/sam_e70_xult/crypto/src/idea.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/integer.o: ../src/config/sam_e70_xult/crypto/src/integer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/integer.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/integer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/integer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/integer.o.d" -o ${OBJECTDIR}/_ext/675556972/integer.o ../src/config/sam_e70_xult/crypto/src/integer.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/logging.o: ../src/config/sam_e70_xult/crypto/src/logging.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/logging.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/logging.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/logging.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/logging.o.d" -o ${OBJECTDIR}/_ext/675556972/logging.o ../src/config/sam_e70_xult/crypto/src/logging.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/md2.o: ../src/config/sam_e70_xult/crypto/src/md2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/md2.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/md2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/md2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/md2.o.d" -o ${OBJECTDIR}/_ext/675556972/md2.o ../src/config/sam_e70_xult/crypto/src/md2.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/md4.o: ../src/config/sam_e70_xult/crypto/src/md4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/md4.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/md4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/md4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/md4.o.d" -o ${OBJECTDIR}/_ext/675556972/md4.o ../src/config/sam_e70_xult/crypto/src/md4.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/md5.o: ../src/config/sam_e70_xult/crypto/src/md5.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/md5.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/md5.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/md5.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/md5.o.d" -o ${OBJECTDIR}/_ext/675556972/md5.o ../src/config/sam_e70_xult/crypto/src/md5.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/memory.o: ../src/config/sam_e70_xult/crypto/src/memory.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/memory.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/memory.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/memory.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/memory.o.d" -o ${OBJECTDIR}/_ext/675556972/memory.o ../src/config/sam_e70_xult/crypto/src/memory.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/misc.o: ../src/config/sam_e70_xult/crypto/src/misc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/misc.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/misc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/misc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/misc.o.d" -o ${OBJECTDIR}/_ext/675556972/misc.o ../src/config/sam_e70_xult/crypto/src/misc.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/pkcs12.o: ../src/config/sam_e70_xult/crypto/src/pkcs12.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/pkcs12.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/pkcs12.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/pkcs12.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/pkcs12.o.d" -o ${OBJECTDIR}/_ext/675556972/pkcs12.o ../src/config/sam_e70_xult/crypto/src/pkcs12.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/pkcs7.o: ../src/config/sam_e70_xult/crypto/src/pkcs7.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/pkcs7.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/pkcs7.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/pkcs7.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/pkcs7.o.d" -o ${OBJECTDIR}/_ext/675556972/pkcs7.o ../src/config/sam_e70_xult/crypto/src/pkcs7.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/poly1305.o: ../src/config/sam_e70_xult/crypto/src/poly1305.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/poly1305.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/poly1305.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/poly1305.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/poly1305.o.d" -o ${OBJECTDIR}/_ext/675556972/poly1305.o ../src/config/sam_e70_xult/crypto/src/poly1305.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/pwdbased.o: ../src/config/sam_e70_xult/crypto/src/pwdbased.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/pwdbased.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/pwdbased.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/pwdbased.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/pwdbased.o.d" -o ${OBJECTDIR}/_ext/675556972/pwdbased.o ../src/config/sam_e70_xult/crypto/src/pwdbased.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/rabbit.o: ../src/config/sam_e70_xult/crypto/src/rabbit.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/rabbit.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/rabbit.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/rabbit.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/rabbit.o.d" -o ${OBJECTDIR}/_ext/675556972/rabbit.o ../src/config/sam_e70_xult/crypto/src/rabbit.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/ripemd.o: ../src/config/sam_e70_xult/crypto/src/ripemd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/ripemd.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/ripemd.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/ripemd.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/ripemd.o.d" -o ${OBJECTDIR}/_ext/675556972/ripemd.o ../src/config/sam_e70_xult/crypto/src/ripemd.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/rsa.o: ../src/config/sam_e70_xult/crypto/src/rsa.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/rsa.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/rsa.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/rsa.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/rsa.o.d" -o ${OBJECTDIR}/_ext/675556972/rsa.o ../src/config/sam_e70_xult/crypto/src/rsa.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/sha3.o: ../src/config/sam_e70_xult/crypto/src/sha3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/sha3.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/sha3.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/sha3.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/sha3.o.d" -o ${OBJECTDIR}/_ext/675556972/sha3.o ../src/config/sam_e70_xult/crypto/src/sha3.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/sha512.o: ../src/config/sam_e70_xult/crypto/src/sha512.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/sha512.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/sha512.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/sha512.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/sha512.o.d" -o ${OBJECTDIR}/_ext/675556972/sha512.o ../src/config/sam_e70_xult/crypto/src/sha512.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/signature.o: ../src/config/sam_e70_xult/crypto/src/signature.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/signature.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/signature.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/signature.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/signature.o.d" -o ${OBJECTDIR}/_ext/675556972/signature.o ../src/config/sam_e70_xult/crypto/src/signature.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/srp.o: ../src/config/sam_e70_xult/crypto/src/srp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/srp.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/srp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/srp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/srp.o.d" -o ${OBJECTDIR}/_ext/675556972/srp.o ../src/config/sam_e70_xult/crypto/src/srp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/tfm.o: ../src/config/sam_e70_xult/crypto/src/tfm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/tfm.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/tfm.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/tfm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/tfm.o.d" -o ${OBJECTDIR}/_ext/675556972/tfm.o ../src/config/sam_e70_xult/crypto/src/tfm.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/wc_encrypt.o: ../src/config/sam_e70_xult/crypto/src/wc_encrypt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/wc_encrypt.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/wc_encrypt.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/wc_encrypt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/wc_encrypt.o.d" -o ${OBJECTDIR}/_ext/675556972/wc_encrypt.o ../src/config/sam_e70_xult/crypto/src/wc_encrypt.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/wc_port.o: ../src/config/sam_e70_xult/crypto/src/wc_port.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/wc_port.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/wc_port.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/wc_port.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/wc_port.o.d" -o ${OBJECTDIR}/_ext/675556972/wc_port.o ../src/config/sam_e70_xult/crypto/src/wc_port.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/wolfevent.o: ../src/config/sam_e70_xult/crypto/src/wolfevent.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/wolfevent.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/wolfevent.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/wolfevent.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/wolfevent.o.d" -o ${OBJECTDIR}/_ext/675556972/wolfevent.o ../src/config/sam_e70_xult/crypto/src/wolfevent.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/wolfmath.o: ../src/config/sam_e70_xult/crypto/src/wolfmath.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/wolfmath.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/wolfmath.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/wolfmath.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/wolfmath.o.d" -o ${OBJECTDIR}/_ext/675556972/wolfmath.o ../src/config/sam_e70_xult/crypto/src/wolfmath.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1758509375/adler32.o: ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/adler32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1758509375" 
	@${RM} ${OBJECTDIR}/_ext/1758509375/adler32.o.d 
	@${RM} ${OBJECTDIR}/_ext/1758509375/adler32.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1758509375/adler32.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1758509375/adler32.o.d" -o ${OBJECTDIR}/_ext/1758509375/adler32.o ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/adler32.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1758509375/crc32.o: ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/crc32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1758509375" 
	@${RM} ${OBJECTDIR}/_ext/1758509375/crc32.o.d 
	@${RM} ${OBJECTDIR}/_ext/1758509375/crc32.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1758509375/crc32.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1758509375/crc32.o.d" -o ${OBJECTDIR}/_ext/1758509375/crc32.o ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/crc32.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1758509375/deflate.o: ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/deflate.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1758509375" 
	@${RM} ${OBJECTDIR}/_ext/1758509375/deflate.o.d 
	@${RM} ${OBJECTDIR}/_ext/1758509375/deflate.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1758509375/deflate.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1758509375/deflate.o.d" -o ${OBJECTDIR}/_ext/1758509375/deflate.o ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/deflate.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1758509375/inflate.o: ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/inflate.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1758509375" 
	@${RM} ${OBJECTDIR}/_ext/1758509375/inflate.o.d 
	@${RM} ${OBJECTDIR}/_ext/1758509375/inflate.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1758509375/inflate.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1758509375/inflate.o.d" -o ${OBJECTDIR}/_ext/1758509375/inflate.o ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/inflate.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1758509375/trees.o: ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/trees.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1758509375" 
	@${RM} ${OBJECTDIR}/_ext/1758509375/trees.o.d 
	@${RM} ${OBJECTDIR}/_ext/1758509375/trees.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1758509375/trees.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1758509375/trees.o.d" -o ${OBJECTDIR}/_ext/1758509375/trees.o ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/trees.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1758509375/zutil.o: ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/zutil.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1758509375" 
	@${RM} ${OBJECTDIR}/_ext/1758509375/zutil.o.d 
	@${RM} ${OBJECTDIR}/_ext/1758509375/zutil.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1758509375/zutil.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1758509375/zutil.o.d" -o ${OBJECTDIR}/_ext/1758509375/zutil.o ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/zutil.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1758509375/inftrees.o: ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/inftrees.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1758509375" 
	@${RM} ${OBJECTDIR}/_ext/1758509375/inftrees.o.d 
	@${RM} ${OBJECTDIR}/_ext/1758509375/inftrees.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1758509375/inftrees.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1758509375/inftrees.o.d" -o ${OBJECTDIR}/_ext/1758509375/inftrees.o ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/inftrees.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1758509375/inffast.o: ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/inffast.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1758509375" 
	@${RM} ${OBJECTDIR}/_ext/1758509375/inffast.o.d 
	@${RM} ${OBJECTDIR}/_ext/1758509375/inffast.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1758509375/inffast.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1758509375/inffast.o.d" -o ${OBJECTDIR}/_ext/1758509375/inffast.o ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/inffast.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/aes.o: ../src/config/sam_e70_xult/crypto/src/aes.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/aes.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/aes.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/aes.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/aes.o.d" -o ${OBJECTDIR}/_ext/675556972/aes.o ../src/config/sam_e70_xult/crypto/src/aes.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/sha.o: ../src/config/sam_e70_xult/crypto/src/sha.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/sha.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/sha.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/sha.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/sha.o.d" -o ${OBJECTDIR}/_ext/675556972/sha.o ../src/config/sam_e70_xult/crypto/src/sha.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/sha256.o: ../src/config/sam_e70_xult/crypto/src/sha256.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/sha256.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/sha256.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/sha256.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/sha256.o.d" -o ${OBJECTDIR}/_ext/675556972/sha256.o ../src/config/sam_e70_xult/crypto/src/sha256.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/random.o: ../src/config/sam_e70_xult/crypto/src/random.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/random.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/random.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/random.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/random.o.d" -o ${OBJECTDIR}/_ext/675556972/random.o ../src/config/sam_e70_xult/crypto/src/random.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/random_same70.o: ../src/config/sam_e70_xult/crypto/src/random_same70.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/random_same70.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/random_same70.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/random_same70.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/random_same70.o.d" -o ${OBJECTDIR}/_ext/675556972/random_same70.o ../src/config/sam_e70_xult/crypto/src/random_same70.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/581080307/net_pres.o: ../src/config/sam_e70_xult/net/pres/src/net_pres.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/581080307" 
	@${RM} ${OBJECTDIR}/_ext/581080307/net_pres.o.d 
	@${RM} ${OBJECTDIR}/_ext/581080307/net_pres.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581080307/net_pres.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/581080307/net_pres.o.d" -o ${OBJECTDIR}/_ext/581080307/net_pres.o ../src/config/sam_e70_xult/net/pres/src/net_pres.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o: ../src/config/sam_e70_xult/net/pres/net_pres_enc_glue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2090003384" 
	@${RM} ${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o.d 
	@${RM} ${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o.d" -o ${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o ../src/config/sam_e70_xult/net/pres/net_pres_enc_glue.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1953779166/sys_console.o: ../src/config/sam_e70_xult/system/console/src/sys_console.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1953779166" 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_console.o.d 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_console.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1953779166/sys_console.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1953779166/sys_console.o.d" -o ${OBJECTDIR}/_ext/1953779166/sys_console.o ../src/config/sam_e70_xult/system/console/src/sys_console.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1953779166/sys_console_uart.o: ../src/config/sam_e70_xult/system/console/src/sys_console_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1953779166" 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_console_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_console_uart.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1953779166/sys_console_uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1953779166/sys_console_uart.o.d" -o ${OBJECTDIR}/_ext/1953779166/sys_console_uart.o ../src/config/sam_e70_xult/system/console/src/sys_console_uart.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1953779166/sys_debug.o: ../src/config/sam_e70_xult/system/console/src/sys_debug.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1953779166" 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_debug.o.d 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_debug.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1953779166/sys_debug.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1953779166/sys_debug.o.d" -o ${OBJECTDIR}/_ext/1953779166/sys_debug.o ../src/config/sam_e70_xult/system/console/src/sys_debug.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1953779166/sys_command.o: ../src/config/sam_e70_xult/system/console/src/sys_command.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1953779166" 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_command.o.d 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_command.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1953779166/sys_command.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1953779166/sys_command.o.d" -o ${OBJECTDIR}/_ext/1953779166/sys_command.o ../src/config/sam_e70_xult/system/console/src/sys_command.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1305476372/sys_fs.o: ../src/config/sam_e70_xult/system/fs/src/sys_fs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1305476372" 
	@${RM} ${OBJECTDIR}/_ext/1305476372/sys_fs.o.d 
	@${RM} ${OBJECTDIR}/_ext/1305476372/sys_fs.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1305476372/sys_fs.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1305476372/sys_fs.o.d" -o ${OBJECTDIR}/_ext/1305476372/sys_fs.o ../src/config/sam_e70_xult/system/fs/src/sys_fs.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1305476372/sys_fs_media_manager.o: ../src/config/sam_e70_xult/system/fs/src/sys_fs_media_manager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1305476372" 
	@${RM} ${OBJECTDIR}/_ext/1305476372/sys_fs_media_manager.o.d 
	@${RM} ${OBJECTDIR}/_ext/1305476372/sys_fs_media_manager.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1305476372/sys_fs_media_manager.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1305476372/sys_fs_media_manager.o.d" -o ${OBJECTDIR}/_ext/1305476372/sys_fs_media_manager.o ../src/config/sam_e70_xult/system/fs/src/sys_fs_media_manager.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/111743104/ff.o: ../src/config/sam_e70_xult/system/fs/fat_fs/src/ff.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/111743104" 
	@${RM} ${OBJECTDIR}/_ext/111743104/ff.o.d 
	@${RM} ${OBJECTDIR}/_ext/111743104/ff.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/111743104/ff.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/111743104/ff.o.d" -o ${OBJECTDIR}/_ext/111743104/ff.o ../src/config/sam_e70_xult/system/fs/fat_fs/src/ff.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/111743104/diskio.o: ../src/config/sam_e70_xult/system/fs/fat_fs/src/diskio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/111743104" 
	@${RM} ${OBJECTDIR}/_ext/111743104/diskio.o.d 
	@${RM} ${OBJECTDIR}/_ext/111743104/diskio.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/111743104/diskio.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/111743104/diskio.o.d" -o ${OBJECTDIR}/_ext/111743104/diskio.o ../src/config/sam_e70_xult/system/fs/fat_fs/src/diskio.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1521459453/mpfs.o: ../src/config/sam_e70_xult/system/fs/mpfs/src/mpfs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1521459453" 
	@${RM} ${OBJECTDIR}/_ext/1521459453/mpfs.o.d 
	@${RM} ${OBJECTDIR}/_ext/1521459453/mpfs.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1521459453/mpfs.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1521459453/mpfs.o.d" -o ${OBJECTDIR}/_ext/1521459453/mpfs.o ../src/config/sam_e70_xult/system/fs/mpfs/src/mpfs.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/327803284/sys_time.o: ../src/config/sam_e70_xult/system/time/src/sys_time.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/327803284" 
	@${RM} ${OBJECTDIR}/_ext/327803284/sys_time.o.d 
	@${RM} ${OBJECTDIR}/_ext/327803284/sys_time.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/327803284/sys_time.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/327803284/sys_time.o.d" -o ${OBJECTDIR}/_ext/327803284/sys_time.o ../src/config/sam_e70_xult/system/time/src/sys_time.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/865331892/plib_tc0.o: ../src/config/sam_e70_xult/peripheral/tc/plib_tc0.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/865331892" 
	@${RM} ${OBJECTDIR}/_ext/865331892/plib_tc0.o.d 
	@${RM} ${OBJECTDIR}/_ext/865331892/plib_tc0.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/865331892/plib_tc0.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/865331892/plib_tc0.o.d" -o ${OBJECTDIR}/_ext/865331892/plib_tc0.o ../src/config/sam_e70_xult/peripheral/tc/plib_tc0.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_announce.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_announce.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_announce.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_announce.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_announce.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_announce.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_announce.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_announce.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/arp.o: ../src/config/sam_e70_xult/library/tcpip/src/arp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/arp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/arp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/arp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/arp.o.d" -o ${OBJECTDIR}/_ext/902702131/arp.o ../src/config/sam_e70_xult/library/tcpip/src/arp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_commands.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_commands.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_commands.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_commands.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_commands.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_commands.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_commands.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_commands.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/dhcp.o: ../src/config/sam_e70_xult/library/tcpip/src/dhcp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/dhcp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/dhcp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/dhcp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/dhcp.o.d" -o ${OBJECTDIR}/_ext/902702131/dhcp.o ../src/config/sam_e70_xult/library/tcpip/src/dhcp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/dns.o: ../src/config/sam_e70_xult/library/tcpip/src/dns.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/dns.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/dns.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/dns.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/dns.o.d" -o ${OBJECTDIR}/_ext/902702131/dns.o ../src/config/sam_e70_xult/library/tcpip/src/dns.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/http.o: ../src/config/sam_e70_xult/library/tcpip/src/http.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/http.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/http.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/http.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/http.o.d" -o ${OBJECTDIR}/_ext/902702131/http.o ../src/config/sam_e70_xult/library/tcpip/src/http.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/917763083/http_print.o: ../src/config/sam_e70_xult/app/http_print.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/917763083" 
	@${RM} ${OBJECTDIR}/_ext/917763083/http_print.o.d 
	@${RM} ${OBJECTDIR}/_ext/917763083/http_print.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/917763083/http_print.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/917763083/http_print.o.d" -o ${OBJECTDIR}/_ext/917763083/http_print.o ../src/config/sam_e70_xult/app/http_print.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/917763083/custom_http_app.o: ../src/config/sam_e70_xult/app/custom_http_app.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/917763083" 
	@${RM} ${OBJECTDIR}/_ext/917763083/custom_http_app.o.d 
	@${RM} ${OBJECTDIR}/_ext/917763083/custom_http_app.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/917763083/custom_http_app.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/917763083/custom_http_app.o.d" -o ${OBJECTDIR}/_ext/917763083/custom_http_app.o ../src/config/sam_e70_xult/app/custom_http_app.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/ipv4.o: ../src/config/sam_e70_xult/library/tcpip/src/ipv4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/ipv4.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/ipv4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/ipv4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/ipv4.o.d" -o ${OBJECTDIR}/_ext/902702131/ipv4.o ../src/config/sam_e70_xult/library/tcpip/src/ipv4.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/ipv6.o: ../src/config/sam_e70_xult/library/tcpip/src/ipv6.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/ipv6.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/ipv6.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/ipv6.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/ipv6.o.d" -o ${OBJECTDIR}/_ext/902702131/ipv6.o ../src/config/sam_e70_xult/library/tcpip/src/ipv6.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/icmpv6.o: ../src/config/sam_e70_xult/library/tcpip/src/icmpv6.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/icmpv6.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/icmpv6.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/icmpv6.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/icmpv6.o.d" -o ${OBJECTDIR}/_ext/902702131/icmpv6.o ../src/config/sam_e70_xult/library/tcpip/src/icmpv6.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/icmp.o: ../src/config/sam_e70_xult/library/tcpip/src/icmp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/icmp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/icmp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/icmp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/icmp.o.d" -o ${OBJECTDIR}/_ext/902702131/icmp.o ../src/config/sam_e70_xult/library/tcpip/src/icmp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/nbns.o: ../src/config/sam_e70_xult/library/tcpip/src/nbns.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/nbns.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/nbns.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/nbns.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/nbns.o.d" -o ${OBJECTDIR}/_ext/902702131/nbns.o ../src/config/sam_e70_xult/library/tcpip/src/nbns.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/ndp.o: ../src/config/sam_e70_xult/library/tcpip/src/ndp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/ndp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/ndp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/ndp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/ndp.o.d" -o ${OBJECTDIR}/_ext/902702131/ndp.o ../src/config/sam_e70_xult/library/tcpip/src/ndp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/smtpc.o: ../src/config/sam_e70_xult/library/tcpip/src/smtpc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/smtpc.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/smtpc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/smtpc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/smtpc.o.d" -o ${OBJECTDIR}/_ext/902702131/smtpc.o ../src/config/sam_e70_xult/library/tcpip/src/smtpc.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/snmp.o: ../src/config/sam_e70_xult/library/tcpip/src/snmp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/snmp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/snmp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/snmp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/snmp.o.d" -o ${OBJECTDIR}/_ext/902702131/snmp.o ../src/config/sam_e70_xult/library/tcpip/src/snmp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/custom_snmp_app.o: ../src/config/sam_e70_xult/custom_snmp_app.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/custom_snmp_app.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/custom_snmp_app.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/custom_snmp_app.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/custom_snmp_app.o.d" -o ${OBJECTDIR}/_ext/1728594169/custom_snmp_app.o ../src/config/sam_e70_xult/custom_snmp_app.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/snmpv3.o: ../src/config/sam_e70_xult/library/tcpip/src/snmpv3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/snmpv3.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/snmpv3.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/snmpv3.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/snmpv3.o.d" -o ${OBJECTDIR}/_ext/902702131/snmpv3.o ../src/config/sam_e70_xult/library/tcpip/src/snmpv3.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/snmpv3_usm.o: ../src/config/sam_e70_xult/library/tcpip/src/snmpv3_usm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/snmpv3_usm.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/snmpv3_usm.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/snmpv3_usm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/snmpv3_usm.o.d" -o ${OBJECTDIR}/_ext/902702131/snmpv3_usm.o ../src/config/sam_e70_xult/library/tcpip/src/snmpv3_usm.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/sntp.o: ../src/config/sam_e70_xult/library/tcpip/src/sntp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/sntp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/sntp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/sntp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/sntp.o.d" -o ${OBJECTDIR}/_ext/902702131/sntp.o ../src/config/sam_e70_xult/library/tcpip/src/sntp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_alloc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_alloc.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_internal.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_internal.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/helpers.o: ../src/config/sam_e70_xult/library/tcpip/src/helpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/helpers.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/helpers.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/helpers.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/helpers.o.d" -o ${OBJECTDIR}/_ext/902702131/helpers.o ../src/config/sam_e70_xult/library/tcpip/src/helpers.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/hash_fnv.o: ../src/config/sam_e70_xult/library/tcpip/src/hash_fnv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/hash_fnv.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/hash_fnv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/hash_fnv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/hash_fnv.o.d" -o ${OBJECTDIR}/_ext/902702131/hash_fnv.o ../src/config/sam_e70_xult/library/tcpip/src/hash_fnv.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/oahash.o: ../src/config/sam_e70_xult/library/tcpip/src/oahash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/oahash.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/oahash.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/oahash.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/oahash.o.d" -o ${OBJECTDIR}/_ext/902702131/oahash.o ../src/config/sam_e70_xult/library/tcpip/src/oahash.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_helpers.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_helpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_helpers.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_helpers.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_helpers.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_helpers.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_helpers.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_helpers.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_manager.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_manager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_manager.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_manager.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_manager.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_manager.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_manager.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_manager.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_notify.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_notify.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_notify.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_notify.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_notify.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_notify.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_notify.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_notify.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_packet.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_packet.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_packet.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_packet.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_packet.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_packet.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_packet.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_packet.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o: ../src/config/sam_e70_xult/system/sys_time_h2_adapter.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/157960187" 
	@${RM} ${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o.d 
	@${RM} ${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o.d" -o ${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o ../src/config/sam_e70_xult/system/sys_time_h2_adapter.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/136402759/sys_fs_wrapper.o: ../src/config/sam_e70_xult/library/tcpip/src/common/sys_fs_wrapper.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/136402759" 
	@${RM} ${OBJECTDIR}/_ext/136402759/sys_fs_wrapper.o.d 
	@${RM} ${OBJECTDIR}/_ext/136402759/sys_fs_wrapper.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/136402759/sys_fs_wrapper.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/136402759/sys_fs_wrapper.o.d" -o ${OBJECTDIR}/_ext/136402759/sys_fs_wrapper.o ../src/config/sam_e70_xult/library/tcpip/src/common/sys_fs_wrapper.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcp.o: ../src/config/sam_e70_xult/library/tcpip/src/tcp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcp.o.d" -o ${OBJECTDIR}/_ext/902702131/tcp.o ../src/config/sam_e70_xult/library/tcpip/src/tcp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/udp.o: ../src/config/sam_e70_xult/library/tcpip/src/udp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/udp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/udp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/udp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/udp.o.d" -o ${OBJECTDIR}/_ext/902702131/udp.o ../src/config/sam_e70_xult/library/tcpip/src/udp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/707186936/plib_usart1.o: ../src/config/sam_e70_xult/peripheral/usart/plib_usart1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/707186936" 
	@${RM} ${OBJECTDIR}/_ext/707186936/plib_usart1.o.d 
	@${RM} ${OBJECTDIR}/_ext/707186936/plib_usart1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/707186936/plib_usart1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/707186936/plib_usart1.o.d" -o ${OBJECTDIR}/_ext/707186936/plib_usart1.o ../src/config/sam_e70_xult/peripheral/usart/plib_usart1.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
else
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/initialization.o: ../src/config/sam_e70_xult/initialization.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/initialization.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/initialization.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/initialization.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/initialization.o.d" -o ${OBJECTDIR}/_ext/1728594169/initialization.o ../src/config/sam_e70_xult/initialization.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/interrupts.o: ../src/config/sam_e70_xult/interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/interrupts.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/interrupts.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/interrupts.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/interrupts.o.d" -o ${OBJECTDIR}/_ext/1728594169/interrupts.o ../src/config/sam_e70_xult/interrupts.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1518761459/xc32_monitor.o: ../src/config/sam_e70_xult/stdio/xc32_monitor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1518761459" 
	@${RM} ${OBJECTDIR}/_ext/1518761459/xc32_monitor.o.d 
	@${RM} ${OBJECTDIR}/_ext/1518761459/xc32_monitor.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1518761459/xc32_monitor.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1518761459/xc32_monitor.o.d" -o ${OBJECTDIR}/_ext/1518761459/xc32_monitor.o ../src/config/sam_e70_xult/stdio/xc32_monitor.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/exceptions.o: ../src/config/sam_e70_xult/exceptions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/exceptions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/exceptions.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/exceptions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/exceptions.o.d" -o ${OBJECTDIR}/_ext/1728594169/exceptions.o ../src/config/sam_e70_xult/exceptions.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1055500827/plib_clk.o: ../src/config/sam_e70_xult/peripheral/clk/plib_clk.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1055500827" 
	@${RM} ${OBJECTDIR}/_ext/1055500827/plib_clk.o.d 
	@${RM} ${OBJECTDIR}/_ext/1055500827/plib_clk.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055500827/plib_clk.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1055500827/plib_clk.o.d" -o ${OBJECTDIR}/_ext/1055500827/plib_clk.o ../src/config/sam_e70_xult/peripheral/clk/plib_clk.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1055488423/plib_pio.o: ../src/config/sam_e70_xult/peripheral/pio/plib_pio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1055488423" 
	@${RM} ${OBJECTDIR}/_ext/1055488423/plib_pio.o.d 
	@${RM} ${OBJECTDIR}/_ext/1055488423/plib_pio.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055488423/plib_pio.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1055488423/plib_pio.o.d" -o ${OBJECTDIR}/_ext/1055488423/plib_pio.o ../src/config/sam_e70_xult/peripheral/pio/plib_pio.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1639550079/plib_nvic.o: ../src/config/sam_e70_xult/peripheral/nvic/plib_nvic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1639550079" 
	@${RM} ${OBJECTDIR}/_ext/1639550079/plib_nvic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1639550079/plib_nvic.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1639550079/plib_nvic.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1639550079/plib_nvic.o.d" -o ${OBJECTDIR}/_ext/1639550079/plib_nvic.o ../src/config/sam_e70_xult/peripheral/nvic/plib_nvic.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1055491083/plib_mpu.o: ../src/config/sam_e70_xult/peripheral/mpu/plib_mpu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1055491083" 
	@${RM} ${OBJECTDIR}/_ext/1055491083/plib_mpu.o.d 
	@${RM} ${OBJECTDIR}/_ext/1055491083/plib_mpu.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055491083/plib_mpu.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1055491083/plib_mpu.o.d" -o ${OBJECTDIR}/_ext/1055491083/plib_mpu.o ../src/config/sam_e70_xult/peripheral/mpu/plib_mpu.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/704852250/plib_xdmac.o: ../src/config/sam_e70_xult/peripheral/xdmac/plib_xdmac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/704852250" 
	@${RM} ${OBJECTDIR}/_ext/704852250/plib_xdmac.o.d 
	@${RM} ${OBJECTDIR}/_ext/704852250/plib_xdmac.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/704852250/plib_xdmac.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/704852250/plib_xdmac.o.d" -o ${OBJECTDIR}/_ext/704852250/plib_xdmac.o ../src/config/sam_e70_xult/peripheral/xdmac/plib_xdmac.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/startup.o: ../src/config/sam_e70_xult/startup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/startup.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/startup.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/startup.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/startup.o.d" -o ${OBJECTDIR}/_ext/1728594169/startup.o ../src/config/sam_e70_xult/startup.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/libc_syscalls.o: ../src/config/sam_e70_xult/libc_syscalls.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/libc_syscalls.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/libc_syscalls.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/libc_syscalls.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/libc_syscalls.o.d" -o ${OBJECTDIR}/_ext/1728594169/libc_syscalls.o ../src/config/sam_e70_xult/libc_syscalls.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/917764137/bsp.o: ../src/config/sam_e70_xult/bsp/bsp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/917764137" 
	@${RM} ${OBJECTDIR}/_ext/917764137/bsp.o.d 
	@${RM} ${OBJECTDIR}/_ext/917764137/bsp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/917764137/bsp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/917764137/bsp.o.d" -o ${OBJECTDIR}/_ext/917764137/bsp.o ../src/config/sam_e70_xult/bsp/bsp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/766002106/sys_int.o: ../src/config/sam_e70_xult/system/int/src/sys_int.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/766002106" 
	@${RM} ${OBJECTDIR}/_ext/766002106/sys_int.o.d 
	@${RM} ${OBJECTDIR}/_ext/766002106/sys_int.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/766002106/sys_int.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/766002106/sys_int.o.d" -o ${OBJECTDIR}/_ext/766002106/sys_int.o ../src/config/sam_e70_xult/system/int/src/sys_int.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/app.o: ../src/app.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/app.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1360937237/app.o.d" -o ${OBJECTDIR}/_ext/1360937237/app.o ../src/app.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/tasks.o: ../src/config/sam_e70_xult/tasks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/tasks.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/tasks.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/tasks.o.d" -o ${OBJECTDIR}/_ext/1728594169/tasks.o ../src/config/sam_e70_xult/tasks.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1015456894/drv_ethphy.o: ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_ethphy.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1015456894" 
	@${RM} ${OBJECTDIR}/_ext/1015456894/drv_ethphy.o.d 
	@${RM} ${OBJECTDIR}/_ext/1015456894/drv_ethphy.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1015456894/drv_ethphy.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1015456894/drv_ethphy.o.d" -o ${OBJECTDIR}/_ext/1015456894/drv_ethphy.o ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_ethphy.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o: ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_extphy_smsc8740.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1015456894" 
	@${RM} ${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o.d 
	@${RM} ${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o.d" -o ${OBJECTDIR}/_ext/1015456894/drv_extphy_smsc8740.o ../src/config/sam_e70_xult/driver/ethphy/src/dynamic/drv_extphy_smsc8740.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/478869302/drv_miim.o: ../src/config/sam_e70_xult/driver/miim/src/dynamic/drv_miim.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/478869302" 
	@${RM} ${OBJECTDIR}/_ext/478869302/drv_miim.o.d 
	@${RM} ${OBJECTDIR}/_ext/478869302/drv_miim.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/478869302/drv_miim.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/478869302/drv_miim.o.d" -o ${OBJECTDIR}/_ext/478869302/drv_miim.o ../src/config/sam_e70_xult/driver/miim/src/dynamic/drv_miim.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/629088514/drv_gmac.o: ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/629088514" 
	@${RM} ${OBJECTDIR}/_ext/629088514/drv_gmac.o.d 
	@${RM} ${OBJECTDIR}/_ext/629088514/drv_gmac.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/629088514/drv_gmac.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/629088514/drv_gmac.o.d" -o ${OBJECTDIR}/_ext/629088514/drv_gmac.o ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o: ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac_lib.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/629088514" 
	@${RM} ${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o.d 
	@${RM} ${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o.d" -o ${OBJECTDIR}/_ext/629088514/drv_gmac_lib.o ../src/config/sam_e70_xult/driver/gmac/src/dynamic/drv_gmac_lib.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2043027118/drv_sdhc_file_system.o: ../src/config/sam_e70_xult/driver/sdhc/src/drv_sdhc_file_system.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2043027118" 
	@${RM} ${OBJECTDIR}/_ext/2043027118/drv_sdhc_file_system.o.d 
	@${RM} ${OBJECTDIR}/_ext/2043027118/drv_sdhc_file_system.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2043027118/drv_sdhc_file_system.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/2043027118/drv_sdhc_file_system.o.d" -o ${OBJECTDIR}/_ext/2043027118/drv_sdhc_file_system.o ../src/config/sam_e70_xult/driver/sdhc/src/drv_sdhc_file_system.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2043027118/drv_sdhc.o: ../src/config/sam_e70_xult/driver/sdhc/src/drv_sdhc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2043027118" 
	@${RM} ${OBJECTDIR}/_ext/2043027118/drv_sdhc.o.d 
	@${RM} ${OBJECTDIR}/_ext/2043027118/drv_sdhc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2043027118/drv_sdhc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/2043027118/drv_sdhc.o.d" -o ${OBJECTDIR}/_ext/2043027118/drv_sdhc.o ../src/config/sam_e70_xult/driver/sdhc/src/drv_sdhc.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2043027118/drv_sdhc_host.o: ../src/config/sam_e70_xult/driver/sdhc/src/drv_sdhc_host.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2043027118" 
	@${RM} ${OBJECTDIR}/_ext/2043027118/drv_sdhc_host.o.d 
	@${RM} ${OBJECTDIR}/_ext/2043027118/drv_sdhc_host.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2043027118/drv_sdhc_host.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/2043027118/drv_sdhc_host.o.d" -o ${OBJECTDIR}/_ext/2043027118/drv_sdhc_host.o ../src/config/sam_e70_xult/driver/sdhc/src/drv_sdhc_host.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/arc4.o: ../src/config/sam_e70_xult/crypto/src/arc4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/arc4.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/arc4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/arc4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/arc4.o.d" -o ${OBJECTDIR}/_ext/675556972/arc4.o ../src/config/sam_e70_xult/crypto/src/arc4.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/asm.o: ../src/config/sam_e70_xult/crypto/src/asm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/asm.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/asm.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/asm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/asm.o.d" -o ${OBJECTDIR}/_ext/675556972/asm.o ../src/config/sam_e70_xult/crypto/src/asm.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/asn.o: ../src/config/sam_e70_xult/crypto/src/asn.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/asn.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/asn.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/asn.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/asn.o.d" -o ${OBJECTDIR}/_ext/675556972/asn.o ../src/config/sam_e70_xult/crypto/src/asn.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/async.o: ../src/config/sam_e70_xult/crypto/src/async.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/async.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/async.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/async.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/async.o.d" -o ${OBJECTDIR}/_ext/675556972/async.o ../src/config/sam_e70_xult/crypto/src/async.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/chacha.o: ../src/config/sam_e70_xult/crypto/src/chacha.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/chacha.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/chacha.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/chacha.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/chacha.o.d" -o ${OBJECTDIR}/_ext/675556972/chacha.o ../src/config/sam_e70_xult/crypto/src/chacha.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/chacha20_poly1305.o: ../src/config/sam_e70_xult/crypto/src/chacha20_poly1305.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/chacha20_poly1305.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/chacha20_poly1305.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/chacha20_poly1305.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/chacha20_poly1305.o.d" -o ${OBJECTDIR}/_ext/675556972/chacha20_poly1305.o ../src/config/sam_e70_xult/crypto/src/chacha20_poly1305.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/cmac.o: ../src/config/sam_e70_xult/crypto/src/cmac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/cmac.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/cmac.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/cmac.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/cmac.o.d" -o ${OBJECTDIR}/_ext/675556972/cmac.o ../src/config/sam_e70_xult/crypto/src/cmac.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/coding.o: ../src/config/sam_e70_xult/crypto/src/coding.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/coding.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/coding.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/coding.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/coding.o.d" -o ${OBJECTDIR}/_ext/675556972/coding.o ../src/config/sam_e70_xult/crypto/src/coding.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/compress.o: ../src/config/sam_e70_xult/crypto/src/compress.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/compress.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/compress.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/compress.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/compress.o.d" -o ${OBJECTDIR}/_ext/675556972/compress.o ../src/config/sam_e70_xult/crypto/src/compress.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/crypto.o: ../src/config/sam_e70_xult/crypto/src/crypto.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/crypto.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/crypto.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/crypto.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/crypto.o.d" -o ${OBJECTDIR}/_ext/675556972/crypto.o ../src/config/sam_e70_xult/crypto/src/crypto.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/curve25519.o: ../src/config/sam_e70_xult/crypto/src/curve25519.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/curve25519.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/curve25519.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/curve25519.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/curve25519.o.d" -o ${OBJECTDIR}/_ext/675556972/curve25519.o ../src/config/sam_e70_xult/crypto/src/curve25519.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/des3.o: ../src/config/sam_e70_xult/crypto/src/des3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/des3.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/des3.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/des3.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/des3.o.d" -o ${OBJECTDIR}/_ext/675556972/des3.o ../src/config/sam_e70_xult/crypto/src/des3.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/dh.o: ../src/config/sam_e70_xult/crypto/src/dh.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/dh.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/dh.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/dh.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/dh.o.d" -o ${OBJECTDIR}/_ext/675556972/dh.o ../src/config/sam_e70_xult/crypto/src/dh.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/dsa.o: ../src/config/sam_e70_xult/crypto/src/dsa.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/dsa.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/dsa.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/dsa.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/dsa.o.d" -o ${OBJECTDIR}/_ext/675556972/dsa.o ../src/config/sam_e70_xult/crypto/src/dsa.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/ecc.o: ../src/config/sam_e70_xult/crypto/src/ecc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/ecc.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/ecc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/ecc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/ecc.o.d" -o ${OBJECTDIR}/_ext/675556972/ecc.o ../src/config/sam_e70_xult/crypto/src/ecc.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/ecc_fp.o: ../src/config/sam_e70_xult/crypto/src/ecc_fp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/ecc_fp.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/ecc_fp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/ecc_fp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/ecc_fp.o.d" -o ${OBJECTDIR}/_ext/675556972/ecc_fp.o ../src/config/sam_e70_xult/crypto/src/ecc_fp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/ed25519.o: ../src/config/sam_e70_xult/crypto/src/ed25519.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/ed25519.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/ed25519.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/ed25519.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/ed25519.o.d" -o ${OBJECTDIR}/_ext/675556972/ed25519.o ../src/config/sam_e70_xult/crypto/src/ed25519.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/error.o: ../src/config/sam_e70_xult/crypto/src/error.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/error.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/error.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/error.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/error.o.d" -o ${OBJECTDIR}/_ext/675556972/error.o ../src/config/sam_e70_xult/crypto/src/error.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/fe_low_mem.o: ../src/config/sam_e70_xult/crypto/src/fe_low_mem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/fe_low_mem.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/fe_low_mem.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/fe_low_mem.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/fe_low_mem.o.d" -o ${OBJECTDIR}/_ext/675556972/fe_low_mem.o ../src/config/sam_e70_xult/crypto/src/fe_low_mem.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/fe_operations.o: ../src/config/sam_e70_xult/crypto/src/fe_operations.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/fe_operations.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/fe_operations.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/fe_operations.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/fe_operations.o.d" -o ${OBJECTDIR}/_ext/675556972/fe_operations.o ../src/config/sam_e70_xult/crypto/src/fe_operations.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/ge_low_mem.o: ../src/config/sam_e70_xult/crypto/src/ge_low_mem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/ge_low_mem.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/ge_low_mem.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/ge_low_mem.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/ge_low_mem.o.d" -o ${OBJECTDIR}/_ext/675556972/ge_low_mem.o ../src/config/sam_e70_xult/crypto/src/ge_low_mem.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/ge_operations.o: ../src/config/sam_e70_xult/crypto/src/ge_operations.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/ge_operations.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/ge_operations.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/ge_operations.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/ge_operations.o.d" -o ${OBJECTDIR}/_ext/675556972/ge_operations.o ../src/config/sam_e70_xult/crypto/src/ge_operations.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/hash.o: ../src/config/sam_e70_xult/crypto/src/hash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/hash.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/hash.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/hash.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/hash.o.d" -o ${OBJECTDIR}/_ext/675556972/hash.o ../src/config/sam_e70_xult/crypto/src/hash.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/hc128.o: ../src/config/sam_e70_xult/crypto/src/hc128.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/hc128.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/hc128.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/hc128.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/hc128.o.d" -o ${OBJECTDIR}/_ext/675556972/hc128.o ../src/config/sam_e70_xult/crypto/src/hc128.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/hmac.o: ../src/config/sam_e70_xult/crypto/src/hmac.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/hmac.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/hmac.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/hmac.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/hmac.o.d" -o ${OBJECTDIR}/_ext/675556972/hmac.o ../src/config/sam_e70_xult/crypto/src/hmac.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/idea.o: ../src/config/sam_e70_xult/crypto/src/idea.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/idea.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/idea.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/idea.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/idea.o.d" -o ${OBJECTDIR}/_ext/675556972/idea.o ../src/config/sam_e70_xult/crypto/src/idea.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/integer.o: ../src/config/sam_e70_xult/crypto/src/integer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/integer.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/integer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/integer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/integer.o.d" -o ${OBJECTDIR}/_ext/675556972/integer.o ../src/config/sam_e70_xult/crypto/src/integer.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/logging.o: ../src/config/sam_e70_xult/crypto/src/logging.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/logging.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/logging.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/logging.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/logging.o.d" -o ${OBJECTDIR}/_ext/675556972/logging.o ../src/config/sam_e70_xult/crypto/src/logging.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/md2.o: ../src/config/sam_e70_xult/crypto/src/md2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/md2.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/md2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/md2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/md2.o.d" -o ${OBJECTDIR}/_ext/675556972/md2.o ../src/config/sam_e70_xult/crypto/src/md2.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/md4.o: ../src/config/sam_e70_xult/crypto/src/md4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/md4.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/md4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/md4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/md4.o.d" -o ${OBJECTDIR}/_ext/675556972/md4.o ../src/config/sam_e70_xult/crypto/src/md4.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/md5.o: ../src/config/sam_e70_xult/crypto/src/md5.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/md5.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/md5.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/md5.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/md5.o.d" -o ${OBJECTDIR}/_ext/675556972/md5.o ../src/config/sam_e70_xult/crypto/src/md5.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/memory.o: ../src/config/sam_e70_xult/crypto/src/memory.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/memory.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/memory.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/memory.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/memory.o.d" -o ${OBJECTDIR}/_ext/675556972/memory.o ../src/config/sam_e70_xult/crypto/src/memory.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/misc.o: ../src/config/sam_e70_xult/crypto/src/misc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/misc.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/misc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/misc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/misc.o.d" -o ${OBJECTDIR}/_ext/675556972/misc.o ../src/config/sam_e70_xult/crypto/src/misc.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/pkcs12.o: ../src/config/sam_e70_xult/crypto/src/pkcs12.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/pkcs12.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/pkcs12.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/pkcs12.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/pkcs12.o.d" -o ${OBJECTDIR}/_ext/675556972/pkcs12.o ../src/config/sam_e70_xult/crypto/src/pkcs12.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/pkcs7.o: ../src/config/sam_e70_xult/crypto/src/pkcs7.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/pkcs7.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/pkcs7.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/pkcs7.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/pkcs7.o.d" -o ${OBJECTDIR}/_ext/675556972/pkcs7.o ../src/config/sam_e70_xult/crypto/src/pkcs7.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/poly1305.o: ../src/config/sam_e70_xult/crypto/src/poly1305.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/poly1305.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/poly1305.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/poly1305.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/poly1305.o.d" -o ${OBJECTDIR}/_ext/675556972/poly1305.o ../src/config/sam_e70_xult/crypto/src/poly1305.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/pwdbased.o: ../src/config/sam_e70_xult/crypto/src/pwdbased.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/pwdbased.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/pwdbased.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/pwdbased.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/pwdbased.o.d" -o ${OBJECTDIR}/_ext/675556972/pwdbased.o ../src/config/sam_e70_xult/crypto/src/pwdbased.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/rabbit.o: ../src/config/sam_e70_xult/crypto/src/rabbit.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/rabbit.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/rabbit.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/rabbit.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/rabbit.o.d" -o ${OBJECTDIR}/_ext/675556972/rabbit.o ../src/config/sam_e70_xult/crypto/src/rabbit.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/ripemd.o: ../src/config/sam_e70_xult/crypto/src/ripemd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/ripemd.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/ripemd.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/ripemd.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/ripemd.o.d" -o ${OBJECTDIR}/_ext/675556972/ripemd.o ../src/config/sam_e70_xult/crypto/src/ripemd.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/rsa.o: ../src/config/sam_e70_xult/crypto/src/rsa.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/rsa.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/rsa.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/rsa.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/rsa.o.d" -o ${OBJECTDIR}/_ext/675556972/rsa.o ../src/config/sam_e70_xult/crypto/src/rsa.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/sha3.o: ../src/config/sam_e70_xult/crypto/src/sha3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/sha3.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/sha3.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/sha3.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/sha3.o.d" -o ${OBJECTDIR}/_ext/675556972/sha3.o ../src/config/sam_e70_xult/crypto/src/sha3.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/sha512.o: ../src/config/sam_e70_xult/crypto/src/sha512.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/sha512.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/sha512.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/sha512.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/sha512.o.d" -o ${OBJECTDIR}/_ext/675556972/sha512.o ../src/config/sam_e70_xult/crypto/src/sha512.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/signature.o: ../src/config/sam_e70_xult/crypto/src/signature.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/signature.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/signature.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/signature.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/signature.o.d" -o ${OBJECTDIR}/_ext/675556972/signature.o ../src/config/sam_e70_xult/crypto/src/signature.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/srp.o: ../src/config/sam_e70_xult/crypto/src/srp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/srp.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/srp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/srp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/srp.o.d" -o ${OBJECTDIR}/_ext/675556972/srp.o ../src/config/sam_e70_xult/crypto/src/srp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/tfm.o: ../src/config/sam_e70_xult/crypto/src/tfm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/tfm.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/tfm.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/tfm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/tfm.o.d" -o ${OBJECTDIR}/_ext/675556972/tfm.o ../src/config/sam_e70_xult/crypto/src/tfm.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/wc_encrypt.o: ../src/config/sam_e70_xult/crypto/src/wc_encrypt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/wc_encrypt.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/wc_encrypt.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/wc_encrypt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/wc_encrypt.o.d" -o ${OBJECTDIR}/_ext/675556972/wc_encrypt.o ../src/config/sam_e70_xult/crypto/src/wc_encrypt.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/wc_port.o: ../src/config/sam_e70_xult/crypto/src/wc_port.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/wc_port.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/wc_port.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/wc_port.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/wc_port.o.d" -o ${OBJECTDIR}/_ext/675556972/wc_port.o ../src/config/sam_e70_xult/crypto/src/wc_port.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/wolfevent.o: ../src/config/sam_e70_xult/crypto/src/wolfevent.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/wolfevent.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/wolfevent.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/wolfevent.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/wolfevent.o.d" -o ${OBJECTDIR}/_ext/675556972/wolfevent.o ../src/config/sam_e70_xult/crypto/src/wolfevent.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/wolfmath.o: ../src/config/sam_e70_xult/crypto/src/wolfmath.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/wolfmath.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/wolfmath.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/wolfmath.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/wolfmath.o.d" -o ${OBJECTDIR}/_ext/675556972/wolfmath.o ../src/config/sam_e70_xult/crypto/src/wolfmath.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1758509375/adler32.o: ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/adler32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1758509375" 
	@${RM} ${OBJECTDIR}/_ext/1758509375/adler32.o.d 
	@${RM} ${OBJECTDIR}/_ext/1758509375/adler32.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1758509375/adler32.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1758509375/adler32.o.d" -o ${OBJECTDIR}/_ext/1758509375/adler32.o ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/adler32.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1758509375/crc32.o: ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/crc32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1758509375" 
	@${RM} ${OBJECTDIR}/_ext/1758509375/crc32.o.d 
	@${RM} ${OBJECTDIR}/_ext/1758509375/crc32.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1758509375/crc32.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1758509375/crc32.o.d" -o ${OBJECTDIR}/_ext/1758509375/crc32.o ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/crc32.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1758509375/deflate.o: ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/deflate.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1758509375" 
	@${RM} ${OBJECTDIR}/_ext/1758509375/deflate.o.d 
	@${RM} ${OBJECTDIR}/_ext/1758509375/deflate.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1758509375/deflate.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1758509375/deflate.o.d" -o ${OBJECTDIR}/_ext/1758509375/deflate.o ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/deflate.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1758509375/inflate.o: ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/inflate.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1758509375" 
	@${RM} ${OBJECTDIR}/_ext/1758509375/inflate.o.d 
	@${RM} ${OBJECTDIR}/_ext/1758509375/inflate.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1758509375/inflate.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1758509375/inflate.o.d" -o ${OBJECTDIR}/_ext/1758509375/inflate.o ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/inflate.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1758509375/trees.o: ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/trees.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1758509375" 
	@${RM} ${OBJECTDIR}/_ext/1758509375/trees.o.d 
	@${RM} ${OBJECTDIR}/_ext/1758509375/trees.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1758509375/trees.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1758509375/trees.o.d" -o ${OBJECTDIR}/_ext/1758509375/trees.o ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/trees.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1758509375/zutil.o: ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/zutil.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1758509375" 
	@${RM} ${OBJECTDIR}/_ext/1758509375/zutil.o.d 
	@${RM} ${OBJECTDIR}/_ext/1758509375/zutil.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1758509375/zutil.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1758509375/zutil.o.d" -o ${OBJECTDIR}/_ext/1758509375/zutil.o ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/zutil.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1758509375/inftrees.o: ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/inftrees.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1758509375" 
	@${RM} ${OBJECTDIR}/_ext/1758509375/inftrees.o.d 
	@${RM} ${OBJECTDIR}/_ext/1758509375/inftrees.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1758509375/inftrees.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1758509375/inftrees.o.d" -o ${OBJECTDIR}/_ext/1758509375/inftrees.o ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/inftrees.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1758509375/inffast.o: ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/inffast.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1758509375" 
	@${RM} ${OBJECTDIR}/_ext/1758509375/inffast.o.d 
	@${RM} ${OBJECTDIR}/_ext/1758509375/inffast.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1758509375/inffast.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1758509375/inffast.o.d" -o ${OBJECTDIR}/_ext/1758509375/inffast.o ../src/config/sam_e70_xult/crypto/src/zlib-1.2.7/inffast.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/aes.o: ../src/config/sam_e70_xult/crypto/src/aes.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/aes.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/aes.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/aes.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/aes.o.d" -o ${OBJECTDIR}/_ext/675556972/aes.o ../src/config/sam_e70_xult/crypto/src/aes.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/sha.o: ../src/config/sam_e70_xult/crypto/src/sha.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/sha.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/sha.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/sha.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/sha.o.d" -o ${OBJECTDIR}/_ext/675556972/sha.o ../src/config/sam_e70_xult/crypto/src/sha.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/sha256.o: ../src/config/sam_e70_xult/crypto/src/sha256.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/sha256.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/sha256.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/sha256.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/sha256.o.d" -o ${OBJECTDIR}/_ext/675556972/sha256.o ../src/config/sam_e70_xult/crypto/src/sha256.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/random.o: ../src/config/sam_e70_xult/crypto/src/random.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/random.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/random.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/random.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/random.o.d" -o ${OBJECTDIR}/_ext/675556972/random.o ../src/config/sam_e70_xult/crypto/src/random.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/675556972/random_same70.o: ../src/config/sam_e70_xult/crypto/src/random_same70.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/675556972" 
	@${RM} ${OBJECTDIR}/_ext/675556972/random_same70.o.d 
	@${RM} ${OBJECTDIR}/_ext/675556972/random_same70.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/675556972/random_same70.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/675556972/random_same70.o.d" -o ${OBJECTDIR}/_ext/675556972/random_same70.o ../src/config/sam_e70_xult/crypto/src/random_same70.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/581080307/net_pres.o: ../src/config/sam_e70_xult/net/pres/src/net_pres.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/581080307" 
	@${RM} ${OBJECTDIR}/_ext/581080307/net_pres.o.d 
	@${RM} ${OBJECTDIR}/_ext/581080307/net_pres.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581080307/net_pres.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/581080307/net_pres.o.d" -o ${OBJECTDIR}/_ext/581080307/net_pres.o ../src/config/sam_e70_xult/net/pres/src/net_pres.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o: ../src/config/sam_e70_xult/net/pres/net_pres_enc_glue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2090003384" 
	@${RM} ${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o.d 
	@${RM} ${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o.d" -o ${OBJECTDIR}/_ext/2090003384/net_pres_enc_glue.o ../src/config/sam_e70_xult/net/pres/net_pres_enc_glue.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1953779166/sys_console.o: ../src/config/sam_e70_xult/system/console/src/sys_console.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1953779166" 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_console.o.d 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_console.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1953779166/sys_console.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1953779166/sys_console.o.d" -o ${OBJECTDIR}/_ext/1953779166/sys_console.o ../src/config/sam_e70_xult/system/console/src/sys_console.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1953779166/sys_console_uart.o: ../src/config/sam_e70_xult/system/console/src/sys_console_uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1953779166" 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_console_uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_console_uart.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1953779166/sys_console_uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1953779166/sys_console_uart.o.d" -o ${OBJECTDIR}/_ext/1953779166/sys_console_uart.o ../src/config/sam_e70_xult/system/console/src/sys_console_uart.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1953779166/sys_debug.o: ../src/config/sam_e70_xult/system/console/src/sys_debug.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1953779166" 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_debug.o.d 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_debug.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1953779166/sys_debug.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1953779166/sys_debug.o.d" -o ${OBJECTDIR}/_ext/1953779166/sys_debug.o ../src/config/sam_e70_xult/system/console/src/sys_debug.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1953779166/sys_command.o: ../src/config/sam_e70_xult/system/console/src/sys_command.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1953779166" 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_command.o.d 
	@${RM} ${OBJECTDIR}/_ext/1953779166/sys_command.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1953779166/sys_command.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1953779166/sys_command.o.d" -o ${OBJECTDIR}/_ext/1953779166/sys_command.o ../src/config/sam_e70_xult/system/console/src/sys_command.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1305476372/sys_fs.o: ../src/config/sam_e70_xult/system/fs/src/sys_fs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1305476372" 
	@${RM} ${OBJECTDIR}/_ext/1305476372/sys_fs.o.d 
	@${RM} ${OBJECTDIR}/_ext/1305476372/sys_fs.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1305476372/sys_fs.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1305476372/sys_fs.o.d" -o ${OBJECTDIR}/_ext/1305476372/sys_fs.o ../src/config/sam_e70_xult/system/fs/src/sys_fs.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1305476372/sys_fs_media_manager.o: ../src/config/sam_e70_xult/system/fs/src/sys_fs_media_manager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1305476372" 
	@${RM} ${OBJECTDIR}/_ext/1305476372/sys_fs_media_manager.o.d 
	@${RM} ${OBJECTDIR}/_ext/1305476372/sys_fs_media_manager.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1305476372/sys_fs_media_manager.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1305476372/sys_fs_media_manager.o.d" -o ${OBJECTDIR}/_ext/1305476372/sys_fs_media_manager.o ../src/config/sam_e70_xult/system/fs/src/sys_fs_media_manager.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/111743104/ff.o: ../src/config/sam_e70_xult/system/fs/fat_fs/src/ff.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/111743104" 
	@${RM} ${OBJECTDIR}/_ext/111743104/ff.o.d 
	@${RM} ${OBJECTDIR}/_ext/111743104/ff.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/111743104/ff.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/111743104/ff.o.d" -o ${OBJECTDIR}/_ext/111743104/ff.o ../src/config/sam_e70_xult/system/fs/fat_fs/src/ff.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/111743104/diskio.o: ../src/config/sam_e70_xult/system/fs/fat_fs/src/diskio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/111743104" 
	@${RM} ${OBJECTDIR}/_ext/111743104/diskio.o.d 
	@${RM} ${OBJECTDIR}/_ext/111743104/diskio.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/111743104/diskio.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/111743104/diskio.o.d" -o ${OBJECTDIR}/_ext/111743104/diskio.o ../src/config/sam_e70_xult/system/fs/fat_fs/src/diskio.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1521459453/mpfs.o: ../src/config/sam_e70_xult/system/fs/mpfs/src/mpfs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1521459453" 
	@${RM} ${OBJECTDIR}/_ext/1521459453/mpfs.o.d 
	@${RM} ${OBJECTDIR}/_ext/1521459453/mpfs.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1521459453/mpfs.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1521459453/mpfs.o.d" -o ${OBJECTDIR}/_ext/1521459453/mpfs.o ../src/config/sam_e70_xult/system/fs/mpfs/src/mpfs.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/327803284/sys_time.o: ../src/config/sam_e70_xult/system/time/src/sys_time.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/327803284" 
	@${RM} ${OBJECTDIR}/_ext/327803284/sys_time.o.d 
	@${RM} ${OBJECTDIR}/_ext/327803284/sys_time.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/327803284/sys_time.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/327803284/sys_time.o.d" -o ${OBJECTDIR}/_ext/327803284/sys_time.o ../src/config/sam_e70_xult/system/time/src/sys_time.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/865331892/plib_tc0.o: ../src/config/sam_e70_xult/peripheral/tc/plib_tc0.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/865331892" 
	@${RM} ${OBJECTDIR}/_ext/865331892/plib_tc0.o.d 
	@${RM} ${OBJECTDIR}/_ext/865331892/plib_tc0.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/865331892/plib_tc0.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/865331892/plib_tc0.o.d" -o ${OBJECTDIR}/_ext/865331892/plib_tc0.o ../src/config/sam_e70_xult/peripheral/tc/plib_tc0.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_announce.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_announce.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_announce.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_announce.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_announce.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_announce.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_announce.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_announce.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/arp.o: ../src/config/sam_e70_xult/library/tcpip/src/arp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/arp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/arp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/arp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/arp.o.d" -o ${OBJECTDIR}/_ext/902702131/arp.o ../src/config/sam_e70_xult/library/tcpip/src/arp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_commands.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_commands.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_commands.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_commands.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_commands.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_commands.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_commands.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_commands.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/dhcp.o: ../src/config/sam_e70_xult/library/tcpip/src/dhcp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/dhcp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/dhcp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/dhcp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/dhcp.o.d" -o ${OBJECTDIR}/_ext/902702131/dhcp.o ../src/config/sam_e70_xult/library/tcpip/src/dhcp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/dns.o: ../src/config/sam_e70_xult/library/tcpip/src/dns.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/dns.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/dns.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/dns.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/dns.o.d" -o ${OBJECTDIR}/_ext/902702131/dns.o ../src/config/sam_e70_xult/library/tcpip/src/dns.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/http.o: ../src/config/sam_e70_xult/library/tcpip/src/http.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/http.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/http.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/http.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/http.o.d" -o ${OBJECTDIR}/_ext/902702131/http.o ../src/config/sam_e70_xult/library/tcpip/src/http.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/917763083/http_print.o: ../src/config/sam_e70_xult/app/http_print.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/917763083" 
	@${RM} ${OBJECTDIR}/_ext/917763083/http_print.o.d 
	@${RM} ${OBJECTDIR}/_ext/917763083/http_print.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/917763083/http_print.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/917763083/http_print.o.d" -o ${OBJECTDIR}/_ext/917763083/http_print.o ../src/config/sam_e70_xult/app/http_print.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/917763083/custom_http_app.o: ../src/config/sam_e70_xult/app/custom_http_app.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/917763083" 
	@${RM} ${OBJECTDIR}/_ext/917763083/custom_http_app.o.d 
	@${RM} ${OBJECTDIR}/_ext/917763083/custom_http_app.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/917763083/custom_http_app.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/917763083/custom_http_app.o.d" -o ${OBJECTDIR}/_ext/917763083/custom_http_app.o ../src/config/sam_e70_xult/app/custom_http_app.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/ipv4.o: ../src/config/sam_e70_xult/library/tcpip/src/ipv4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/ipv4.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/ipv4.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/ipv4.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/ipv4.o.d" -o ${OBJECTDIR}/_ext/902702131/ipv4.o ../src/config/sam_e70_xult/library/tcpip/src/ipv4.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/ipv6.o: ../src/config/sam_e70_xult/library/tcpip/src/ipv6.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/ipv6.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/ipv6.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/ipv6.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/ipv6.o.d" -o ${OBJECTDIR}/_ext/902702131/ipv6.o ../src/config/sam_e70_xult/library/tcpip/src/ipv6.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/icmpv6.o: ../src/config/sam_e70_xult/library/tcpip/src/icmpv6.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/icmpv6.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/icmpv6.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/icmpv6.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/icmpv6.o.d" -o ${OBJECTDIR}/_ext/902702131/icmpv6.o ../src/config/sam_e70_xult/library/tcpip/src/icmpv6.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/icmp.o: ../src/config/sam_e70_xult/library/tcpip/src/icmp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/icmp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/icmp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/icmp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/icmp.o.d" -o ${OBJECTDIR}/_ext/902702131/icmp.o ../src/config/sam_e70_xult/library/tcpip/src/icmp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/nbns.o: ../src/config/sam_e70_xult/library/tcpip/src/nbns.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/nbns.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/nbns.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/nbns.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/nbns.o.d" -o ${OBJECTDIR}/_ext/902702131/nbns.o ../src/config/sam_e70_xult/library/tcpip/src/nbns.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/ndp.o: ../src/config/sam_e70_xult/library/tcpip/src/ndp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/ndp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/ndp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/ndp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/ndp.o.d" -o ${OBJECTDIR}/_ext/902702131/ndp.o ../src/config/sam_e70_xult/library/tcpip/src/ndp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/smtpc.o: ../src/config/sam_e70_xult/library/tcpip/src/smtpc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/smtpc.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/smtpc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/smtpc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/smtpc.o.d" -o ${OBJECTDIR}/_ext/902702131/smtpc.o ../src/config/sam_e70_xult/library/tcpip/src/smtpc.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/snmp.o: ../src/config/sam_e70_xult/library/tcpip/src/snmp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/snmp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/snmp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/snmp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/snmp.o.d" -o ${OBJECTDIR}/_ext/902702131/snmp.o ../src/config/sam_e70_xult/library/tcpip/src/snmp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1728594169/custom_snmp_app.o: ../src/config/sam_e70_xult/custom_snmp_app.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1728594169" 
	@${RM} ${OBJECTDIR}/_ext/1728594169/custom_snmp_app.o.d 
	@${RM} ${OBJECTDIR}/_ext/1728594169/custom_snmp_app.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1728594169/custom_snmp_app.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/1728594169/custom_snmp_app.o.d" -o ${OBJECTDIR}/_ext/1728594169/custom_snmp_app.o ../src/config/sam_e70_xult/custom_snmp_app.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/snmpv3.o: ../src/config/sam_e70_xult/library/tcpip/src/snmpv3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/snmpv3.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/snmpv3.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/snmpv3.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/snmpv3.o.d" -o ${OBJECTDIR}/_ext/902702131/snmpv3.o ../src/config/sam_e70_xult/library/tcpip/src/snmpv3.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/snmpv3_usm.o: ../src/config/sam_e70_xult/library/tcpip/src/snmpv3_usm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/snmpv3_usm.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/snmpv3_usm.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/snmpv3_usm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/snmpv3_usm.o.d" -o ${OBJECTDIR}/_ext/902702131/snmpv3_usm.o ../src/config/sam_e70_xult/library/tcpip/src/snmpv3_usm.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/sntp.o: ../src/config/sam_e70_xult/library/tcpip/src/sntp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/sntp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/sntp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/sntp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/sntp.o.d" -o ${OBJECTDIR}/_ext/902702131/sntp.o ../src/config/sam_e70_xult/library/tcpip/src/sntp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_alloc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_heap_alloc.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_alloc.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_internal.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_heap_internal.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_heap_internal.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/helpers.o: ../src/config/sam_e70_xult/library/tcpip/src/helpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/helpers.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/helpers.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/helpers.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/helpers.o.d" -o ${OBJECTDIR}/_ext/902702131/helpers.o ../src/config/sam_e70_xult/library/tcpip/src/helpers.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/hash_fnv.o: ../src/config/sam_e70_xult/library/tcpip/src/hash_fnv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/hash_fnv.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/hash_fnv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/hash_fnv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/hash_fnv.o.d" -o ${OBJECTDIR}/_ext/902702131/hash_fnv.o ../src/config/sam_e70_xult/library/tcpip/src/hash_fnv.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/oahash.o: ../src/config/sam_e70_xult/library/tcpip/src/oahash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/oahash.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/oahash.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/oahash.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/oahash.o.d" -o ${OBJECTDIR}/_ext/902702131/oahash.o ../src/config/sam_e70_xult/library/tcpip/src/oahash.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_helpers.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_helpers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_helpers.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_helpers.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_helpers.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_helpers.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_helpers.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_helpers.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_manager.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_manager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_manager.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_manager.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_manager.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_manager.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_manager.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_manager.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_notify.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_notify.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_notify.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_notify.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_notify.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_notify.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_notify.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_notify.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcpip_packet.o: ../src/config/sam_e70_xult/library/tcpip/src/tcpip_packet.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_packet.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcpip_packet.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcpip_packet.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcpip_packet.o.d" -o ${OBJECTDIR}/_ext/902702131/tcpip_packet.o ../src/config/sam_e70_xult/library/tcpip/src/tcpip_packet.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o: ../src/config/sam_e70_xult/system/sys_time_h2_adapter.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/157960187" 
	@${RM} ${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o.d 
	@${RM} ${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o.d" -o ${OBJECTDIR}/_ext/157960187/sys_time_h2_adapter.o ../src/config/sam_e70_xult/system/sys_time_h2_adapter.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/136402759/sys_fs_wrapper.o: ../src/config/sam_e70_xult/library/tcpip/src/common/sys_fs_wrapper.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/136402759" 
	@${RM} ${OBJECTDIR}/_ext/136402759/sys_fs_wrapper.o.d 
	@${RM} ${OBJECTDIR}/_ext/136402759/sys_fs_wrapper.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/136402759/sys_fs_wrapper.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/136402759/sys_fs_wrapper.o.d" -o ${OBJECTDIR}/_ext/136402759/sys_fs_wrapper.o ../src/config/sam_e70_xult/library/tcpip/src/common/sys_fs_wrapper.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/tcp.o: ../src/config/sam_e70_xult/library/tcpip/src/tcp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/tcp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/tcp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/tcp.o.d" -o ${OBJECTDIR}/_ext/902702131/tcp.o ../src/config/sam_e70_xult/library/tcpip/src/tcp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/902702131/udp.o: ../src/config/sam_e70_xult/library/tcpip/src/udp.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/902702131" 
	@${RM} ${OBJECTDIR}/_ext/902702131/udp.o.d 
	@${RM} ${OBJECTDIR}/_ext/902702131/udp.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/902702131/udp.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/902702131/udp.o.d" -o ${OBJECTDIR}/_ext/902702131/udp.o ../src/config/sam_e70_xult/library/tcpip/src/udp.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/707186936/plib_usart1.o: ../src/config/sam_e70_xult/peripheral/usart/plib_usart1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/707186936" 
	@${RM} ${OBJECTDIR}/_ext/707186936/plib_usart1.o.d 
	@${RM} ${OBJECTDIR}/_ext/707186936/plib_usart1.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/707186936/plib_usart1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS" -I"../src/config/sam_e70_xult/library" -I"../src/config/sam_e70_xult/library/tcpip/src" -I"../src/config/sam_e70_xult/library/tcpip/src/common" -I"../src" -I"../src/config/sam_e70_xult" -I"../src/packs/ATSAME70Q21B_DFP" -I"../src/packs/CMSIS/CMSIS/Core/Include" -I"../src/packs/CMSIS/" -Werror -Wall -MMD -MF "${OBJECTDIR}/_ext/707186936/plib_usart1.o.d" -o ${OBJECTDIR}/_ext/707186936/plib_usart1.o ../src/config/sam_e70_xult/peripheral/usart/plib_usart1.c    -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD) 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/sam_e70_xult.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../src/config/sam_e70_xult/library/tcpip/src/crypto/aes_pic32mc.a  
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -g   -mprocessor=$(MP_PROCESSOR_OPTION) -mno-device-startup-code -o dist/${CND_CONF}/${IMAGE_TYPE}/sam_e70_xult.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}    ..\src\config\sam_e70_xult\library\tcpip\src\crypto\aes_pic32mc.a      -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D=__DEBUG_D,--defsym=_min_heap_size=75000,--defsym=_min_stack_size=2048,--gc-sections,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/sam_e70_xult.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../src/config/sam_e70_xult/library/tcpip/src/crypto/aes_pic32mc.a 
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION) -mno-device-startup-code -o dist/${CND_CONF}/${IMAGE_TYPE}/sam_e70_xult.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}    ..\src\config\sam_e70_xult\library\tcpip\src\crypto\aes_pic32mc.a      -DXPRJ_sam_e70_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=75000,--defsym=_min_stack_size=2048,--gc-sections,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml
	${MP_CC_DIR}\\xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/sam_e70_xult.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
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
