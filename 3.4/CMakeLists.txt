# Минимальная версия CMake
cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

# Имя проекта
project(test)

# Версия стандарта
# set(CMAKE_CXX_STANDART 17)

# Тип сборки
# set(CMAKE_BUILD_TYPE Release)

# Флаги компиляции
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
# add_definitions(-O2)

# Подкаталог содержит ещё один CMakeLists.txt

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D")

# Собираем все файлы
file(GLOB SOURCES ./src/*.cpp)

# Добавляем исполняемый файл main
# Он собирается из 1 файла исходного кода: main.cpp
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