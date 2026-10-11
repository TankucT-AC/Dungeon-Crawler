cmake_minimum_required(VERSION 3.20)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(server_test CXX)
  find_package(Drogon CONFIG REQUIRED)
endif()

add_executable(server_test test_main.cc)

target_link_libraries(server_test PRIVATE Drogon::Drogon)

if(NOT COMMAND ParseAndAddDrogonTests AND DEFINED drogon_SOURCE_DIR)
  include("${drogon_SOURCE_DIR}/cmake/DrogonUtilities.cmake" OPTIONAL)
endif()
if(COMMAND ParseAndAddDrogonTests)
  ParseAndAddDrogonTests(server_test)
endif()
