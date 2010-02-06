/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include "peerlist.hpp"

bronco::peerlist::peerlist(const protocol::Config &config)
    : config_(config)
{
    /* Initialize random generator used to select peers */
    srand(time(0));

    /* Create list id */
    create_id();
}

void bronco::peerlist::create_id()
{
    list_hash_ = config_.file_hash();
}

protocol::Peers bronco::peerlist::get_peers(const size_t no)
{
    scoped_lock lock(list_mutex_);

    if (peers_.size() <= no) {
        /* We have less peers than requested */
        protocol::Peers peers;
        typedef peer_list::iterator it;

        /* Copy peers from member list to return list */
        for (it beg (peers_.begin()), end(peers_.end()); (size_t)peers.peers_size() <= no && beg != end; ++beg) {
            protocol::Peer* peer = peers.add_peers();
            peer->CopyFrom(beg->second.first);
        }

        return peers;
    } else {
        /* We have more peers than requested */
        return get_random_peers(no);
    }

}

protocol::Peers bronco::peerlist::get_random_peers(const size_t no)
{
    size_t max = peers_.size();
    typedef peer_list::const_iterator const_it;

    /* Selecting random peers from peers_ */
    protocol::Peers peers;
    std::map<std::string, const_it> selected;
    while (selected.size() < no)
    {
        const_it beg(peers_.begin());
        std::advance(beg, rand() % max);
        selected.insert(std::pair<std::string, const_it>(beg->first, beg));
    }

    /* Copy selected peers to return list */
    typedef std::map<std::string, const_it>::iterator it;
    for (it beg(selected.begin()), end(selected.end()); beg != end; ++beg)
    {
        protocol::Peer* one_peer = peers.add_peers();
        one_peer->CopyFrom(beg->second->second.first);
    }

    return peers;
}
