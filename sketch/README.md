# Control de Ventanas por CO2 - TP2 IOT 2025

## Integrantes

- **Ignacio Traberg** - Legajo: 18244/3
- **Martín Tau** - Legajo: 7881/8

---

## Descripción del Proyecto

Este proyecto corresponde a la **segunda parte** del sistema de control de ventanas basado en CO2, desarrollado para la materia *Internet de las Cosas (2025)*. Se eliminó la automatización mediante Telegram y se incorporaron herramientas propias de entornos industriales IoT para lograr una solución robusta y monitoreable.

Se simula el comportamiento de un sistema donde un **ESP32** publica lecturas de **CO2** a un **broker MQTT**, y **Node-RED** se encarga del procesamiento, ordenamiento y almacenamiento de los datos, incluyendo el control de apertura de las ventanas. La visualización histórica y en tiempo real se realiza mediante **InfluxDB** y **Grafana**.

Entre las funcionalidades principales:

- Simulación verosímil de variables físicas (CO2).
- Publicación periódica al broker MQTT.
- Subscripción del ESP32 para recibir órdenes desde Node-RED (abrir/cerrar ventana).
- Almacenamiento histórico en base de datos InfluxDB.
- Visualización mediante dashboards en Grafana.
- Contenerización completa del entorno con Docker Compose.

---

## Tecnologías Utilizadas

- **ESP32 DevKit V1**
- **MQTT (Mosquitto)**
- **Node-RED**
- **InfluxDB**
- **Grafana**
- **Docker Compose**
- Lenguaje: **C++**

---

## Capturas de Pantalla

---

## Contenedor

- Las aplicaciones provistas por la cátedra son gestionadas en un contenedor de kubernetes, cuya imagen puede generarse con el archivo docker-compose.yaml del siguiente repositorio:
  - https://gitlab.com/dgraselli/stack-nodered-mqtt/-/tree/master

---

## Flujo de Datos

1. **ESP32** publica CO2 en un tópico MQTT "esp/sensores" cada 5 segundos. A su vez, se suscribe a "esp32/ventanas".
2. **Node-RED**:
   - Se suscribe a "esp32/sensores" y almacena los datos en **InfluxDB**.
   - Evalúa las condiciones de CO2 y para valores por debajo de 20 o por encima de 30, publica en el tópico "esp/ventanas" los mensajes "cerrar" o "abrir" respectivamente.
3. **ESP32** actúa según lo recibido en su suscripción a "camara/01/vetilacion" mediante el método _callback()_: ordenando al sistema de ventilación a prenderse o apagarse.
4. **Grafana** consulta InfluxDB y presenta los datos de CO2 en tiempo real o de forma histórica.

---

## Notas

- El archivo `./sketch/env.h` debe contener las credenciales necesarias para conectar el ESP32 a la red Wi-Fi y al broker MQTT.
- `config.h` define constantes como los umbrales de CO2 para abrir/cerrar la ventana, tiempo entre publicaciones, etc.
- La simulación de CO2 puede activarse según el entorno (real o Wokwi).
- El entorno completo puede correr en Docker (ver `docker-compose.yml` en el repositorio referenciado en este readme).
- Para ver los dashboards, acceder a Grafana (`localhost:3000`) y cargar el dashboard con la configuración guardada en _grafana.json_.

---

## Simulación de Variables

- Simula valores aleatorios entre **5 y 45 ppm** de CO2, de manera ligeramente cambiante para gráficos no tan lineales.
