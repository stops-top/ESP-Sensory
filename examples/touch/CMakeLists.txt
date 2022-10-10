# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(EXTRA_COMPONENT_DIRS  $ENV{IDF_PATH}/examples/common_components/led_strip
    ../../components
)

if(${IDF_VERSION_MAJOR} LESS 4 AND ${IDF_VERSION_MINOR} LESS 3)
    message(FATAL_ERROR "ESP-IDF v${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR} is not supported! "
        "Please use v4.3 or newer.")
endif()
#add_compile_options(-fdiagnostics-color=always -Wno-pointer-sign -Wno-format-zero-length -Wno-return-type -Wno-parentheses -Wno-implicit-function-declaration -Wno-switch)
add_compile_options(-fdiagnostics-color=always -w)

project(touch)
