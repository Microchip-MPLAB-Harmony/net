/*******************************************************************************
  Hash table helper file

  Company:
    Microchip Technology Inc.
    
  File Name:
    oahash.h

  Summary:
    Hash Table manipulation Interface Header
    
  Description:
    This header file contains the function prototypes and definitions of the 
    hash table manipulation routines
*******************************************************************************/
// DOM-IGNORE-BEGIN
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








// DOM-IGNORE-END

#ifndef H_OA_HASH_H_
#define H_OA_HASH_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


// Define this symbol to use a double hash probing scheme
// otherwise the hash will use a linear probing approach
//#define OA_DOUBLE_HASH_PROBING


// Define this symbol to use dynamic key manipulation functions
// (i.e. an hash object needs to be instantiated, and the key manipulation
// functions belong to the hash object).
// This is useful when multiple hashes, having different key types,
// have to be maintained (at the expense of some run time overhead).  
// 
// Otherwise the key manipulation functions are static, they are global
// and do not belong to a particular instance of a hash object
// This is useful when all hashes use exactly the same type of key and
// the overhead is minimum.
// 
// Either way, the key manipulation functions have to be provided by the hash user
// For the TCP/IP stack that needs to be defined because multiple modules use the OA_HASH
// with different key types (ARP, DHCPs, DNS, HTTP, etc.)
#define OA_HASH_DYNAMIC_KEY_MANIPULATION


// forward references
typedef struct S_TAG_OA_HASH_DCPT    OA_HASH_DCPT;
typedef struct S_TAG_OA_HASH_ENTRY   OA_HASH_ENTRY;

#if defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )

// dynamic key manipulation routines

// function that hashes a key and returns a hash value (index)
// this function has to return an index between (-pOH->hEntries, pOH->hEntries)
typedef size_t (*OA_HASH_F)(OA_HASH_DCPT* pOH, const void* key);

// function to return an entry that can be deleted when the hash is full
typedef OA_HASH_ENTRY*   (*OA_HASH_DEL_ENTRY_F)(OA_HASH_DCPT* pOH);


// function that performs comparison between a key and an entry
// returns 0 if keys match 
typedef int (*OA_HASH_KEY_COMPARE_F)(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key);

// function that copies a key to a destination entry
typedef void (*OA_HASH_KEY_COPY_F)(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* srcKey);

#endif  // defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )



// Descriptor of an Open Addressing Hash
// This implementation uses either
// a Linear Probing or a Double Hashing approach
// Like any other hash table, a OA hash has a number of slots/buckets
// each containing a hash entry.
// An OA hash table works with a main/regular hashing function
// If the entry pointed by this hash function
// is taken, then a new slot is searched incrementally
// using either linear probing or a second hash function
// This approach is preferred for fixed sizes hashes,
// when no memory allocation is needed/desired.
// The number of entries is fixed and all the slots are
// gradually filled.
// However, like with all Open Addressing hashes,
// the performance degrades when the load factor gets >= 0.7
// Use the probeCount number to check the hash
// performance and choose the number of entries carefully!
// For best performance the number of elements should be prime
// and a good hash function should be used.
//
// Note that the actual size of each hash entry is
// pOH->hEntrySize which has to include the sizeof(OA_HASH_ENTRY)
struct S_TAG_OA_HASH_DCPT 
{
    void*                   memBlk;     // area to create the hash table in
    const void*             hParam;     // caller parameter; could be used for dispatch
                                        // ignored by the hash
    size_t                  hEntrySize; // size of each hash entry
                                        // INCLUDING the OA_HASH_ENTRY fields!
    size_t                  hEntries;  // number of elements/slots that the
                                        // memBlk can hold
    size_t                  probeStep;  // linear probing step
                                        // both probeStep and probeHash
                                        // have to return a value in (-hEntries, hEntries)
                                        // They have to be prime with hEntries!
                                        // Note: that means that when OA_DOUBLE_HASH_PROBING
                                        // is defined, hEntries has to be prime!!!
                                        // Note: probeStep is used even when OA_DOUBLE_HASH_PROBING is defined
                                        // if probeHash returns 0
#if defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )
    OA_HASH_F               hashF;      // hashing function returning a value
                                        // (-hEntries, hEntries)
#if defined(OA_DOUBLE_HASH_PROBING)
    OA_HASH_F               probeHash;  // hash probing function
                                        // returns a probe step
#endif  // defined(OA_DOUBLE_HASH_PROBING)
    OA_HASH_DEL_ENTRY_F     delF;       // delete entry function
                                        // if NULL, no entry will be removed
    OA_HASH_KEY_COMPARE_F   cmpF;       // compare two keys function
    
