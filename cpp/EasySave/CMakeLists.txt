message("EasySave: --- ----- ----- ----- ----- --")

include_directories(.)

BoostRefer()

aux_source_directory(./include cpps)
file(GLOB Root_Header_List *.h)

message("EasySave files: ${Root_Header_List}")

add_library(EasySave ${cpps} ${Root_Header_List})
include_directories(../Convention)
target_link_libraries(EasySave Convention)

set_target_properties(EasySave PROPERTIES PUBLIC_HEADER "${Root_Header_List}")

install(TARGETS EasySave 
        PUBLIC_HEADER DESTINATION           ${CMAKE_INSTALL_PREFIX}/include/
        RUNTIME DESTINATION                 ${CMAKE_INSTALL_PREFIX}/bin
        LIBRARY DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib/static)


message("EasySave: ----- ----- ----- ----- -----")