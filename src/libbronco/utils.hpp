/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _UTIL_H_
#define _UTIL_H_

#include <boost/regex.hpp>

#include <string>
#include <sstream>
#include <cstdlib>
#include <openssl/sha.h>

namespace bronco {
    class utils {
        public:
            /**
             * Convert objects to std::string
             * \param t Object to convert to string
             * \return String made from object
             */
            template<typename T>
            static std::string to_string(const T &t)
            {
                std::stringstream ss;
                ss << t;
                return ss.str();
            }

            static std::string sha1(const char* data, const size_t length)
            {
                /* Compute sha1 sum */
                char *hash_ptr = new char[SHA_DIGEST_LENGTH];
                SHA1(reinterpret_cast<const unsigned char*>(data),
                        length,
                        reinterpret_cast<unsigned char*>(hash_ptr));

                /* Read hex values into string */
                char *hex_hash(new char[2*SHA_DIGEST_LENGTH + 1]);

                for (size_t i(0); i < SHA_DIGEST_LENGTH; ++i) {
                    snprintf(hex_hash+i*2, 3, "%02x", hash_ptr[i]);
                }

                return std::string(hex_hash);
            }

    };

}
#endif
