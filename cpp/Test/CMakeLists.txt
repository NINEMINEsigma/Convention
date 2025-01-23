add_executable(TEST main.cpp)
include_directories(../Convention)
target_link_libraries(TEST Convention)
install(TARGETS TEST 
        PUBLIC_HEADER DESTINATION           ${CMAKE_INSTALL_PREFIX}/include/
        RUNTIME DESTINATION                 ${CMAKE_INSTALL_PREFIX}/bin
        LIBRARY DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib/static)

add_executable(Music_Player music_player.cpp)
target_link_libraries(Music_Player Convention)
install(TARGETS Music_Player 
        PUBLIC_HEADER DESTINATION           ${CMAKE_INSTALL_PREFIX}/include/
        RUNTIME DESTINATION                 ${CMAKE_INSTALL_PREFIX}/bin
        LIBRARY DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib/static)