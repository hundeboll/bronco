/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _PEERLIST_H
#define _PEERLIST_H

#include <boost/shared_ptr.hpp>

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

        private:
            protocol::Announce announce_;
            std::string list_hash_;

            /**
             * Create list identifier
             */
            void create_id();
    };
} // Namespace bronco

#endif
