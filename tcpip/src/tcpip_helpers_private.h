/*******************************************************************************
  Header file for tcpip_helpers_private

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_helpers_private.h

  Summary:
    
  Description:
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

#ifndef H_TCPIP_HELPERS_PRIVATE_H_
#define H_TCPIP_HELPERS_PRIVATE_H_

uint8_t TCPIP_Helper_FindCommonPrefix (const uint8_t* addr1, const uint8_t* addr2, uint8_t bytes);

TCPIP_MAC_POWER_MODE TCPIP_Helper_StringToPowerMode(const char* str);

const char*     TCPIP_Helper_PowerModeToString(TCPIP_MAC_POWER_MODE mode);

/* MISRA C-2012 Rule 8.6 deviated:1 Deviation record ID -  H3_MISRAC_2012_R_8_6_NET_DR_19 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma coverity compliance block deviate:1 "MISRA C-2012 Rule 8.6" "H3_MISRAC_2012_R_8_6_NET_DR_19" 
uint16_t        TCPIP_Helper_CalcIPChecksum(const uint8_t* dataBuffer, uint16_t len, uint16_t seed);
#pragma coverity compliance end_block "MISRA C-2012 Rule 8.6"
#pragma GCC diagnostic pop
/* MISRAC 2012 deviation block end */


#if defined(__mips__)
#define TCPIP_Helper_Memcpy(dst, src, len)     memcpy(dst, src, len)
#else
void            TCPIP_Helper_Memcpy(void *dst, const void *src, size_t len);
#endif
uint16_t        TCPIP_Helper_PacketChecksum(TCPIP_MAC_PACKET* pPkt, uint8_t* startAdd, uint16_t len, uint16_t seed);

uint16_t        TCPIP_Helper_ChecksumFold(uint32_t rawChksum);

uint16_t        TCPIP_Helper_PacketCopy(TCPIP_MAC_PACKET* pSrcPkt, uint8_t* pDest, uint8_t** pStartAdd, uint16_t len, bool srchTransport);

// conversion functions/helpers
static __inline__ NET_PRES_ADDRESS*  __attribute__((always_inline)) FC_MultiAdd2PresAdd(IP_MULTI_ADDRESS* pmAdd)
{
    union
    {
        IP_MULTI_ADDRESS* pmAdd;
        NET_PRES_ADDRESS* netAddr;
    }U_MULTI_ADD_PRES_ADD;

    U_MULTI_ADD_PRES_ADD.pmAdd = pmAdd;
    return U_MULTI_ADD_PRES_ADD.netAddr;
}

static __inline__ IPV4_ADDR* __attribute__((always_inline)) FC_U8Ptr2Ip4Add(uint8_t* uPtr)
{
    union
    {
        uint8_t*    uPtr;
        IPV4_ADDR* pAdd;
    }U_UPTR_IPV4;

    U_UPTR_IPV4.uPtr = uPtr;
    return U_UPTR_IPV4.pAdd;
}

static __inline__ IPV6_ADDR* __attribute__((always_inline)) FC_U8Ptr2Ip6Add(uint8_t* u8Ptr)
{
    union
    {
        uint8_t* u8Ptr;
        IPV6_ADDR* pAdd;
    }U_U8_PTR_IP6_ADD;

    U_U8_PTR_IP6_ADD.u8Ptr = u8Ptr;
    return U_U8_PTR_IP6_ADD.pAdd;
}


// conversion functions/helpers

static __inline__ TCPIP_MAC_ETHERNET_HEADER* __attribute__((always_inline)) FC_Uptr2MacEthHdr(uint8_t* uptr)
{
    union
    {
        uint8_t*    uptr;
        TCPIP_MAC_ETHERNET_HEADER* pHdr;
    }U_UPTR_MAC_ETH_HDR;

    U_UPTR_MAC_ETH_HDR.uptr = uptr;
    return U_UPTR_MAC_ETH_HDR.pHdr;
}

static __inline__ int32_t __attribute__((always_inline)) FC_Ui322I32(uint32_t u32Val)
{
    union
    {
        uint32_t  u32Val;
        int32_t i32Val;
    }U_UI32_I32;

    U_UI32_I32.u32Val = u32Val;
    return U_UI32_I32.i32Val;
}

