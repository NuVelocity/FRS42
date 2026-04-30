target_sources(
    frs42 PRIVATE
    MainMenuScene.cpp
    MainMenuScene.h
    MainMenuButton.cpp
    MainMenuButton.h
    SplashScene.cpp
    SplashScene.h
    ArenaScene.cpp
    ArenaScene.h
    Megovision.cpp
    Megovision.h
)

add_subdirectory(windows)

target_include_directories(
    frs42 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
