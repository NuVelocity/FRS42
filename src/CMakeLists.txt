add_executable(
    frs42
    App.cpp
    App.h
)

add_subdirectory(scenes)
add_subdirectory(game)

target_compile_definitions(
    frs42 PUBLIC
    DEBUG
)

target_link_libraries(
    frs42 PRIVATE
    NuVelocity::NuVelocity
)

if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()