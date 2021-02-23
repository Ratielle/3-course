# Минимальная версия CMake
cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

# Имя проекта
project(plotter)

# Собираем все файлы
file(GLOB SOURCES ./src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
set(EXECUTABLE_OUTPUT_PATH ../output)
#====================================================
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
include_directories("3rdparty")

set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML REQUIRED COMPONENTS window graphics system PATHS ./3rdparty)
include_directories("${SFML_DIR}/../../include")

set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_DIR ${CMAKE_CURRENT_LIST_DIR}/3rdparty/imgui)
add_subdirectory(./3rdparty/imgui-sfml)

target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system ImGui-SFML)