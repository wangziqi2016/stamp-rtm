
// This header file defines macros for compiling workloads (if you have the source code)
// These macros should be used with a special simulator that recognizes the macro
// If the workload is executed on bare metal hardware, they cause no visible change to the 
// machine state except using a few more cycles

#ifndef _NVOVERLAY_SIM_H
#define _NVOVERLAY_SIM_H

// XCHG R8, R8 - marks the body of the workload
#define NVOVERLAY_SIM_BEGIN() asm volatile(".byte 0x4D, 0x87, 0xC0" ::: "memory"); 
// XCHG R9, R9 - marks the end of the workload
#define NVOVERLAY_SIM_END() asm volatile(".byte 0x4D, 0x87, 0xC9" ::: "memory"); 
// XCHG R10, R10 - marks the beginning of multithreaded 
#define NVOVERLAY_SIM_MT_BEGIN() asm volatile(".byte 0x4D, 0x87, 0xD2" ::: "memory"); 

#endif