/*******************************************************************************
  Hash table implementation file

  Summary:
    Hash Table manipulation routines
    
  Description:
    This source file contains the functions and storage of the 
    hash table manipulation routines
*******************************************************************************/

/*
Copyright (C) 2012-2025, Microchip Technology Inc., and its subsidiaries. All rights reserved.

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
*/









#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "device.h"
#include "tcpip/src/oahash.h"


// local prototypes
// 

// conversion helpers
//
static __inline__ ptrdiff_t __attribute__((always_inline)) FC_HePtrDiff(OA_HASH_ENTRY* hE1, OA_HASH_ENTRY* hE2)
{
    return hE1 - hE2;
}

static __inline__ ptrdiff_t __attribute__((always_inline)) FC_CU8PtrDiff(uint8_t* ptr1, uint8_t* ptr2)
{
    return ptr1 - ptr2;
}

static __inline__ uint32_t __attribute__((always_inline)) FC_HEntryDiff2UI32(OA_HASH_ENTRY* hPtr, OA_HASH_ENTRY* startPtr)
{
    union
    {
        OA_HASH_ENTRY*  pH;
        uint8_t*        u8Ptr;
    }U_HE_U8_PTR1, U_HE_U8_PTR2;

    union
    {
        ptrdiff_t   pDiff;
        uint32_t    uv32;
    }U_PTR_DIFF_UI32;

    U_HE_U8_PTR1.pH = hPtr;
    U_HE_U8_PTR2.pH = startPtr;

    U_PTR_DIFF_UI32.pDiff = FC_CU8PtrDiff(U_HE_U8_PTR1.u8Ptr, U_HE_U8_PTR2.u8Ptr);
    return U_PTR_DIFF_UI32.uv32;
}

static __inline__ OA_HASH_ENTRY* __attribute__((always_inline)) FC_HEntryInc(OA_HASH_ENTRY* hE, size_t sizeInc)
{
    union
    {
        OA_HASH_ENTRY*  hE;
        uint8_t*        u8Ptr;
    }U_HE_U8_PTR;

    U_HE_U8_PTR.hE = hE;
    U_HE_U8_PTR.u8Ptr += sizeInc;

    return U_HE_U8_PTR.hE;
}

static __inline__ OA_HASH_ENTRY* __attribute__((always_inline)) FC_Vptr2HEntryInc(void* vPtr, size_t sizeInc)
{
    union
    {
        void*           vPtr;
        uint8_t*        u8Ptr;
        OA_HASH_ENTRY*  hE;
    }U_V_PTR_U8_PTR_HE;

    U_V_PTR_U8_PTR_HE.vPtr = vPtr;
    U_V_PTR_U8_PTR_HE.u8Ptr += sizeInc;

    return U_V_PTR_U8_PTR_HE.hE;
}


static OA_HASH_ENTRY*   F_OAHashFindBkt(OA_HASH_DCPT* pOH, const void* key);

static __inline__ void __attribute__((always_inline)) F_OAHashRemoveEntry(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* pOE)
{
    if(pOE->flags.busy != 0U)
    {
        pOE->flags.busy = 0U;
        pOH->fullSlots--;
    }
}


static  size_t  F_OAHashProbeStep(OA_HASH_DCPT* pOH, const void* key)
{
    size_t probeStep;
    
#if defined(OA_DOUBLE_HASH_PROBING)
#if defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )
    probeStep = (*pOH->probeHash)(pOH, key);
#else
    probeStep = TCPIP_OAHASH_HashProbe(pOH, key);
#endif  // defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )
    if(probeStep == 0)
    {   // try to avoid probing the same bucket over and over again
        // when probeHash returns 0.
        probeStep = pOH->probeStep;
    }
#else
    probeStep = pOH->probeStep;
#endif  // defined(OA_DOUBLE_HASH_PROBING)

    return probeStep;
}


// implementation

