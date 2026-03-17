target_sources(
    frs42 PRIVATE
    BrickLayout.cpp
    BrickLayout.h
    CheckPointDefinition.cpp
    CheckPointDefinition.h
    FloatingBrick.cpp
    FloatingBrick.h
    RoundSet.cpp
    RoundSet.h
)

target_include_directories(
    frs42 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