    OA_HASH_KEY_COPY_F      cpyF;       // copy key function
#endif  // defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )

    // fields updated by the TCPIP_OAHASH_Initialize()
    // and maintained by the hash itself  
    size_t                  fullSlots;  // number of elements/slots having valid data                         
};


// Definition of a OA hash entry
// Note that the key is not described here!
// To maintain generality, it is up to 
// the user to define the key, including length, offset
// in some user structures, etc
struct S_TAG_OA_HASH_ENTRY
{
    union
    {
        uint16_t    value;
        struct __attribute__((packed))
        {
            unsigned    busy:       1;          // entry is not empty, in use
            unsigned    newEntry:   1;          // entry just newEntry
                                                // updated by TCPIP_OAHASH_EntryLookupOrInsert() 
                                                // should be used by the caller to 
                                                // update the entry's user info
            unsigned    user:       14;         // user flags
        };
    }flags;      // entry flags
    uint16_t     probeCount;                    // number of steps to get to the
                                                // key's slot
    //
    // void*       data[];  // generic payload up to the application to use
                            // this SHOULD include the key
};


// interface functions
//

// Initializes a OA hash table into the memory
// indicated by pOH->memBlk!
// all entries have their flags field cleared.
void            TCPIP_OAHASH_Initialize(OA_HASH_DCPT* pOH);


// performs look up only
// if no such entry found, it returns NULL
OA_HASH_ENTRY*   TCPIP_OAHASH_EntryLookup(OA_HASH_DCPT* pOH, const void* key);

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
// fullSlots < hElenents)
OA_HASH_ENTRY*   TCPIP_OAHASH_EntryLookupOrInsert(OA_HASH_DCPT* pOH, const void* key);

// Function to remove an entry from the hash
// SHOULD be called when entries have to be deleted!
// because the hash also has to do some internal clean-up.
// Note: when an entry is deleted by TCPIP_OAHASH_EntryLookupOrInsert (calling pOH->delF)
// the hash state is maintained internally, no need to call TCPIP_OAHASH_EntryRemove();
void            TCPIP_OAHASH_EntryRemove(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* pOE);


// Function to remove all entries from the hash
// SHOULD be called when entries have to be deleted!
// because the hash also has to do some internal clean-up.
void            TCPIP_OAHASH_EntriesRemoveAll(OA_HASH_DCPT* pOH);


// helper to return an pointer to an entry
// entryIx should be < hEntries
// otherwise it returns 0 
OA_HASH_ENTRY*  TCPIP_OAHASH_EntryGet(OA_HASH_DCPT* pOH, size_t entryIx);

// helper to return the index of a hash entry
// returns an index >= 0 if success
// returns < 0 if failure
int32_t         TCPIP_OAHASH_EntryGetIndex(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* pHe);       

#if !defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )

// static key manipulation routines

// function that hashes a key and returns a hash value (index)
// this function has to return an index between (-pOH->hEntries, pOH->hEntries)
size_t          TCPIP_OAHASH_KeyHash(OA_HASH_DCPT* pOH, const void* key);

#if defined(OA_DOUBLE_HASH_PROBING)
// probing function
size_t          TCPIP_OAHASH_HashProbe(OA_HASH_DCPT* pOH, const void* key);
#endif  // defined(OA_DOUBLE_HASH_PROBING)

// function to return an entry that can be deleted when the hash is full
// just return 0 if no entry can be deleted
OA_HASH_ENTRY*  TCPIP_OAHASH_EntryDelete(OA_HASH_DCPT* pOH);


// function that performs comparison between a key and an entry
// returns 0 if keys match 
int             TCPIP_OAHASH_KeyCompare(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* hEntry, const void* key);

// function that copies a key to a destination entry
void            TCPIP_OAHASH_KeyCopy(OA_HASH_DCPT* pOH, OA_HASH_ENTRY* dstEntry, const void* srcKey);

#endif  // !defined ( OA_HASH_DYNAMIC_KEY_MANIPULATION )

#endif //  H_OA_HASH_H_


