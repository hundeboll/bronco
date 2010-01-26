/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _PEERMANAGER_H
#define _PERRMANAGER_H

#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>
#include <inttypes.h>

#include "peerconnection.hpp"
#include "serverconnection.hpp"

namespace bronco {
    class peermanager : private boost::noncopyable, public boost::enable_shared_from_this<peermanager> {
        public:
            typedef boost::shared_ptr<peermanager> pointer;

            /**
             * Construct manager object to accept and create peer connections
             * \param io io_service to use
             */
            peermanager(boost::asio::io_service &io);

            /**
             * Wrapper to update connections
             */
            void update_connections()
            {
                update_connections(in_peers_);
                update_connections(out_peers_);
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
             * \param peers Vector with connections to update
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
