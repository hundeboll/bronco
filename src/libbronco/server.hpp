/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <boost/noncopyable.hpp>

#include <string>

#include "clientconnection.hpp"
#include "messages.pb.cc"

namespace bronco {
    class server : private boost::noncopyable {
        public:
            server(std::string &address, uint16_t port);

        private:
            std::string address_;
            uint16_t port_;
            boost::asio::io_service io_;
            boost::asio::ip::tcp::acceptor acceptor_;
            clientconnection::pointer conn_;

            void handle_incoming(const boost::system::error_code &error);
    };
} // Namespace bronco

