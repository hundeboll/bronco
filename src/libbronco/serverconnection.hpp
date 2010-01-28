/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _SERVERCONNECTION_H
#define _SERVERCONNECTION_H

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <string>

#include "connection.hpp"
#include "messages.pb.h"

namespace bronco {
    class serverconnection : public connection {
        public:
            typedef boost::shared_ptr<serverconnection> pointer;

            /**
             * \brief Request more peers from server
             * \param num The number of peers  to reuqest
             */
            protocol::Peers & reuqest_peers(size_t num);

            /**
             * \brief Send keep alive message to server
             * \param peer_hash String identifying the peer
             */
            void keep_alive(std::string &peer_hash);

            /**
             * \brief Notify server about shutdown
             * \param peer_hash String identifying the leaving peer
             */
            void leave(std::string &peer_hash);

        private:
            void handle_read(const boost::system::error_code &error, const size_t type);
            void handle_write(const boost::system::error_code &error);
            void handle_error(const boost::system::error_code &error);

            /* Process read messages */
            void process_type(const size_t type);
            void process_message(const protocol::Config &config);
            void process_message(const protocol::Peers &peers);
    };
} // namespace bronco

#endif