static __inline__ uint32_t __attribute__((always_inline)) FC_I322Ui32(int32_t i32Val)
{
    union
    {
        int32_t i32Val;
        uint32_t  u32Val;
    }U_I32_UI32;

    U_I32_UI32.i32Val = i32Val;
    return U_I32_UI32.u32Val;
}

static __inline__ PROTECTED_SINGLE_LIST* __attribute__((always_inline)) FC_SglList2ProtList(SINGLE_LIST* list)
{
    union
    {
        SINGLE_LIST*  list;
        PROTECTED_SINGLE_LIST* protList;
    }U_SGL_LIST_PROT_LIST;

    U_SGL_LIST_PROT_LIST.list = list;
    return U_SGL_LIST_PROT_LIST.protList;
}

static __inline__ const TCPIP_MAC_ADDR*  __attribute__((always_inline)) FC_CUptr2CMacAdd(const uint8_t* cuPtr)
{
    union
    {
        const uint8_t* cuPtr;
        const TCPIP_MAC_ADDR* cpMacAddr;
    }U_CPTR_MAC_ADD;

    U_CPTR_MAC_ADD.cuPtr = cuPtr;
    return U_CPTR_MAC_ADD.cpMacAddr;
}

static __inline__ TCPIP_MAC_ADDR* __attribute__((always_inline)) FC_CUptr2MacAdd(const uint8_t* uptr)
{
    union
    {
        const uint8_t* uptr;
        TCPIP_MAC_ADDR* pAdd;
    }U_UPTR_MAC_ADD;

    U_UPTR_MAC_ADD.uptr = uptr;
    return U_UPTR_MAC_ADD.pAdd;
}

static __inline__ const TCPIP_MAC_ADDR* __attribute__((always_inline)) FC_CVptr2MacAdd(const void* vptr)
{
    union
    {
        const void* vptr;
        const TCPIP_MAC_ADDR* pAdd;
    }U_V_PTR_MAC_ADD;

    U_V_PTR_MAC_ADD.vptr = vptr;
    return U_V_PTR_MAC_ADD.pAdd;
}

static __inline__ const uint8_t* __attribute__((always_inline)) FC_CVPtr2CU8(const void* cvPtr)
{
    union
    {
        const void* cvPtr;
        const uint8_t* uPtr;
    }U_CVPTR_U8PTR;

    U_CVPTR_U8PTR.cvPtr = cvPtr;
    return U_CVPTR_U8PTR.uPtr;
}

static __inline__ uint8_t* __attribute__((always_inline)) FC_Cvptr2Uptr(const void* vPtr)
{
    union
    {
        const void* vPtr;
        uint8_t*    uPtr;
    }U_CONST_U_PTR;

    U_CONST_U_PTR.vPtr = vPtr;
    return U_CONST_U_PTR.uPtr;
}



static __inline__  void*  __attribute__((always_inline)) FC_Uint2VPtr(uint32_t val)
{
    union
    {
        uint32_t val;
        void*    vptr;
    }U_UINT32_VPTR;

    U_UINT32_VPTR.val = val;
    return U_UINT32_VPTR.vptr;
} 

static __inline__ void* __attribute__((always_inline)) FC_Uint162VPtr(uint16_t u16Val)
{
    union
    {
        uintptr_t uPtrT; 
        void* vPtr;
    }U_UI16_V_PTR;

    U_UI16_V_PTR.uPtrT = (uintptr_t)u16Val;
    return U_UI16_V_PTR.vPtr;
}


static __inline__ uint32_t __attribute__((always_inline)) FC_UI162UI32(uint16_t u16Val)
{
    union
    {
        uint16_t  u16Val;
        uint32_t  u32Val;
    }U_UI16_UI32;

    U_UI16_UI32.u16Val = u16Val;
    return U_UI16_UI32.u32Val;
}

