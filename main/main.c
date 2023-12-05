#include <stdio.h>
#include <stdlib.h>
#include <string.h> //Requires by memset
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <esp_http_server.h>
#include "nvs_flash.h"
#include "esp_spiffs.h"

#include "connect_wifi.h"
#include "http_request.h"
#include <esp_https_server.h>
#include "esp_tls.h"
#include "keep_alive.h"


static const char *TAG = "espressif"; // TAG for debug

void app_main()
{
    // Initialize NVS
    static httpd_handle_t server = NULL;
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    connect_wifi();
    if (wifi_connect_status)
    {
        ESP_LOGI(TAG, "SPIFFS Web Server is running ... ...\n");
        init_web_page_buffer();

        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

        setup_server();
    }
}