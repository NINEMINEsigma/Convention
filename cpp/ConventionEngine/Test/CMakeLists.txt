include_directories(${CMAKE_INSTALL_PREFIX}/include)

add_executable(CETest cli.cpp)

target_link_libraries(CETest Convention)
install(TARGETS CETest 
        RUNTIME DESTINATION                 ${CMAKE_INSTALL_PREFIX}/bin)