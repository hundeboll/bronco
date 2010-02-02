#ifndef GF2_ENCODER_H
#define GF2_ENCODER_H

#include <vector>

namespace Gf2
{    
    /**
     * The encoder object is responsible for creating encoded data
     * from an specified data buffer. The encoder supports a certain
     * generation size i.e. number of packets to be encoded of an
     * arbitrary size.
     */
    class Encoder
    {
    public:
	/**
	 * Creates a new encoder supporting the specified
	 * number of packets (generation size) and packet size.
	 * @param aPackets number of packets 
	 * @param aPacketSize supported packet size in bytes
	 */
	Encoder(int aPackets, int aPacketSize);

	/**
	 * Returns the number of packets used. This number also
	 * reflects the number of used bits in the encoding vector.
	 */
	int Packets() const;

	/**
	 * Returns the nubmer of bytes needed to store an
	 * encoded packet.
	 */
	int PacketSize() const;

	/**
	 * Return the number of bytes needed by an encoding vector
	 */
	size_t VectorSize() const;

	/**
	 * Return the number of bytes encoded by this encoder
	 */
	int DataBufferSize() const;

	/**
	 * Adds a "orginal" packet to the encoder. Before the encoder
	 * can be used the encoder must be "full" meaning that exactly
	 * as many packets as the generation size specifies must be
	 * added.
	 */
	void AddBuffer(char *aPacket);

	/**
	 * Adds a buffer containing a number of "orginal" data packets.
	 */
	void AddBuffer(char* aBuffer, size_t aPacketCount);

	/**
	 * Gets a encoded packet.
	 * The two buffers passed point to valid memory area, and be
	 * able to contain PacketSize() and VectorSize() bytes
	 * respectively. The calle must first initialize the
	 * aEncodingVector buffer with Packets() bits.
	 */
	void Encode(char *aEncodedPacket, char *aEncodingVector);

	/**
	 * Returns a trivially/uncoded packet.
	 */
	void Encode(size_t aPacketId, char *aEncodedPacket);

    private:
	size_t iPackets;
	size_t iPacketSize;
	size_t iVectorSize;
	
	std::vector<char*> iPacketBuffer;
    };


}


#endif
