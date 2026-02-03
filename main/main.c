#include "nvs_flash.h"
#include "wifi_app.h"
#include "api_rest.h"

void app_main(void) {
    // Inicializar memoria no volátil
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // Iniciar WiFi
    wifi_init_sta();

    // Esperar a que el WiFi esté conectado antes de lanzar servicios
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

    // Iniciar servicios una vez tengamos IP
    start_mdns();
    start_webserver();
}
