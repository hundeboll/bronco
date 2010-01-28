/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>

#include "connection.hpp"

namespace bronco {
    class clientconnection : public connection, public boost::enable_shared_from_this<clientconnection> {
        public:
            typedef boost::shared_ptr<clientconnection> pointer;

            /**
             * Create and return pointer to new connection object
             * \param io io service to use
             * \return Boost shared pointer to created object
             */
            static pointer create(boost::asio::io_service &io)
            {
                return pointer(new clientconnection(io));
            }

            /**
             * Initiate communication with connecting client
             */
            void handle_client();

        private:
            /**
             * Virtual function in connection called when async write operations completes
             * \param error Possible error occurred in operation
             */
            void handle_write(const boost::system::error_code &error);

            /** Virtual function in connection called when async read operations completes
             * \param error Possivle error occurred in operation
             */
            void handle_read(const boost::system::error_code &error, const size_t type);

            /**
             * Private constructor used by ::create
             * \param io io service to use in connection
             */
            clientconnection(boost::asio::io_service &io)
                : connection(io)
            {}

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
            void process_message(const protocol::Announce &announce);
            void process_message(const protocol::Peer &peer);
            void process_message(const protocol::Keepalive &keepalive);
            void process_message(const protocol::Leave &leave);
    };
}
