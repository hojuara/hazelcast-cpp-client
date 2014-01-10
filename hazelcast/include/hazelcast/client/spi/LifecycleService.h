//
// Created by sancar koyunlu on 6/17/13.
// Copyright (c) 2013 hazelcast. All rights reserved.



#ifndef HAZELCAST_LIFECYCLE_SERVICE
#define HAZELCAST_LIFECYCLE_SERVICE

#include "hazelcast/client/spi/LifecycleListener.h"
#include "hazelcast/client/spi/LifecycleEvent.h"
#include <boost/thread/mutex.hpp>
#include <boost/atomic.hpp>
#include <set>

namespace hazelcast {
    namespace client {
        namespace spi {

            class ClientContext;

            class HAZELCAST_API LifecycleService {
            public:
                LifecycleService(ClientContext& clientContext);

                ~LifecycleService();

                void start();

                void addLifecycleListener(LifecycleListener *lifecycleListener);

                bool removeLifecycleListener(LifecycleListener *lifecycleListener);

                bool isRunning();

                void setShutdown();

            private:
                ClientContext& clientContext;
                std::set<LifecycleListener *> listeners;
                boost::mutex listenerLock;
                boost::atomic<bool> active;

                void fireLifecycleEvent(LifecycleEvent lifecycleEvent);

            };

        }
    }
}


#endif //__LifecycleService_H_
