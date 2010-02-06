#include "encoder.h"
#include "encoding_vector.h"
#include "gf2_common.h"

#include <cassert>
#include <cstring>

Gf2::Encoder::Encoder(int aPackets, int aPacketSize) : iPackets(aPackets), iPacketSize(aPacketSize), iVectorSize(EncodingVector::Bytes(aPackets))
{
    assert(iPacketSize > 0);
    assert(iPackets > 0);
}

int Gf2::Encoder::Packets() const
{
    return iPackets;
}

int Gf2::Encoder::PacketSize() const
{
    return iPacketSize;
}

int Gf2::Encoder::DataBufferSize() const
{
    return iPackets*iPacketSize;
}

size_t Gf2::Encoder::VectorSize() const
{
    return iVectorSize;
}

void Gf2::Encoder::AddBuffer(const char *aPacket)
{
    assert(aPacket != 0);
    assert(iPacketBuffer.size() <= iPackets);
    
    iPacketBuffer.push_back(aPacket);
}

void Gf2::Encoder::AddBuffer(const char* aBuffer, size_t aPacketCount)
{
    assert(aBuffer != 0);
    assert(aPacketCount <= iPackets);
    
    for(size_t i = 0; i < aPacketCount; ++i)
    {
	AddBuffer(aBuffer + i*iPacketSize);
    }
}

void Gf2::Encoder::Encode(char *aEncodedPacket, char *aEncodingVector)
{
    assert(aEncodedPacket != 0);
    assert(aEncodingVector != 0);

    // Zero packet buffer
    memset(aEncodedPacket, '\0', iPacketSize);

    EncodingVector vector(aEncodingVector, iPackets);
    
    for(size_t i = 0 ; i < iPackets; ++i)
    {
	if( vector.IsSet(i) )
	{
	    XorBuffer(aEncodedPacket, iPacketBuffer[i], iPacketSize);
	}      
    }
}

void Gf2::Encoder::Encode(size_t aPacketId, char *aEncodedPacket)
{
    assert(aPacketId < iPackets);
    assert(aEncodedPacket != 0);

    memcpy(aEncodedPacket, iPacketBuffer[aPacketId], iPacketSize);
}
