
BoostRefer()

add_executable(TEST main.cpp)
include_directories(${CMAKE_INSTALL_PREFIX}/include)
target_link_libraries(TEST Convention)
install(TARGETS TEST 
        RUNTIME DESTINATION                 ${CMAKE_INSTALL_PREFIX}/bin)

#add_executable(Music_Player music_player.cpp)
#target_link_libraries(Music_Player Convention)
#install(TARGETS Music_Player 
#        RUNTIME DESTINATION                 ${CMAKE_INSTALL_PREFIX}/bin)
#        
#add_executable(WebTest web_test.cpp)
#target_link_libraries(WebTest Convention)
#install(TARGETS WebTest 
#        RUNTIME DESTINATION                 ${CMAKE_INSTALL_PREFIX}/bin)