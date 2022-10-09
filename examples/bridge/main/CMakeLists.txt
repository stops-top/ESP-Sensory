idf_component_register(SRC_DIRS "." 
                         INCLUDE_DIRS "." 
                         LDFRAGMENTS "noflash.lf"
                         # EMBED_FILES "test.bin"
)

# spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)
