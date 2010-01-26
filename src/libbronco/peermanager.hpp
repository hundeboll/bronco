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
            /**
             * Construct manager object to accept and create peer connections
             * \param io io_service to use
             */
            peermanager(boost::asio::io_service &io);

            /**
             * Count connected peers
             * \return counted value
             */
            size_t count_peers()
            {
                return in_peers_.size() + out_peers_.size();
            }

            /** Connect to specified peer
             * \param address IP-address or hostname of remote peer
             * \param port Port on remote peer
             */
            void connect_peer(const std::string &address, const size_t port);
            void connect_peer(const std::string &address, const std::string &port);

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

            /**
             * Clean up closed connections
             */
            size_t update_connections(std::vector<peerconnection::pointer> &peers);

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
