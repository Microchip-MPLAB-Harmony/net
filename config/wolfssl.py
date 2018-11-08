def instantiateComponent(wolfsslComponent):
	wolfssl = wolfsslComponent.createBooleanSymbol("wolfssl", None)
	wolfssl.setLabel("Use wolfSSL?")
	wolfssl.setDescription("Enables wolfSSL")

	wolfssl.setVisible(True)
	wolfssl.setDefaultValue(False)
	
	wolfsslIsWolfsslEnabled = wolfsslComponent.createBooleanSymbol("wolfsslIsWolfsslEnabled",None)
	wolfsslIsWolfsslEnabled.setVisible(False)
	Database.setSymbolValue("lib_wolfssl","wolfsslIsWolfsslEnabled", False, 1)
	
	maskFamily = wolfsslComponent.createStringSymbol("maskFamily", None)
	maskFamily.setVisible(False)
	if "PIC32M" in Variables.get("__PROCESSOR"):
		maskFamily.setDefaultValue("PIC32M")
	else:
		maskFamily.setDefaultValue("SAME70")

	wolfsslMenu = wolfsslComponent.createMenuSymbol("wolfssl_menu", None)
	wolfsslMenu.setLabel("wolfSSL Settings")
 	wolfsslMenu.setDependencies(onWolfsslChangedDoLabel, ["wolfssl"])
	wolfsslMenu.setVisible(False)
	
	wolfsslMULTITHREAD = wolfsslComponent.createBooleanSymbol("wolfsslMULTITHREAD", wolfsslMenu)
	wolfsslMULTITHREAD.setLabel("Multi-threaded support")
	wolfsslMULTITHREAD.setDescription("Enables multi-threaded support")
	wolfsslMULTITHREAD.setDefaultValue(False)

	wolfsslFASTMATH = wolfsslComponent.createBooleanSymbol("wolfsslFASTMATH", wolfsslMenu)
	wolfsslFASTMATH.setLabel("Use Fast Math")
	wolfsslFASTMATH.setDescription("Enables Fast Math")
	wolfsslFASTMATH.setDefaultValue(True)
 
	wolfsslProtocolMenu = wolfsslComponent.createMenuSymbol("wolfssl_protocol_menu", None)
	wolfsslProtocolMenu.setLabel("Protocol and Key Exchange")
 	wolfsslProtocolMenu.setDependencies(onWolfsslChangedDoLabel, ["wolfssl"])
	wolfsslProtocolMenu.setVisible(False)

	wolfsslTLS = wolfsslComponent.createBooleanSymbol("wolfsslTLS", wolfsslProtocolMenu)
	wolfsslTLS.setLabel("TLS")
	wolfsslTLS.setDescription("Enables TLS")
	wolfsslTLS.setDefaultValue(True)

	wolfsslDH = wolfsslComponent.createBooleanSymbol("wolfsslDH", wolfsslProtocolMenu)
	wolfsslDH.setLabel("Diffie-Hellman")
	wolfsslDH.setDescription("Enables Diffie-Hellman")
	wolfsslDH.setDefaultValue(True)

	wolfsslPSK = wolfsslComponent.createBooleanSymbol("wolfsslPSK", wolfsslProtocolMenu)
	wolfsslPSK.setLabel("PSK")
	wolfsslPSK.setDescription("Enables PSK")
	wolfsslPSK.setDefaultValue(True)

	wolfsslPWDBASED = wolfsslComponent.createBooleanSymbol("wolfsslPWDBASED", wolfsslProtocolMenu)
	wolfsslPWDBASED.setLabel("PWDBASED")
	wolfsslPWDBASED.setDescription("Enables PWDBASED")
	wolfsslPWDBASED.setDefaultValue(True)

	wolfsslCertGen = wolfsslComponent.createBooleanSymbol("wolfsslCertGen", wolfsslProtocolMenu)
	wolfsslCertGen.setLabel("Certificate Generation")
	wolfsslCertGen.setDescription("Enables Certificate Generation")
	wolfsslCertGen.setDefaultValue(False)

	wolfsslDERLoad = wolfsslComponent.createBooleanSymbol("wolfsslDERLoad", wolfsslProtocolMenu)
	wolfsslDERLoad.setLabel("Allow loading if DER-formatted CA Certificates?")
	wolfsslDERLoad.setDescription("Allows loading if DER-formatted CA Certificates")
	wolfsslDERLoad.setDefaultValue(False)

	wolfsslDTLS = wolfsslComponent.createBooleanSymbol("wolfsslDTLS", wolfsslProtocolMenu)
	wolfsslDTLS.setLabel("DTLS")
	wolfsslDTLS.setDescription("Enables DTLS")
	wolfsslDTLS.setDefaultValue(False)

	wolfsslRSAKeyGen = wolfsslComponent.createBooleanSymbol("wolfsslRSAKeyGen", wolfsslProtocolMenu)
	wolfsslRSAKeyGen.setLabel("RSA Key Generation")
	wolfsslRSAKeyGen.setDescription("Enables RSA Key Generation")
	wolfsslRSAKeyGen.setDefaultValue(False)

	wolfsslCRL = wolfsslComponent.createBooleanSymbol("wolfsslCRL", wolfsslProtocolMenu)
	wolfsslCRL.setLabel("Certificate Revocation List")
	wolfsslCRL.setDescription("Enables Certificate Revocation List")
	wolfsslCRL.setDefaultValue(False)
		
	wolfsslECC = wolfsslComponent.createBooleanSymbol("wolfsslECC", wolfsslProtocolMenu)
	wolfsslECC.setLabel("Elliptical Curve Cryptography")
	wolfsslECC.setDescription("Enables ECC")
	wolfsslECC.setDefaultValue(False)
		
	wolfsslCompress = wolfsslComponent.createBooleanSymbol("wolfsslCompress", wolfsslProtocolMenu)
	wolfsslCompress.setLabel("Connection Compression")
	wolfsslCompress.setDescription("Enables Connection Compression")
	wolfsslCompress.setDefaultValue(False)
	
	wolfsslOCSP = wolfsslComponent.createBooleanSymbol("wolfsslOCSP", wolfsslProtocolMenu)
	wolfsslOCSP.setLabel("Online Certificate Status Protocol (OCSP)")
	wolfsslOCSP.setDescription("Enables OCSP")
	wolfsslOCSP.setDefaultValue(False)
	
	wolfsslTLS13 = wolfsslComponent.createBooleanSymbol("wolfsslTLS13", wolfsslProtocolMenu)
	wolfsslTLS13.setLabel("TLS 1.3")
	wolfsslTLS13.setDescription("Enables TLS 1.3")
	wolfsslTLS13.setDefaultValue(False)
    
	wolfsslCiphersMenu = wolfsslComponent.createMenuSymbol("wolfssl_ciphers_menu", None)
	wolfsslCiphersMenu.setLabel("Ciphers")
 	wolfsslCiphersMenu.setDependencies(onWolfsslChangedDoLabel, ["wolfssl"])
	wolfsslCiphersMenu.setVisible(False)

	wolfsslDES3 = wolfsslComponent.createBooleanSymbol("wolfsslDES3", wolfsslCiphersMenu)
	wolfsslDES3.setLabel("DES3")
	wolfsslDES3.setDescription("Enables DES3")
	wolfsslDES3.setDefaultValue(True)

	wolfsslAESCTR = wolfsslComponent.createBooleanSymbol("wolfsslAESCTR", wolfsslCiphersMenu)
	wolfsslAESCTR.setLabel("AES Counter")
	wolfsslAESCTR.setDescription("Enables AES Counter")
	wolfsslAESCTR.setDefaultValue(True)

	wolfsslAESCBC = wolfsslComponent.createBooleanSymbol("wolfsslAESCBC", wolfsslCiphersMenu)
	wolfsslAESCBC.setLabel("AES CBC")
	wolfsslAESCBC.setDescription("Enables AES CBC")
	wolfsslAESCBC.setDefaultValue(True)
	
	wolfsslAESCCM = wolfsslComponent.createBooleanSymbol("wolfsslAESCCM", wolfsslCiphersMenu)
	wolfsslAESCCM.setLabel("AES CCM")
	wolfsslAESCCM.setDescription("Enables AES CCM")
	wolfsslAESCCM.setDefaultValue(False)

	wolfsslAESGCM = wolfsslComponent.createBooleanSymbol("wolfsslAESGCM", wolfsslCiphersMenu)
	wolfsslAESGCM.setLabel("AES GCM")
	wolfsslAESGCM.setDescription("Enables AES GCM")
	wolfsslAESGCM.setDefaultValue(False)
	
	wolfsslAESNI = wolfsslComponent.createBooleanSymbol("wolfsslAESNI", wolfsslCiphersMenu)
	wolfsslAESNI.setLabel("AESNI")
	wolfsslAESNI.setDescription("Enables AESNI")
	wolfsslAESNI.setDefaultValue(False)

	wolfsslDSA = wolfsslComponent.createBooleanSymbol("wolfsslDSA", wolfsslCiphersMenu)
	wolfsslDSA.setLabel("DSA")
	wolfsslDSA.setDescription("Enables DSA")
	wolfsslDSA.setDefaultValue(True)
		
	wolfsslHMAC = wolfsslComponent.createBooleanSymbol("wolfsslHMAC", wolfsslCiphersMenu)
	wolfsslHMAC.setLabel("HMAC")
	wolfsslHMAC.setDescription("Enables HMAC")
	wolfsslHMAC.setDefaultValue(True)
	
	wolfsslRC4 = wolfsslComponent.createBooleanSymbol("wolfsslRC4", wolfsslCiphersMenu)
	wolfsslRC4.setLabel("RC4")
	wolfsslRC4.setDescription("Enables RC4")
	wolfsslRC4.setDefaultValue(True)
	
	wolfsslRABBIT = wolfsslComponent.createBooleanSymbol("wolfsslRABBIT", wolfsslCiphersMenu)
	wolfsslRABBIT.setLabel("RABBIT")
	wolfsslRABBIT.setDescription("Enables RABBIT")
	wolfsslRABBIT.setDefaultValue(True)

	wolfsslHC128 = wolfsslComponent.createBooleanSymbol("wolfsslHC128", wolfsslCiphersMenu)
	wolfsslHC128.setLabel("HC128")
	wolfsslHC128.setDescription("Enables HC128")
	wolfsslHC128.setDefaultValue(True)

	wolfsslCAMELLIA = wolfsslComponent.createBooleanSymbol("wolfsslCAMELLIA", wolfsslCiphersMenu)
	wolfsslCAMELLIA.setLabel("CAMELLIA")
	wolfsslCAMELLIA.setDescription("Enables CAMELLIA")
	wolfsslCAMELLIA.setDefaultValue(False)

	wolfsslARMASM = wolfsslComponent.createBooleanSymbol("wolfsslARMASM", wolfsslCiphersMenu)
	wolfsslARMASM.setLabel("ARM ASM")
	wolfsslARMASM.setDescription("Enables ARM ASM")
	wolfsslARMASM.setDefaultValue(False)

	wolfsslCMAC = wolfsslComponent.createBooleanSymbol("wolfsslCMAC", wolfsslCiphersMenu)
	wolfsslCMAC.setLabel("CMAC")
	wolfsslCMAC.setDescription("Enables CMAC")
	wolfsslCMAC.setDefaultValue(False)

	wolfsslRSA = wolfsslComponent.createBooleanSymbol("wolfsslRSA", wolfsslCiphersMenu)
	wolfsslRSA.setLabel("RSA")
	wolfsslRSA.setDescription("Enables RSA")
	wolfsslRSA.setDefaultValue(False)
 
	wolfsslHashesMenu = wolfsslComponent.createMenuSymbol("wolfssl_hashes_menu", None)
	wolfsslHashesMenu.setLabel("Hashes")
 	wolfsslHashesMenu.setDependencies(onWolfsslChangedDoLabel, ["wolfssl"])
	wolfsslHashesMenu.setVisible(False)

	wolfsslRNG = wolfsslComponent.createBooleanSymbol("wolfsslRNG", wolfsslHashesMenu)
	wolfsslRNG.setLabel("RNG")
	wolfsslRNG.setDescription("Enables RNG")
	wolfsslRNG.setDefaultValue(False)

	wolfsslMD4 = wolfsslComponent.createBooleanSymbol("wolfsslMD4", wolfsslHashesMenu)
	wolfsslMD4.setLabel("MD4")
	wolfsslMD4.setDescription("Enables MD4")
	wolfsslMD4.setDefaultValue(True)

	wolfsslMD5 = wolfsslComponent.createBooleanSymbol("wolfsslMD5", wolfsslHashesMenu)
	wolfsslMD5.setLabel("MD5")
	wolfsslMD5.setDescription("Enables MD5")
	wolfsslMD5.setDefaultValue(False)
		
	wolfsslRIPEMD = wolfsslComponent.createBooleanSymbol("wolfsslRIPEMD", wolfsslHashesMenu)
	wolfsslRIPEMD.setLabel("RIPEMD-160")
	wolfsslRIPEMD.setDescription("RIPEMD-160")
	wolfsslRIPEMD.setDefaultValue(False)
 	
	wolfsslSHA = wolfsslComponent.createBooleanSymbol("wolfsslSHA", wolfsslHashesMenu)
	wolfsslSHA.setLabel("SHA")
	wolfsslSHA.setDescription("Enables SHA")
	wolfsslSHA.setDefaultValue(False)

	wolfsslSHA3 = wolfsslComponent.createBooleanSymbol("wolfsslSHA3", wolfsslHashesMenu)
	wolfsslSHA3.setLabel("SHA 3")
	wolfsslSHA3.setDescription("Enables SHA 3")
	wolfsslSHA3.setDefaultValue(False)

	wolfsslSHA256 = wolfsslComponent.createBooleanSymbol("wolfsslSHA256", wolfsslHashesMenu)
	wolfsslSHA256.setLabel("SHA 256")
	wolfsslSHA256.setDescription("Enables SHA 256")
	wolfsslSHA256.setDefaultValue(True)

	wolfsslSHA384 = wolfsslComponent.createBooleanSymbol("wolfsslSHA384", wolfsslHashesMenu)
	wolfsslSHA384.setLabel("SHA 384")
	wolfsslSHA384.setDescription("Enables SHA 384")
	wolfsslSHA384.setDefaultValue(False)
		
	wolfsslSHA512 = wolfsslComponent.createBooleanSymbol("wolfsslSHA512", wolfsslHashesMenu)
	wolfsslSHA512.setLabel("SHA 512")
	wolfsslSHA512.setDescription("Enables SHA 512")
	wolfsslSHA512.setDefaultValue(False)
 
	wolfsslMemoryMenu = wolfsslComponent.createMenuSymbol("wolfssl_memory_menu", None)
	wolfsslMemoryMenu.setLabel("Memory Optimization")
 	wolfsslMemoryMenu.setDependencies(onWolfsslChangedDoLabel, ["wolfssl"])
	wolfsslMemoryMenu.setVisible(False)
	
	wolfsslCache = wolfsslComponent.createBooleanSymbol("wolfsslCache", wolfsslMemoryMenu)
	wolfsslCache.setLabel("Use Session Cache?")
	wolfsslCache.setDescription("Enables Session Cache")
	wolfsslCache.setDefaultValue(True)
		
	wolfsslSmallCache = wolfsslComponent.createBooleanSymbol("wolfsslSmallCache", wolfsslMemoryMenu)
	wolfsslSmallCache.setLabel("Use Small Session Cache?")
	wolfsslSmallCache.setDescription("Enables Small Session Cache")
	wolfsslSmallCache.setDefaultValue(False)
	
	wolfsslCsoMenu = wolfsslComponent.createMenuSymbol("wolfssl_cso_menu", None)
	wolfsslCsoMenu.setLabel("Code Size Optimizations")
 	wolfsslCsoMenu.setDependencies(onWolfsslChangedDoLabel, ["wolfssl"])
	wolfsslCsoMenu.setVisible(False)

	wolfsslErrStr = wolfsslComponent.createBooleanSymbol("wolfsslErrStr", wolfsslCsoMenu)
	wolfsslErrStr.setLabel("Use Error Strings?")
	wolfsslErrStr.setDescription("Enables Error Strings")
	wolfsslErrStr.setDefaultValue(True)
		
	wolfsslClient = wolfsslComponent.createBooleanSymbol("wolfsslClient", wolfsslCsoMenu)
	wolfsslClient.setLabel("Use wolfSSL Client?")
	wolfsslClient.setDescription("Enables wolfSSL Client")
	wolfsslClient.setDefaultValue(True)
		
	wolfsslServer = wolfsslComponent.createBooleanSymbol("wolfsslServer", wolfsslCsoMenu)
	wolfsslServer.setLabel("Use wolfSSL Server?")
	wolfsslServer.setDescription("Enables wolfSSL Server")
	wolfsslServer.setDefaultValue(True)
		
	wolfsslDebug = wolfsslComponent.createBooleanSymbol("wolfsslDebug", wolfsslCsoMenu)
	wolfsslDebug.setLabel("Debug Support?")
	wolfsslDebug.setDescription("Enables Debug Support")
	wolfsslDebug.setDefaultValue(False)
		
	wolfsslOpenSSL_API = wolfsslComponent.createBooleanSymbol("wolfsslOpenSSL_API", wolfsslCsoMenu)
	wolfsslOpenSSL_API.setLabel("OpenSSL API Support?")
	wolfsslOpenSSL_API.setDescription("Enables OpenSSL API Support")
	wolfsslOpenSSL_API.setDefaultValue(False)
		
	wolfsslOldTLS = wolfsslComponent.createBooleanSymbol("wolfsslOldTLS", wolfsslCsoMenu)
	wolfsslOldTLS.setLabel("Support older versions of TLS & SSL 3.0?")
	wolfsslOldTLS.setDescription("Enables support for older versions of TLS & SSL 3.0")
	wolfsslOldTLS.setDefaultValue(True)
	
	wolfsslMqttMenu = wolfsslComponent.createMenuSymbol("wolfssl_mqtt_menu", None)
	wolfsslMqttMenu.setLabel("wolfMQTT")
 	wolfsslMqttMenu.setDependencies(onWolfsslChangedDoLabel, ["wolfssl"])
	wolfsslMqttMenu.setVisible(False)

	wolfsslMQTT = wolfsslComponent.createBooleanSymbol("wolfsslMQTT", wolfsslMqttMenu)
	wolfsslMQTT.setLabel("Use wolfMQTT?")
	wolfsslMQTT.setDescription("Enables wolfMQTT")
	wolfsslMQTT.setDefaultValue(False)
		
	wolfsslMQTTTLS = wolfsslComponent.createBooleanSymbol("wolfsslMQTTTLS", wolfsslMqttMenu)
	wolfsslMQTTTLS.setLabel("Use TLS?")
	wolfsslMQTTTLS.setDescription("Enables wolfMQTT TLS")
	wolfsslMQTTTLS.setDefaultValue(False)
	
	# generate the config info and add it to list in configuration.h
	wolfsslConfigInfo = wolfsslComponent.createFileSymbol("wolfssl_config_info", None)
	wolfsslConfigInfo.setSourcePath("templates/wolfssl_config.h.ftl")
	wolfsslConfigInfo.setOutputName("core.LIST_SYSTEM_CONFIG_H_MIDDLEWARE_CONFIGURATION")
	wolfsslConfigInfo.setMarkup(True)
	wolfsslConfigInfo.setType("STRING")
	wolfsslConfigInfo.setEnabled(True)

	# add config.h to the project directory
	includeFilename = wolfsslComponent.createFileSymbol("include_filename", None)
	includeFilename.setProjectPath("config/" + Variables.get("__CONFIGURATION_NAME"))
	includeFilename.setSourcePath("config/config.h")
	includeFilename.setDestPath("")
	includeFilename.setOutputName("config.h")
	includeFilename.setType("HEADER")
	
	#add HAVE_CONFIG_H
	wolfsslConfigH = wolfsslComponent.createSettingSymbol("wolfsslConfigH", None)
	wolfsslConfigH.setCategory("C32")
	wolfsslConfigH.setKey("preprocessor-macros")
	wolfsslConfigH.setValue("HAVE_CONFIG_H")
	
	#add include directory 
	wolfsslSrcPath = wolfsslComponent.createSettingSymbol("wolfsslSrcPath", None)
	wolfsslSrcPath.setValue("../src/config/" + Variables.get("__CONFIGURATION_NAME") + "/wolfssl")
	wolfsslSrcPath.setCategory("C32")
	wolfsslSrcPath.setKey("extra-include-directories")
	wolfsslSrcPath.setAppend(True, ";")
	
	#turn off warnings as errors 
	wolfsslWarnings = wolfsslComponent.createSettingSymbol("wolfsslWarnings", None)
	wolfsslWarnings.setValue("false")
	wolfsslWarnings.setCategory("C32")
	wolfsslWarnings.setKey("make-warnings-into-errors")
	
	#turn off additional warnings
	wolfsslAddtlWarnings = wolfsslComponent.createSettingSymbol("wolfsslAddtlWarnings", None)
	wolfsslAddtlWarnings.setValue("false")
	wolfsslAddtlWarnings.setCategory("C32")
	wolfsslAddtlWarnings.setKey("additional-warnings")	
	

	# these files are in wolfssl/src/
	fileList_common = [
		'bio.c', 'crl.c', 'internal.c', 'keys.c', 'ocsp.c', 'sniffer.c',
		'ssl.c', 'tls.c', 'tls13.c', 'wolfio.c', 
	]	
 
	# these files are in wolfssl/wolfcrypt/src/
	fileList_common_wolfcrypt = [
		'hash.c','cpuid.c','wolfevent.c','asm.c',
		'logging.c', 'wc_encrypt.c', 'wc_port.c', 'error.c', 
		'signature.c', 'wolfmath.c','memory.c','asn.c','coding.c',
		'poly1305.c','arc4.c','md4.c','md5.c','pwdbased.c','pkcs12.c',
		'dsa.c','md2.c','ripemd.c','blake2b.c','hc128.c','rabbit.c',
		'chacha.c','chacha20_poly1305.c','misc.c','tfm.c','integer.c',
		'curve25519.c','ed25519.c','compress.c','pkcs7.c','srp.c',
		'idea.c','camellia.c',
#       'hmac.c','random.c','sha256.c','rsa.c','ecc.c',
#		'aes.c','aes_asm.S','des3.c','sha.c','sha512.c','sha3.c','dh.c',
#		'cmac.c'		
    ]
	
	# these files are in wolfssl/wolfssl/openssl/
	fileList_openssl = [	 
		'evp.h','md4.h','md5.h','sha.h','ripemd.h','rsa.h','dsa.h','ec.h',
		'bn.h' 
	]
	
	# these header files are in wolfssl/wolfssl/
	fileList_wolfssl_headers = [
		'callbacks.h','certs_test.h','crl.h','error-ssl.h','internal.h','ocsp.h',
		'sniffer.h','sniffer_error.h','ssl.h','test.h','version.h','wolfio.h'
	]
	
	# these header files are in wolfssl/wolfssl/wolfcrypt/
	# took this list from C:\microchip\harmony\h3\wolfssl\ctaocrypt\include.am
	fileList_wolfcrypt_headers = [
		'aes.h','arc4.h','asn.h','asn_public.h','camellia.h','coding.h',
		'compress.h','cpuid.h','des3.h','dh.h','dsa.h','ecc.h','error-crypt.h','hash.h',
		'hc128.h','hmac.h','integer.h','md2.h','md4.h','md5.h','misc.h','pkcs7.h',
		'pwdbased.h','rabbit.h','random.h','ripemd.h','rsa.h',
		'settings.h','sha256.h','sha512.h','sha.h','blake2.h','blake2-int.h',
		'blake2-impl.h','tfm.h','types.h','visibility.h','logging.h',
		'memory.h','mpi_class.h','mpi_superclass.h','wc_port.h','settings.h',
		'wolfmath.h','pkcs12.h','wc_encrypt.h','chacha.h','signature.h'
	]
	
	# these header files are in wolfssl/ctaocrypt/src/
	fileList_ctaocrypt_src = [
		'wolfcrypt_first.c','hmac.c','random.c','sha256.c','rsa.c','aes.c','des3.c',
		'sha512.c','wolfcrypt_last.c'
		#'sha.3','fips.c','fips_test.c',
	]
	
	# these header files are in wolfssl/cyassl/ctaocrypt/
	fileList_cyassl_ctaocrypt = [
		'settings.h','visibility.h','settings_comp.h'
	]
	if wolfssl.getValue() == True:
	# add all common files as enabled 
		for filename in fileList_common:
			addFileName(filename, wolfsslComponent, "../third_party/tcpip/wolfssl/src/", "wolfssl/src/", True, onWolfsslChangedAdd)
		for filename in fileList_common_wolfcrypt:
			addFileName(filename, wolfsslComponent, "../third_party/tcpip/wolfssl/wolfcrypt/src/", "wolfssl/wolfcrypt/src/", True, onWolfsslChangedAdd)
		for filename in fileList_wolfssl_headers:
			addFileName(filename, wolfsslComponent, "../third_party/tcpip/wolfssl/wolfssl/", "wolfssl/wolfssl/", True, onWolfsslChangedAdd)
		for filename in fileList_wolfcrypt_headers:
			addFileName(filename, wolfsslComponent, "../third_party/tcpip/wolfssl/wolfssl/wolfcrypt/", "wolfssl/wolfssl/wolfcrypt/", True, onWolfsslChangedAdd)
		for filename in fileList_openssl:
			addFileName(filename, wolfsslComponent, "../third_party/tcpip/wolfssl/wolfssl/openssl/", "wolfssl/wolfssl/openssl/", True, onRSAKeyGenChangedAdd)
		for filename in fileList_ctaocrypt_src:
			addFileName(filename, wolfsslComponent, "../third_party/tcpip/wolfssl/ctaocrypt/src/", "wolfssl/ctaocrypt/src/", True, onWolfsslChangedAdd)
		for filename in fileList_cyassl_ctaocrypt:
			addFileName(filename, wolfsslComponent, "../third_party/tcpip/wolfssl/cyassl/ctaocrypt/", "wolfssl/cyassl/ctaocrypt/", True, onWolfsslChangedAdd)
	
