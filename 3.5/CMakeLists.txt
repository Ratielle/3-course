# Минимальная версия CMake
cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

# Имя проекта
project(plotter)

# Собираем все файлы
file(GLOB SOURCES ./src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
set(EXECUTABLE_OUTPUT_PATH ../output)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MT")
#====================================================
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
include_directories("3rdparty")

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_USE_STATIC_STD_LIBS TRUE)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(./3rdparty/SFML)

set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_DIR ${CMAKE_CURRENT_LIST_DIR}/3rdparty/imgui)
add_subdirectory(./3rdparty/imgui-sfml)

target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system ImGui-SFML)