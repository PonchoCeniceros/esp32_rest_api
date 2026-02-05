idf_component_register(
    SRCS 
      "main.c"
      "wifi/wifi_app.c"
      "api/api_rest.c"
    INCLUDE_DIRS 
      "."
      "wifi"
      "api"
    REQUIRES
      nvs_flash        # para la memoria
      esp_wifi         # Para el WiFi
      esp_event        # Para el manejo de eventos
      esp_netif        # Para la pila TCP/IP
      mdns             # Para el nombre .local
      esp_http_server  # Para la API REST
      esp_driver_gpio  # para manejar los pines
)
