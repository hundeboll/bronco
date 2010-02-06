/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <cstdlib>
#include <stdexcept>

#include "coder.hpp"
#include "utils.hpp"

bronco::coder::coder(protocol::Config &config, const std::string &path)
    : generation_(0),
    random_(rand),
    config_(config)
{
    /* Seed random generator */
    srand(time(0));

    /* Setup encoder */
    open_file(path);
    Gf2::CreateCoders(config_.file_size(),
            config_.generation_size(),
            config_.packet_size(),
            encoders_);
    config_.set_generation_count(encoders_.size());
}

bronco::coder::coder(protocol::Config &config)
    : generation_(0),
    random_(rand),
    config_(config)
{
    /* Seed random generator */
    srand(time(0));

    /* Setup decoder */
    Gf2::CreateCoders(config_.file_size(),
            config_.generation_size(),
            config_.packet_size(),
            decoders_);

    /* Find buffer sizes */
    size_t file_size = Gf2::SumFunction(decoders_, std::mem_fun_ref(&Gf2::Decoder::DataBufferSize));
    size_t vector_size = Gf2::SumFunction(decoders_, std::mem_fun_ref(&Gf2::Decoder::VectorBufferSize));

    /* Setup buffers */
    file_buf_.reset(new char[file_size]);
    vector_buf_.reset(new char[vector_size]);

    /* Add buffers */
    Gf2::AddBuffer(decoders_, file_buf_.get(), vector_buf_.get());
}

void bronco::coder::open_file(const std::string &path)
{
    /* Open file */
    std::ifstream infile(path.c_str(), std::ios::binary | std::ios::ate);
    if (!infile.is_open()) {
        throw std::runtime_error("Unable to open file");
    }

    /* Read and save file size */
    size_t file_size(infile.tellg());
    config_.set_file_size(file_size);

    /* Read file */
    infile.seekg(0, std::ios::beg);
    file_buf_.reset(new char[file_size]);
    infile.read(file_buf_.get(), file_size);

    /* Compute check sum */
    config_.set_file_hash(utils::sha1(file_buf_.get(), file_size));
}
