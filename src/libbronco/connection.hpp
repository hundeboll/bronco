/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _CONNECTION_H
#define  _CONNECTION_H

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>

#include <iostream>

#include "messages.pb.h"

namespace bronco {
    class connection : private boost::noncopyable {
        public:
            /**
             * Virtual function used by read_type to handle type
             */
            virtual void handle_read(const boost::system::error_code &error, size_t type) = 0;
            virtual void handle_write(const boost::system::error_code &error) = 0;

            /**
             * Constructor to create socket and io_service
             * \param io The io_service to use
             */
            connection(boost::asio::io_service &io)
                : io_(io),
                socket_(io),
                type_(type_header_size),
                hex_length_(length_header_size)
            {
                GOOGLE_PROTOBUF_VERIFY_VERSION;
            }

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
            template<typename T>
            void write_message(T &message)
            {
                /* Serialize message */
                if (!message.SerializeToString(&out_message_)) {
                    throw std::runtime_error("Failed to serialize object");
                }

                /* Create header with type of message and size of serialized data */
                make_header(message.type(), out_message_.size());

                /* Transfer header and message */
                boost::asio::write(socket_, boost::asio::buffer(out_header_.str()));
                boost::asio::async_write(socket_,
                        boost::asio::buffer(out_message_),
                        boost::asio::transfer_all(),
                        boost::bind(&connection::handle_write, this, boost::asio::placeholders::error));
            }

            template<typename T>
            void deserialize(T &message)
            {
                /* Prepare streambuf for output */
                in_message_.commit(length_);

                /* Create istream by reference */
                std::istream is(&in_message_);

                /* Deserialize */
                if (!message.ParseFromIstream(&is)) {
                    throw std::runtime_error("Failed to deserialize object");
                }
            }

            /**
             * Receive message from socket and pass to appropriate handler
             * \param message Message defined in protocol:: to store read data in
             */
            void read_message(const boost::system::error_code &error);

        private:
            boost::asio::io_service &io_;
            boost::asio::ip::tcp::socket socket_;

            enum { type_header_size = 4, length_header_size = 4 };
            std::vector<char> type_, hex_length_;
            size_t length_;
            std::ostringstream out_header_;

            /**
             * Create header containing size and type of payload
             * \param type Type of message (enum)
             * \param size Size of message
             * \param header Resulting header
             */
            void make_header(protocol::packettype type, size_t size);

            /**
             * Read size of next message from socket
             */
            void read_length(const boost::system::error_code &error);

            /**
             * Convert header from hex to size_t
             * \param header char array containing header - must support members data() and size()
             */
            size_t convert_header(std::vector<char> &header);

            template<typename T>
            void serialize(T &message) {

            }

        protected:
            std::string out_message_;
            boost::asio::streambuf in_message_;
            /**
             * Read type of next message from socket
             */
            void read_type();
    };
} // namespace bronco;

#endif
