/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _PEERMANAGER_H
#define _PERRMANAGER_H

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include <string>
#include <vector>
#include <inttypes.h>
#include <cstdio>
#include <cstdlib>

#include "peerconnection.hpp"
#include "serverconnection.hpp"
#include "parser.hpp"
#include "arbitrator.hpp"


namespace bronco {
    class peermanager : private boost::noncopyable {
        public:
            struct nc_parameters {
                std::string file_path;
                size_t generation_size;
                size_t packet_size;
            };

            struct peer_config {
                size_t max_peers_in;
                size_t max_peers_out;
                size_t spare_peers;
                std::string address;
                uint16_t port;
            };

            typedef boost::shared_ptr<peermanager> pointer;
            int (*print)(const char *format, ...);

            /**
             * Construct manager object to accept and create peer connections
             * \param port Port to listen for peer connections on
             */
            peermanager(const std::string &url,
                    const peer_config *cfg,
                    const nc_parameters *par = NULL,
                    int (*f)(const char *format, ...) = &printf);

            /**
             * Stop io services and unlock mutex to close object
             */
            void close()
            {
                /* Create leave message */
                protocol::Leave leave;
                leave.set_peer_hash(me_.peer_hash());
                leave.set_content_id(me_.content_id());
                server_conn_->send(*srv_endpoint_, leave);

                /* Now close down */
                acceptor_.close();
                io_.stop();
                stop_ = true;
                update_cond_.notify_all();
                io_thread.join();
            }

            /**
             * Set function to use when printing
             * \param f Function pointer with printf-like syntax
             */
            void set_print(int (*f)(const char *format, ...))
            {
                print = f;
            }

            /**
             * Save content id generated on server
             * \param id Received content id
             */
            void set_content_id(const std::string &id)
            {
                me_.set_content_id(id);
                print("URL: bronco://%s:%d/%s\n",
                        server_conn_->socket().remote_endpoint().address().to_string().c_str(),
                        server_conn_->socket().remote_endpoint().port(),
                        id.c_str());
            }

            /**
             * Save received configuration start arbitrator
             * \param config Configuration received from server
             */
            void set_config(const protocol::Config &config)
            {
                /* Save config */
                config_.CopyFrom(config);

                /* Start coder */
                arbitrator_ = new arbitrator(config_, this);
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

            /**
             * Check if more outgoing connections are wanted
             * \return True if more peers are wanted
             */
            bool need_peers()
            {
                return  out_peers_.size() < me_.out_conn_max();
            }

            /**
             * Provide info about self to peerconnections
             * \return protocol::Peer holding info about self
             */
            const protocol::Peer me()
            {
                return me_;
            }

            /** Connect to specified peer
             * \param address IP-address or hostname of remote peer
             * \param port Port on remote peer
             */
            void connect_peer(const protocol::Peer &peer);

        private:
            /* Connection */
            parser parsed_url_;
            uint16_t port_;
            typedef std::vector<peerconnection::pointer> peer_list;
            peer_list in_peers_, out_peers_;
            static boost::asio::io_service io_;
            boost::asio::ip::tcp::acceptor acceptor_;
            peerconnection::pointer out_conn_, in_conn_;
            serverconnection::pointer server_conn_;
            typedef boost::asio::ip::tcp::resolver::iterator endpoint_it;
            endpoint_it srv_endpoint_;

            /* Coding */
            protocol::Peer me_;
            protocol::Config config_;
            arbitrator *arbitrator_;

            /* Thread */
            bool stop_;
            typedef boost::mutex::scoped_lock scoped_lock;
            static boost::mutex update_mutex_;
            static boost::condition_variable update_cond_;
            boost::thread io_thread;

            /**
             * Clean up closed connections and create new if needed
             */
            void updater();

            /**
             * Send keep-alive messages to server every third minute
             */
            void keepalive();

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
            size_t update_connections(peer_list &peers);

            /**
             * Connect to server to join network
             * \param address Address or hostname of server
             * \param port Port of server
             */
            void connect_server();

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

            /**
             * Check if peer already is connected
             * \param hash Peer hash of peer to check
             * \return True if already connected
             */
            bool peer_connected(const std::string &peer)
            {
                /* Don't connect to self */
                if (peer == me_.peer_hash()) {
                    return true;
                }

                typedef peer_list::iterator list_it;

                /* Check ingoing connections */
                for (list_it it(in_peers_.begin()), end(in_peers_.end()); it != end; ++it)
                {
                    if ((*it)->peer_hash() == peer) {
                        /* Existing connection found */
                        return true;
                    }
                }

                /* Check outgoing connections */
                for (list_it it(out_peers_.begin()), end(out_peers_.end()); it != end; ++it)
                {
                    if ((*it)->peer_hash() == peer) {
                        /* Existing connection found */
                        return true;
                    }
                }

                return false;
            }

            /**
             * Wrapper to io_service::run()
             */
            void run()
            {
                io_.run();
            }

            /**
             * Load file and announce it to server
             * \param path Path to file for sharing
             */
            void announce_file(const std::string &path);
    };
} // namespace bronco

#endif