def destroyComponent(wolfsslComponent):
	args = {"wolfsslEnabled":False}
	Database.sendMessage("lib_crypto", "isWolfsslEnabled", args)
	
# message handler
def handleMessage(messageID, args):
	val = Database.getSymbolValue("lib_wolfssl","wolfsslIsWolfsslEnabled")
	if val is True:
		result = {"wolfsslEnabled":True}
	else:
		result = {"wolfsslEnabled":False}
	return result
	
# callback for when wolfSSL is toggled
def onWolfsslChangedDoLabel(symbol, isWolfssl):
	if isWolfssl['value'] == False:
		symbol.setVisible(False)
		Database.setSymbolValue("lib_wolfssl","wolfsslIsWolfsslEnabled", False, 1)
		args = {"wolfsslEnabled":False}
		Database.sendMessage("lib_crypto", "isWolfsslEnabled", args)
	else:
		symbol.setVisible(True)
		Database.setSymbolValue("lib_wolfssl","wolfsslIsWolfsslEnabled", True, 1)
		args = {"wolfsslEnabled":True}
		Database.sendMessage("lib_crypto", "isWolfsslEnabled", args)
		
# This callback enables files to be written depending on wolfSSL Enabled
def onWolfsslChangedAdd(filesymbol, isWolfsslEnabled):
	if isWolfsslEnabled['value'] == True:
		filesymbol.setEnabled(True)
	else:
		filesymbol.setEnabled(False)
		
# This callback enables files to be written on RSAKeyGen Enabled
def onRSAKeyGenChangedAdd(filesymbol, isRSAKeyGen):
	if isRSAKeyGen['value'] == True:
		filesymbol.setEnabled(True)
	else:
		filesymbol.setEnabled(False)

# This callback disables files to be written on RSAKeyGen disabled
def onRSAKeyGenChangedSub(filesymbol, isRSAKeyGen):
	if isRSAKeyGen['value'] == True:
		filesymbol.setEnabled(False)
	else:
		filesymbol.setEnabled(True)

# all files go into or under src/
def addFileName(fileName, component, srcPath, destPath, enabled, callback):
	filename = component.createFileSymbol(None, None)
	filename.setProjectPath("config/" + Variables.get("__CONFIGURATION_NAME") + "/wolfssl")
	filename.setSourcePath(srcPath + fileName)
	filename.setOutputName(fileName)

	if fileName[-2:] == '.h':
		filename.setDestPath(destPath)
		filename.setType("HEADER")
	else:
		filename.setDestPath(destPath)
		filename.setType("SOURCE")

	filename.setEnabled(enabled)
	if callback != None:
		filename.setDependencies(callback, ["wolfssl"])


