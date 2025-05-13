message("Convention Engine Runtime: --- ----- ----- ----- ----- --")

aux_source_directory(./include cpps)
add_library(ConventionEngineRuntime SHARED "${cpps}")

target_link_libraries(ConventionEngineRuntime PUBLIC Convention-Shared)

set_target_properties(ConventionEngineRuntime PROPERTIES PUBLIC_HEADER GlobalConfig.h)

install(TARGETS ConventionEngineRuntime
        PUBLIC_HEADER DESTINATION           ${CMAKE_INSTALL_PREFIX}/include/ConventionEngine/Runtime
        RUNTIME DESTINATION                 ${CMAKE_INSTALL_PREFIX}/bin
        LIBRARY DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib/static)
        
message("Convention Engine Runtime: ----- ----- ----- ----- -----")