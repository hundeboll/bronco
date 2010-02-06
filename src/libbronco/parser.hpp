/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _PARSER_H
#define _PARSER_H

namespace bronco {
    class parser {
        public:
            /**
             * Validate and parse bronco url
             * \param url bronco url: bronco://<hostname>[:<port>]/<content_id>
             */
            parser(const std::string &url)
                : url_(url),
                url_regex_("(bronco):\\/\\/((\\w+\\.)*(\\w*))(:(\\d+))?\\/([\\w\\d]+\\/?)?")
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

            /** Read port from url and convert to int
             * \return port as integer
             */
            uint16_t int_port() const
            {
                uint16_t port;
                std::istringstream p(std::string(pieces_[6].first, pieces_[6].second));
                p >> port;
                return port;
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
} // Namespace bronco

#endif
