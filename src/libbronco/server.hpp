/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _SERVER_H
#define _SERVER_H

#include <boost/noncopyable.hpp>

#include <string>
#include <vector>
#include <map>

#include "clientconnection.hpp"
#include "peerlist.hpp"

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

            /**
             * Create new peerlist
             * \param announce Announce message received from peer
             * \return Pointer to created peerlist
             */
            peerlist::pointer new_peerlist(const protocol::Config &config)
            {
                /* Create peerlist */
                peerlist::pointer pl(new peerlist(config));

                /* Save peerlist */
                peerlists_.insert(std::pair<std::string, peerlist::pointer>(pl->list_hash(), pl));

                return pl;
            }

            /**
             * Fetch existing peerlist
             * \param list_hash Hash identifying list
             * \return Pointer to list
             */
            peerlist::pointer get_peerlist(const std::string &list_hash)
            {
                /* Try to find list */
                std::map<std::string, peerlist::pointer>::iterator list_it(peerlists_.find(list_hash));

                /* Check if list is found */
                if (list_it != peerlists_.end()) {
                    return list_it->second;
                } else {
                    /* Return null pointer */
                    peerlist::pointer p;
                    return p;
                }
            }

        private:
            /* Members used in connection */
            std::string address_;
            uint16_t port_;
            static boost::asio::io_service io_;
            boost::asio::ip::tcp::acceptor acceptor_;
            clientconnection::pointer conn_;
            std::vector<clientconnection::pointer> conn_list_;

            /* Members used for file management / peer management */
            std::map<std::string, peerlist::pointer> peerlists_;

            /**
             * Recursive function called when clients connect
             * \param error Possible error occurred in accept operation
             */
            void handle_incoming(const boost::system::error_code &error);

            /**
             * Walk through peers in each list and remove timeouts
             */
            void clean_peers();
    };
} // Namespace bronco

#endif
