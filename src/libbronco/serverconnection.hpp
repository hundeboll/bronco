/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _SERVERCONNECTION_H
#define _SERVERCONNECTION_H

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <string>

#include "connection.hpp"
#include "messages.pb.h"

namespace bronco {
    class peermanager;
    class serverconnection : public connection {
        public:
            typedef boost::shared_ptr<serverconnection> pointer;

            /**
             * Create connection object to server
             * \param io io service to use in connection
             */
            serverconnection(boost::asio::io_service &io, peermanager *manager)
                : connection(io),
                manager_(manager)
            {}

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
             * Start communication with server to announce new file when connected
             * \param error Possible error occurring in connect operation
             * \param announce Announce message
             */
            void handle_announce(const boost::system::error_code &error, const protocol::Announce &announce);

            /**
             * Start communication with server to request more peers
             * \param error Possible error occurring in connect operation
             * \param announce Request message
             */
            void handle_request(const boost::system::error_code &error, const protocol::Request &request);

            /**
             * Start communication with server to join network when connected
             * \param error Possible error occurring in connect operation
             * \param me Information about the joining peer
             */
            void handle_connect(const boost::system::error_code &error, const protocol::Peer &me);

            /**
             * Tell server that we are leaving
             * \param srv_endpoint Resolved endpoint to server
             * \param message protocol::Leave object to be send
             */
            template<typename T>
            void send(boost::asio::ip::tcp::endpoint srv_endpoint, const T &message)
            {
                boost::system::error_code error;

                /* Connect to server and send message */
                socket().connect(srv_endpoint, error);
                if (!error) {
                    write_sync_message(message);
                    close_socket();
                } else if (error == boost::asio::error::connection_refused) {
                } else {
                    throw error;
                }
            }

        private:
            peermanager *manager_;

            /** Virtual function in connection called when async read operations completes
             * \param error Possivle error occurred in operation
             */
            void handle_read(const boost::system::error_code &error, const size_t type);

            /**
             * Virtual function in connection called when async write operations completes
             * \param error Possible error occurred in operation
             */
            void handle_write(const boost::system::error_code &error);

            /**
             * Checks if error is acceptable or throws exception
             * \param error Error occuring in async read and write operations
             */
            void handle_error(const boost::system::error_code &error);

            /**
             * Process read data based on message type
             * \param Message type read from header
             */
            void process_type(const size_t type);

            /* Message processing */
            void process_message(const protocol::Confirm &confirm);
            void process_message(const protocol::Config &config);
            void process_message(const protocol::Peers &peers);
    };
} // namespace bronco

#endif