static __inline__  IP_MULTI_ADDRESS*  __attribute__((always_inline)) FC_Ip4Add2Multi(const IPV4_ADDR* pAdd4)
{
    union
    {
        const IPV4_ADDR*   pAdd4;
        IP_MULTI_ADDRESS*  pMultAdd;
    }U_ADD4_MULT;

    U_ADD4_MULT.pAdd4 = pAdd4;
    return U_ADD4_MULT.pMultAdd;
}

static __inline__ IP_MULTI_ADDRESS* __attribute__((always_inline)) FC_Ip6Add2Multi(const IPV6_ADDR* pAdd)
{
    union
    {
        const IPV6_ADDR*    pAdd;
        IP_MULTI_ADDRESS* pMulti;
    }U_IPV4_MULTI_ADD;

    U_IPV4_MULTI_ADD.pAdd = pAdd;
    return U_IPV4_MULTI_ADD.pMulti;
}

static __inline__ int32_t __attribute__((always_inline)) FC_UI322I32(uint32_t u32Val)
{
    union
    {
        uint32_t  u32Val;
        int32_t i32Val;
    }U_UI32_I32;

    U_UI32_I32.u32Val = u32Val;
    return U_UI32_I32.i32Val;
}


static __inline__ ptrdiff_t __attribute__((always_inline)) FC_CU8PtrDiff(const uint8_t* ptr1, const uint8_t* ptr2)
{
    return ptr1 - ptr2;
}


static __inline__ ptrdiff_t __attribute__((always_inline)) FC_CU32PtrDiff(const uint32_t* ptr1, const uint32_t* ptr2)
{
    return ptr1 - ptr2;
}

static __inline__ uint16_t __attribute__((always_inline)) FC_PtrDiff2UI16(ptrdiff_t pDiff)
{
    union
    {
        ptrdiff_t   pDiff;
        uint16_t    v16;
    }U_PTR_DIFF_UI16;

    TCPIPStack_Assert(pDiff >= 0, __FILE__, __func__, __LINE__);
    U_PTR_DIFF_UI16.pDiff = pDiff;
    return U_PTR_DIFF_UI16.v16;
}

static __inline__ uint16_t __attribute__((always_inline)) FC_ChPtrDiff2UI16(const char* ptr1, const char* ptr2)
{
    union
    {
        ptrdiff_t   pDiff;
        uint16_t    v16;
    }U_PTR_DIFF_UI16;

    U_PTR_DIFF_UI16.pDiff = ptr1 - ptr2;
    TCPIPStack_Assert(U_PTR_DIFF_UI16.pDiff >= 0, __FILE__, __func__, __LINE__);
    return U_PTR_DIFF_UI16.v16;
}

static __inline__ uint16_t __attribute__((always_inline)) FC_U8PtrDiff2UI16(const uint8_t* ptr1, const uint8_t* ptr2)
{
    union
    {
        ptrdiff_t   pDiff;
        uint16_t    v16;
    }U_PTR_DIFF_UI16;

    U_PTR_DIFF_UI16.pDiff = ptr1 - ptr2;
    TCPIPStack_Assert(U_PTR_DIFF_UI16.pDiff >= 0, __FILE__, __func__, __LINE__);
    return U_PTR_DIFF_UI16.v16;
}


static __inline__ int16_t __attribute__((always_inline)) FC_PtrDiff2I16(uint8_t* ptr1, uint8_t* ptr2)
{
    union
    {
        ptrdiff_t   pDiff;
        int16_t     v16;
    }U_PTR_DIFF_I16;

    U_PTR_DIFF_I16.pDiff = ptr1 - ptr2;
    return U_PTR_DIFF_I16.v16;
}

static __inline__ uint32_t __attribute__((always_inline)) FC_PtrDiff2UI32(ptrdiff_t pDiff)
{
    union
    {
        ptrdiff_t   pDiff;
        uint32_t    uv32;
    }U_PTR_DIFF_UI32;

    TCPIPStack_Assert(pDiff >= 0, __FILE__, __func__, __LINE__);
    U_PTR_DIFF_UI32.pDiff = pDiff;
    return U_PTR_DIFF_UI32.uv32;
}

