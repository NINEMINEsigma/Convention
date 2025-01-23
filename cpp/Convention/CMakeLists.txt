message("Convention: --- ----- ----- ----- ----- --")

aux_source_directory    (./include cpps)
file(GLOB Convention_Header_List Convention/*.h)
file(GLOB Root_Header_List *.h)

message("Convention files: ${Convention_Header_List};${Root_Header_List}")

include_directories(.)
add_library(Convention ${cpps} ${Convention_Header_List} ${Root_Header_List})

set_target_properties(Convention PROPERTIES PUBLIC_HEADER "${Root_Header_List}")

install(FILES ${Convention_Header_List}
        DESTINATION                         ${CMAKE_INSTALL_PREFIX}/include/Convention)
install(TARGETS Convention 
        PUBLIC_HEADER DESTINATION           ${CMAKE_INSTALL_PREFIX}/include/
        RUNTIME DESTINATION                 ${CMAKE_INSTALL_PREFIX}/bin
        LIBRARY DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib/static)


message("Convention: ----- ----- ----- ----- -----")