include_directories(../)
include_directories(${CMAKE_SOURCE_DIR}/Convention)

function(VA_single_target_execute type name)
    message(STATUS ${type}/${name}.cpp build-in VA)
    add_executable(${name} ${type}/${name}.cpp)
    target_link_libraries(${name} Convention)
    install(TARGETS ${name} 
            RUNTIME DESTINATION                 ${CMAKE_INSTALL_PREFIX}/bin/${type})
endfunction(VA_single_target_execute)

VA_single_target_execute(sort va-select)
VA_single_target_execute(sort va-counting)