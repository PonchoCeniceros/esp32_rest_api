# ESP32 REST API

![C Badge](https://img.shields.io/badge/C-A8B9CC?logo=c&logoColor=fff&style=for-the-badge)![CMake Badge](https://img.shields.io/badge/CMake-064F8C?logo=cmake&logoColor=fff&style=for-the-badge)![Espressif Badge](https://img.shields.io/badge/Espressif-E7352C?logo=espressif&logoColor=fff&style=for-the-badge)

Proyecto simple para ESP32 que implementa un servidor REST API básico con conexión WiFi y servicio mDNS.

## Funcionalidades

- Conexión WiFi en modo estación
- Servidor HTTP REST API en puerto 80
- Servicio mDNS para descubrimiento local (`http://esp32.local`)
- Endpoint `/status` que devuelve JSON con estado del dispositivo

## Requisitos

- ESP-IDF Framework
- ESP32 Development Board
- Red WiFi disponible

## Compilación y Flasheo

```bash
# Configurar el proyecto
idf.py menuconfig

# Compilar y flashear
idf.py build flash

# Monitor serial
idf.py monitor
```

## API Endpoints

### GET /status
Respuesta JSON con estado del dispositivo:
```json
{
  "device": "esp32",
  "status": "ok"
}
```

## Configuración

Las credenciales WiFi deben configurarse a través del menú de configuración de ESP-IDF:
```
idf.py menuconfig
```
Navegar a `WiFi Configuration` y configurar:
- WiFi SSID
- WiFi Password

## Estructura del Proyecto

```
├── main/
│   ├── main.c           # Punto de entrada principal
│   ├── wifi/            # Módulo de conexión WiFi
│   └── api/             # Servidor REST API
├── CMakeLists.txt       # Configuración de construcción
└── README.md            # Este archivo
```

## Guía de librerías/Framework ESP-IDF

| Librería | ¿Qué es? | Su función crítica en tu proyecto |
| :--- | :--- | :--- |
| **nvs_flash.h** | El "Disco Duro" (No-Volátil) | Guarda datos que no se borran al apagar el chip. El WiFi la usa para recordar la configuración y llaves de cifrado. |
| **freertos/FreeRTOS.h** | El Director de Orquesta | Es el núcleo del sistema operativo. Permite que el ESP32 haga varias cosas "a la vez" (Multitarea). |
| **freertos/event_groups.h** | El Tablero de Anuncios | Permite que una parte del código (WiFi) le avise a otra (Main) que ya ocurrió algo importante. |
| **esp_wifi.h** | El Driver de la Antena | Controla el hardware de radio. Se encarga de buscar redes, conectarse y gestionar la seguridad WPA2. |
| **esp_event.h** | El Sistema de Notificaciones | Gestiona los eventos del sistema (ej: "cable desconectado", "IP obtenida", "error de clave"). |
| **esp_netif.h** | La Tarjeta de Red (Software) | Es la interfaz entre el WiFi y el protocolo TCP/IP. Sin esto, el WiFi conecta pero no hay navegación. |
| **mdns.h** | El Alias de Red | Permite que uses esp32.local en lugar de una IP numérica. Es como un mini-servidor de nombres local. |
| **esp_http_server.h** | El Servidor Web | Escucha peticiones en el puerto 80 y decide qué función de tu código (Handler) debe responder. |
| **esp_log.h** | La Consola de Depuración | Envía mensajes de texto por el puerto serie (USB) para que veas qué pasa dentro del chip desde tu PC. |


## Configuración básica de una API REST

### Controlador
Es la función que se ejecuta cuando alguien visita la URL.

* httpd_req_t *req:
Es un puntero al objeto de la petición. Contiene todo: headers, parámetros de URL y el socket de conexión.

* httpd_resp_set_type:
Define el MIME type. Es vital para que el navegador o Postman sepan que estás enviando JSON y no texto plano.

* HTTPD_RESP_USE_STRLEN:
Un valor especial que le dice al ESP32: "calcula tú mismo el tamaño de la cadena usando strlen". Si no lo usas, tendrías que pasar el tamaño en bytes manualmente.

### 1. GET Simple
```c
static esp_err_t get_handler(httpd_req_t *req) {
    const char* resp = "Hola desde GET simple";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
```

### 2. GET con Parámetros (Query Strings)
```c
static esp_err_t get_params_handler(httpd_req_t *req) {
    char* buf;
    size_t buf_len;

    // 1. Obtener la longitud de la cadena de consulta (query string)
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        // 2. Extraer la cadena
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            char param[32];
            // 3. Buscar un parámetro específico (ej: "id")
            if (httpd_query_key_value(buf, "id", param, sizeof(param)) == ESP_OK) {
                httpd_resp_sendstr_chunk(req, "ID recibido: ");
                httpd_resp_sendstr_chunk(req, param);
            }
        }
        free(buf);
    }
    httpd_resp_send_chunk(req, NULL, 0); // Finalizar respuesta
    return ESP_OK;
}
```

### 3. POST (Recibir Datos)
```c
static esp_err_t post_handler(httpd_req_t *req) {
    char content[100]; // Buffer para los datos recibidos

    // 1. Leer los datos del request
    int ret = httpd_req_recv(req, content, req->content_len);
    if (ret <= 0) return ESP_FAIL;

    content[ret] = '\0'; // Asegurar que sea un string válido
    printf("Datos recibidos: %s\n", content);

    // 2. Responder confirmación
    httpd_resp_send(req, "Datos procesados con éxito", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
```

### Router
Antes de registrar la ruta, debes definir sus propiedades en una estructura httpd_uri_t.

* .uri:
El endpoint o ruta (ej. /status).

* .method:
El verbo HTTP. Podría ser HTTP_GET, HTTP_POST, HTTP_PUT, etc.

* .handler:
El nombre de la función que creaste arriba. Es un callback: el servidor la llamará cuando la ruta coincida.

```c
void start_webserver(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &config) == ESP_OK) {
        // Registro de GET
        httpd_uri_t get_uri = { .uri = "/hola", .method = HTTP_GET, .handler = get_handler };
        httpd_register_uri_handler(server, &get_uri);

        // Registro de GET con Params
        httpd_uri_t params_uri = { .uri = "/sensor", .method = HTTP_GET, .handler = get_params_handler };
        httpd_register_uri_handler(server, &params_uri);

        // Registro de POST
        httpd_uri_t post_uri = { .uri = "/data", .method = HTTP_POST, .handler = post_handler };
        httpd_register_uri_handler(server, &post_uri);
    }
}
```

