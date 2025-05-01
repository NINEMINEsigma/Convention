message("Convention Engine Kernel: --- ----- ----- ----- ----- --")

include_directories(./)

add_library(ConventionEngineKernel SHARED ./cpps/CoreModule.cpp)
target_link_libraries(ConventionEngineKernel Convention)

target_compile_definitions(ConventionEngineKernel 
    PUBLIC __NameMaxLength=256
)

set_target_properties(ConventionEngineKernel PROPERTIES PUBLIC_HEADER 
    ./CoreModule.h
)

install(TARGETS ConventionEngineKernel
        PUBLIC_HEADER DESTINATION           ${CMAKE_INSTALL_PREFIX}/include/Shared
        RUNTIME DESTINATION                 ${CMAKE_INSTALL_PREFIX}/bin
        LIBRARY DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION                 ${CMAKE_INSTALL_PREFIX}/lib/static)
        
message("Convention Engine Kernel: ----- ----- ----- ----- -----")