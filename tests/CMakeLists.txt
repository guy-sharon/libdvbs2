add_executable(dvbs2_test_version test_version.c)
target_link_libraries(dvbs2_test_version PRIVATE dvbs2::dvbs2)
add_test(NAME dvbs2.version COMMAND dvbs2_test_version)
