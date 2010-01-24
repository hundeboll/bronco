/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _CONNECTION_H
#define  _CONNECTION_H

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <iostream>

#include "messages.pb.h"

namespace bronco {
    class connection : private boost::noncopyable {
        public:
            /**
             * Virtual function used by read_type to handle type
             */
            virtual void handle_read() = 0;

            /**
             * Constructor to create socket and io_service
             * \param io The io_service to use
             */
            connection(boost::asio::io_service &io)
                : type_(type_header_size),
                length_(length_header_size),
                io_(io),
                socket_(io)
            {}

            /**
             * Return socket to use in caller classes
             */
            boost::asio::ip::tcp::socket& socket() {
                return socket_;
            }

            /**
             * Create headers and write data to the socket
             * \param message Message defined in protocol:: to be written
             */
            void write_message();

            /**
             * Receive message from socket and pass to appropriate handler
             * \param message Message defined in protocol:: to store read data in
             */
            void read_message(const boost::system::error_code &error);

        private:
            std::vector<char> type_, length_;
            boost::asio::io_service &io_;
            boost::asio::ip::tcp::socket socket_;

            /**
             * Create header containing size and type of payload
             * \param type Type of message (enum)
             * \param size Size of message
             * \param header Resulting header
             */
            void make_header(protocol::packettype type, size_t size, std::string &header);

            /**
             * Read size of next message from socket
             */
            void read_size(const boost::system::error_code &error);

        protected:
            enum { type_header_size = 4, length_header_size = 4 };
            /**
             * Read type of next message from socket
             */
            void read_header();
    };
} // namespace bronco;

#endif
