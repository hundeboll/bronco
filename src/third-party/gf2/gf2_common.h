#ifndef GF2_GF2_H
#define GF2_GF2_H

namespace Gf2
{
    /**
     * The random generator object is intialized using a ponter to the desired
     * random function the random function used should return a random signed
     * integer i.e. 31 bits.
     */
    class RandomGenerator
    {
    public:
	RandomGenerator( int (*aFunc)() );

	void Fill(char *aData, int aBytes) const;

    private:
	int (*iFunc)();
    };

    void XorBuffer(int *aA, const int *aB, int aLength);
    void XorBuffer(char *aA, const char *aB, int aLength);

}

#endif
