target_sources(
    frs42 PRIVATE
    LevelSelectWindow.cpp
    LevelSelectWindow.h
    OptionsWindow.cpp
    OptionsWindow.h
    PauseWindow.cpp
    PauseWindow.h
    NewGameOptionsWindow.cpp
    NewGameOptionsWindow.h
)

target_include_directories(
    frs42 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
