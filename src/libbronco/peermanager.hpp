/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <boost/shared_ptr.hpp>

#include <vector>
#include <inttypes.h>

#include "peerconnection.hpp"

namespace bronco {
    class peermanager {
        public:
            peermanager();

            uint16_t count_peers()
            {
                return 0;
            }

        private:
            std::vector<peerconnection::pointer> peers;
    };
} // namespace bronco
