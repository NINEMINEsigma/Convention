add_executable(TEST main.cpp)
include_directories(../Convention)
target_link_libraries(TEST Convention)
install(TARGETS TEST 
        PUBLIC_HEADER DESTINATION           ${CMAKE_INSTALL_PREFIX}/include/
        RUNTIME DESTINATION                 ${CMAKE_INSTALL_PREFIX}/bin
        LIBRARY DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib/static)