set(srcs "main.c"
	     "../SRC/connect_wifi.c"
         "../SRC/http_request.c"
         "../SRC/station_info.c")
         idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "." "../INC")

spiffs_create_partition_image(storage ../data FLASH_IN_PROJECT)
