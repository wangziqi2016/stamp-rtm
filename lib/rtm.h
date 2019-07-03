/*
 * rtm.h
 *
 * Created on: 03.07.2012
 *     Introduces intrinsics for Intel compiler as defines.
 */

#ifndef RTM_H_
#define RTM_H_

#include <stdint.h>
#include <assert.h>

/* Check CPUID Leaf 0x7 EBX[11] */
#define CPUID_RTM_CHECK {            \
        int rtm_present;             \
        uint32_t ebx, eax = 0x7;     \
        asm volatile("cpuid"         \
                     :"=b"(ebx)      \
                     :"a"(eax)       \
                     :"ecx","edx");  \
        rtm_present = ebx & (1u<<11); \
        assert(rtm_present && "RTM is not present"); \
        }

// These macros are never defined
#ifdef OLD_RTM_MACROSES
// Issue 'XBEGIN -6' to jump to the beginning of the Tx
#define _xbegin() {\
        __asm__ __volatile__ (".byte 0xc7, 0xf8, 0xfa, 0xff, 0xff, 0xff":::"eax") ;\
        }

#define _xend() {\
        __asm__ __volatile__ (".byte 0x0f, 0x01, 0xd5") ;\
        }

#define _xabort(byte) {\
        __asm__ __volatile__ (".byte 0xc6, 0xf8, " #byte :::"eax") ;\
        }
#endif // OLD_RTM_MACROSES

#define XBEGIN(label)   \
     asm volatile goto(".byte 0xc7,0xf8 ; .long %l0-1f\n1:" ::: "eax","memory" : label)
#define XEND()    asm volatile(".byte 0x0f,0x01,0xd5" ::: "memory")
// 31 c0 ff c8: xor eax, eax; dec eax;  -> This is used to indicate when a fall through path is not executed eax is 0xffffffff
// 48 87 c9: xchg rcx, rcx -> This is for instrumentation
// The last assembly is for the assembler to not assume EAX after this point and do not reorder inst
#define XFAIL(label)        asm volatile(".byte 0x31, 0xC0, 0xFF, 0xC8" ::: "eax", "memory"); \
                     label: asm volatile(".byte 0x48, 0x87, 0xC9" ::: "eax", "memory"); \
                            asm volatile("" ::: "eax", "memory");
#define XFAIL_STATUS(label, status) label: asm volatile("" : "=a" (status) :: "memory")
#define XABORT(status) asm volatile (".byte 0xc6, 0xf8, " #status :::"eax") 
// Abort reason is set to 0xF5, which is interpreted as "USER"
#define XABORT_RESTART() asm volatile (".byte 0xc6, 0xf8, 0xf5" :::"eax") 
#define XTEST() ({ char o = 0 ;                     \
           asm volatile(".byte 0x0f,0x01,0xd6 ; setnz %0" : "+r" (o)::"memory"); \
           o; })

// XCHG EDI, EDI
#define TM_MARK_RO()             asm volatile(".byte 0x87, 0xff" ::: "memory"); 
// XCHG ESI, ESI Toggles whether we track the read set
#define TM_TOGGLE_TRACK_READ()   asm volatile(".byte 0x87, 0xf6" ::: "memory"); 
// XCHG ESP, ESP Toggled an user defined action implemented by the TM
#define TM_TOGGLE_USER_DEFINED_1()   asm volatile(".byte 0x87, 0xe4" ::: "memory"); 

/* Status bits */
#define XABORT_EXPLICIT_ABORT   (1 << 0)
#define XABORT_RETRY        (1 << 1)
#define XABORT_CONFLICT     (1 << 2)
#define XABORT_CAPACITY     (1 << 3)
#define XABORT_DEBUG        (1 << 4)
#define XABORT_STATUS(x)    (((x) >> 24) & 0xff)

// Illegal instruction; Used to check if we should retry
#define ABORT_CODE_ILLEGAL  (0xfe)
// Restart abort code
#define ABORT_CODE_RESTART  (0xf5)
#define XABORT_STATUS_NONE (0xFFFFFFFF) // A new txn just started

#endif /* RTM_H_ */
