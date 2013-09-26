set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

include(cpp_resource)

add_resource(rc_test)
link_resource_file(rc_test FILE ${CMAKE_SOURCE_DIR}/data/1.png VARIABLE png1)
link_resource_file(rc_test FILE ${CMAKE_SOURCE_DIR}/data/1.txt VARIABLE txt1 TEXT)