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
#include "http_request.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "esp_log.h"

#include <esp_https_server.h>
#include "esp_tls.h"

char index_html[4096];
char mainPage_html[10240];
char hwConf_html[20480];
char specFunc_html[25600];
char upload_html[3072];
char modelConf_html[27648];
char sinapsiConf_html[11264];
char style_css[3072];
char response_data[27648];
char favicon_ico[14336];

static const char *TAG = "HTTP"; // TAG for debug

struct async_resp_arg {
    httpd_handle_t hd;
    int fd;
};


/*
*********************************** SCAME ************************************
**                                                                          **
**                    POST and GET URI Definition                           **
**                                                                          **
******************************************************************************
*/
/* GET root*/
httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = get_req_handler,
    .user_ctx = NULL};

/* POST login*/
static const httpd_uri_t uri_login = {
    .uri       = "/login",
    .method    = HTTP_POST,
    .handler   = login_post_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = "Hello World!"
};

/* POST hwConf*/
static const httpd_uri_t uri_hwConf = {
    .uri       = "/hwConf",
    .method    = HTTP_POST,
    .handler   = hw_conf_post_handler,
    .user_ctx  = NULL
};

/* POST specFunc*/
static const httpd_uri_t uri_mainPage = {
    .uri       = "/mainPage",
    .method    = HTTP_GET,
    .handler   = main_page_get_handler,
    .user_ctx  = NULL
};

/* POST specFunc*/
static const httpd_uri_t uri_specFunc = {
    .uri       = "/specFunc",
    .method    = HTTP_POST,
    .handler   = spec_func_post_handler,
    .user_ctx  = NULL
};

/* POST upload*/
static const httpd_uri_t uri_upload = {
    .uri       = "/upload",
    .method    = HTTP_POST,
    .handler   = upload_post_handler,
    .user_ctx  = NULL
};

/* POST upload*/
static const httpd_uri_t uri_modelConf = {
    .uri       = "/modelConf",
    .method    = HTTP_POST,
    .handler   = model_conf_post_handler,
    .user_ctx  = NULL
};

/* POST Sinapsi*/
static const httpd_uri_t uri_sinapsiConf = {
    .uri       = "/sinapsiConf",
    .method    = HTTP_POST,
    .handler   = sinapsi_conf_post_handler,
    .user_ctx  = NULL
};

/* GET style .css*/
static const httpd_uri_t uri_styleCss = {
    .uri       = "/style.css",
    .method    = HTTP_GET,
    .handler   = style_get_handler,
    .user_ctx  = NULL
};

/* GET logo SCAME*/
static const httpd_uri_t uri_favicon = {
    .uri       = "/favicon.ico",
    .method    = HTTP_GET,
    .handler   = favicon_get_handler,
    .user_ctx  = NULL
};

/* POST Info SN*/
static const httpd_uri_t uri_setSerials = {
    .uri       = "/setSerials",
    .method    = HTTP_POST,
    .handler   = info_sn_post_handler,
    .user_ctx  = NULL
};

/* GET SCU serial number*/
static const httpd_uri_t uri_scuSerial = {
    .uri       = "/scuSerial",
    .method    = HTTP_GET,
    .handler   = scu_serial_get_handler,
    .user_ctx  = NULL
};

/* POST Info SN*/
static const httpd_uri_t uri_setHwParams = {
    .uri       = "/setHwParams",
    .method    = HTTP_POST,
    .handler   = set_hw_params_post_handler,
    .user_ctx  = NULL
};

/* GET SCU hardware parameters */
static const httpd_uri_t uri_getHwParams = {
    .uri       = "/getHwParams",
    .method    = HTTP_GET,
    .handler   = hw_params_get_handler,
    .user_ctx  = NULL
};

/*
*********************************** SCAME ************************************
**                                                                          **
**                            Page SPIFFS definition                        **
**                                                                          **
******************************************************************************
*/

/**
  * @brief  function to init SPIFFS and show login web page
  * @param  
  * @retval None
  */
void init_web_page_buffer(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true};

    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));

    memset((void *)index_html, 0, sizeof(index_html));
    struct stat st;
    if (stat(INDEX_HTML_PATH, &st))
    {
        ESP_LOGE(TAG, "index.html not found");
        return;
    }

    FILE *fp = fopen(INDEX_HTML_PATH, "r");
    if (fread(index_html, st.st_size, 1, fp) == 0)
    {
        ESP_LOGE(TAG, "fread failed");
    }
    fclose(fp);
}

/**
  * @brief  function to create and show main web page (after user login)
  * @param  
  * @retval None
  */
void init_main_page_buffer(void)
{
    memset(response_data, 0x00, sizeof(response_data));
    memset(mainPage_html, 0x00, sizeof(mainPage_html));
    struct stat st;
    if (stat(MAIN_PAGE_HTML_PATH, &st))
    {
        ESP_LOGE(TAG, "mainPage.html not found");
        return;
    }

    FILE *fp = fopen(MAIN_PAGE_HTML_PATH, "r");
    if (fread(mainPage_html, st.st_size, 1, fp) == 0)
    {
        ESP_LOGE(TAG, "fread failed");
    }
    ESP_LOGI(TAG, "mainPage.html read correctly\n");
    fclose(fp);
}

