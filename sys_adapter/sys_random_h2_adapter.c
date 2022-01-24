/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#include "configuration.h"
#include "sys_random_h2_adapter.h"

#include "crypto/crypto.h"
#include "osal/osal.h"


static CRYPT_RNG_CTX sysRandCtx;
static CRYPT_RNG_CTX* pRandCtx = 0;

static OSAL_SEM_HANDLE_TYPE randSemaphore;
static int randLockCount = 0;       // initialization lock count: 0 - uninitialized; 1 - initialized; 2 - within initialization



// a SYS_RANDOM_Crypto call can perform initialization, if needed (since there is no explicit call to SYS_RANDOM_CryptoInitialize)
// Currently there is no protection against multiple threads performing initialization/de-initialization
// only against multiple threads calling directly a SYS_RANDOM_Crypto service wich can perform initialization, if needed
// stop all calls before calling SYS_RANDOM_CryptoDeinitialize()!
static bool _InitSysRand(void)
{
    bool initRes = false;
    bool doInit = false;
    bool abortInit = false;

    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
    if(randLockCount == 0)
    {   // OK, performing initialization
        randLockCount = 2;  // lock
        doInit = true;
    }
    else if (randLockCount == 1)
    {   // already initialized
        initRes = true;
    }
    else
    {   // fail: another init in place...
        abortInit = true;
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);

    if(abortInit)
    {   // another init in place...
        return false;
    }

    if(doInit)
    {
        pRandCtx = 0;
        initRes = false;

        while(true)
        {   // initialize
            if(OSAL_SEM_Create(&randSemaphore, OSAL_SEM_TYPE_BINARY, 1, 1) != OSAL_RESULT_TRUE)
            {   // failed
                break;
            }

            if(CRYPT_RNG_Initialize(&sysRandCtx) >= 0)
            {   // success
                pRandCtx = &sysRandCtx;
                initRes = true;
                break;
            }

            // failure
            OSAL_SEM_Delete(&randSemaphore);
            break;
        }
    }

    if(initRes == true)
    {   // went well
        randLockCount = 1;  // release the lock
        return true;
    }

    // failed
    randLockCount = 0;  // leave it uninitialized
    return false;
}


SYS_MODULE_OBJ SYS_RANDOM_CryptoInitialize(void)
{
    _InitSysRand();

    return (SYS_MODULE_OBJ)pRandCtx;    
}

void SYS_RANDOM_CryptoDeinitialize( SYS_MODULE_OBJ object )
{
    bool doDeinit = false;
    bool abortDeinit = false;

    OSAL_CRITSECT_DATA_TYPE status = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    while(true)
    {
        if((CRYPT_RNG_CTX*)object != pRandCtx)
        {   // not us?
            abortDeinit = true;
            break;
        }

        if(randLockCount == 0)
        {   // not initialized, nothing to do
        }
        else if (randLockCount == 1)
        {   // initialized, kill it
            randLockCount = 2;
            doDeinit = true;
        }
        else
        {   // fail: another init/deinit in place...
            abortDeinit = true;
        }

        break;
    }


    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, status);

    if(abortDeinit)
    {   // not us or somebody else messing with this
        return;
    }

    if(doDeinit)
    {
        // CRYPT_RNG_Deinitialize(pRandCtx);
        OSAL_SEM_Delete(&randSemaphore);
        pRandCtx = 0;
        randLockCount = 0;
    } 
}

static CRYPT_RNG_CTX* _SYS_RANDOM_CryptoLock(void)
{
    if(pRandCtx == 0)
    {
        _InitSysRand();
    }

    if(pRandCtx != 0)
    {
         (void)OSAL_SEM_Pend(&randSemaphore, OSAL_WAIT_FOREVER);
    }
    return pRandCtx;    
} 

static __inline__ void  __attribute__((always_inline)) _SYS_RANDOM_CryptoUnlock(void)
{
    (void)OSAL_SEM_Post(&randSemaphore);
}

uint32_t SYS_RANDOM_CryptoGet( void )
{
    union
    {
        uint32_t    u32;
        uint8_t     u8[4];
    }sUint;

    sUint.u32 = 0;
    CRYPT_RNG_CTX* pCtx = _SYS_RANDOM_CryptoLock();

    if(pCtx)
    {
        CRYPT_RNG_BlockGenerate(pCtx, (unsigned char*)sUint.u8, sizeof(sUint.u8));
    }

    _SYS_RANDOM_CryptoUnlock();

    return sUint.u32;
}



size_t SYS_RANDOM_CryptoBlockGet( void *buffer, size_t size )
{
    size_t blkSize = 0;

    if(buffer != 0 && size != 0)
    {
        CRYPT_RNG_CTX* pCtx = _SYS_RANDOM_CryptoLock();
        if(pCtx)
        {
            CRYPT_RNG_BlockGenerate(pCtx, (uint8_t*)buffer, size);
            blkSize = size;
        }
        _SYS_RANDOM_CryptoUnlock();
    }

    return blkSize;
}

uint8_t SYS_RANDOM_CryptoByteGet( void )
{
    uint8_t rNo = 0;

    CRYPT_RNG_CTX* pCtx = _SYS_RANDOM_CryptoLock();
    if(pCtx)
    {
        CRYPT_RNG_Get(pCtx, &rNo);
    }
    _SYS_RANDOM_CryptoUnlock();

    return rNo;
}


