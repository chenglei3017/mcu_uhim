#ifndef _PLIB_PMP_PROCESSOR_H
#define _PLIB_PMP_PROCESSOR_H

#if defined(__PIC16F__)
    #include "pmp_pic_other.h"

#elif defined(__18CXX)
    #include "pmp_pic18.h"

#elif defined(_PIC18)
    #include "pmp_pic18.h"

#elif defined(__PIC24F__)
    #include "pmp_p24Fxxxx.h"

#elif defined(__PIC24H__)
    #include "pmp_p24Hxxxx.h"

#elif defined(__dsPIC30F__)
    #include "pmp_p30Fxxxx.h"

#elif defined(__dsPIC33E__)
    #include "pmp_p33Exxxx.h"

#elif defined(__dsPIC33F__)
    #include "pmp_p33Fxxxx.h"

#elif defined(__PIC32MX__)
    #include "pmp_p32xxxx.h"

#elif defined(__PIC32MZ__)
    #include "pmp_p32xxxx.h"
	
#elif defined(__PIC32WK__)
    #include "pmp_p32xxxx.h"


#else
    #error "Can't find header"

#endif

#endif//_PLIB_PMP_PROCESSOR_H