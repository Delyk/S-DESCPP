cmake_minimum_required(VERSION 3.14)
project(sdes_project LANGUAGES CXX)

# Версия C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Опция для сборки тестов
option(TEST "Build with tests" OFF)

# Создаем библиотеку с возможностью компиляции как статической, так и динамической
# По умолчанию тип библиотеки выбирается через переменную BUILD_SHARED_LIBS (OFF)
add_library(sdes
    sdes/sdes.cpp
    sdes/sdes.h
)

# Устанавливаем include директорию для библиотеки
target_include_directories(sdes
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sdes>
        $<INSTALL_INTERFACE:include>
)

# Добавляем возможности установки библиотеки и заголовков
# Установка заголовков
install(DIRECTORY sdes/
    DESTINATION include/sdes
    FILES_MATCHING PATTERN "*.h"
)

# Установка библиотеки (статической или динамической)
install(TARGETS sdes
    EXPORT sdesTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Создаем и устанавливаем экспортируемые конфигурационные файлы (для find_package)
install(EXPORT sdesTargets
    FILE sdesTargets.cmake
    NAMESPACE sdes::
    DESTINATION lib/cmake/sdes
)

if(TEST)
    enable_testing()

    find_package(GTest QUIET)

    if (NOT GTest_FOUND)
        # Если не найден - скачать исходники googletest
        include(FetchContent)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
        )
        FetchContent_MakeAvailable(googletest)

        add_executable(test_sdes sdes/test.cpp)
        target_include_directories(test_sdes PRIVATE ${googletest_SOURCE_DIR}/googletest/include ${CMAKE_CURRENT_SOURCE_DIR}/sdes)
        target_link_libraries(test_sdes PRIVATE sdes gtest_main)
    else()
        add_executable(test_sdes sdes/test.cpp)
        target_include_directories(test_sdes PRIVATE ${GTEST_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/sdes)
        target_link_libraries(test_sdes PRIVATE sdes GTest::GTest GTest::Main)
    endif()

    add_test(NAME TestSDES COMMAND test_sdes)
endif()

