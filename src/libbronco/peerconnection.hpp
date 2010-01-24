/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>

#include <connection.hpp>

namespace bronco {
    class peerconnection : public boost::enable_shared_from_this<peerconnection>, private boost::noncopyable {
        public:
            typedef boost::shared_ptr<peerconnection> pointer;

    };
} // namespace bronco
