#ifndef GF2_ENCODING_VECTOR_H
#define GF2_ENCODING_VECTOR_H

namespace Gf2
{
    class RandomGenerator;

    /**
     * An encoding vector object
     */
    class EncodingVector
    {
    public:

	/**
	 * Creates an encoding vector
	 */
	EncodingVector(char *aEncodingVector, int aPackets);
	
	/**
	 * Sets a specfic index in the encoding vector.
	 */
	void Set(int aIndex);
	
	/**
	 * Returns true if the aIndex element in the encoding vector is set
	 */
	bool IsSet(int aIndex) const;

	 /**
	 * Returns the needed length in bytes of an encoding vector describing
	 * aPackets
	 */
	static int Bytes(int aPackets);
	
    private:
	EncodingVector(const EncodingVector &aEncodingVector);
	EncodingVector& operator=(const EncodingVector &aEncodingVector);
	
    private:
	char *iVector;
	int iPackets;
    };


}



#endif
