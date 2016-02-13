project(test_cpp_rsc)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

include(cpp_resource)

add_resource(rc_test)
link_resource_file(rc_test FILE ${CMAKE_SOURCE_DIR}/data/1.png VARIABLE png1)
link_resource_file(rc_test FILE ${CMAKE_SOURCE_DIR}/data/1.txt VARIABLE txt1 TEXT)

get_property(RSC_CPP_FILE TARGET rc_test PROPERTY _AR_SRC_FILE)
get_property(RSC_H_DIR TARGET rc_test PROPERTY _AR_H_DIR)

include_directories(${RSC_H_DIR})

add_executable(rc_test_main rc_test_main.cpp ${RSC_CPP_FILE})