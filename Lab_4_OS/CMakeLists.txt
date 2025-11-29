cmake_minimum_required(VERSION 3.10)
project(Lab4_DynamicLibraries)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Директория для заголовочных файлов
include_directories(include)

# Директория для скомпилированных библиотек
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Динамические библиотеки для функции 1 (НОД)
add_library(gcd_euclid SHARED src/lib1_v1.c)
add_library(gcd_naive SHARED src/lib1_v2.c)
set_target_properties(gcd_euclid PROPERTIES PREFIX "" OUTPUT_NAME "libgcd_euclid")
set_target_properties(gcd_naive PROPERTIES PREFIX "" OUTPUT_NAME "libgcd_naive")

# Динамические библиотеки для функции 2 (Конвертация)
add_library(convert_bin SHARED src/lib2_v1.c)
add_library(convert_tri SHARED src/lib2_v2.c)
set_target_properties(convert_bin PROPERTIES PREFIX "" OUTPUT_NAME "libconvert_bin")
set_target_properties(convert_tri PROPERTIES PREFIX "" OUTPUT_NAME "libconvert_tri")

# Программа 1: Статическая линковка (линкуем одну из реализаций)
add_executable(prog1_static src/prog1_static.c)
target_link_libraries(prog1_static PRIVATE gcd_euclid convert_bin)

# Программа 2: Динамическая загрузка (только компиляция, без линковки библиотек)
add_executable(prog2_dynamic src/prog2_dynamic.c)
target_link_libraries(prog2_dynamic PRIVATE dl)

