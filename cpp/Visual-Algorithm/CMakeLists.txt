message("Visual-Algorithm: --- ----- ----- ----- ----- --")

include_directories(../Convention)
function(VA_target_execute name)
    message(STATUS ${name}.cpp build-in VA)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} Convention)
    install(TARGETS ${name} 
            RUNTIME DESTINATION                 ${CMAKE_INSTALL_PREFIX}/bin)
endfunction(VA_target_execute)

VA_target_execute(va-main)
VA_target_execute(va-test)

add_subdirectory("Singles")

message("Visual-Algorithm: ----- ----- ----- ----- -----")