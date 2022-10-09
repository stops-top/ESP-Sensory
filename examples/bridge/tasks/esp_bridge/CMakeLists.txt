
set(src_dirs "src")
set(include_dirs "include")
set(priv_include_dirs "private" )

idf_component_register(SRC_DIRS "${src_dirs}"
                        INCLUDE_DIRS "${include_dirs}"
                        PRIV_INCLUDE_DIRS "${priv_include_dirs}" 
                        PRIV_REQUIRES nvs_flash spiffs esp_timer tinyusb esp-serial-flasher
)
 
if(${IDF_VERSION_MAJOR} EQUAL 4 AND ${IDF_VERSION_MINOR} EQUAL 3)
    # add_library("tinyusb") was used instead of idf_component_register so "__idf_" wasn't added to the name
    set(tinyusb_name "tinyusb")
else()
    set(tinyusb_name "__idf_tinyusb")
endif()

# Workaround for Vendor class. ESP-IDF v4.3 doesn't support it directly through its tinyusb "additions" layer.
target_compile_definitions(${tinyusb_name} PUBLIC CFG_TUD_VENDOR=1)
target_compile_definitions(${tinyusb_name} PUBLIC CFG_TUD_VENDOR_RX_BUFSIZE=64)
target_compile_definitions(${tinyusb_name} PUBLIC CFG_TUD_VENDOR_TX_BUFSIZE=64)