/**
  * @brief  function to create and show hardware config web page
  * @param  
  * @retval None
  */
void init_hw_conf_page_buffer(void)
{
    memset(response_data, 0x00, sizeof(response_data));
    memset(hwConf_html, 0x00, sizeof(hwConf_html));
    struct stat st;
    if (stat(HWCONF_PAGE_HTML_PATH, &st))
    {
        ESP_LOGE(TAG, "hwConf.html not found");
        return;
    }

    FILE *fp = fopen(HWCONF_PAGE_HTML_PATH, "r");
    if (fread(hwConf_html, st.st_size, 1, fp) == 0)
    {
        ESP_LOGE(TAG, "fread failed");
    }
    ESP_LOGI(TAG, "hwConf.html read correctly\n");
    fclose(fp);
}

/**
  * @brief  function to create and show special function web page
  * @param  
  * @retval None
  */
void init_spec_func_page_buffer(void)
{
    memset(response_data, 0x00, sizeof(response_data));
    memset(specFunc_html, 0x00, sizeof(specFunc_html));
    struct stat st;
    if (stat(SPECFUNC_PAGE_HTML_PATH, &st))
    {
        ESP_LOGE(TAG, "specFunc.html not found");
        return;
    }

    FILE *fp = fopen(SPECFUNC_PAGE_HTML_PATH, "r");
    if (fread(specFunc_html, st.st_size, 1, fp) == 0)
    {
        ESP_LOGE(TAG, "fread failed");
    }
    ESP_LOGI(TAG, "specFunc.html read correctly\n");
    fclose(fp);
}

/**
  * @brief  function to create and show upload firmware web page
  * @param  
  * @retval None
  */
void init_upload_page_buffer(void)
{
    memset(response_data, 0x00, sizeof(response_data));
    memset(upload_html, 0x00, sizeof(upload_html));
    struct stat st;
    if (stat(UPLOAD_PAGE_HTML_PATH, &st))
    {
        ESP_LOGE(TAG, "upload.html not found");
        return;
    }

    FILE *fp = fopen(UPLOAD_PAGE_HTML_PATH, "r");
    if (fread(upload_html, st.st_size, 1, fp) == 0)
    {
        ESP_LOGE(TAG, "fread failed");
    }
    ESP_LOGI(TAG, "upload.html read correctly\n");
    fclose(fp);
}

/**
  * @brief  function to create and show model configuration web page
  * @param  
  * @retval None
  */
void init_model_conf_page_buffer(void)
{
    memset(response_data, 0x00, sizeof(response_data));
    memset(modelConf_html, 0x00, sizeof(modelConf_html));
    struct stat st;
    if (stat(MODELCONF_PAGE_HTML_PATH, &st))
    {
        ESP_LOGE(TAG, "modelConf.html not found");
        return;
    }

    FILE *fp = fopen(MODELCONF_PAGE_HTML_PATH, "r");
    if (fread(modelConf_html, st.st_size, 1, fp) == 0)
    {
        ESP_LOGE(TAG, "fread failed");
    }
    ESP_LOGI(TAG, "modelConf.html read correctly\n");
    fclose(fp);
}

/**
  * @brief  function to create and show sinapsi configuration web page
  * @param  
  * @retval None
  */
void init_sinapsi_conf_page_buffer(void)
{
    memset(response_data, 0x00, sizeof(response_data));
    memset(sinapsiConf_html, 0x00, sizeof(sinapsiConf_html));
    struct stat st;
    if (stat(SINAPSICONF_PAGE_HTML_PATH, &st))
    {
        ESP_LOGE(TAG, "sinapsiConf.html not found");
        return;
    }

    FILE *fp = fopen(SINAPSICONF_PAGE_HTML_PATH, "r");
    if (fread(sinapsiConf_html, st.st_size, 1, fp) == 0)
    {
        ESP_LOGE(TAG, "fread failed");
    }
    ESP_LOGI(TAG, "sinapsiConf.html read correctly\n");
    fclose(fp);
}

/**
  * @brief  function to create and show style.css
  * @param  
  * @retval None
  */
void init_style_page_buffer(void)
{
    memset(response_data, 0x00, sizeof(response_data));
    memset(style_css, 0x00, sizeof(style_css));
    struct stat st;
    if (stat(STYLE_PAGE_CSS_PATH, &st))
    {
        ESP_LOGE(TAG, "style.css not found");
        return;
    }

    FILE *fp = fopen(STYLE_PAGE_CSS_PATH, "r");
    if (fread(style_css, st.st_size, 1, fp) == 0)
    {
        ESP_LOGE(TAG, "fread failed");
    }
    ESP_LOGI(TAG, "style.css read correctly\n");
    fclose(fp);
}

/**
  * @brief  function to create and show Scame logo
  * @param  
  * @retval None
  */
void init_logo_scame_buffer(void)
{
    memset(response_data, 0x00, sizeof(response_data));
    memset(favicon_ico, 0x00, sizeof(favicon_ico));
    struct stat st;
    if (stat(SCAME_LOGO_PATH, &st))
    {
        ESP_LOGE(TAG, "favicon.ico not found");
        return;
    }

    FILE *fp = fopen(SCAME_LOGO_PATH, "r");
    if (fread(favicon_ico, st.st_size, 1, fp) == 0)
    {
        ESP_LOGE(TAG, "fread failed");
    }
    ESP_LOGI(TAG, "favicon.ico read correctly\n");
    fclose(fp);
}