static __inline__ const uint8_t* __attribute__((always_inline)) FC_CStr2CUPtr(const char* cStr)
{
    union
    {
        const char* cStr;
        const uint8_t* cuPtr;
    }U_CHAR_UPTR;

    U_CHAR_UPTR.cStr = cStr;
    return U_CHAR_UPTR.cuPtr;
}

static __inline__ const void* __attribute__((always_inline)) FC_CStr2CVPtr(const char* cStr)
{
    union
    {
        const char* cStr;
        const void* cvPtr;
    }U_CHAR_V_PTR;

    U_CHAR_V_PTR.cStr = cStr;
    return U_CHAR_V_PTR.cvPtr;
}

static __inline__ char* __attribute__((always_inline)) FC_CStr2Str(const char* cStr)
{
    union
    {
        const char* cStr;
        char* str;
    }U_CHAR_STR;

    U_CHAR_STR.cStr = cStr;
    return U_CHAR_STR.str;
}


static __inline__ uint8_t* __attribute__((always_inline)) FC_CUPtr2UPtr(const uint8_t* cuPtr)
{
    union
    {
        const uint8_t* cuPtr;
        uint8_t* pu8;
    }U_CUPTR_UPTR;

    U_CUPTR_UPTR.cuPtr = cuPtr;
    return U_CUPTR_UPTR.pu8;
}

static __inline__  uint32_t  __attribute__((always_inline)) FC_I32Val2U32Val(int32_t i32Val)
{
    union
    {
        int32_t i32Val; 
        uint32_t u32Val;
    }U_I32_U32;

    U_I32_U32.i32Val = i32Val;
    return U_I32_U32.u32Val;
} 

static __inline__  int32_t  __attribute__((always_inline)) FC_U32Val2I32Val(uint32_t u32Val)
{
    union
    {
        uint32_t u32Val;
        int32_t i32Val; 
    }U_U32_I32;

    U_U32_I32.u32Val = u32Val;
    return U_U32_I32.i32Val;
} 

static __inline__ uint32_t __attribute__((always_inline)) FC_CvPtr2U32(const void* cvPtr)
{
    union
    {
        const void* cvPtr;
        uint32_t    u32Val;
    }U_CV_PTR_U32_VAL;

    U_CV_PTR_U32_VAL.cvPtr = cvPtr;
    return U_CV_PTR_U32_VAL.u32Val;
}

static __inline__ const void* __attribute__((always_inline)) FC_U322CvPtr(uint32_t u32Val)
{
    union
    {
        uintptr_t    ptr;
        const void* cvPtr;
    }U_U32_VAL_CV_PTR;

    U_U32_VAL_CV_PTR.ptr = (uintptr_t)u32Val;
    return U_U32_VAL_CV_PTR.cvPtr;
}


static __inline__ TCPIP_MAC_PACKET* __attribute__((always_inline)) FC_CvPtr2MacPkt(const void* cvPtr)
{
    union
    {
        const void* cvPtr;
        TCPIP_MAC_PACKET*  pPkt;
    }U_CV_PTR_MAC_PKT;

    U_CV_PTR_MAC_PKT.cvPtr = cvPtr;
    return U_CV_PTR_MAC_PKT.pPkt;
}

static __inline__ uint8_t* __attribute__((always_inline)) FC_MacPkt2U8Ptr(TCPIP_MAC_PACKET* pPkt)
{
    union
    {
        TCPIP_MAC_PACKET*  pPkt;
        uint8_t* u8Ptr;
    }U_MAC_PKT_U8_PTR;

    U_MAC_PKT_U8_PTR.pPkt = pPkt;
    return U_MAC_PKT_U8_PTR.u8Ptr;
}

static __inline__ SGL_LIST_NODE*  __attribute__((always_inline)) FC_MacPkt2SglNode(TCPIP_MAC_PACKET* pPkt)
{
    union
    {
        TCPIP_MAC_PACKET* pPkt;
        SGL_LIST_NODE*  node;
    }U_MAC_PKT_NODE;

    U_MAC_PKT_NODE.pPkt = pPkt;
    return U_MAC_PKT_NODE.node;
}

