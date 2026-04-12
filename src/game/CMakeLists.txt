set(NVE_OBJECT_TYPES_DEF ${CMAKE_CURRENT_SOURCE_DIR}/GameObjectTypes.def)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${NVE_OBJECT_TYPES_DEF})

file(STRINGS ${NVE_OBJECT_TYPES_DEF} NVE_OBJECT_TYPE_LINES
    REGEX "^NVE_REGISTER_TYPE\\([A-Za-z_][A-Za-z0-9_]*\\)$")

set(FRS42_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(NVE_GAME_OBJECT_REGISTRATION_CPP
    ${FRS42_GENERATED_DIR}/GameObjectRegistration.generated.cpp)
file(MAKE_DIRECTORY ${FRS42_GENERATED_DIR})

file(WRITE ${NVE_GAME_OBJECT_REGISTRATION_CPP} "#include \"GameObjectRegistration.h\"\n\n")

foreach(_line IN LISTS NVE_OBJECT_TYPE_LINES)
    string(REGEX REPLACE "^NVE_REGISTER_TYPE\\(([A-Za-z_][A-Za-z0-9_]*)\\)$" "\\1" _type ${_line})
    file(APPEND ${NVE_GAME_OBJECT_REGISTRATION_CPP} "#include \"${_type}.h\"\n")
endforeach()

file(APPEND ${NVE_GAME_OBJECT_REGISTRATION_CPP}
    "\n#include \"model/ObjectRegistry.h\"\n\nnamespace nuvelocity::frs42\n{\n    void RegisterGameObjectTypes()\n    {\n")

foreach(_line IN LISTS NVE_OBJECT_TYPE_LINES)
    string(REGEX REPLACE "^NVE_REGISTER_TYPE\\(([A-Za-z_][A-Za-z0-9_]*)\\)$" "\\1" _type ${_line})
    file(APPEND ${NVE_GAME_OBJECT_REGISTRATION_CPP}
        "        ObjectRegistry::Get().Register(${_type}::GetClassInfo());\n")
endforeach()

file(APPEND ${NVE_GAME_OBJECT_REGISTRATION_CPP}
    "    }\n} // namespace nuvelocity::frs42\n")
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
    BarrierBrick.cpp
    BarrierBrick.h
    MathUtils.cpp
    MathUtils.h
    GameBoard.cpp
    GameBoard.h
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
)

target_include_directories(
    frs42 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
