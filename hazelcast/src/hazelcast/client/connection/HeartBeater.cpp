//
// Created by sancar koyunlu on 20/08/14.
//

#include "hazelcast/client/spi/ClientContext.h"
#include "hazelcast/client/serialization/pimpl/Data.h"
#include "hazelcast/util/ThreadArgs.h"
#include "hazelcast/util/Util.h"
#include "hazelcast/client/spi/InvocationService.h"
#include "hazelcast/client/connection/ConnectionManager.h"
#include "hazelcast/client/impl/ClientPingRequest.h"
#include "hazelcast/client/connection/CallFuture.h"
#include "hazelcast/client/connection/Connection.h"
#include "hazelcast/client/ClientProperties.h"
#include "hazelcast/util/IOUtil.h"

namespace hazelcast {
    namespace client {
        namespace connection {


            HeartBeater::HeartBeater(spi::ClientContext& clientContext)
            : live(true)
            , clientContext(clientContext) {
                ClientProperties& properties = clientContext.getClientProperties();
                heartBeatTimeoutSeconds = properties.getHeartbeatTimeout().getInteger();
                heartBeatIntervalSeconds = properties.getHeartbeatInterval().getInteger();
                if (heartBeatTimeoutSeconds <= 0) {
                    heartBeatTimeoutSeconds = util::IOUtil::to_value<int>(PROP_HEARTBEAT_TIMEOUT_DEFAULT);
                }
                if (heartBeatIntervalSeconds <= 0) {
                    heartBeatIntervalSeconds = util::IOUtil::to_value<int>(PROP_HEARTBEAT_INTERVAL_DEFAULT);
                }
            }

            void HeartBeater::staticStart(util::ThreadArgs& args) {
                HeartBeater *heartBeater = (HeartBeater *)args.arg0;
                heartBeater->run(args.currentThread);
            }

            void HeartBeater::run(util::Thread *currentThread) {
                currentThread->interruptibleSleep(heartBeatIntervalSeconds);
                spi::InvocationService& invocationService = clientContext.getInvocationService();
                connection::ConnectionManager& connectionManager = clientContext.getConnectionManager();
                while (live) {

                    std::map<boost::shared_ptr<Connection>, CallFuture> futureMap;

                    std::vector<boost::shared_ptr<Connection> > connections = connectionManager.getConnections();
                    std::vector<boost::shared_ptr<Connection> >::iterator it;

                    time_t begin = time(NULL);
                    for (it = connections.begin(); it != connections.end(); ++it) {
                        boost::shared_ptr<Connection> connection = *it;
                        if (begin - connection->lastRead > heartBeatTimeoutSeconds) {
                            impl::ClientPingRequest *request = new impl::ClientPingRequest();
                            futureMap[(*it)] = invocationService.invokeOnConnection(request, connection);;
                        } else {
                            std::cout << " heart beat succedded 1 " << connection->getRemoteEndpoint()  << std::endl;
                            connection->heartBeatingSucceed();
                        }
                    }

                    std::map<boost::shared_ptr<Connection>, CallFuture>::iterator it2;
                    for (it2 = futureMap.begin(); it2 != futureMap.end(); ++it2) {
                        boost::shared_ptr<Connection> connection = it2->first;
                        CallFuture future = it2->second;
                        try {
                            time_t remaining = getRemainingTimeout(begin, heartBeatTimeoutSeconds);
                            time_t current = time(NULL);
                            future.get(remaining);
                            std::cout << " heart beat succedded 2 " << connection->getRemoteEndpoint()  << std::endl;
                            std::cout << " remaining " << remaining << " " << begin <<  " " << heartBeatTimeoutSeconds << " " << current << std::endl;
                            connection->heartBeatingSucceed();
                        } catch (exception::TimeoutException& e) {
                            std::cout << " heart beat failed  " << connection->getRemoteEndpoint()  << std::endl;
                            connection->heartBeatingFailed();
                        }
                    }
                    currentThread->interruptibleSleep(heartBeatIntervalSeconds);
                }
            }

            time_t HeartBeater::getRemainingTimeout(time_t begin, int heartBeatTimeoutSeconds) {
                long timeoutSeconds = heartBeatTimeoutSeconds - time(NULL) + begin;
                return timeoutSeconds < 0 ? 0 : timeoutSeconds;
            }

            void HeartBeater::shutdown() {
                live = false;
            }
        }
    }
}