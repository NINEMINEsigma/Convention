add_executable(CETest cli.cpp)

target_link_libraries(CETest PUBLIC Convention-Static)

install(TARGETS CETest 
        RUNTIME DESTINATION                 ${CMAKE_INSTALL_PREFIX}/bin)