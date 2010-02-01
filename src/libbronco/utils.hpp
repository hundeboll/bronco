/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _UTIL_H_
#define _UTIL_H_

#include <boost/regex.hpp>

#include <string>
#include <sstream>

namespace bronco {
    class utils {
        /**
         * Convert objects to std::string
         * \param t Object to convert to string
         * \return String made from object
         */
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
