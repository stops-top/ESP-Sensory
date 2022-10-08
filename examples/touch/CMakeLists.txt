# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(EXTRA_COMPONENT_DIRS  $ENV{IDF_PATH}/examples/common_components/led_strip
    ../../components
 )

#add_compile_options(-fdiagnostics-color=always -Wno-pointer-sign -Wno-format-zero-length -Wno-return-type -Wno-parentheses -Wno-implicit-function-declaration -Wno-switch)
add_compile_options(-fdiagnostics-color=always -w)

project(fingerTouch)
