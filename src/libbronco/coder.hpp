/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef _CODER_H
#define _CODER_H

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

#include <vector>
#include <inttypes.h>

#include "messages.pb.h"
#include "gf2/buffer_coders.h"
#include "gf2/encoder.h"
#include "gf2/gf2_common.h"

namespace bronco {
    class coder : private boost::noncopyable {
        public:
            typedef boost::shared_ptr<coder> pointer;

            /**
             * Constructor to setup encoder
             * \param path Path to file to encode
             */
            coder(const std::string &path, protocol::Config &config);

            /**
             * Constructor to setup decoder and recoder
             * \param config Configuration stores en protobuf message
             */
            coder(protocol::Config &config);

        private:
            /* Coding */
            enum { align_boundary = 16 };
            uint32_t generation_;
            Gf2::RandomGenerator random_;
            std::vector<Gf2::Encoder> encoders_;
            std::vector<Gf2::Decoder> decoders_;
            boost::shared_array<char> vector_buf_na_, file_buf_na_;
            char *vector_buf_, *file_buf_;

            protocol::Config config_;

            /**
             * Open file, read file size and read data
             * \param path File to open
             */
            void open_file(const std::string &path);

            /**
             * Find pointer to aligned address
             * \return Aligned pointer
             */
            char* find_aligned(char* na)
            {
                while ((reinterpret_cast<intptr_t>(na) % align_boundary) != 0)
                    ++na;

                return na;
            }


    };
} // namespace bronco

#endif
