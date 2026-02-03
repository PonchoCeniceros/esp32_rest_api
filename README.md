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

---
