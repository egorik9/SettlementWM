cmake_minimum_required(VERSION 3.10)
project(SettlementWM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настройки компилятора
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2 -g")

# Поиск библиотек
find_package(PkgConfig REQUIRED)

# X11
find_package(X11 REQUIRED)

# Cairo
pkg_check_modules(CAIRO REQUIRED cairo)

# Xft
pkg_check_modules(XFT REQUIRED xft)

# Исходные файлы
set(SOURCES Settlement.cpp)

# Исполняемый файл
add_executable(SettlementWM ${SOURCES})

# Связывание библиотек
target_link_libraries(SettlementWM
        ${X11_LIBRARIES}
        ${CAIRO_LIBRARIES}
        ${XFT_LIBRARIES}
        pthread
)