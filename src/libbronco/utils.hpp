#ifndef _UTIL_H_
#define _UTIL_H_

#include <boost/regex.hpp>

#include <string>
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

    class parser {
        public:
            parser(const std::string &url)
                : url_(url),
                validator_("(bronco):\\/\\/(\\w+\\.)*(\\w*)\\/([\\w\\d]+\\/{0,1})+")
            {
                /* Validate URL */
                if (!boost::regex_match(url_.c_str(), pieces_, validator_))
                    throw std::runtime_error("Invalid URL");
            }

            const std::string scheme()
            {
                return std::string(pieces_[1].first, pieces_[1].second);
            }

        private:
            const std::string url_;
            boost::regex validator_;
            boost::cmatch pieces_;

    };
}
#endif