/**
  * @brief  function to send login web page
  * @param  req: HTTP request received from GET/POST method
  * @retval result of operation
  */
esp_err_t send_web_page(httpd_req_t *req)
{
    memset(response_data, 0x00, sizeof(response_data));
    int response;
    sprintf(response_data, index_html, "OK");
    response = httpd_resp_send(req, response_data, HTTPD_RESP_USE_STRLEN);
    return response;
}

/**
  * @brief  function to send main web page
  * @param  req: HTTP request received from GET/POST method
  * @retval result of operation
  */
esp_err_t send_web_main_page(httpd_req_t *req)
{
    int response;
    sprintf(response_data, mainPage_html, "OK");
    response = httpd_resp_send(req, response_data, HTTPD_RESP_USE_STRLEN);
    return response;
}

/**
  * @brief  function to send hardware configuration web page
  * @param  req: HTTP request received from GET/POST method
  * @retval result of operation
  */
esp_err_t send_web_hw_conf_page(httpd_req_t *req)
{
    int response;
    sprintf(response_data, hwConf_html, "OK");
    response = httpd_resp_send(req, response_data, HTTPD_RESP_USE_STRLEN);
    return response;
}

/**
  * @brief  function to send special function web page
  * @param  req: HTTP request received from GET/POST method
  * @retval result of operation
  */
esp_err_t send_web_spec_func_page(httpd_req_t *req)
{
    int response;
    sprintf(response_data, specFunc_html, "OK");
    response = httpd_resp_send(req, response_data, HTTPD_RESP_USE_STRLEN);
    return response;
}

/**
  * @brief  function to send upload web page
  * @param  req: HTTP request received from GET/POST method
  * @retval result of operation
  */
esp_err_t send_web_upload_page(httpd_req_t *req)
{
    int response;
    sprintf(response_data, upload_html, "OK");
    response = httpd_resp_send(req, response_data, HTTPD_RESP_USE_STRLEN);
    return response;
}

/**
  * @brief  function to send model configuration web page
  * @param  req: HTTP request received from GET/POST method
  * @retval result of operation
  */
esp_err_t send_web_model_conf_page(httpd_req_t *req)
{
    int response;
    sprintf(response_data, modelConf_html, "OK");
    response = httpd_resp_send(req, response_data, HTTPD_RESP_USE_STRLEN);
    return response;
}

/**
  * @brief  function to send sinapsi configuration web page
  * @param  req: HTTP request received from GET/POST method
  * @retval result of operation
  */
esp_err_t send_web_sinapsi_conf_page(httpd_req_t *req)
{
    int response;
    sprintf(response_data, sinapsiConf_html, "OK");
    response = httpd_resp_send(req, response_data, HTTPD_RESP_USE_STRLEN);
    return response;
}

/**
  * @brief  function to send style.css web page
  * @param  req: HTTP request received from GET/POST method
  * @retval result of operation
  */
esp_err_t send_web_style_page(httpd_req_t *req)
{
    int response;
    httpd_resp_set_type(req, "text/css");
    response = httpd_resp_send(req, style_css, sizeof(style_css));
    return response;
}

/**
  * @brief  function to send logo scame.png
  * @param  req: HTTP request received from GET/POST method
  * @retval result of operation
  */
esp_err_t send_web_logo_scame(httpd_req_t *req)
{
    int response;
    httpd_resp_set_type(req, "image/ico");
    response = httpd_resp_send(req, favicon_ico, sizeof(favicon_ico));
    return response;
}

/**
  * @brief  function to send SCU serial number after receiving AJAX request
  *         implemented upload every 10s, so every 10s we upload the values shown
  * @param  req: HTTP request received from GET/POST method
  * @retval result of operation
  */
esp_err_t send_web_scu_serial(httpd_req_t *req)
{
    int response;
    char resp[70];
    char* scusn = getScuSerialNumber();
    char* prodsn = getProductSerialNumber();

    /* Json response format: 
    {
    "scuSerialNumber": "12345678", 
    "productSerialNumber": "987654321"
    }
    */
    httpd_resp_set_type(req, "application/json");
    memset(resp, 0x00, 70);
    memcpy(resp, "{\n\"scuSerialNumber\": \"", 23);
    memcpy(&resp[22], scusn, SERIAL_NUMBER_LENGTH);
    memcpy(&resp[30], "\", \n\"productSerialNumber\": \"", 28);
    memcpy(&resp[58], prodsn, PRODUCT_NUMBER_LENGTH);
    memcpy(&resp[67], "\"\n}", 3);

    response = httpd_resp_send(req, resp, 70);
    return response;
}

/**
  * @brief  function to send hardware parameters after receiving AJAX request
  *         implemented upload every N sec, so every N seconds we upload the values shown
  * @param  req: HTTP request received from GET/POST method
  * @retval result of operation
  */
