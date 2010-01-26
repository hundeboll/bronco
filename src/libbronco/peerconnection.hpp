/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _PEERCONNECTION_H
#define _PEERCONNECTION_H

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>

#include <vector>

#include "connection.hpp"


namespace bronco {
    class peerconnection : public connection, public boost::enable_shared_from_this<peerconnection> {
        public:
            typedef boost::shared_ptr<peerconnection> pointer;

            /**
             * Constructor to construct base class
             * \param io The io_service to use
             */
            peerconnection(boost::asio::io_service &io)
                : connection(io),
                accept_(true),
                connected_(false),
                started_(false)
            {}

            /**
             * Create new connection object
             * \param io io_service to use
             */
            static pointer create(boost::asio::io_service &io)
            {
                return pointer(new peerconnection(io));
            }

            /* Handle new connections */
            void handle_peer(const bool accept);
            void handle_connect(const boost::system::error_code &error);

            /* Handle read and write completions */
            void handle_write(const boost::system::error_code &error);
            void handle_read(const boost::system::error_code &error, const size_t type);
            void handle_error(const boost::system::error_code &error);

            /* Methods to process received messages */
            void process_type(const size_t type);
            void process_message(const protocol::Connect &connect);
            void process_message(const protocol::Reply &reply);
            void process_message(const protocol::Start &start);
            void process_message(const protocol::Data &data);
            void process_message(const protocol::Stop &stop);
            void process_message(const protocol::Leave &leave);

        private:
            /* States */
            bool accept_, connected_, started_;
    };
} // namespace bronco

#endif
