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
