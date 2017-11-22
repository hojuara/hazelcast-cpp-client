/*
 * Copyright (c) 2008-2017, Hazelcast, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hazelcast/client/MixedQueue.h"
#include "hazelcast/client/ItemListener.h"
#include "hazelcast/client/impl/ItemEventHandler.h"
#include "hazelcast/client/protocol/codec/QueueAddListenerCodec.h"

#include <boost/foreach.hpp>

namespace hazelcast {
    namespace client {
        std::string MixedQueue::addItemListener(MixedItemListener &listener, bool includeValue) {
            spi::ClusterService &cs = context->getClusterService();
            serialization::pimpl::SerializationService &ss = context->getSerializationService();
            impl::MixedItemEventHandler<protocol::codec::QueueAddListenerCodec::AbstractEventHandler> *itemEventHandler =
                    new impl::MixedItemEventHandler<protocol::codec::QueueAddListenerCodec::AbstractEventHandler>(
                            getName(), cs, ss, listener);
            return proxy::IQueueImpl::addItemListener(itemEventHandler, includeValue);
        }

        bool MixedQueue::removeItemListener(const std::string &registrationId) {
            return proxy::IQueueImpl::removeItemListener(registrationId);
        }

        int MixedQueue::remainingCapacity() {
            return proxy::IQueueImpl::remainingCapacity();
        }

        TypedData MixedQueue::take() {
            return poll(-1);
        }

        size_t MixedQueue::drainTo(std::vector<TypedData> &elements) {
            return drainTo(elements, -1);
        }

        size_t MixedQueue::drainTo(std::vector<TypedData> &elements, int64_t maxElements) {
            typedef std::vector<serialization::pimpl::Data> DATA_VECTOR;
            serialization::pimpl::SerializationService &serializationService = context->getSerializationService();
            size_t numElements = 0;
            BOOST_FOREACH(const DATA_VECTOR::value_type data , proxy::IQueueImpl::drainToData(maxElements)) {
                            elements.push_back(TypedData(std::auto_ptr<serialization::pimpl::Data>(
                                    new serialization::pimpl::Data(data)), serializationService));
                            ++numElements;
                        }
            return numElements;
        }

        TypedData MixedQueue::poll() {
            return poll(0);
        }

        TypedData MixedQueue::poll(long timeoutInMillis) {
            return TypedData(proxy::IQueueImpl::pollData(timeoutInMillis), context->getSerializationService());
        }

        TypedData MixedQueue::peek() {
            return TypedData(proxy::IQueueImpl::peekData(), context->getSerializationService());
        }

        int MixedQueue::size() {
            return proxy::IQueueImpl::size();
        }

        bool MixedQueue::isEmpty() {
            return size() == 0;
        }

        std::vector<TypedData> MixedQueue::toArray() {
            return toTypedDataCollection(proxy::IQueueImpl::toArrayData());
        }

        void MixedQueue::clear() {
            proxy::IQueueImpl::clear();
        }

        MixedQueue::MixedQueue(const std::string &instanceName, spi::ClientContext *context)
                : proxy::IQueueImpl(instanceName, context) {
        }

    }
}
