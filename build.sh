#!/bin/bash

# Скрипт для сборки проекта с выбором:
# Usage:
#   ./build.sh [static|shared] [test|notest]
#
# Параметры:
#   static  - сборка статической библиотеки (по умолчанию)
#   shared  - сборка динамической библиотеки
#   test    - сборка с тестами (по умолчанию без тестов)
#   notest  - сборка без тестов

# Значения по умолчанию
BUILD_TYPE="static"
BUILD_TESTS="notest"

# Парсим аргументы
for arg in "$@"
do
    case $arg in
        static)
            BUILD_TYPE="static"
            ;;
        shared)
            BUILD_TYPE="shared"
            ;;
        test)
            BUILD_TESTS="test"
            ;;
        notest)
            BUILD_TESTS="notest"
            ;;
        *)
            echo "Неизвестный параметр: $arg"
            echo "Использование: $0 [static|shared] [test|notest]"
            exit 1
            ;;
    esac
done

echo "Сборка типа: $BUILD_TYPE"
echo "Тесты: $BUILD_TESTS"

# Опции CMake в зависимости от ввода
CMAKE_OPTS=""

# Опция сборки динамической библиотеки
if [ "$BUILD_TYPE" == "shared" ]; then
    CMAKE_OPTS="$CMAKE_OPTS -DBUILD_SHARED_LIBS=ON"
else
    CMAKE_OPTS="$CMAKE_OPTS -DBUILD_SHARED_LIBS=OFF"
fi

# Опция сборки тестов
if [ "$BUILD_TESTS" == "test" ]; then
    CMAKE_OPTS="$CMAKE_OPTS -DTEST=ON"
else
    CMAKE_OPTS="$CMAKE_OPTS -DTEST=OFF"
fi

# Создаем отдельную папку для сборки, в зависимости от типа сборки и тестов
BUILD_DIR="build_${BUILD_TYPE}"
if [ "$BUILD_TESTS" == "test" ]; then
    BUILD_DIR="${BUILD_DIR}_tests"
fi

echo "Каталог сборки: $BUILD_DIR"

# Создаем каталог сборки
mkdir -p "$BUILD_DIR"

# Запускаем конфигурацию CMake
cmake -S . -B "$BUILD_DIR" $CMAKE_OPTS
if [ $? -ne 0 ]; then
    echo "Ошибка конфигурации CMake"
    exit 1
fi

# Стартуем сборку
cmake --build "$BUILD_DIR"
if [ $? -ne 0 ]; then
    echo "Ошибка сборки"
    exit 2
fi

# Если тесты включены - запускаем ctest
if [ "$BUILD_TESTS" == "test" ]; then
    echo "Запуск тестов"
    ctest --test-dir "$BUILD_DIR" --output-on-failure
    if [ $? -ne 0 ]; then
        echo "Тесты не пройдены"
        exit 3
    fi
fi

echo "Сборка успешно завершена!"
echo "Для установки библиотеки запустите: cmake --install $BUILD_DIR"
exit 0

