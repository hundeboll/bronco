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
                : connection(io)
            {}

            /**
             * Create new connection object
             * \param io io_service to use
             */
            static pointer create(boost::asio::io_service &io)
            {
                return pointer(new peerconnection(io));
            }

            void handle_peer();
            void handle_read();
            void handle_connect(const boost::system::error_code &error);

        private:

    };
} // namespace bronco

#endif
