/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _PEERMANAGER_H
#define _PERRMANAGER_H

#include <boost/noncopyable.hpp>

#include <vector>
#include <inttypes.h>

#include "peerconnection.hpp"
#include "serverconnection.hpp"

namespace bronco {
    class peermanager : private boost::noncopyable {
        public:
            peermanager();

            size_t count_peers()
            {
                return peers_.size();
            }

        private:
            std::vector<peerconnection::pointer> peers_;
            serverconnection::pointer server_ptr_;
    };
} // namespace bronco

#endif
