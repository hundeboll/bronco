/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _ARBITRATOR_H
#define _ARBITRATOR_H

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "coder.hpp"

namespace bronco {
    class arbitrator : private boost::noncopyable {
        public:
            typedef boost::shared_ptr<arbitrator> pointer;

            arbitrator(const std::string &path);

        private:
            coder::pointer coder_ptr_;

    };
} // namespace bronco

#endif
