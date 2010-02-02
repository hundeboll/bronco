#ifndef MT19937AR_H
#define MT19937AR_H

/**
 * Init functions
 */
void init_genrand(unsigned long s);
void init_by_array(unsigned long init_key[], int key_length);

/**
 * Random functions
 */
unsigned long genrand_int32(void);
int genrand_int31(void);
double genrand_real1(void);
double genrand_real2(void);
double genrand_real3(void);
double genrand_res53(void) ;




#endif
