target_sources(
    frs42 PRIVATE
    MainMenuScene.cpp
    MainMenuScene.h
    MainMenuButton.cpp
    MainMenuButton.h
    SplashScene.cpp
    SplashScene.h
)

target_include_directories(
    frs42 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
