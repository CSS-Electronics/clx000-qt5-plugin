cmake_minimum_required(VERSION 3.15)
project(clx000canbus)

set(CMAKE_VERBOSE_MAKEFILE ON)

enable_language(CXX)

set(RELEASE_NAME clx000canbus)

# Find required Qt dependencies.
set(REQUIRED_QT5_COMPONENTS
    Core
    SerialBus
    SerialPort
    )

find_package(Qt5 COMPONENTS ${REQUIRED_QT5_COMPONENTS} REQUIRED)

# All core source and header files, sorted alphabetically.
set(SOURCE_FILES
    CLX000CanBus.cpp
    CLX000CanBusUSB.cpp
    CLX000Plugin.cpp
    CRC16.cpp
    )

set(HEADERS_FILES
    CLX000CanBus.h
    CLX000CanBusUSB.h
    CLX000Plugin.h
    CRC16.h
    )

set(OTHER_FILES
    plugin.json
    )

add_library(${RELEASE_NAME} MODULE ${SOURCE_FILES} ${HEADERS_FILES} ${OTHER_FILES})

# Enable and configure automatic moc generation.
set_property(TARGET ${RELEASE_NAME} PROPERTY AUTOMOC ON)

# CXX standard configuration.
set_property(TARGET ${RELEASE_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${RELEASE_NAME} PROPERTY CXX_STANDARD_REQUIRED YES)
set_property(TARGET ${RELEASE_NAME} PROPERTY CXX_EXTENSIONS NO)

# General Qt configuration.
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_compile_definitions(${RELEASE_NAME} QT_DEBUG)
else()
    add_compile_definitions(${RELEASE_NAME} QT_NO_DEBUG)
    add_compile_definitions(${RELEASE_NAME} QT_NO_DEBUG_OUTPUT)
endif()

# Link against all required Qt modules.
foreach(QT_COMPONENT ${REQUIRED_QT5_COMPONENTS})
    target_link_libraries(${RELEASE_NAME} "Qt5::${QT_COMPONENT}")
endforeach()

add_custom_command(TARGET ${RELEASE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${RELEASE_NAME}> ${CMAKE_BINARY_DIR}/plugins/canbus
    )
