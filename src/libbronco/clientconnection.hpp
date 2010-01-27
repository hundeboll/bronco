/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>

#include "connection.hpp"
#include "messages.pb.h"

namespace bronco {
    class clientconnection : public connection, public boost::enable_shared_from_this<clientconnection> {
        public:
            typedef boost::shared_ptr<clientconnection> pointer;

            static pointer create(boost::asio::io_service &io)
            {
                return pointer(new clientconnection(io));
            }

        private:
            clientconnection(boost::asio::io_service &io)
                : connection(io)
            {}

            void handle_write(const boost::system::error_code &error);
            void handle_read(const boost::system::error_code &error, const size_t type);
            void handle_error(const boost::system::error_code &error);
    };
}