// Initializes a OA hash table
void TCPIP_OAHASH_Initialize(OA_HASH_DCPT* pOH)
{
    OA_HASH_ENTRY*  pHE;    
    size_t  ix;

    pOH->fullSlots = 0; 
    
    pHE = FC_Vptr2HEntryInc(pOH->memBlk, 0U);
    for(ix = 0; ix < pOH->hEntries; ix++)
    {
        pHE->flags.value = 0;
        pHE = FC_HEntryInc(pHE, pOH->hEntrySize);
    }
}


// performs look up only
// if no such entry found, it returns NULL
OA_HASH_ENTRY* TCPIP_OAHASH_EntryLookup(OA_HASH_DCPT* pOH, const void* key)
{
    OA_HASH_ENTRY*  pBkt;
    size_t      bkts = 0;
    size_t      bktIx;
    size_t      probeStep;
   
    probeStep = F_OAHashProbeStep(pOH, key);
#if defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )
    bktIx = (*pOH->hashF)(pOH, key);
#else
    bktIx = TCPIP_OAHASH_KeyHash(pOH, key);
#endif  // defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )

    while(bkts < pOH->hEntries)
    {
        pBkt = FC_Vptr2HEntryInc(pOH->memBlk, bktIx * pOH->hEntrySize);
#if defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )
        if(pBkt->flags.busy != 0U && (*pOH->cmpF)(pOH, pBkt, key) == 0)
        {   // found entry
            pBkt->flags.newEntry = 0U;
            return pBkt;
        }
#else
        if(pBkt->flags.busy != 0U && TCPIP_OAHASH_KeyCompare(pOH, pBkt, key) == 0)
        {   // found entry
            pBkt->flags.newEntry = 0U;
            return pBkt;
        }
#endif  // defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )

        // advance to the next hash slot
        bktIx += probeStep;
        if(bktIx >= pOH->hEntries)
        {
            bktIx -= pOH->hEntries;
        }

        bkts++;
    }
    
    return NULL;   // not found
}

// Performs look up and insert
// if key is found, it returns the pointer to the existing entry
// if key was not found but there's an empty slot it will insert
// the key in this slot and newEntry flag will be set
// If there's no room in the hash it will call the delF to empty
// a slot and place the key there 
// If the delF is NULL it will return NULL.
// NULL is also returned when the probeStep is not properly chosen and
// the hash cannot be properly traversed in one pass
// (detect the situation by probing that the cache was not full:
// fullSlots < hElements)
OA_HASH_ENTRY*   TCPIP_OAHASH_EntryLookupOrInsert(OA_HASH_DCPT* pOH, const void* key)
{
    OA_HASH_ENTRY   *pBkt, *pDel;

    pBkt = F_OAHashFindBkt(pOH, key);
    if(pBkt == NULL)
    {
        if(pOH->fullSlots != pOH->hEntries)
        {   // wrong probeStep!
            return NULL;
        }
        
        // else cache is full
        // discard an old entry and retry
#if defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )
        if(pOH->delF == NULL)
        {   // nothing else we can do
            return NULL;
        }
        else if((pDel = (*pOH->delF)(pOH)) == NULL)
        {   // nothing else we can do
            return NULL;
        }
        else
        {
            // OK
        }
#else
        if((pDel = TCPIP_OAHASH_EntryDelete(pOH)) == NULL)
        {   // nothing else we can do
            return NULL;
        }
#endif  // defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )

        F_OAHashRemoveEntry(pOH, pDel);
        pBkt = F_OAHashFindBkt(pOH, key);
        if(pBkt == NULL)
        {   // probeStep failure, again
            return NULL;
        }
    }

    // we found an entry
    if(pBkt->flags.busy == 0U)
    {
        pBkt->flags.busy = 1U;
        pBkt->flags.newEntry = 1U;
    }
    else
    {   // old entry
        pBkt->flags.newEntry = 0U;
    }
    
    return pBkt;
}


// Function to delete an entry from the hash
// SHOULD be called when entries have to be deleted!
// because the hash also has to do some internal clean-up.
// Note: when an entry is deleted by TCPIP_OAHASH_EntryLookupOrInsert (calling pOH->delF)
// the hash state is maintained internally, no need to call TCPIP_OAHASH_EntryRemove();
void TCPIP_OAHASH_EntryRemove(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* pOE)
{
    F_OAHashRemoveEntry(pOH, pOE);
}

