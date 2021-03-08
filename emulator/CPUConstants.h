#pragma once

#define FLAG_INTERRUPT 0x01
#define FLAG_GREATER   0x02
#define FLAG_OVERFLOW  0x04
#define FLAG_ZERO      0x08
#define FLAG_IN_IRQ    0x10
#define FLAG_PAGING    0x20
#define FLAG_EXCEPTION 0x40

#define INT_INVALID_MATH 0
#define INT_INVALID_INSTRUCTION 1
#define INT_UNSPECIFIED 2
#define INT_PAGE_FAULT 3
#define INT_USER1 6
#define INT_USER2 7

#define UE_REALLY 0x0
#define UE_INVALID_IRET 0x1
#define UE_NMI 0x2
