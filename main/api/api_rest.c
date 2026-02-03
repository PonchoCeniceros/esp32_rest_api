#include "api_rest.h"
#include "esp_http_server.h"
#include "mdns.h"
#include "esp_log.h"

static const char *TAG = "API_MOD";
static httpd_handle_t server = NULL;

void start_mdns(void) {
    mdns_init();
    mdns_hostname_set("esp32");
    mdns_instance_name_set("ESP32 REST API");
    mdns_service_add("ESP32-Web", "_http", "_tcp", 80, NULL, 0);
    ESP_LOGI(TAG, "mDNS activo: http://esp32.local");
}

static esp_err_t status_handler(httpd_req_t *req) {
    const char *resp = "{ \"device\": \"esp32\", \"status\": \"ok\" }";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

void start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t status_uri = {
            .uri = "/status",
            .method = HTTP_GET,
            .handler = status_handler
        };
        httpd_register_uri_handler(server, &status_uri);
        ESP_LOGI(TAG, "Servidor HTTP en puerto 80");
    }
}