static __inline__ TCPIP_MAC_PACKET*  __attribute__((always_inline)) FC_SglNode2MacPkt(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        TCPIP_MAC_PACKET* pPkt;
    }U_NODE_MAC_PKT;

    U_NODE_MAC_PKT.node = node;
    return U_NODE_MAC_PKT.pPkt;
}

static __inline__ TCPIP_MAC_DATA_SEGMENT* __attribute__((always_inline)) FC_Uptr2MacDataSeg(uint8_t* uptr)
{
    union
    {
        uint8_t* uptr;
        TCPIP_MAC_DATA_SEGMENT * dSeg;
    }U_UPTR_MAC_DATA_SEG;

    U_UPTR_MAC_DATA_SEG.uptr = uptr;
    return U_UPTR_MAC_DATA_SEG.dSeg;
}

static __inline__ TCPIP_MAC_SEGMENT_GAP_DCPT* __attribute__((always_inline)) FC_Uptr2MacGapDcpt(uint8_t* uptr)
{
    union
    {
        uint8_t* uptr;
        TCPIP_MAC_SEGMENT_GAP_DCPT * gapDcpt;
    }U_UPTR_MAC_GAP_DCPT;

    U_UPTR_MAC_GAP_DCPT.uptr = uptr;
    return U_UPTR_MAC_GAP_DCPT.gapDcpt;
}

static __inline__ uint16_t* __attribute__((always_inline)) FC_Cptr82Ptr16(const uint8_t* cptr8)
{
    union
    {
        const uint8_t* cptr8;
        uintptr_t      uPtr;
        uint16_t *     ptr16;
    }U_CPTR8_PTR16;

    U_CPTR8_PTR16.cptr8 = cptr8;
    TCPIPStack_Assert((U_CPTR8_PTR16.uPtr & 0x1U) == 0U, __FILE__, __func__, __LINE__);
    return U_CPTR8_PTR16.ptr16; 
}

static __inline__ uint16_t __attribute__((always_inline)) FC_Int322Uint16(int32_t ival)
{
    union
    {
        int32_t ival;
        uint16_t val16;
    }U_INT32_UINT16;

    U_INT32_UINT16.ival = ival;
    return U_INT32_UINT16.val16; 
}

static __inline__ const void*  __attribute__((always_inline)) FC_SglNode2CvPtr(SGL_LIST_NODE* node)
{
    union
    {
        SGL_LIST_NODE*  node;
        const void* cvPtr;
    }U_NODE_CV_PTR;

    U_NODE_CV_PTR.node = node;
    return U_NODE_CV_PTR.cvPtr;
}

static __inline__ SGL_LIST_NODE*  __attribute__((always_inline)) FC_CvPtr2SglNode(const void* cvPtr)
{
    union
    {
        const void* cvPtr;
        SGL_LIST_NODE*  node;
    }U_CV_PTR_NODE;

    U_CV_PTR_NODE.cvPtr = cvPtr;
    return U_CV_PTR_NODE.node;
}

static __inline__ uint16_t __attribute__((always_inline)) FC_MinU16(uint16_t a, uint16_t b)
{
    return (a < b) ? a : b;
}

// end of conversion functions/helpers


// string to long
// returns: 0 if success
//          -1 if failure
//  result is deposited at pRes; could be NULL if just testing
// the return value should be checked by the caller
int FC_Str2L(const char* str, int base, int32_t* pRes);

// string to unsigned long
// returns: >= 0 if success
//          -1 if failure
//  result is deposited at pRes; could be NULL if just testing
// the return value should be checked by the caller
int FC_Str2UL(const char* str, int base, uint32_t* pRes);

// safe sprintf function
// returns the number of charactes assembled into the buffer:
//      >=0 for success
//      < 0 if failure
// the return value should be checked by the caller
int FC_sprintf(char* buff, size_t buffSize, const char* fmt, ...);

// sort function
void FC_Sort (void * base, size_t numElem, size_t elemSize, int (*comparF)(const void* p1, const void* p2));

#endif  // H_TCPIP_HELPERS_PRIVATE_H_
