#ifndef __NC_XORASM_H__
#define __NC_XORASM_H__

#include <inttypes.h>
#include <iostream>

#if defined(__x86_64__)

/* x86_64 - XOR data in 128-bit chunks */
inline void xor_buf(const char *aRowA, const char *aRowB, uint32_t length)
{
    asm(
            "movl %2, %%edi;"                       //set the counter to the length

            "L_%=1:"                                //start loop
            "movdqa -16(%%rdi,%0,), %%xmm0;"
            "pxor -16(%%rdi,%1,), %%xmm0;"
            "movdqa %%xmm0, -16(%%rdi,%0,);"
            "sub $16, %%rdi;"

            "jnz L_%=1;"                            // if %%edi is greater than zero run again

            :                                       // no output
            :"r"(aRowA), "r"(aRowB), "r"(length)    // input operands
            :"%rdi", "%xmm0"                        // clobbered registers

       );
}

#elif defined(__i386__) && defined(__SSE2__)

/* i386 with SSE2 instructions - XOR data in 128-bit chunks */
inline void xor_buf(char *aRowA, const char *aRowB, uint32_t length)
{
    asm(
            "movl %2, %%edi;"                       //set the counter to the length

            "L_%=1:"                                //start loop
            "movdqa -16(%%edi,%0,), %%xmm0;"
            "pxor -16(%%edi,%1,), %%xmm0;"
            "movdqa %%xmm0, -16(%%edi,%0,);"
            "sub $16, %%edi;"

            "jnz L_%=1;"                            // if %%edi is greater than zero run again

            :                            	        // no output
            :"r"(aRowA), "r"(aRowB), "r"(length)	// input operands
            :"%edi", "%xmm0"                        // clobbered registers

       );
}

#elif defined(__i386__) && defined(__MMX__)

/* i386 with MMX instructions - XOR data in 64-bit chunks */
inline void xor_buf(char *aRowA, const char *aRowB, uint32_t length)
{
    asm(
            "movl %2, %%edi;"                       //set the counter to the length

            "L_%=1:"                                //start loop
            "movq -8(%%edi,%0,),%%mm0;"
            "pxor -8(%%edi,%1,),%%mm0;"
            "movq %%mm0, -8(%%edi,%0,);"
            "sub $8, %%edi;"

            "jnz L_%=1;"                            // if %%edi is greater than zero run again

            :                                       // no output
            :"r"(aRowA), "r"(aRowB), "r"(length)    // input operands
            :"%edi", "%mm0"                         // clobbered registers

       );
}

#elif defined(__i386__)

/* Regular i386 without MMX or SSE - XOR data in 32-bit chunks */
inline void xor_buf(char *aRowA, const char *aRowB, uint32_t length)
{
    asm(
            "movl %2, %%edi;"                       //set the counter to the length

            "L_%=1:"                                //start loop
            "movl -4(%%edi,%0,),%%eax;"
            "xorl -4(%%edi,%1,),%%eax;"
            "movl %%eax, -4(%%edi,%0,);"
            "sub $4, %%edi;"

            "jnz L_%=1;"                            // if %%edi is greater than zero run again	    

            :                                       // no output
            :"r"(aRowA), "r"(aRowB), "r"(length)    // input operands
            :"%edi", "%eax"                         // clobbered registers

       );
}

#else

#ifdef __GCC__
#warning "Compiling for a non-optimized processor architecture. Execution may be very slow."
#endif // __GCC__

/* Every other architecture - XOR data in whatever chunk size the compiler selects */
inline void xor_buf(char *aRowA, const char *aRowB, uint32_t length)
{
    for(unsigned i = 0; i < length; ++i)
	aRowA[i] ^= aRowB[i];
}

#endif // Arch switch

#endif // __NC_XORASM_H__
