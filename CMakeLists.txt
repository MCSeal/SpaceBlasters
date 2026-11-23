cmake_minimum_required(VERSION 3.16)
project(SpaceBlasters)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Svg SvgWidgets)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Entity.cpp
    src/Player.cpp
    src/Planet.cpp
    src/GameState.cpp
    src/GameView.cpp
)
target_link_libraries(${PROJECT_NAME} Qt6::Widgets Qt6::Svg Qt6::SvgWidgets)
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# tests. run with: ctest --test-dir build --output-on-failure
# skipped quietly if Qt Test is not installed so the game still builds
find_package(Qt6 QUIET COMPONENTS Test)
if(Qt6Test_FOUND)
    enable_testing()

    add_executable(test_gamestate tests/test_gamestate.cpp src/GameState.cpp)
    target_include_directories(test_gamestate PRIVATE src)
    target_link_libraries(test_gamestate Qt6::Test)
    add_test(NAME GameState COMMAND test_gamestate)

    add_executable(test_highscore tests/test_highscore.cpp)
    target_include_directories(test_highscore PRIVATE src)
    target_link_libraries(test_highscore Qt6::Test)
    add_test(NAME HighScore COMMAND test_highscore)

    # on windows the tests need the Qt dlls on the path to start from ctest
    if(WIN32 AND NOT CMAKE_VERSION VERSION_LESS 3.22)
        foreach(t GameState HighScore)
            set_tests_properties(${t} PROPERTIES ENVIRONMENT_MODIFICATION
                "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>")
        endforeach()
    endif()
endif()
