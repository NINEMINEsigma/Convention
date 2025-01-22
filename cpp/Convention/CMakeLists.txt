message("Convention: --- ----- ----- ----- ----- --")

aux_source_directory    (./include cpps)
file(GLOB Header_List *.h)
file(GLOB Instance_Header_List ./instance/*.h)

include_directories(.)
add_library(Convention ${cpps} ${Header_List})

set_target_properties(Convention PROPERTIES PUBLIC_HEADER ${Header_List})

install(FILES ${Instance_Header_List}
        DESTINATION                         ${CMAKE_INSTALL_PREFIX}/include/instance)
install(TARGETS Convention 
        PUBLIC_HEADER DESTINATION           ${CMAKE_INSTALL_PREFIX}/include/
        RUNTIME DESTINATION                 ${CMAKE_INSTALL_PREFIX}/bin
        LIBRARY DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib/static)


message("Convention: ----- ----- ----- ----- -----")