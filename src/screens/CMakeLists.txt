target_sources(
    frs42 PRIVATE
    MainMenuScreen.cpp
    MainMenuScreen.h
    MainMenuButton.cpp
    MainMenuButton.h
    SplashScreen.cpp
    SplashScreen.h
    ArenaScreen.cpp
    ArenaScreen.h
    Megovision.cpp
    Megovision.h
)

add_subdirectory(windows)

target_include_directories(
    frs42 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
