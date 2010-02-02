#include "gf2_common.h"
#include <cassert>
#include <cstring>
#include <iostream>

///
/// RandomGenerator
///
Gf2::RandomGenerator::RandomGenerator(int (*aFunc)() ) : iFunc(aFunc)
{
}


void Gf2::RandomGenerator::Fill(char *aData, int aBytes) const
{
    assert(aData != 0);
    assert(aBytes > 0);
    
    int bytes = aBytes;

    for(int i = 0; i < aBytes; i += 3)
    {
	int random = (*iFunc)();
	
	switch(bytes)
	{
	case 1: memcpy(aData+i, (const char*)&random, 1); break;
	case 2: memcpy(aData+i, (const char*)&random, 2); break;
        default:
	    memcpy(aData+i, (const char*)&random, 3);
	    bytes -= 3;
	    break;
	}
    }
}

// void Gf2::RandomGenerator::Fill(char *aData, int aBytes) const
// {
//     for(int i = 0; i < aBytes; ++i)
//     {
// 	*(short int*)aData = (*iFunc)() % 1024 + 1;
//     }
// }


void Gf2::XorBuffer(int *aA, const int *aB, int aLength)
{
    for(int i = 0; i < aLength; ++i)
	aA[i] ^= aB[i];
}


void Gf2::XorBuffer(char *aA, const char *aB, int aLength)
{
    for(int i = 0; i < aLength; ++i)
	aA[i] ^= aB[i];
}

