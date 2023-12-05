#include "station_info.h"
#include <esp_https_server.h>
#include "esp_tls.h"

/*
*********************************** SCAME ************************************
**                                                                          **
**           LOCAL MACROS, TYPEDEF, STRUCTURE, ENUM                         **
**                                                                          **
******************************************************************************
*/
#define INDEX_HTML_PATH             "/spiffs/index.html"
#define MAIN_PAGE_HTML_PATH         "/spiffs/mainPage.html"
#define HWCONF_PAGE_HTML_PATH       "/spiffs/hwConf.html"
#define SPECFUNC_PAGE_HTML_PATH     "/spiffs/specFunc.html"
#define UPLOAD_PAGE_HTML_PATH       "/spiffs/upload.html"
#define MODELCONF_PAGE_HTML_PATH    "/spiffs/modelConf.html"
#define SINAPSICONF_PAGE_HTML_PATH  "/spiffs/sinapsiInfo.html"
#define STYLE_PAGE_CSS_PATH         "/spiffs/style.css"
#define SCAME_LOGO_PATH             "/spiffs/favicon.ico"
#define WEB_UI_USERID               "scame"
#define WEB_UI_PASSWORD             "Scame01"

#define SERIAL_NUMBER_INDEX         16
#define SERIAL_LENGTH_URI_REQ       18
#define SERIAL_NUMBER_LENGTH        8
#define PRODUCT_NUMBER_INDEX        7
#define PRODUCT_NUMBER_LENGTH       9

/*                                                             
*********************************** SCAME ************************************
**                                                                          **
**                            External function                             **
**                                                                          **
******************************************************************************
*/
void init_web_page_buffer(void);
void init_main_page_buffer(void);
void init_hw_conf_page_buffer(void);
void init_spec_func_page_buffer(void);
void init_upload_page_buffer(void);
void init_model_conf_page_buffer(void);
void init_sinapsi_conf_page_buffer(void);
void init_style_page_buffer(void);
void init_logo_scame_buffer(void);

/*                                                             
*********************************** SCAME ************************************
**                                                                          **
**                            Local function                                **
**                                                                          **
******************************************************************************
*/
static esp_err_t get_req_handler(httpd_req_t *req);
static esp_err_t login_post_handler(httpd_req_t *req);
static esp_err_t hw_conf_post_handler(httpd_req_t *req);
static esp_err_t spec_func_post_handler(httpd_req_t *req);
static esp_err_t upload_post_handler(httpd_req_t *req);
static esp_err_t model_conf_post_handler(httpd_req_t *req);
static esp_err_t sinapsi_conf_post_handler(httpd_req_t *req);
static esp_err_t style_get_handler(httpd_req_t *req);
static esp_err_t favicon_get_handler(httpd_req_t *req);
static esp_err_t info_sn_post_handler(httpd_req_t *req);
static esp_err_t main_page_get_handler(httpd_req_t *req);
static esp_err_t scu_serial_get_handler(httpd_req_t *req);
static esp_err_t set_hw_params_post_handler(httpd_req_t *req);
static esp_err_t hw_params_get_handler(httpd_req_t *req);

esp_err_t send_web_page(httpd_req_t *req);
esp_err_t send_web_main_page(httpd_req_t *req);
esp_err_t send_web_hw_conf_page(httpd_req_t *req);
esp_err_t send_web_spec_func_page(httpd_req_t *req);
esp_err_t send_web_upload_page(httpd_req_t *req);
esp_err_t send_web_model_conf_page(httpd_req_t *req);
esp_err_t send_web_sinapsi_conf_page(httpd_req_t *req);
esp_err_t send_web_style_page(httpd_req_t *req);
esp_err_t send_web_logo_scame(httpd_req_t *req);
esp_err_t send_web_main_page(httpd_req_t *req);
esp_err_t send_web_scu_serial(httpd_req_t *req);
esp_err_t send_web_hw_parameters(httpd_req_t *req);

static void extract_user_pass_from_content (char* content, char* user, char* pass);
static void extract_serial_numbers_from_content (char* content, char* scu_sn, char* prod_sn);
static void extract_hardware_parameters_from_content (char* content);
static uint32_t extract_hardware_checks_from_string (char* string);
static uint16_t extract_hardware_actuators_from_string (char* string);
static sck_wiring_e extract_socket_type_from_string (char* string);
static energy_meter_e extract_energy_meter_from_string (char* string);
static uint32_t extract_typical_current_from_string (char* string); 
static uint32_t extract_simplified_current_from_string (char* string);
static batteryBackup_e extract_battery_backup_from_string (char* string);
static uint16_t extract_scu_address_from_string (char* string);
static uint16_t extract_led_strip_code_from_string (char* string);
static uint16_t extract_operative_mode_from_string (char* string);

httpd_handle_t setup_server(void);

void https_server_user_callback(esp_https_server_user_cb_arg_t *user_cb);
void print_peer_cert_info(const mbedtls_ssl_context *ssl);

void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data);
void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data);