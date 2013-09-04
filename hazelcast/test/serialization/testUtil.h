#ifndef TEST_UTIL
#define TEST_UTIL

#include "TestMainPortable.h"

#define SERVER_ADDRESS "localhost"
#define SERVER_PORT 5701


namespace hazelcast {
    namespace client {
        namespace test {

            TestMainPortable getTestMainPortable();
        }
    }
}

#endif