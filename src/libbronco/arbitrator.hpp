/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _ARBITRATOR_H
#define _ARBITRATOR_H

#include <boost/noncopyable.hpp>

#include "coder.hpp"

namespace bronco {
    class arbitrator : private boost::noncopyable {
        public:
            arbitrator();

        private:
            coder::pointer coder_ptr_;

    };
} // namespace bronco

#endif
