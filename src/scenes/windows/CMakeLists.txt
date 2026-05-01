target_sources(
    frs42 PRIVATE
    ConfirmWindow.cpp
    ConfirmWindow.h
    LevelSelectWindow.cpp
    LevelSelectWindow.h
    OptionsWindow.cpp
    OptionsWindow.h
    PauseWindow.cpp
    PauseWindow.h
    NewGameOptionsWindow.cpp
    NewGameOptionsWindow.h
    NewPlayerWindow.cpp
    NewPlayerWindow.h
    SelectPlayerWindow.cpp
    SelectPlayerWindow.h
    StatisticsWindow.cpp
    StatisticsWindow.h
)

target_include_directories(
    frs42 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