esp_err_t send_web_hw_parameters(httpd_req_t *req)
{
    int response;
    char resp[150];
    uint32_t hw_checks = 0;
    uint16_t partial_len = 0;
    uint16_t hw_actuators = 0;
    sck_wiring_e hw_socket = 0;
    energy_meter_e hw_energy = 0;
    uint32_t hw_typical = 0;
    uint32_t hw_simplified = 0;
    batteryBackup_e hw_battery = 0;
    uint16_t hw_address = 0;
    uint16_t hw_led = 0;
    uint16_t hw_operation = 0;

    hw_checks = (getHardwareChecks2() << 16) | getHardwareChecks1();
    hw_actuators = getHardwareActuators();
    hw_socket = getSocketType();
    hw_energy = getEnergyMeterType();
    hw_typical = getTypicalCurrent();
    hw_simplified = getSimplifiedCurrent();
    hw_address = getScuAddress();
    hw_led = getLedStripCode();
    hw_operation = getOperativeMode();

    hw_battery = getBatteryBackup();
    httpd_resp_set_type(req, "application/json");
    memset(resp, 0x00, 100);
    sprintf(resp, "{\n\"check\": \"%lu", hw_checks);
    strcat(resp, "\",\n\"act\": \"");
    sprintf(&resp[strlen(resp)], "%d", hw_actuators);
    strcat(resp, "\",\n\"skType\": \"");
    sprintf(&resp[strlen(resp)], "%d", hw_socket);
    strcat(resp, "\",\n\"emCode\": \"");
    sprintf(&resp[strlen(resp)], "%d", hw_energy);
    strcat(resp, "\",\n\"ITyp\": \"");
    sprintf(&resp[strlen(resp)], "%lu", hw_typical);
    strcat(resp, "\",\n\"ISem\": \"");
    sprintf(&resp[strlen(resp)], "%lu", hw_simplified);
    strcat(resp, "\",\n\"Back\": \"");
    sprintf(&resp[strlen(resp)], "%d", hw_battery);
    strcat(resp, "\",\n\"iAddr\": \"");
    sprintf(&resp[strlen(resp)], "%d", hw_address);
    strcat(resp, "\",\n\"lsCode\": \"");
    sprintf(&resp[strlen(resp)], "%d", hw_led);
    strcat(resp, "\",\n\"moCode\": \"");
    sprintf(&resp[strlen(resp)], "%d", hw_operation);
    strcat(resp, "\"\n}");

    ESP_LOGI(TAG, "Response: %s", resp);
    response = httpd_resp_send(req, resp, strlen(resp));
    return response;
}

void https_server_user_callback(esp_https_server_user_cb_arg_t *user_cb)
{
    ESP_LOGI(TAG, "User callback invoked!");
#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
    mbedtls_ssl_context *ssl_ctx = NULL;
#endif
    switch(user_cb->user_cb_state) {
        case HTTPD_SSL_USER_CB_SESS_CREATE:
            ESP_LOGI(TAG, "At session creation");

            // Logging the socket FD
            int sockfd = -1;
            esp_err_t esp_ret;
            esp_ret = esp_tls_get_conn_sockfd(user_cb->tls, &sockfd);
            if (esp_ret != ESP_OK) {
                ESP_LOGE(TAG, "Error in obtaining the sockfd from tls context");
                break;
            }
            ESP_LOGI(TAG, "Socket FD: %d", sockfd);
#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
            ssl_ctx = (mbedtls_ssl_context *) esp_tls_get_ssl_context(user_cb->tls);
            if (ssl_ctx == NULL) {
                ESP_LOGE(TAG, "Error in obtaining ssl context");
                break;
            }
            // Logging the current ciphersuite
            ESP_LOGI(TAG, "Current Ciphersuite: %s", mbedtls_ssl_get_ciphersuite(ssl_ctx));
#endif
            break;

        case HTTPD_SSL_USER_CB_SESS_CLOSE:
            ESP_LOGI(TAG, "At session close");
#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
            // Logging the peer certificate
            ssl_ctx = (mbedtls_ssl_context *) esp_tls_get_ssl_context(user_cb->tls);
            if (ssl_ctx == NULL) {
                ESP_LOGE(TAG, "Error in obtaining ssl context");
                break;
            }
            print_peer_cert_info(ssl_ctx);
#endif
            break;
        default:
            ESP_LOGE(TAG, "Illegal state!");
    }
            return;
}

void print_peer_cert_info(const mbedtls_ssl_context *ssl)
{
    const mbedtls_x509_crt *cert;
    const size_t buf_size = 1024;
    char *buf = calloc(buf_size, sizeof(char));
    if (buf == NULL) {
        ESP_LOGE(TAG, "Out of memory - Callback execution failed!");
        return;
    }

    // Logging the peer certificate info
    cert = mbedtls_ssl_get_peer_cert(ssl);
    if (cert != NULL) {
        mbedtls_x509_crt_info((char *) buf, buf_size - 1, "    ", cert);
        ESP_LOGI(TAG, "Peer certificate info:\n%s", buf);
    } else {
        ESP_LOGW(TAG, "Could not obtain the peer certificate!");
    }

    free(buf);
}

static esp_err_t stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    return httpd_ssl_stop(server);
}

void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        if (stop_webserver(*server) == ESP_OK) {
            *server = NULL;
        } else {
            ESP_LOGE(TAG, "Failed to stop https server");
        }
    }
}

void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    ESP_LOGI(TAG, "Passing to connect_handler");
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        *server = setup_server();
    }
}

