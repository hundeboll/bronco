/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _CODER_H
#define _CODER_H

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

#include <vector>
#include <inttypes.h>

#include "gf2/buffer_coders.h"
#include "gf2/encoder.h"
#include "gf2/gf2_common.h"

namespace bronco {
    class coder : private boost::noncopyable {
        public:
            typedef boost::shared_ptr<coder> pointer;

        private:
            /* Coding */
            uint32_t generation_;
            Gf2::RandomGenerator random_;
            std::vector<Gf2::Encoder> encoders_;
            std::vector<Gf2::Decoder> decoders_;
            boost::shared_array<char> vector_buf_na_, file_buf_na_;
            char *vector_buf_, *file_buf_;
    };
} // namespace bronco

#endif
