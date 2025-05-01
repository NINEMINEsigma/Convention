message("Convention: --- ----- ----- ----- ----- --")

BoostRefer()

file(GLOB Root_Headers *.h)
message("Root_Headers: ${Root_Headers}")
aux_source_directory(./include  cpps)
file(GLOB Instance_Headers instance/*.h)
message("Instance_Headers: ${Instance_Headers}")


add_library(Convention-Static   STATIC ${cpps})
add_library(Convention-Shared   SHARED ${cpps})

install(FILES ${Root_Headers}
        DESTINATION                         ${CMAKE_INSTALL_PREFIX}/include/Convention)
install(FILES ${Instance_Headers}
        DESTINATION                         ${CMAKE_INSTALL_PREFIX}/include/Convention/instance)
install(TARGETS Convention-Static
        RUNTIME DESTINATION                 ${CMAKE_INSTALL_PREFIX}/bin
        LIBRARY DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib/static)
install(TARGETS Convention-Shared
        RUNTIME DESTINATION                 ${CMAKE_INSTALL_PREFIX}/bin
        LIBRARY DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib/static)

message("Convention: ----- ----- ----- ----- -----")