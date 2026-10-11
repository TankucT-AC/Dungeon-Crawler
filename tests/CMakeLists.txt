cmake_minimum_required(VERSION 3.20)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
)
FetchContent_MakeAvailable(googletest)

find_package(OpenSSL REQUIRED)
find_package(SFML 2.6 REQUIRED COMPONENTS system window graphics audio network)

add_executable(test_auth
    test_auth.cpp
)

target_include_directories(test_auth PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${OPENSSL_INCLUDE_DIR}
)
target_link_libraries(test_auth PRIVATE
    game_core
    Qt6::Core
    Qt6::Network
    gtest
    gtest_main
    gmock
    gmock_main
    sfml-graphics sfml-window sfml-system sfml-audio sfml-network
    OpenSSL::SSL OpenSSL::Crypto
    pthread
)
include(GoogleTest)
gtest_discover_tests(test_auth)

add_executable(test_app_options
    test_app_options.cpp
)

target_include_directories(test_app_options PRIVATE
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(test_app_options PRIVATE
    game_core
    gtest
    gtest_main
    pthread
)
gtest_discover_tests(test_app_options)
