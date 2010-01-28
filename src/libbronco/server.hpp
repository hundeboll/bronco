/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <boost/noncopyable.hpp>

#include <string>

#include "clientconnection.hpp"
#include "messages.pb.cc"

namespace bronco {
    class server : private boost::noncopyable {
        public:
            /**
             * Start server object
             * \param address Address of local interface to bind to
             * \param port TCP port to listen on
             */
            server(std::string &address, uint16_t port);

            /**
             * Tell io_service to listen for connections
             */
            void run()
            {
                io_.run();
            }

            /**
             * Stop io_service to close opbject
             */
            static void stop(int signo)
            {
                io_.stop();
            }

        private:
            std::string address_;
            uint16_t port_;
            static boost::asio::io_service io_;
            boost::asio::ip::tcp::acceptor acceptor_;
            clientconnection::pointer conn_;
            std::vector<clientconnection::pointer> conn_list_;

            /**
             * Recursive function called when clients connect
             * \param error Possible error occurred in accept operation
             */
            void handle_incoming(const boost::system::error_code &error);
    };
} // Namespace bronco

