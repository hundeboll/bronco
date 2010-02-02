#include "encoding_vector.h"
#include "gf2_common.h"

#include <cassert>
#include <cstring>


/**
 * The number of useful random bits in each char of the encoding vector
 */
const int KRandomBits = 8;


Gf2::EncodingVector::EncodingVector(char *aVector, int aPackets)
    : iVector(aVector), iPackets(aPackets)
{
    assert(iVector != 0);
    assert(iPackets > 0); 
}


int Gf2::EncodingVector::Bytes(int aPackets)
{
    assert(aPackets > 0);
    
    int datalength = 0;
    
    // the number of bytes needed
    if(aPackets <= KRandomBits)
	datalength = 1;
    else
	datalength = (aPackets / KRandomBits) + (aPackets % KRandomBits == 0 ? 0 : 1);

    return datalength;
}


void Gf2::EncodingVector::Set(int aIndex)
{
    int mask = 1 << (aIndex % KRandomBits);
    iVector[aIndex / KRandomBits] |= mask;
}


bool Gf2::EncodingVector::IsSet(int aIndex) const
{
    assert(aIndex >= 0);
    
    if((iVector[aIndex / KRandomBits] >> (aIndex % KRandomBits)) & 0x1)
    {
	return true;
    }
    else
    {
	return false;
    }
}







