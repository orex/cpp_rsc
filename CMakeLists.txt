cmake_minimum_required (VERSION 2.8)

project(cpp_rsc)

find_package(Boost 1.46 COMPONENTS unit_test_framework)

if(Boost_UNIT_TEST_FRAMEWORK_FOUND)
  enable_testing()
else()
  message(STATUS "No Boost::Test found. All tests have been disabled.")
endif()  

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(src)

if(Boost_UNIT_TEST_FRAMEWORK_FOUND)
  add_subdirectory(tests)
endif()

#add_subdirectory(cmake_test)

