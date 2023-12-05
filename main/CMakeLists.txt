set(srcs "main.c"
	     "../SRC/connect_wifi.c"
         "../SRC/http_request.c"
         "../SRC/station_info.c"
         "../SRC/esp_https_server.c"
         "../SRC/keep_alive.c")
idf_component_register(SRCS ${srcs}
                        INCLUDE_DIRS "." "../INC"
                        EMBED_TXTFILES "../certs/servercert.crt"
                        EMBED_TXTFILES "../certs/prvtkey.crt")

 
spiffs_create_partition_image(storage ../data FLASH_IN_PROJECT)