// Function to remove all entries from the hash
// SHOULD be called when entries have to be deleted!
// because the hash also has to do some internal clean-up.
void TCPIP_OAHASH_EntriesRemoveAll(OA_HASH_DCPT* pOH)
{
    OA_HASH_ENTRY*  pBkt;
    size_t      bktIx;
    
    pBkt = FC_Vptr2HEntryInc(pOH->memBlk, 0U);
    for(bktIx = 0; bktIx < pOH->hEntries; bktIx++)
    {
        if(pBkt->flags.busy != 0U)
        {   // found entry
            F_OAHashRemoveEntry(pOH, pBkt);
        }

        pBkt = FC_HEntryInc(pBkt, pOH->hEntrySize);
    }
}

OA_HASH_ENTRY* TCPIP_OAHASH_EntryGet(OA_HASH_DCPT* pOH, size_t entryIx)
{
    if(entryIx < pOH->hEntries)
    {
        return FC_Vptr2HEntryInc(pOH->memBlk, entryIx * pOH->hEntrySize);
    }

    return NULL;
}

int32_t TCPIP_OAHASH_EntryGetIndex(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* pHe)
{
    if(pOH != NULL && pHe != NULL)
    {
        OA_HASH_ENTRY   *pStartBkt, *pEndBkt, *pBkt;

        pStartBkt = FC_Vptr2HEntryInc(pOH->memBlk, 0U);
        pEndBkt = FC_Vptr2HEntryInc(pOH->memBlk, pOH->hEntries * pOH->hEntrySize);

        // check that (pStartBkt <= pHe && pHe < pEndBkt)
        ptrdiff_t startDiff = FC_HePtrDiff(pHe, pStartBkt); // pHe - pStartBkt
        ptrdiff_t endDiff = FC_HePtrDiff(pEndBkt, pHe); // pEndBkt - pHe 
        if(startDiff >= 0 && endDiff > 0)
        {
            size_t entryIx = FC_HEntryDiff2UI32(pHe, pStartBkt) / pOH->hEntrySize;
            pBkt = FC_HEntryInc(pStartBkt, entryIx * pOH->hEntrySize);
            if(pBkt == pHe)
            {
                return (int32_t)entryIx;
            }
        } 
    }

    return -1;
}

// implementation

// finds a entry that either contains the desired key
// or is empty and can be used to insert the key 
static OA_HASH_ENTRY* F_OAHashFindBkt(OA_HASH_DCPT* pOH, const void* key)
{
    OA_HASH_ENTRY*  pBkt;
    size_t      bktIx;
    size_t      probeStep;
    size_t      bkts = 0U;

    probeStep = F_OAHashProbeStep(pOH, key);
#if defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )
    bktIx = (*pOH->hashF)(pOH, key);
#else
    bktIx = TCPIP_OAHASH_KeyHash(pOH, key);
#endif  // defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )

    while(bkts < pOH->hEntries)
    {
        pBkt = FC_Vptr2HEntryInc(pOH->memBlk, bktIx * pOH->hEntrySize);
        if(pBkt->flags.busy == 0U)
        {   // found unused entry
#if defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )
            (*pOH->cpyF)(pOH, pBkt, key);   // set the key
#else
            TCPIP_OAHASH_KeyCopy(pOH, pBkt, key);   // set the key
#endif  // defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )
            pBkt->probeCount = (uint16_t)bkts;
            pOH->fullSlots++;
            return pBkt;
        }

#if defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )
        if((*pOH->cmpF)(pOH, pBkt, key) == 0)
        {   // found entry
            return pBkt;
        }
#else
        if(TCPIP_OAHASH_KeyCompare(pOH, pBkt, key) == 0)
        {   // found entry
            return pBkt;
        }
#endif  // defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )

        // advance to the next hash slot
        bktIx += probeStep;
        if(bktIx >= pOH->hEntries)
        {
            bktIx -= pOH->hEntries;
        }

        bkts++;
    }
    
    return NULL;   // cache full, not found
}



