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
            /**
             * Validate and parse bronco url
             * \param url bronco url: bronco://<hostname>[:<port>]/<content_id>
             */
            parser(const std::string &url)
                : url_(url),
                url_regex_("(bronco):\\/\\/((\\w+\\.)*(\\w*))(:(\\d+)){0,1}\\/([\\w\\d]+\\/{0,1})+")
            {
                /* Validate URL */
                if (!boost::regex_match(url_.c_str(), pieces_, url_regex_))
                    throw std::runtime_error("Invalid URL");
            }

            /**
             * Read uri scheme name from url
             * \return Scheme name
             */
            std::string scheme() const
            {
                return std::string(pieces_[1].first, pieces_[1].second);
            }

            /**
             * Read hostname from url
             * \return Host name
             */
            std::string host() const
            {
                return std::string(pieces_[2].first, pieces_[2].second);
            }

            /**
             * Read port from url
             * \return port number from url (60100 if none given)
             */
            std::string port() const
            {
                if (pieces_[6].first < pieces_[6].second) {
                    /* Return found port */
                    return std::string(pieces_[6].first, pieces_[6].second);
                } else {
                    /* Return default port */
                    return std::string("60100");
                }
            }

            /**
             * Read content id hash from url
             * \return Content id given in url
             */
            std::string content_id() const
            {
                return std::string(pieces_[7].first, pieces_[7].second);
            }

        private:
            const std::string url_;
            boost::regex url_regex_;
            boost::cmatch pieces_;
    };
}
#endif