/**
  * @brief  function to register URI of the server
  * @param  
  * @retval HHTP handler with URI registered
  */
httpd_handle_t setup_server(void)
{
    //httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    ESP_LOGI(TAG, "Starting server");
    httpd_ssl_config_t config = HTTPD_SSL_CONFIG_DEFAULT();
    httpd_handle_t server = NULL;
    config.user_cb = https_server_user_callback;
    
    extern const unsigned char servercert_start[] asm("_binary_servercert_crt_start");
    extern const unsigned char servercert_end[]   asm("_binary_servercert_crt_end");
    config.servercert = servercert_start;
    config.servercert_len = servercert_end - servercert_start;

    extern const unsigned char prvtkey_crt_start[] asm("_binary_prvtkey_crt_start");
    extern const unsigned char prvtkey_crt_end[]   asm("_binary_prvtkey_crt_end");
    config.prvtkey_pem = prvtkey_crt_start;
    config.prvtkey_len = prvtkey_crt_end - prvtkey_crt_start;

    initStationInfo();
    
    if (httpd_ssl_start(&server, &config) == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTPS started OK\n");
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_login);
        httpd_register_uri_handler(server, &uri_hwConf);
        httpd_register_uri_handler(server, &uri_specFunc);
        httpd_register_uri_handler(server, &uri_upload);
        httpd_register_uri_handler(server, &uri_modelConf);
        httpd_register_uri_handler(server, &uri_sinapsiConf);
        httpd_register_uri_handler(server, &uri_styleCss);
        httpd_register_uri_handler(server, &uri_favicon);
        httpd_register_uri_handler(server, &uri_setSerials);
        httpd_register_uri_handler(server, &uri_mainPage);
        httpd_register_uri_handler(server, &uri_scuSerial);
        httpd_register_uri_handler(server, &uri_setHwParams);
        httpd_register_uri_handler(server, &uri_getHwParams);
    }

    return server;
}

/*
*********************************** SCAME ************************************
**                                                                          **
**              GET and POST URI handler definition                         **
**                                                                          **
******************************************************************************
*/

/**
  * @brief  function to handle get root request
  * @param  
  * @retval result of operation
  */
static esp_err_t get_req_handler(httpd_req_t *req)
{
    return send_web_page(req);
}

/**
  * @brief  function to handle post login request
  * @param  
  * @retval result of operation
  */
static esp_err_t login_post_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;
    char username[20];
    char password[20];

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Content-Length") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Content-Length", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Content-Length: %s", buf);
        }
        free(buf);
    }

    /* Extract dara from post body request*/
    char content[100];
    memset(content, 0x00, 100);
    size_t recv_size = MIN(req->content_len, sizeof(content));
    int ret = httpd_req_recv(req, content, recv_size);
    if(ret > 0) {
        ESP_LOGI(TAG, "Received content: %s", content);
    }

    /* Extract username and password from received string*/
    extract_user_pass_from_content (content, username, password);
    ESP_LOGI(TAG, "Username: %s", username);
    ESP_LOGI(TAG, "Password: %s", password);

    /* Verify username and password */
    const char* resp_str;
    if(strcmp(username, WEB_UI_USERID) == 0) {
        if(strcmp(password, WEB_UI_PASSWORD) == 0) {
            resp_str = "Login ok";
            ESP_LOGI(TAG, "Login ok!");
            init_main_page_buffer();
            send_web_main_page(req);
        }
        else{
            resp_str = "Wrong password";
            ESP_LOGI(TAG, "wrong password");
            httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
        }
    }
    else {
        resp_str = "Wrong username";
        ESP_LOGI(TAG, "Wrong username");
        httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    }

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

/**
  * @brief  function to handle post main page request
  * @param  
  * @retval result of operation
  */
static esp_err_t main_page_get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Content-Length") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Content-Length", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Content-Length: %s", buf);
        }
        free(buf);
    }

    init_main_page_buffer();
    send_web_main_page(req);

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

/**
  * @brief  function to handle post hardware configuration request
  * @param  
  * @retval result of operation
  */
static esp_err_t hw_conf_post_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Content-Length") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Content-Length", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Content-Length: %s", buf);
        }
        free(buf);
    }

    init_hw_conf_page_buffer();
    send_web_hw_conf_page(req);

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

/**
  * @brief  function to handle post special function request
  * @param  
  * @retval result of operation
  */
static esp_err_t spec_func_post_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Content-Length") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Content-Length", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Content-Length: %s", buf);
        }
        free(buf);
    }

    init_spec_func_page_buffer();
    send_web_spec_func_page(req);

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

/**
  * @brief  function to handle post upload firmware request
  * @param  
  * @retval result of operation
  */
static esp_err_t upload_post_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Content-Length") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Content-Length", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Content-Length: %s", buf);
        }
        free(buf);
    }

    init_upload_page_buffer();
    send_web_upload_page(req);

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

/**
  * @brief  function to handle post model configuration request
  * @param  
  * @retval result of operation
  */
static esp_err_t model_conf_post_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Content-Length") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Content-Length", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Content-Length: %s", buf);
        }
        free(buf);
    }

    init_model_conf_page_buffer();
    send_web_model_conf_page(req);

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

