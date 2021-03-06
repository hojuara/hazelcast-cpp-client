﻿#
# Copyright (c) 2008-2020, Hazelcast, Inc. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
cmake_minimum_required (VERSION 3.10)
project (HazelcastExamples
        VERSION 4.0
        DESCRIPTION "Hazelcast C++ Client Code Examples"
        HOMEPAGE_URL https://hazelcast.org/clients/cplusplus/
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

INCLUDE(TestBigEndian)
INCLUDE(FindOpenSSL)
include(FindBoost)

# if HAZELCAST_INSTALL_DIR option is provided, the examples shall compile with the released library
if (NOT("${HAZELCAST_INSTALL_DIR}x" STREQUAL "x"))
    message (STATUS "Using hazelcast installation at ${HAZELCAST_INSTALL_DIR}.")

    if("${HZ_VERSION}x" STREQUAL "x")
        message(FATAL_ERROR "Release verification requires that the HZ_VERSION variable is provided and this folder is NOT included from a parent CMakeLists.txt.")
    endif ()

    if ("${HZ_BIT}x" STREQUAL "x")
        message(FATAL_ERROR "You have to specify HZ_BIT variable!")
    endif()

    IF(NOT ("${HZ_LIB_TYPE}" STREQUAL "STATIC" OR "${HZ_LIB_TYPE}" STREQUAL "SHARED") )
        message( STATUS "Build needs HZ_LIB_TYPE. Setting default as -DHZ_LIB_TYPE=STATIC (other option -DHZ_LIB_TYPE=SHARED)" )
        set(HZ_LIB_TYPE STATIC)
    ENDIF()

    IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        SET(HAZELCAST_PLATFORM_INSTALL_DIR ${HAZELCAST_INSTALL_DIR}\\${CMAKE_SYSTEM_NAME}_${HZ_BIT})
    ELSEIF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        SET(HAZELCAST_PLATFORM_INSTALL_DIR ${HAZELCAST_INSTALL_DIR}/Mac_${HZ_BIT})
    ELSE()
        SET(HAZELCAST_PLATFORM_INSTALL_DIR ${HAZELCAST_INSTALL_DIR}/${CMAKE_SYSTEM_NAME}_${HZ_BIT})
    ENDIF()

    SET(HAZELCAST_INCLUDE_DIRECTORIES ${HAZELCAST_INSTALL_DIR}/include)
    if (${HZ_COMPILE_WITH_SSL} MATCHES "ON")
        SET(HAZELCAST_LIB_DIRECTORY ${HAZELCAST_PLATFORM_INSTALL_DIR}/lib/tls)
    else()
        SET(HAZELCAST_LIB_DIRECTORY ${HAZELCAST_PLATFORM_INSTALL_DIR}/lib)
    endif()

    message(STATUS "Shall try building the examples with installed Hazelcast client at ${HAZELCAST_PLATFORM_INSTALL_DIR}")
    message(STATUS "Include directories: ${HAZELCAST_INCLUDE_DIRECTORIES}")
    message(STATUS "Link directory: ${HAZELCAST_LIB_DIRECTORY}")

    #detect endianness
    TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
    if (${IS_BIG_ENDIAN})
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DHZ_BIG_ENDIAN")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DHZ_BIG_ENDIAN")
    ENDIF (${IS_BIG_ENDIAN})

    include_directories(SYSTEM ${HAZELCAST_INCLUDE_DIRECTORIES})

    SET(HZ_LIB_NAME HazelcastClient${HZ_VERSION}_${HZ_BIT})

    ADD_LIBRARY(${HZ_LIB_NAME} ${HZ_LIB_TYPE} IMPORTED)

    message(STATUS "Operating System: ${CMAKE_SYSTEM}")

    if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        # This is a bug in CMake that causes it to prefer the system version over
        # the one in the specified ROOT folder. see
        # https://stackoverflow.com/questions/58446253/xcode-11-ld-error-your-binary-is-not-an-allowed-client-of-usr-lib-libcrypto-dy
        set(OPENSSL_ROOT_DIR /usr/local/Cellar/openssl@1.1/1.1.1h/)
        set(OPENSSL_INCLUDE_DIR ${OPENSSL_ROOT_DIR}/include CACHE FILEPATH "" FORCE)
        set(OPENSSL_CRYPTO_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libcrypto.dylib CACHE FILEPATH "" FORCE)
        set(OPENSSL_SSL_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libssl.dylib CACHE FILEPATH "" FORCE)
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        if (${CMAKE_BUILD_TYPE} MATCHES "Release")
            set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
            set(Boost_USE_MULTITHREADED      ON)
            set(Boost_USE_STATIC_RUNTIME     OFF)
            set(Boost_USE_DEBUG_RUNTIME      OFF)

            set(Boost_USE_DEBUG_LIBS         OFF)  # ignore debug libs and
            set(Boost_USE_RELEASE_LIBS       ON)   # only find release libs
        else()
            set(Boost_USE_DEBUG_LIBS         ON)   # only find debug libs
            set(Boost_USE_RELEASE_LIBS       OFF)  # ignore release libs
        endif()
        message(STATUS "Using Boost_USE_DEBUG_LIBS: ${Boost_USE_DEBUG_LIBS}")
        message(STATUS "Using Boost_USE_RELEASE_LIBS: ${Boost_USE_RELEASE_LIBS}")
    endif()
    find_package(Boost REQUIRED COMPONENTS thread chrono)
    include_directories(${Boost_INCLUDE_DIRS})
    link_libraries(Boost::thread Boost::chrono)
    add_definitions("-DBOOST_THREAD_VERSION=5")
    message(STATUS "Using Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Using Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")

    if (HZ_COMPILE_WITH_SSL)
        message(STATUS "Building the with SSL support.")
        ADD_DEFINITIONS(-DHZ_BUILD_WITH_SSL)
        find_package(OpenSSL REQUIRED)
        include_directories(${OPENSSL_INCLUDE_DIR})
        link_libraries(${OPENSSL_SSL_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARIES})
    ELSE ()
        message(STATUS "Building without SSL.")
    ENDIF()

    IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        message(STATUS "APPLE ENVIRONMENT DETECTED")

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall")

        set(HZ_LIB_PATH "")
        IF (${HZ_LIB_TYPE} MATCHES "STATIC")
            set(HZ_LIB_PATH "${HAZELCAST_LIB_DIRECTORY}/lib${HZ_LIB_NAME}.a")
        ELSE(${HZ_LIB_TYPE} MATCHES "STATIC")
            set(HZ_LIB_PATH "${HAZELCAST_LIB_DIRECTORY}/lib${HZ_LIB_NAME}.dylib")
        ENDIF(${HZ_LIB_TYPE} MATCHES "STATIC")

        message(STATUS "Using ${HZ_LIB_TYPE} library at ${HZ_LIB_PATH}")

        set_property(TARGET ${HZ_LIB_NAME} PROPERTY IMPORTED_LOCATION "${HZ_LIB_PATH}")

        link_libraries(${HZ_LIB_NAME})
    ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    IF( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
        message(STATUS "LINUX ENVIRONMENT DETECTED")

        message(STATUS "${HZ_BIT} Bit")

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall")

        set(HZ_LIB_PATH "")
        IF (${HZ_LIB_TYPE} MATCHES "STATIC")
            set(HZ_LIB_PATH "${HAZELCAST_LIB_DIRECTORY}/lib${HZ_LIB_NAME}.a")
        ELSE(${HZ_LIB_TYPE} MATCHES "STATIC")
            set(HZ_LIB_PATH "${HAZELCAST_LIB_DIRECTORY}/lib${HZ_LIB_NAME}.so")
        ENDIF(${HZ_LIB_TYPE} MATCHES "STATIC")

        message(STATUS "Using ${HZ_LIB_TYPE} library at ${HZ_LIB_PATH}")

        set_property(TARGET ${HZ_LIB_NAME} PROPERTY IMPORTED_LOCATION "${HZ_LIB_PATH}")

        link_libraries(${HZ_LIB_NAME} pthread rt )
    ENDIF( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )

    IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        message(STATUS "WINDOWS ENVIRONMENT DETECTED ${CMAKE_GENERATOR} ${CMAKE_BUILD_TYPE} ")

        add_definitions(-D__WIN${HZ_BIT}__ -DWIN${HZ_BIT} -D_WIN${HZ_BIT})

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /EHsc")

        add_definitions(-DHAZELCAST_EXPORTS=0)

        IF (${HZ_LIB_TYPE} MATCHES "STATIC")
            set(HZ_LIB_PATH "${HAZELCAST_LIB_DIRECTORY}\\static\\${HZ_LIB_NAME}.lib")
            set_property(TARGET ${HZ_LIB_NAME} PROPERTY IMPORTED_LOCATION ${HZ_LIB_PATH})

            message(STATUS "Using ${HZ_LIB_TYPE} library at ${HZ_LIB_PATH}")
        ELSE(${HZ_LIB_TYPE} MATCHES "STATIC")
            set(HZ_LIB_PATH "${HAZELCAST_LIB_DIRECTORY}\\shared\\${HZ_LIB_NAME}.dll")
            set(HZ_LIB_IMPLIB_PATH "${HAZELCAST_LIB_DIRECTORY}\\shared\\${HZ_LIB_NAME}.lib")

            set_property(TARGET ${HZ_LIB_NAME} PROPERTY IMPORTED_LOCATION ${HZ_LIB_PATH})
            set_property(TARGET ${HZ_LIB_NAME} PROPERTY IMPORTED_IMPLIB ${HZ_LIB_IMPLIB_PATH})

            message(STATUS "Using ${HZ_LIB_TYPE} library at ${HZ_LIB_PATH}")
        ENDIF(${HZ_LIB_TYPE} MATCHES "STATIC")

        link_libraries(${HZ_LIB_NAME})
    ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
endif (NOT("${HAZELCAST_INSTALL_DIR}x" STREQUAL "x"))

IF( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
    SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed -ldl")
ENDIF()

add_subdirectory(transactions)
add_subdirectory(spi)
add_subdirectory(serialization)
add_subdirectory(network-configuration)
add_subdirectory(monitoring)
add_subdirectory(learning-basics)
add_subdirectory(distributed-topic)
add_subdirectory(distributed-primitives)
add_subdirectory(distributed-map)
add_subdirectory(distributed-collections)
add_subdirectory(Org.Website.Samples)
add_subdirectory(replicated-map)
add_subdirectory(invocation-timeouts)
add_subdirectory(backpressure)
add_subdirectory(event-properties)
add_subdirectory(client-statistics)
add_subdirectory(pipeline)
add_subdirectory(authentication)
add_subdirectory(cp)
add_subdirectory(soak-test)
if (${HZ_COMPILE_WITH_SSL} MATCHES "ON")
    add_subdirectory(tls)
    add_subdirectory(aws)
endif()







