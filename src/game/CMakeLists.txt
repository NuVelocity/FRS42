set(NVE_OBJECT_TYPES_DEF ${CMAKE_CURRENT_SOURCE_DIR}/GameObjectTypes.def)
set(FRS42_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(NVE_GAME_OBJECT_REGISTRATION_CPP ${FRS42_GENERATED_DIR}/GameObjectRegistration.generated.cpp)

file(MAKE_DIRECTORY ${FRS42_GENERATED_DIR})

add_custom_command(
    OUTPUT ${NVE_GAME_OBJECT_REGISTRATION_CPP}
    COMMAND ${CMAKE_COMMAND} 
        -D INPUT_DEF_FILE="${NVE_OBJECT_TYPES_DEF}"
        -D OUTPUT_CPP_FILE="${NVE_GAME_OBJECT_REGISTRATION_CPP}"
        -D HEADER_NAME="GameObjectRegistration.h"
        -D NAMESPACE_NAME="nuvelocity::frs42"
        -D FUNCTION_NAME="RegisterGameObjectTypes"
        -P "${PROJECT_SOURCE_DIR}/engine/cmake/GenerateObjectRegistration.cmake"
    DEPENDS ${NVE_OBJECT_TYPES_DEF} "${PROJECT_SOURCE_DIR}/engine/cmake/GenerateObjectRegistration.cmake"
    COMMENT "Generating GameObjectRegistration.generated.cpp from .def file"
)

add_custom_target(NuVelocityGameObjGen DEPENDS ${NVE_GAME_OBJECT_REGISTRATION_CPP})
add_dependencies(NuVelocity NuVelocityGameObjGen)
set_source_files_properties(${NVE_GAME_OBJECT_REGISTRATION_CPP} PROPERTIES GENERATED TRUE)

target_sources(
    frs42 PRIVATE
    BrickLayout.cpp
    BrickLayout.h
    BrickInfo.h
    BrickType.h
    Brick.cpp
    Brick.h
    Ball.cpp
    Ball.h
    PlayfieldBarrier.cpp
    PlayfieldBarrier.h
    MathUtils.cpp
    MathUtils.h
    Playfield.cpp
    Playfield.h
    Ship.cpp
    Ship.h
    CheckpointButton.cpp
    CheckpointButton.h
    DebugPrinter.cpp
    DebugPrinter.h
    GameObjectTypes.def
    ${NVE_GAME_OBJECT_REGISTRATION_CPP}
    CheckPointDefinition.cpp
    CheckPointDefinition.h
    FloatingBrick.cpp
    FloatingBrick.h
    GameObjectRegistration.h
    RoundSet.cpp
    RoundSet.h
    BackgroundDefinition.h
    MusicMixerData.h
    BackgroundSpriteGenerator.h
    BackgroundSpriteAI.h
    FishAI.h
    PlanktonAI.h
    RandomDebrisAI.h
    GameStats.cpp
    GameStats.h
    Difficulty.cpp
    Difficulty.h
    SuspendedGameStats.cpp
    SuspendedGameStats.h
    PlayerStats.cpp
    PlayerStats.h
    Stats.cpp
    Stats.h
    Projectile.cpp
    Projectile.h
    PowerUp.cpp
    PowerUp.h
    Bomb.cpp
    Bomb.h
    DisplayShip.cpp
    DisplayShip.h
    RoundSetManager.cpp
    RoundSetManager.h
    StatsManager.cpp
    StatsManager.h
)

target_include_directories(
    frs42 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