/**
  * @brief  function to handle post sinapsi configuration request
  * @param  
  * @retval result of operation
  */
static esp_err_t sinapsi_conf_post_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Content-Length") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Content-Length", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Content-Length: %s", buf);
        }
        free(buf);
    }

    init_sinapsi_conf_page_buffer();
    send_web_sinapsi_conf_page(req);

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

/**
  * @brief  function to handle get style.css request
  * @param  
  * @retval result of operation
  */
static esp_err_t style_get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Content-Length") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Content-Length", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Content-Length: %s", buf);
        }
        free(buf);
    }

    init_style_page_buffer();
    send_web_style_page(req);

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

/**
  * @brief  function to handle get logo scame request
  * @param  
  * @retval result of operation
  */
static esp_err_t favicon_get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    init_logo_scame_buffer();
    send_web_logo_scame(req);

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

/**
  * @brief  function to handle info serial number e product number set
  * @param  
  * @retval result of operation
  */
static esp_err_t info_sn_post_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;
    const char* resp_str;
    char scu_sn[9];
    char prod_sn[10];

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Content-Length") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Content-Length", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Content-Length: %s", buf);
        }
        free(buf);
    }

    /* Extract data from post body request*/
    char content[200];
    memset(content, 0x00, 200);
    size_t recv_size = MIN(req->content_len, sizeof(content));
    int ret = httpd_req_recv(req, content, recv_size);
    if(ret > 0) {
        ESP_LOGI(TAG, "Received content: %s", content);
    }

    /* Extract scu serial number and product serial number from received string*/
    extract_serial_numbers_from_content (content, scu_sn, prod_sn);
    ESP_LOGI(TAG, "SCU serial number received: %s", scu_sn);
    ESP_LOGI(TAG, "Product serial number received: %s", prod_sn);
    setScuSerialNumber(scu_sn);
    setProductSerialNumber(prod_sn);
    ESP_LOGI(TAG, "SCU serial number memorized: %s", getScuSerialNumber());
    ESP_LOGI(TAG, "Product serial number memorized: %s", getProductSerialNumber());

    /* Send reply to POST request*/
    resp_str = "ok";   
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

/**
  * @brief  function to handle get serial number request
  * @param  
  * @retval result of operation
  */
static esp_err_t scu_serial_get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    send_web_scu_serial(req);

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

/**
  * @brief  function to handle set hardware parameters of page hwConf.html
  * @param  
  * @retval result of operation
  */
static esp_err_t set_hw_params_post_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;
    const char* resp_str;
    char scu_sn[9];
    char prod_sn[10];

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Content-Length") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Content-Length", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Content-Length: %s", buf);
        }
        free(buf);
    }

    /* Extract data from post body request*/
    char content[300];
    memset(content, 0x00, 300);
    size_t recv_size = MIN(req->content_len, sizeof(content));
    int ret = httpd_req_recv(req, content, recv_size);
    if(ret > 0) {
        ESP_LOGI(TAG, "Received content: %s", content);
    }

    /* Extract hardware parameters received from hwConf.html page */
    extract_hardware_parameters_from_content(content);

    /* Send reply to POST request*/
    resp_str = "ok";   
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

/**
  * @brief  function to handle get hardware parameters request
  * @param  
  * @retval result of operation
  */
static esp_err_t hw_params_get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    int x = httpd_req_to_sockfd(req);
    ESP_LOGI(TAG, "Receive message through secure socket %d", x);

    send_web_hw_parameters(req);

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

/*
*********************************** SCAME ************************************
**                                                                          **
**                        Utils function definition                         **
**                                                                          **
******************************************************************************
*/
/**
  * @brief  function to extract username and password from the post content request message
  * @param  content: message received from post req
  * @param  user: destination for username string
  * @param  pass: destination for password string 
  * @retval 
  */
static void extract_user_pass_from_content (char* content, char* user, char* pass){
    char * usernameString;
    char * passwordString;
    char * usernameToken;
    char * usernameValue;
    char * passwordToken;
    char * passwordValue;

    ESP_LOGI(TAG, "Content received: %s", content);
    usernameString = strtok(content, "&");
    passwordString = strtok(NULL, "");
    usernameToken = strtok(usernameString, "=");
    usernameValue = strtok(NULL, "");
    memcpy(user, usernameValue, strlen(usernameValue));
    passwordToken = strtok(passwordString, "=");
    passwordValue = strtok(NULL, "");
    memcpy(pass, passwordValue, strlen(passwordValue));
}

/**
  * @brief  function to extract SCU serial number and product serial number from the post content request message
  * @param  content: message received from post req
  * @param  scu_sn: destination for scu serial number string
  * @param  prod_sn: destination for product serial number string
  * @retval 
  */
static void extract_serial_numbers_from_content (char* content, char* scu_sn, char* prod_sn) {
    char *findSubstring;

    findSubstring = strstr(content, "scuSerialNumber");
    if (findSubstring) {
        memcpy(scu_sn, &findSubstring[18], SERIAL_NUMBER_LENGTH);
        scu_sn[SERIAL_NUMBER_LENGTH] = '\0';
        findSubstring = strstr(findSubstring, "productSerialNumber");
        if (findSubstring) {
            memcpy(prod_sn, &findSubstring[22], PRODUCT_NUMBER_LENGTH);
            prod_sn[PRODUCT_NUMBER_LENGTH] = '\0';
        }
    }
}

