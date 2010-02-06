/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <iostream>
#include <sstream>
#include <time.h>
#include <cstdlib>
#include <cstdio>
#include <stdexcept>
#include <sys/mman.h>

#include "coder.hpp"
#include "utils.hpp"
#include "peermanager.hpp"

bronco::coder::coder(protocol::Config &config, const std::string &path, peermanager *manager)
    : generation_(0),
    random_(rand),
    config_(config),
    manager_(manager)
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

bronco::coder::coder(protocol::Config &config, peermanager *manager)
    : generation_(0),
    random_(rand),
    config_(config),
    manager_(manager)
{
    /* Seed random generator */
    srand(time(0));

    /* Setup decoder */
    Gf2::CreateCoders(config_.file_size(),
            config_.generation_size(),
            config_.packet_size(),
            decoders_);

    open_parts();
}

void bronco::coder::open_file(const std::string &path)
{
    /* Open file */
    manager_->print("Opening %s\n", path.c_str());
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

void bronco::coder::open_parts()
{
    for (size_t i(0); i < decoders_.size(); ++i) {
        /* Create filenames */
        std::string fn(config_.file_name() + ".part");
        fn.append(utils::to_string(i));

        std::string vn(config_.file_name() + ".vector");
        vn.append(utils::to_string(i));

        /* Open files */
        boost::iostreams::mapped_file_params buffer_params(fn);
        buffer_params.new_file_size = decoders_[i].DataBufferSize();
        buffer_params.mode = BOOST_IOS::out;
        decoders_[i].buffer_file.open(buffer_params);
        if (!decoders_[i].buffer_file.is_open()) {
            throw std::runtime_error("Unable to open part");
        }

        boost::iostreams::mapped_file_params vector_params(vn);
        vector_params.new_file_size = decoders_[i].VectorBufferSize();
        vector_params.mode = BOOST_IOS::out;
        decoders_[i].vector_file.open(vector_params);
        if (!decoders_[i].vector_file.is_open()) {
            throw std::runtime_error("Unable to open vector");
        }

        /* Add buffers to decoder */
        decoders_[i].AddBuffer(
                decoders_[i].buffer_file.data(),
                decoders_[i].vector_file.data(),
                decoders_[i].Packets());
    }
}
