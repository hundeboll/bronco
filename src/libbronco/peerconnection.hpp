/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _PEERCONNECTION_H
#define _PEERCONNECTION_H

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>

#include "connection.hpp"

namespace bronco {
    class peerconnection : public boost::enable_shared_from_this<peerconnection>, public connection {
        public:
            typedef boost::shared_ptr<peerconnection> pointer;

    };
} // namespace bronco

#endif