/**
  * @brief  function to extract hardware parameters from content received from setHwParams post req
  * @param  content: message received from post req
  * @retval 
  */
static void extract_hardware_parameters_from_content (char* content) {
    char *findSubstring;
    char* ptr;
    uint32_t hw_checks;
    uint16_t hw_actuators;
    sck_wiring_e socketType;
    energy_meter_e energyMeter;
    uint32_t typicalCurrent;
    uint32_t simplifiedCurrent;
    batteryBackup_e batteryBackup;
    uint16_t scuAddress;
    uint16_t ledStripCode;
    uint16_t operativeMode;
    uint8_t i;

    findSubstring = strstr(content, "check");
    if(findSubstring){
        hw_checks = extract_hardware_checks_from_string(findSubstring);
        setHardwareChecks1((uint16_t)(hw_checks));
        setHardwareChecks2(hw_checks >> 16);
        ESP_LOGI(TAG, "Hardware checks1 value : %d", (uint16_t)(hw_checks));
        ESP_LOGI(TAG, "Hardware checks2 value: %lu", hw_checks >> 16);
    }
    findSubstring = strstr(content, "act");
    if(findSubstring){
        hw_actuators = extract_hardware_actuators_from_string(findSubstring);
        setHardwareActuators(hw_actuators);
    }
    findSubstring = strstr(content, "skType");
    if(findSubstring){
        socketType = extract_socket_type_from_string(findSubstring);
        setSocketType(socketType);
    }
    findSubstring = strstr(content, "emCode");
    if(findSubstring){
        energyMeter = extract_energy_meter_from_string(findSubstring);
        setEnergyMeterType(energyMeter);
    }
    findSubstring = strstr(content, "ITyp");
    if(findSubstring){
        typicalCurrent = extract_typical_current_from_string(findSubstring);
        setTypicalCurrent(typicalCurrent);
    }
    findSubstring = strstr(content, "ISem");
    if(findSubstring){
        simplifiedCurrent = extract_simplified_current_from_string(findSubstring);
        setSimplifiedCurrent(simplifiedCurrent);
    }
    findSubstring = strstr(content, "Back");
    if(findSubstring){
        batteryBackup = extract_battery_backup_from_string(findSubstring);
        setBatteryBackup(batteryBackup);
    }
    findSubstring = strstr(content, "iAddr");
    if(findSubstring){
        scuAddress = extract_scu_address_from_string(findSubstring);
        setScuAddress(scuAddress);
    }
    findSubstring = strstr(content, "lsCode");
    if(findSubstring){
        ledStripCode = extract_led_strip_code_from_string(findSubstring);
        setLedStripCode(ledStripCode);
    }
    findSubstring = strstr(content, "moCode");
    if(findSubstring){
        operativeMode = extract_operative_mode_from_string(findSubstring);
        setOperativeMode(operativeMode);
    }
}

/**
  * @brief  function to extract hardware checks for substring received
  * @param  string: substring received from post req
  * @retval uint16_t: value of harware checks received
  */
static uint32_t extract_hardware_checks_from_string (char* string) {
    char dest[8] = {'\0', '\0', '\0','\0', '\0', '\0', '\0', '\0'};
    char* ptr;
    uint32_t hw_checks = 0;
    long ret = 0;
    uint8_t i;

    for(i = 0; i < 8; i++) {
            if(string[i+7] != '>') {
                dest[i] = string[i+7];
            }
            else {
                break;
            }
        }
    ret = strtol(dest, &ptr, 10);
    hw_checks = (uint32_t) ret;
    ESP_LOGI(TAG, "Hardware checks value: %lu", hw_checks);
    return hw_checks;
}

/**
  * @brief  function to extract hardware actuators for substring received
  * @param  string: substring received from post req
  * @retval uint16_t: value of hardware actuators received
  */
static uint16_t extract_hardware_actuators_from_string (char* string) {
    char dest[3] = {'\0', '\0', '\0'};
    char* ptr;
    uint16_t hw_actuators = 0;
    long ret = 0;
    uint8_t i;

    for(i = 0; i < 3; i++) {
            if(string[i+5] != '>') {
                dest[i] = string[i+5];
            }
            else {
                break;
            }
        }
    ret = strtol(dest, &ptr, 10);
    hw_actuators = (uint16_t) ret;
    ESP_LOGI(TAG, "Hardware actuators value: %d", hw_actuators);
    return hw_actuators;
}

/**
  * @brief  function to extract socket type from received req context
  * @param  string: substring received from post req
  * @retval sck_wiring_e: value of socket type
  */
static sck_wiring_e extract_socket_type_from_string (char* string) {
    char dest[3] = {'\0', '\0', '\0'};
    char* ptr;
    sck_wiring_e socket_type = 0;
    long ret = 0;
    uint8_t i;

    for(i = 0; i < 3; i++) {
            if(string[i+9] != '>') {
                dest[i] = string[i+9];
            }
            else {
                break;
            }
        }
    ret = strtol(dest, &ptr, 10);
    socket_type = (uint16_t) ret;
    socket_type = (sck_wiring_e) socket_type;
    ESP_LOGI(TAG, "Socket type value: %d", socket_type);
    return socket_type;
}

