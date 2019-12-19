add_library(tests_main tests_main.cpp)

add_executable(geometry_tests geometry_tests.cpp)
target_link_libraries(geometry_tests tests_main PathTracerLib)

add_test(geometry_tests geometry_tests)