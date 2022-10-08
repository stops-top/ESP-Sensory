
idf_component_register(
    SRC_DIRS
        "icm42670"
        "touch_panel"
        "io_expander"
    INCLUDE_DIRS
        "icm42670/include"
        "touch_panel/include"
        "io_expander"
    REQUIRES
        bsp)

target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wno-unused-variable
    -Wno-unused-function
    -Wno-incompatible-pointer-types)
