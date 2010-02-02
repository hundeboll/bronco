/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <iostream>
#include <fstream>
#include <time.h>
#include <cstdlib>
#include <stdexcept>

#include "coder.hpp"

bronco::coder::coder(const std::string &path, protocol::Config &config)
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
    file_buf_na_.reset(new char[file_size + align_boundary]);
    vector_buf_na_.reset(new char[vector_size + align_boundary]);

    /* Align buffers */
    file_buf_ = find_aligned(file_buf_na_.get());
    vector_buf_ = find_aligned(vector_buf_na_.get());

    /* Add buffers */
    Gf2::AddBuffer(decoders_, file_buf_, vector_buf_);
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
    file_buf_na_.reset(new char[file_size + align_boundary]);
    file_buf_ = find_aligned(file_buf_na_.get());
    infile.read(file_buf_, file_size);
}
