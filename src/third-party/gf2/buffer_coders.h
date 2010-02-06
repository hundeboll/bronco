#ifndef GF2_BUFFER_CODERS_H
#define GF2_BUFFER_CODERS_H

#include <cmath>
#include <cassert>
#include <iostream>
#include <vector>

#include "encoder.h"
#include "decoder.h"
#include "encoding_vector.h"

namespace Gf2
{

    template<class Coder, class Function>
    inline int SumFunction(std::vector<Coder> &aCoders, Function aFunc)
    {
	int size = 0;
	for(size_t i = 0; i < aCoders.size(); ++i)
	    size += aFunc( aCoders[i] );
	
	return size;
    }

    
    
    
    /**
     * Based on the desired number of packets (aPackets) and the packets size (aPacketSize)
     * for each decoder the function creates a vector of decoders to decoded aDataSize bytes.
     */
    template<class T>
    inline void CreateCoders(int aDataSize, int aPackets, int aPacketSize, std::vector<T> &aCoders)
    {
	assert(aDataSize > 0);
	assert(aPackets > 0);
	assert(aPacketSize > 0);
        assert(aCoders.size() == 0);
	
	int g = aPackets;
	int p = aPacketSize;
	
	// Target size of each generation
	int gp = g*p;
	
	while(aDataSize > 0)
	{
	    if(aDataSize < gp)
	    {
		// We don't have enough data to fill
		// a generation - devide size of buffer
		// with the packet-size to see how large
		// the generation should be.
		float temp = aDataSize / (float)p;
		g = ceil(temp);

		// Create the encoder
                aCoders.push_back( T(g,p) );

		// Update the buffer variable
		aDataSize -= g*p;
	    }
	    else
	    {
		// There is enough data to fill an entire
		// generation
                aCoders.push_back( T(g,p) );
		
		// Update the buffer variable
		aDataSize -= gp;
	    }
	}
    }



    inline void AddBuffer(std::vector<Encoder> &aEncoders, const char *aData)
    {
	assert(aData != 0);
	assert(aEncoders.size() > 0);
	
	// Divide the buffer into the created encoders
	// starting from the offset = 0
	int offset = 0;
	
	for(unsigned i = 0; i < aEncoders.size(); ++i)
	{
	    int packets = aEncoders[i].Packets();
	    
	    aEncoders[i].AddBuffer(aData + offset, packets);
	    
	    offset += aEncoders[i].DataBufferSize();
	}
    }


    inline void AddBuffer(std::vector<Decoder> &aDecoders, char *aDataBuffer, char *aVectorBuffer)
    {
	assert(aDataBuffer != 0);
	assert(aVectorBuffer != 0);
	assert(aDecoders.size() > 0);
	
	// Divide the buffer into the created encoders
	// starting from the offset = 0
	int bufferOffset = 0;
	int vectorOffset = 0;
	
	for(unsigned i = 0; i < aDecoders.size(); ++i)
	{
	    int packets = aDecoders[i].Packets();
	    
	    char *data = aDataBuffer + bufferOffset;
	    char *vector = aVectorBuffer + vectorOffset;
	    
	    aDecoders[i].AddBuffer(data, vector, packets);
	    
	    bufferOffset += aDecoders[i].DataBufferSize();
	    vectorOffset += aDecoders[i].VectorBufferSize();
	}
    }
    


/*     class BufferEncoder */
/*     { */
/*     public: */
/* 	BufferEncoder(size_t aDataSize, size_t aPackets, size_t aPacketSize); */

/* 	/\** */
/* 	 * Sets the random generator function */
/* 	 *\/ */
/* 	void SetRandomizer(); */
	
/* 	/\** */
/* 	 * Returns the size in bytes of the original data being encoded */
/* 	 *\/ */
/* 	size_t DataSize() const; */

/* 	/\** */
/* 	 * Returns the size in bytes of the buffer needed by the encoder  */
/* 	 *\/ */
/* 	size_t BufferSize() const; */

/* 	/\** */
/* 	 * Returns the number of encoders / generations used to encode */
/* 	 * the data */
/* 	 *\/ */
/* 	size_t EncoderCount() const; */

/* 	/\** */
/* 	 * Returns the number of packets in the i'th encoder / generation. */
/* 	 *\/ */
/* 	size_t PacketCount(int encoder) const; */

/* 	/\** */
/* 	 * Returns the size in bytes of an encoded data packet in the i'th */
/* 	 * encoder / generation */
/* 	 *\/ */
/* 	size_t PacketSize(int encoder) const; */

/* 	/\** */
/* 	 * Returns the size in bytes of the encoding vector for the i'th */
/* 	 * encoder / generation	  */
/* 	 *\/ */
/* 	size_t VectorSize(int encoder) const; */

/* 	/\** */
/* 	 * Sets the data buffer that the encoder should use during the encoding. */
/* 	 * The data buffer should have a capacity of BufferSize() bytes, in addition */
/* 	 * the buffer should be memset to '\0' before copying the original data into */
/* 	 * the buffer. */
/* 	 *\/ */
/* 	void AddBuffer(char *aData); */


/* 	/\** */
/* 	 * Encodes a packet from the i'th encoder / generation and places the */
/* 	 * resulting packet and encoding vector into the supplied data buffers. */
/* 	 * The data buffers should be allocated hold at PacketSize() and VectorSize() */
/* 	 * bytes respectivly. */
/* 	 *\/ */
/* 	void Encode( */
	
/*     }; */
    
}


#endif
