/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _PEERLIST_H
#define _PEERLIST_H

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include <cstdlib>
#include <iostream>

#include "messages.pb.h"

namespace bronco {
    class peerlist {
        public:
            typedef boost::shared_ptr<peerlist> pointer;

            peerlist( const protocol::Announce &announce);

            /**
             * Get list identifier
             * \return Identifier string
             */
            std::string list_hash()
            {
                return list_hash_;
            }

            /**
             * Assign new peer to list
             * \param peer The peer to assign
             */
            void assign(const protocol::Peer &peer) {
                scoped_lock lock(list_mutex_);

                list_entry entry(peer.peer_hash(), list_peer(peer, time(0)));
                peers_.insert(entry);

                std::cout << "Inserted peer " << peer.peer_hash() << " into list " << list_hash_ << std::endl;
            }

            /**
             * Remove specified peer from list
             */
            void assign(const std::string &peer_hash)
            {
                peers_.erase(peer_hash);
            }

            /**
             * Select a subset of peers from the list
             * \param no The number of peers to select
             * \return List of peers
             */
            protocol::Peers get_peers(const size_t no);

            /**
             * Select a random subset of peers from the list
             * \param no The number of peers to select
             * \return List of random peers
             */
            protocol::Peers get_random_peers(const size_t no);

        private:
            protocol::Announce announce_;
            std::string list_hash_;

            typedef std::pair<protocol::Peer, uint64_t> list_peer;
            typedef std::map<std::string, list_peer> peer_list;
            typedef std::pair<std::string, list_peer> list_entry;
            peer_list peers_;

            boost::mutex list_mutex_;
            typedef boost::mutex::scoped_lock scoped_lock;

            /**
             * Create list identifier
             */
            void create_id();
    };
} // Namespace bronco

#endif