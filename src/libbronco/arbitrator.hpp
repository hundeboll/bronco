/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _ARBITRATOR_H
#define _ARBITRATOR_H

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "coder.hpp"

namespace bronco {
    class peermanager;
    class arbitrator : private boost::noncopyable {
        public:
            typedef boost::shared_ptr<arbitrator> pointer;

            /**
             * Start arbitrator in encoder mode
             * \param config Contains parameters for encoder
             * \param path Path of file to encode
             */
            arbitrator(protocol::Config &config, const std::string &path, peermanager *manager)
                : coder_(config, path, manager),
                manager_(manager)
            {
                print_status();
            }

            /**
             * Start arbitrator in decoder mode
             * \param config Configuration received from server
             */
            arbitrator(protocol::Config &config, peermanager *manager)
                : coder_(config, manager),
                manager_(manager)
            {}

            /**
             * Return state of coder to caller
             * \return Percent of download complete
             */
            uint16_t progress()
            {
                return coder_.progress();
            }

            protocol::Config get_config()
            {
                return coder_.get_config();
            }

        private:
            coder coder_;
            peermanager *manager_;

            void print_status();

    };
} // namespace bronco

#endif
