/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <iostream>

#include "peermanager.hpp"
#include "utils.hpp"

/* Initialize statics */
boost::mutex bronco::peermanager::update_mutex_;
boost::condition_variable bronco::peermanager::update_cond_;
boost::asio::io_service bronco::peermanager::io_;

bronco::peermanager::peermanager(const std::string &url,
        const peer_config *cfg,
        const nc_parameters *par,
        int (*f)(const char *format, ...))
    : print(f),
    parsed_url_(url),
    acceptor_(io_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), cfg->port)),
    stop_(false)
{
    print("Bronco peer listening on port %s\n", utils::to_string(cfg->port).c_str());

    print("Scheme: %s\n", parsed_url_.scheme().c_str());
    print("Host: %s\n", parsed_url_.host().c_str());
    print("Port: %s\n", parsed_url_.port().c_str());

    /* Setup configuration */
    me_.set_in_conn_max(cfg->max_peers_in);
    me_.set_out_conn_max(cfg->max_peers_out);
    me_.set_spare_peers(cfg->spare_peers);
    me_.set_port(utils::to_string(cfg->port));
    me_.set_peer_hash(me_.port());

    /* Open port for listening */
    listen();

    /* Watch connections and send keep-alives */
    boost::thread t1(boost::bind(&peermanager::updater, this));
    boost::thread t2(boost::bind(&peermanager::keepalive, this));

    /* Resolve server host */
    server_conn_.reset(new serverconnection(io_, this));
    srv_endpoint_ = resolve_host(parsed_url_.host(), parsed_url_.port());

    /* Start io_service */
    boost::thread t3(boost::bind(&peermanager::run, this));

    /* Start connecting */
    if (cfg) {
        config_.set_generation_size(par->generation_size);
        config_.set_packet_size(par->packet_size);
        announce_file(par->file_path);
    } else {
        me_.set_content_id(parsed_url_.content_id());
        connect_server();
    }
}

void bronco::peermanager::announce_file(const std::string &path)
{
    /* Load file */
    coder_ = new coder(path, config_);

    protocol::Announce announce;
    announce.mutable_peer()->CopyFrom(me_);
    announce.mutable_config()->CopyFrom(config_);

    /* Send announce to server */
    server_conn_->socket().async_connect(*srv_endpoint_,
            boost::bind(&serverconnection::handle_announce, server_conn_, boost::asio::placeholders::error, announce));
}

void bronco::peermanager::updater()
{
    while (!stop_)
    {
        /* Wait for closed connections */
        scoped_lock lock(update_mutex_);
        update_cond_.wait(lock);

        if (stop_)
            break;

        /* Clean up */
        update_connections(in_peers_);
        update_connections(out_peers_);

        /* Connect to more if needed */
        if (need_peers()) {
            print("Requesting %d peers from server\n", me_.spare_peers());

            /* Request peers in server connection */
            protocol::Request request;
            request.set_spare_peers(me_.spare_peers());
            request.set_peer_hash(me_.peer_hash());
            request.set_content_id(me_.content_id());

            server_conn_->socket().async_connect(*srv_endpoint_,
                    boost::bind(&serverconnection::handle_request, server_conn_, boost::asio::placeholders::error, request));
        }
    }
}

void bronco::peermanager::keepalive()
{
    protocol::Keepalive keepalive;

    while (!stop_)
    {
        /* Sleep three minutes */
        boost::this_thread::sleep(boost::posix_time::minutes(3));

        /* Send keep-alive to server */
        keepalive.set_peer_hash(me_.peer_hash());
        keepalive.set_content_id(me_.content_id());
        server_conn_->send(*srv_endpoint_, keepalive);
    }
}

void bronco::peermanager::listen()
{
    /* Initiate accepting loop */
    in_conn_ = peerconnection::create(io_, this);
    acceptor_.async_accept(in_conn_->socket(),
            boost::bind(&peermanager::handle_incoming, this, boost::asio::placeholders::error));
}

void bronco::peermanager::handle_incoming(const boost::system::error_code &error)
{
    /* Check for error from async_accept */
    if (!error) {
        /* Save connection to list */
        in_peers_.push_back(in_conn_);

        /* Handle control to connection */
        in_conn_->handle_peer(in_peers_.size() <= me_.in_conn_max());
    }

    /* Listen for next incoming connection */
    in_conn_ = peerconnection::create(io_, this);
    acceptor_.async_accept(in_conn_->socket(),
            boost::bind(&peermanager::handle_incoming, this, boost::asio::placeholders::error));
}

void bronco::peermanager::connect_peer(const protocol::Peer &peer)
{
    /* Check if we are connecting to self or already connected peers */
    if (peer_connected(peer.peer_hash())) {
        return;
    }

    print("Connecting to %s:%s\n", peer.address().c_str(), peer.port().c_str());
    endpoint_it peer_end(resolve_host(peer.address(), peer.port()));

    /* Open connection */
    out_conn_ = peerconnection::create(io_, this);
    out_conn_->socket().async_connect(*peer_end,
            boost::bind(&peerconnection::handle_connect, out_conn_, boost::asio::placeholders::error, peer));

    /* Save connection */
    out_peers_.push_back(out_conn_);
}

size_t bronco::peermanager::update_connections(peer_list &peers)
{
    /* Remove objects with closed sockets */
    typedef peer_list::iterator peer_it;
    for (peer_it it(peers.begin()), end(peers.end()); it != end; it++) {
        if (!(*it)->socket().is_open()) {
            std::cout << "Removing connection" << std::endl;
            peers.erase(it);
        }
    }

    /* Return new size */
    return peers.size();
}

void bronco::peermanager::connect_server()
{
    server_conn_->socket().async_connect(*srv_endpoint_,
            boost::bind(&serverconnection::handle_connect, server_conn_, boost::asio::placeholders::error, me_));
}
