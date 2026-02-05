#include "api_rest.h"
#include "esp_http_server.h"
#include "mdns.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define BLINK_GPIO 2

static const char *TAG = "API_MOD";
static httpd_handle_t server = NULL;

void start_mdns(void) {
    mdns_init();
    mdns_hostname_set("esp32");
    mdns_instance_name_set("ESP32 REST API");
    mdns_service_add("ESP32-Web", "_http", "_tcp", 80, NULL, 0);
    ESP_LOGI(TAG, "mDNS activo: http://esp32.local");
}

/**
 * Controladores
 */
static esp_err_t get_handler(httpd_req_t *req) {
    const char *resp = "{ \"device\": \"esp32\", \"status\": \"ok\" }";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t get_params_handler(httpd_req_t *req) {
    char* buf;
    size_t buf_len;

    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            char param[32];
            if (httpd_query_key_value(buf, "id", param, sizeof(param)) == ESP_OK) {
              httpd_resp_set_type(req, "application/json");
              httpd_resp_sendstr_chunk(req, "{ \"device\": \"esp32\", \"sensor\": ");
              httpd_resp_sendstr_chunk(req, param);
              httpd_resp_sendstr_chunk(req, " }");
            }
        }
        free(buf);
    }
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static esp_err_t set_led_handler(httpd_req_t *req) {
    char* buf;
    size_t buf_len;

    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            char param[32];
            if (httpd_query_key_value(buf, "status", param, sizeof(param)) == ESP_OK) {
              int status = atoi(param);
              gpio_set_level(BLINK_GPIO, status);

              if (status) {
                const char *resp = "{ \"device\": \"esp32\", \"status\": \"on\" }";
                httpd_resp_set_type(req, "application/json");
                httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

              } else {
                const char *resp = "{ \"device\": \"esp32\", \"status\": \"off\" }";
                httpd_resp_set_type(req, "application/json");
                httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
              }
            }
        }
        free(buf);
    }
    return ESP_OK;
}

static esp_err_t post_handler(httpd_req_t *req) {
    char content[100];
    int ret = httpd_req_recv(req, content, req->content_len);
    if (ret <= 0) return ESP_FAIL;

    content[ret] = '\0';
    printf("Datos recibidos: %s\n", content);

    httpd_resp_send(req, "Datos procesados con éxito", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/**
 * Router
 */
void start_webserver(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &config) == ESP_OK) {
        // Registro de GET
        httpd_uri_t get_uri = { .uri = "/", .method = HTTP_GET, .handler = get_handler };
        httpd_register_uri_handler(server, &get_uri);

        // Registro de GET con Params
        httpd_uri_t params_uri = { .uri = "/sensor", .method = HTTP_GET, .handler = get_params_handler };
        httpd_register_uri_handler(server, &params_uri);

        httpd_uri_t set_led_uri = { .uri = "/led", .method = HTTP_GET, .handler = set_led_handler };
        httpd_register_uri_handler(server, &set_led_uri);

        // Registro de POST
        httpd_uri_t post_uri = { .uri = "/", .method = HTTP_POST, .handler = post_handler };
        httpd_register_uri_handler(server, &post_uri);
    }
}
