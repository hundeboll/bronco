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

            size_t count_peers()
            {
                return peers_.size();
            }

        private:
            size_t port_;
            std::vector<peerconnection::pointer> peers_;
            serverconnection::pointer server_ptr_;
            boost::asio::io_service &io_;
            boost::asio::ip::tcp::acceptor acceptor_;
            peerconnection::pointer out_conn_, in_conn_;

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
             */
            size_t select_port()
            {
                srand(time(0));
                return (rand() % 1024) + 49151;
            }

    };
} // namespace bronco

#endif
