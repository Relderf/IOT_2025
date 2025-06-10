#define MOTOR_PIN 2                   // Pin de conexión del motor.
#define MOTOR_DEFAULT_ESTADO false    // Estado inicial del motor. TRUE = encendido; FALSE = apagado.
#define MOTOR_DURACION_MS 10000       // =10s. Tiempo que tarda el motor en abrir/cerrar las ventanas.

#define VENTANAS_DEFAULT_ESTADO false     // Estado inicial de las ventanas. TRUE = abiertas; FALSE = cerradas.
#define VENTANAS_MODO_AUTOMATICO false    // Estado inicial del modo automático. TRUE = activado; FALSE = desactivado.

#define TEMP_SENSOR_PIN 15        // Pin de conexión del sensor de temperatura.
#define TEMP_ALTA 30.0            // Temperatura sobre la cual se abrirán las ventanas en el modo automático.
#define TEMP_BAJA 20.0            // Ídem TEMP_ALTA, pero para cerrar las ventanas.
#define TEMP_SENSOR_DELAY 2000    // =2s. Espera desde la última lectura de temperatura para tomar la temp. actual.
#define TEMP_MIN_SIMULADA 5.0    // Temperatura mínima simulada para el sensor de temperatura.
#define TEMP_MAX_SIMULADA 45.0   // Temperatura máxima simulada para el sensor de temperatura.
#define HUMEDAD_MINIMA 30.0      // Humedad mínima para abrir las ventanas en el modo automático.
#define HUMEDAD_MAXIMA 60.0      // Humedad máxima para cerrar las ventanas en el modo automático.
#define HUMEDAD_MIN_SIMULADA 10.0 // Humedad mínima simulada para el sensor de temperatura.
#define HUMEDAD_MAX_SIMULADA 90.0 // Humedad máxima simulada para el sensor de temperatura.

#define WIFI_CONNECTION_TIMEOUT 1000    // =1s. Espera entre intentos de conexión WiFi.
#define WIFI_MAX_ATTEMPTS 5             // Cantidad de intentos de conexión a WiFi antes de fallar.

#define BOT_INTERVALO_NOTIFICACION 300000      // =5m. Intervalo de espera entre informes de estado del Bot.
#define BOT_INTERVALO_CHEQUEO_MENSAJES 5000    // =5s. Intervalo de espera entre chequeo de mensajes nuevos del Bot.

#define MQTT_INTERVALO_PUSH 5000    // =5s. Intervalo de espera entre publicación de tópicos MQTT.
#define MQTT_CONN_DELAY 5000        // =5s. Intervalo de espera entre intentos de conexión a broker MQTT.

#define LOOP_DELAY 3000    // =3s. Intervalo de espera de loop principal de ejecución.

#define MSG_MAX_LENGTH 64    // Longitud máxima de los mensajes que se envían por MQTT.