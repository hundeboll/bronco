#include "decoder.h"
#include "encoding_vector.h"
#include "gf2_common.h"

#include <cstring>
#include <cassert>

Gf2::Decoder::Decoder(size_t aPackets, size_t aPacketSize)
  : iPackets(aPackets), iPacketSize(aPacketSize), iVectorSize(EncodingVector::Bytes(aPackets)),
    iRank(0), iMaxPivotId(1)
{  
    iUncoded.resize(iPackets, false);
    iCoded.resize(iPackets, false);   
}


size_t Gf2::Decoder::Packets() const
{
    return iPackets;
}

size_t Gf2::Decoder::PacketSize() const
{
    return iPacketSize;
}

size_t Gf2::Decoder::DataBufferSize() const
{
    return iPackets*iPacketSize;
}

size_t Gf2::Decoder::VectorSize() const
{
    return iVectorSize;
}

size_t Gf2::Decoder::VectorBufferSize() const
{
    return iPackets*iVectorSize;
}


bool Gf2::Decoder::IsComplete() const
{
    return iRank == iPackets;
}


void Gf2::Decoder::AddBuffer(char *aPacket, char *aEncodingVector)
{
    assert(iPacketBuffer.size() < iPackets);
    assert(iVectorBuffer.size() < iPackets);
    assert(aPacket != 0);
    assert(aEncodingVector != 0);

    // Zero output buffers
    memset(aPacket, '\0', iPacketSize);
    memset(aEncodingVector, '\0', iVectorSize);
    
    iPacketBuffer.push_back(aPacket);
    iVectorBuffer.push_back(aEncodingVector);
}


void Gf2::Decoder::AddBuffer(char *aBuffer, char *aEncodingVectors, size_t aPacketCount)
{
    assert(iPacketBuffer.size() < iPackets);
    assert(iVectorBuffer.size() < iPackets);
    assert(aBuffer != 0);
    assert(aEncodingVectors != 0);
    assert(aPacketCount <= iPackets);
    
    for(size_t i = 0; i < aPacketCount; ++i)
    {
	AddBuffer(aBuffer + i*iPacketSize, aEncodingVectors + i*iVectorSize);
    }   
}
								

void Gf2::Decoder::Decode(char *aPacket, char *aEncodingVector)
{
    assert(iPacketBuffer.size() == iPackets);
    assert(iVectorBuffer.size() == iPackets);
    assert(aPacket != 0);
    assert(aEncodingVector != 0);

    EncodingVector vector(aEncodingVector, iPackets);
    
    bool pivot = false;
    size_t pivotId = 0;

    // First we reduce the new packet with the packet we already had
    for(size_t i = 0; i < iPackets; ++i)
    {
	// If we have a "1" on the i'th place in the encoding vector
	if( vector.IsSet(i) )
	{
	    // If we have the packet with pivot on the i'th place
	    if(iUncoded[i] || iCoded[i])
	    {
		// We "remove" that packet to reduce the newly received packet
		XorBuffer(aPacket, iPacketBuffer[i], iPacketSize); 
		
		// And we update encoding vector
		XorBuffer(aEncodingVector, iVectorBuffer[i], iVectorSize);
	    }
	    else
	    {
		// We do not already have a "1" on the i'th place as pivot we therefore save 
		// this packet with  a pivot element on the i'th index
		if(!pivot)
		{
		    pivot = true;
		    pivotId = i;
		    ++iRank;
		    if(pivotId > iMaxPivotId)
		    {
			iMaxPivotId = pivotId;
			break;
		    }
		}
	    }
	}
    }

    // Did we find a "1" we did not already have as pivot?
    if(!pivot)
    {
	// We did not find any pivot elements in the new packet so we return without saving the packet
	return;
    }

    // We found a "1" that nobody else had as pivot, we now substract this packet
    // from other coded packets - if they have a "1" on our pivot place
    for(size_t i = 0; i < iMaxPivotId; ++i)
    {
	if(iCoded[i])
	{
	    EncodingVector vector(iVectorBuffer[i], iPackets);
	    
	    if( vector.IsSet(pivotId) )
	    {
		XorBuffer(iPacketBuffer[i], aPacket, iPacketSize);
		XorBuffer(iVectorBuffer[i], aEncodingVector, iVectorSize);		
	    }
	}
    }

    iPivot.push_back(pivotId);
    iCoded[pivotId] = true;

    // Copy the new packet and encoding vector
    memcpy(iPacketBuffer[pivotId], aPacket, iPacketSize);
    memcpy(iVectorBuffer[pivotId], aEncodingVector, iVectorSize);
}


void Gf2::Decoder::Decode(const char *aPacket, size_t aPacketNumber)
{
    assert(iPacketBuffer.size() == iPackets);
    assert(iVectorBuffer.size() == iPackets);
    assert(aPacketNumber >= 0);
    assert(aPacketNumber < iPackets);
    assert(aPacket != 0);

    // Copy the packet
    memcpy(iPacketBuffer[aPacketNumber], aPacket, iPacketSize);
    
    // Update the encoding vector
    EncodingVector vector(iVectorBuffer[aPacketNumber], iPackets);
    vector.Set(aPacketNumber);
    
    // Mark that we have this packet
    iUncoded[aPacketNumber] = true;
    iPivot.push_back(aPacketNumber);
    ++iRank;
    
    if(aPacketNumber > iMaxPivotId)
	iMaxPivotId = aPacketNumber;
}


void Gf2::Decoder::Recode(char *aPacket, char *aVector, char *aEncodingVector)
{
    assert(iPacketBuffer.size() == iPackets);
    assert(iVectorBuffer.size() == iPackets);
    assert(aPacket != 0);
    assert(aVector != 0);
    assert(aEncodingVector != 0);

    // Zero output buffers
    memset(aPacket, '\0', iPacketSize);
    memset(aVector, '\0', iVectorSize);
    
    EncodingVector vector(aEncodingVector, iRank);
    for(size_t i = 0; i < iPivot.size(); ++i)
    {
	if(vector.IsSet(i))
	{
	    XorBuffer(aPacket, iPacketBuffer[iPivot[i]], iPacketSize); 	
	    XorBuffer(aVector, iVectorBuffer[iPivot[i]], iVectorSize);
	}
    }  
}


size_t Gf2::Decoder::Rank() const
{
    return iRank;
}
