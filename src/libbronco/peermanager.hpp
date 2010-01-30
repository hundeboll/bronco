/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _PEERMANAGER_H
#define _PERRMANAGER_H

#define PRINTF (*bprint)

#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include <string>
#include <vector>
#include <inttypes.h>
#include <cstdio>
#include <cstdlib>

#include "peerconnection.hpp"
#include "serverconnection.hpp"

namespace bronco {
    class peermanager : private boost::noncopyable, public boost::enable_shared_from_this<peermanager> {
        public:
            typedef boost::shared_ptr<peermanager> pointer;
            int (*bprint)(const char *format, ...);

            /**
             * Construct manager object to accept and create peer connections
             * \param port Port to listen for peer connections on
             */
            peermanager(uint16_t port, int (*f)(const char *format, ...) = &printf);

            /**
             * Unlock mutex and stop threads before closing
             */
            ~peermanager()
            {
                stop_ = true;
                update_cond_.notify_all();
            }

            /**
             * Wrapper to io_service::run()
             */
            void run()
            {
                io_.run();
            }

            /**
             * Stop io services to close object
             */
            void close()
            {
                io_.stop();
            }

            void set_print(int (*f)(const char *format, ...))
            {
                bprint = f;
            }

            /**
             * Select random port
             * \return Selected port
             */
            static uint16_t select_port()
            {
                srand(time(0));
                return (rand() % 1024) + 49151;
            }

            /**
             * Wrapper to update connections
             */
            void update_connections()
            {
                update_cond_.notify_one();
            }

            /** Connect to specified peer
             * \param address IP-address or hostname of remote peer
             * \param port Port on remote peer
             */
            void connect_peer(const std::string &address, const std::string &port);

        private:
            /* Connection */
            uint16_t port_;
            std::vector<peerconnection::pointer> in_peers_, out_peers_;
            static boost::asio::io_service io_;
            boost::asio::ip::tcp::acceptor acceptor_;
            peerconnection::pointer out_conn_, in_conn_;
            serverconnection::pointer server_conn_;
            typedef boost::asio::ip::tcp::resolver::iterator endpoint_it;
            endpoint_it srv_endpoint_;
            protocol::Peer me_;

            /* Thread */
            bool stop_;
            typedef boost::mutex::scoped_lock scoped_lock;
            static boost::mutex update_mutex_;
            static boost::condition_variable update_cond_;

            /**
             * Clean up closed connections and create new if needed
             */
            void updater();

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
             * Connect to server to join network
             * \param address Address or hostname of server
             * \param port Port of server
             */
            void connect_server();

            /**
             * Connect to server and announce new file
             */
            void announce_server(const protocol::Announce &announce);

            /**
             * Connect to server and send leave
             */
            void leave_server();

            /**
             * Resolve address and port to endpoint iterator
             * \param address Address of host to resolve
             * \param port Port on host to connect
             * \return iterator to endpoint of host
             */
            boost::asio::ip::tcp::resolver::iterator resolve_host(const std::string &address, const std::string port)
            {
                /* Resolve IP and port to an endpoint */
                using boost::asio::ip::tcp;
                tcp::resolver resolver(io_);
                tcp::resolver::query query(address, port);

                return resolver.resolve(query);
            }
    };
} // namespace bronco

#endif
