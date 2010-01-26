/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _PEERMANAGER_H
#define _PERRMANAGER_H

#include <boost/noncopyable.hpp>

#include <string>
#include <vector>
#include <inttypes.h>

#include "peerconnection.hpp"
#include "serverconnection.hpp"

namespace bronco {
    class peermanager : private boost::noncopyable {
        public:
            peermanager(boost::asio::io_service &io);

            /**
             * Determine if a new connection is accepted
             * \return True if more connections are allowed
             */
            bool allow_connection()
            {
                if (in_peers_.size() < me.in_conn_max()) {
                    /* New peer is welcome */
                    return true;
                } else {
                    /* Reject connection */
                    return false;
                }
            }

            /**
             * Count connected peers
             * \return counted value
             */
            size_t count_peers()
            {
                return in_peers_.size() + out_peers_.size();
            }

        private:
            size_t port_;
            std::vector<peerconnection::pointer> in_peers_, out_peers_;
            boost::asio::io_service &io_;
            boost::asio::ip::tcp::acceptor acceptor_;
            peerconnection::pointer out_conn_, in_conn_;
            serverconnection::pointer server_ptr_;
            protocol::Peer me;

            /**
             * Accept incoming connections
             * \param port Port to listen on
             */
            void listen();

            /**
             * Handle incoming connections
             * \param error Error passed by boost::bind in async_accept
             */
            void handle_incoming(const boost::system::error_code &error);

            /** Connect to specified peer
             * \param address IP-address or hostname of remote peer
             * \param port Port on remote peer
             */
            void connect_peer(const std::string &address, const size_t port);

            /**
             * Select random port
             * \return Selected port
             */
            size_t select_port()
            {
                srand(time(0));
                return (rand() % 1024) + 49151;
            }

    };
} // namespace bronco

#endif
