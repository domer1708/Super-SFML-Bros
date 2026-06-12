cmake_minimum_required(VERSION 3.15)

# Zmieniłem nazwę projektu, by pasowała do Twojego kodu
project(SuperSFMLBros VERSION 1.0 LANGUAGES CXX)

# Ustawienia standardu C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Znalezienie pakietu SFML (dodane 'audio'!)
find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)

# Dodanie pliku wykonywalnego i WSZYSTKICH plików .cpp z Twojego Visual Studio
add_executable(${PROJECT_NAME} 
    main.cpp
    Elements.cpp
    Enemy.cpp
    Entity.cpp
    Game.cpp
    Level.cpp
    MenuState.cpp
    PauseMenu.cpp
    PlayState.cpp
)

# Linkowanie bibliotek (poprawiony zapis na SFML::)
target_link_libraries(${PROJECT_NAME} PRIVATE 
    SFML::graphics 
    SFML::window 
    SFML::system
    SFML::audio
)

# Kopiowanie folderu z grafikami i dźwiękami do folderu z kompilacją
# Dzięki temu prowadzący nie będzie miał błędów z brakiem plików .png/.ogg
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/pliki DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Opcjonalne: Kopiowanie DLLek na Windowsie
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SFML::graphics>
        $<TARGET_FILE:SFML::window>
        $<TARGET_FILE:SFML::system>
        $<TARGET_FILE:SFML::audio>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()