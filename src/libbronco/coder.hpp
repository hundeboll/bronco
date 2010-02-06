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
    class peermanager;
    class coder : private boost::noncopyable {
        public:
            /**
             * Constructor to setup encoder
             * \param path Path to file to encode
             */
            coder(protocol::Config &config, const std::string &path, peermanager *manager);

            /**
             * Constructor to setup decoder and recoder
             * \param config Configuration stores en protobuf message
             */
            coder(protocol::Config &config, peermanager *manager);

            ~coder()
            {
                if (file_buf_.is_open()) {
                    file_buf_.close();
                }
            }

            /**
             * Tell progress of download
             */
            uint16_t progress()
            {
                if (encoders_.size() != 0) {
                    return 100;
                }
            }

            protocol::Config get_config()
            {
                return config_;
            }

        private:
            /* Coding */
            enum { align_boundary = 16 };
            uint32_t generation_;
            Gf2::RandomGenerator random_;
            std::vector<Gf2::Encoder> encoders_;
            std::vector<Gf2::Decoder> decoders_;
            boost::shared_array<char> vector_buf_;
            boost::iostreams::mapped_file_source file_buf_;

            protocol::Config config_;
            peermanager *manager_;

            /**
             * Open file, read file size and read data
             * \param path File to open
             */
            void open_file(const std::string &path);

            void open_parts();
    };
} // namespace bronco

#endif
