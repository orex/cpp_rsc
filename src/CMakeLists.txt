find_package(Boost 1.46 REQUIRED)

add_executable(cpp_rsc cpp_rsc.cpp)

install(TARGETS cpp_rsc RUNTIME DESTINATION bin)