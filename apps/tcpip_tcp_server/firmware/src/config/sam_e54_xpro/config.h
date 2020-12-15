#ifndef _WOLFCRYPT_REQUIRED_CONFIG_H_
#define _WOLFCRYPT_REQUIRED_CONFIG_H_

#include "configuration.h"

#include <stddef.h>
extern void* OSAL_Malloc(size_t size);
extern void OSAL_Free(void* pData);

#endif