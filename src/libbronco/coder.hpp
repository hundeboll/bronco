/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _CODER_H
#define _CODER_H

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace bronco {
    class coder : private boost::noncopyable {
        public:
            typedef boost::shared_ptr<coder> pointer;
    };
} // namespace bronco

#endif
