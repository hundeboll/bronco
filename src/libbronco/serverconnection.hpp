/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _SERVERCONNECTION_H
#define _SERVERCONNECTION_H

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "connection.hpp"

namespace bronco {
    class serverconnection : public connection {
        public:
            typedef boost::shared_ptr<serverconnection> pointer;

    };
} // namespace bronco

#endif
