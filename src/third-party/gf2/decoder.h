#ifndef GF2_DECODER_H
#define GF2_DECODER_H

#include <boost/iostreams/device/mapped_file.hpp>

#include <vector>
#include <fstream>

namespace Gf2
{  
    class Decoder
    {
    public:
        boost::iostreams::mapped_file_sink buffer_file;
        boost::iostreams::mapped_file_sink vector_file;

	/**
	 * Constructs a new decoder object
	 */
	Decoder(size_t aPackets, size_t aPacketSize);

    ~Decoder()
    {
        buffer_file.close();
        vector_file.close();
    }

	/**
	 * Returns the number of packets used. This number also
	 * reflects the number of used bits in the encoding vector.
	 */
	size_t Packets() const;

	/**
	 * Returns the nubmer of bytes needed to store an
	 * encoded packet.
	 */
	size_t PacketSize() const;

	/**
	 * Return the number of bytes needed in the decoding buffer.
	 */
	size_t DataBufferSize() const;

	/**
	 * Return the number of bytes needed by an encoding vector
	 */
	size_t VectorSize() const;

	/**
	 * Return the number of bytes needed by the encoding vector buffer
	 * This buffer contains all encoding vectors as they are being encoded.
	 */
	size_t VectorBufferSize() const;

	/**
	 * Returns whether the decoder is complete
	 */
	bool IsComplete() const;
	
	/**
	 * Adds pointers to the packet and encoding vector buffers.
	 * These buffers must remain valid throughout the decoding
	 * process.
	 */
	void AddBuffer(char *aPacket, char *aEncodingVector);

	/**
	 * Adds a buffer in which the decoded packets and
	 * encoding vectors will be placed during the decoding
	 * process
	 */

	void AddBuffer(char *aBuffer, char *aEncodingVectors, size_t aPacketCount);

	/**
	 * Adds a packet to be decoded
	 */
	void Decode(char *aPacket, char *aEncodingVector);

	/**
	 * Adds an uncoded packet
	 */
	void Decode(const char *aPacket, size_t aPacketNumber);

	/**
	 * Creates a recoded packet from the data currently
	 * in the decoding buffer according to the bits in the
	 * aEncodingVector. The length of the aEncodingVector should
	 * match the Rank() of the decoder and should be intialized by the calle.
	 * @todo the recode function should be revised atm. it may re-code packets
	 * which haven't not actually been received - since the length of the recoding
	 * vector can be very short. If e.g. the first packet in the generation was not
	 * received - but this is the only place where the recoding vector has a "1"
	 * the recoeded packet will be just 0's.
	 */
	void Recode(char *aPacket, char *aVector, char *aEncodingVector);
	
	/**
	 * Returns the rank of the decoder. The rank denotes for how many
	 * packets a pivot element have been found.
	 */
	size_t Rank() const;
	
    private:
	size_t iPackets;
	size_t iPacketSize;
	size_t iVectorSize;
	
	size_t iRank;
	size_t iMaxPivotId;
	
	std::vector<bool> iUncoded;
	std::vector<bool> iCoded;
	std::vector<size_t> iPivot;
	
	std::vector<char*> iPacketBuffer;
	std::vector<char*> iVectorBuffer;
    };
    
}


#endif