/**
  * @brief  function to extract energy meter from received req context
  * @param  string: substring received from post req
  * @retval energy_meter_e: value of energy meter
  */
static energy_meter_e extract_energy_meter_from_string (char* string) {
    char dest[3] = {'\0', '\0', '\0'};
    char* ptr;
    energy_meter_e energy_meter = 0;
    long ret = 0;
    uint8_t i;

    for(i = 0; i < 3; i++) {
            if(string[i+9] != '>') {
                dest[i] = string[i+9];
            }
            else {
                break;
            }
        }
    ret = strtol(dest, &ptr, 10);
    energy_meter = (uint16_t) ret;
    energy_meter = (energy_meter_e) energy_meter;
    ESP_LOGI(TAG, "Energy meter value: %d", energy_meter);
    return energy_meter;
}

/**
  * @brief  function to extract typical current from received req context
  * @param  string: substring received from post req
  * @retval uint32_t: value of typical current
  */
static uint32_t extract_typical_current_from_string (char* string) {
    char dest[2] = {'\0', '\0'};
    char* ptr;
    uint32_t typ_current = 0;
    long ret = 0;
    uint8_t i;

    for(i = 0; i < 2; i++) {
            if(string[i+7] != '>') {
                dest[i] = string[i+7];
            }
            else {
                break;
            }
        }
    ret = strtol(dest, &ptr, 10);
    typ_current = (uint32_t) ret;
    ESP_LOGI(TAG, "Typical current value: %lu", typ_current);
    return typ_current;
}

/**
  * @brief  function to extract simplified current from received req context
  * @param  string: substring received from post req
  * @retval uint32_t: value of simplified current
  */
static uint32_t extract_simplified_current_from_string (char* string) {
    char dest[2] = {'\0', '\0'};
    char* ptr;
    uint32_t sim_current = 0;
    long ret = 0;
    uint8_t i;

    for(i = 0; i < 2; i++) {
            if(string[i+7] != '>') {
                dest[i] = string[i+7];
            }
            else {
                break;
            }
        }
    ret = strtol(dest, &ptr, 10);
    sim_current = (uint32_t) ret;
    ESP_LOGI(TAG, "Simplified current value: %lu", sim_current);
    return sim_current;
}

/**
  * @brief  function to extract battery backup on/off from received req context
  * @param  string: substring received from post req
  * @retval batteryBackup_e: battery backup ON/OFF
  */
static batteryBackup_e extract_battery_backup_from_string (char* string) {
    char dest[2] = {'\0', '\0'};
    char* ptr;
    uint16_t batteryBack = 0;
    long ret = 0;
    uint8_t i;

    for(i = 0; i < 1; i++) {
            if(string[i+6] != '>') {
                dest[i] = string[i+6];
            }
            else {
                break;
            }
        }
    ret = strtol(dest, &ptr, 10);
    batteryBack = (uint32_t) ret;
    batteryBack = (batteryBackup_e)batteryBack;
    ESP_LOGI(TAG, "Battery backup on/off value: %d", batteryBack);
    return batteryBack;
}

/**
  * @brief  function to extract scu address from received req context
  * @param  string: substring received from post req
  * @retval uint16_t: scu address
  */
static uint16_t extract_scu_address_from_string (char* string) {
    char dest[2] = {'\0', '\0'};
    char* ptr;
    uint16_t scuAddress = 0;
    long ret = 0;
    uint8_t i;

    for(i = 0; i < 2; i++) {
            if(string[i+8] != '>') {
                dest[i] = string[i+8];
            }
            else {
                break;
            }
        }
    ret = strtol(dest, &ptr, 10);
    scuAddress = (uint16_t) ret;
    ESP_LOGI(TAG, "SCU address: %d", scuAddress);
    return scuAddress;
}

/**
  * @brief  function to extract led strip code from received req context
  * @param  string: substring received from post req
  * @retval uint16_t: led strip
  */
static uint16_t extract_led_strip_code_from_string (char* string) {
    char dest[3] = {'\0', '\0', '\0'};
    char* ptr;
    uint16_t ledStrip = 0;
    long ret = 0;
    uint8_t i;

    for(i = 0; i < 3; i++) {
            if(string[i+9] != '>') {
                dest[i] = string[i+9];
            }
            else {
                break;
            }
        }
    ret = strtol(dest, &ptr, 10);
    ledStrip = (uint16_t) ret;
    ESP_LOGI(TAG, "Led strip code: %d", ledStrip);
    return ledStrip;
}

/**
  * @brief  function to extract operative mode from received req context
  * @param  string: substring received from post req
  * @retval uint16_t: operative mode
  */
static uint16_t extract_operative_mode_from_string (char* string) {
    char dest[2] = {'\0', '\0'};
    char* ptr;
    uint16_t opMode = 0;
    long ret = 0;
    uint8_t i;

    for(i = 0; i < 1; i++) {
            if(string[i+8] != '>') {
                dest[i] = string[i+8];
            }
            else {
                break;
            }
        }
    ret = strtol(dest, &ptr, 10);
    opMode = (uint16_t) ret;
    ESP_LOGI(TAG, "SCU operative mode: %d", opMode);
    return opMode;
}