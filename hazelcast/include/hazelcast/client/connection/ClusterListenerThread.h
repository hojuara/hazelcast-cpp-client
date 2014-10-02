//
// Created by sancar koyunlu on 5/23/13.
// Copyright (c) 2013 hazelcast. All rights reserved.

#ifndef HAZELCAST_CLUSTER_LISTENER_THREAD
#define HAZELCAST_CLUSTER_LISTENER_THREAD

#include "hazelcast/util/AtomicBoolean.h"
#include "hazelcast/client/Member.h"
#include "hazelcast/util/CountDownLatch.h"
#include "hazelcast/util/AtomicInt.h"
#include "hazelcast/util/Thread.h"
#include <boost/shared_ptr.hpp>

namespace hazelcast {
    namespace client {

        class Member;

        namespace spi {
            class ClientContext;
        }

        namespace impl {
            class ClientMembershipEvent;

            class MemberAttributeChange;
        }

        namespace connection {
            class Connection;

            class ConnectionManager;

            class HAZELCAST_API ClusterListenerThread {
            public:
                ClusterListenerThread(spi::ClientContext &clientContext);

                void setThread(util::Thread *);

                static void staticRun(util::ThreadArgs &args);

                void run(util::Thread *currentThread);

                void stop();

                std::vector<Address> getSocketAddresses();

                util::CountDownLatch startLatch;
                bool isStartedSuccessfully;
            private:
                spi::ClientContext &clientContext;
                boost::shared_ptr<Connection> conn;
                util::AtomicBoolean deletingConnection;
                std::vector<Member> members;

                std::auto_ptr<util::Thread> clusterListenerThread;

                void loadInitialMemberList();

                void listenMembershipEvents();

                void updateMembersRef();

                void fireMemberAttributeEvent(impl::MemberAttributeChange const &, Member &member);

                std::vector<Address> getClusterAddresses() const;

                std::vector<Address> getConfigAddresses() const;
            };
        }
    }
}

#endif //HAZELCAST_CLUSTER_LISTENER_THREAD

