#ifndef _UTIL_H_
#define _UTIL_H_

#include <string.h>
#include <sstream>

namespace bronco {
    class utils {
        /* Convert objects to std::string */
        public:
            template<typename T>
            static std::string to_string(const T &t)
            {
                std::stringstream ss;
                ss << t;
                return ss.str();
            }
    };
}
#endif
