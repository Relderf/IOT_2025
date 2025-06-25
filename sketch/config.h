#define MOTOR_PIN 2                   // Pin de conexión del motor.
#define MOTOR_DEFAULT_ESTADO false    // Estado inicial del motor. TRUE = encendido; FALSE = apagado.
#define MOTOR_DURACION_MS 10000       // =10s. Tiempo que tarda el motor en abrir/cerrar las ventanas.

#define VENTANAS_DEFAULT_ESTADO false     // Estado inicial de las ventanas. TRUE = abiertas; FALSE = cerradas.
#define VENTANAS_MODO_AUTOMATICO false    // Estado inicial del modo automático. TRUE = activado; FALSE = desactivado.

#define CO2_SENSOR_PIN 15        // Pin de conexión del sensor de CO2.
#define CO2_ALTO 30.0            // CO2 sobre el cual se abrirán las ventanas en el modo automático.
#define CO2_BAJO 20.0            // Ídem CO2_ALTO, pero para cerrar las ventanas.
#define CO2_SENSOR_DELAY 2000    // =2s. Espera desde la última lectura de CO2 para tomar el valor actual.
#define CO2_MIN_SIMULADO 5.0     // CO2 mínimo simulado para el sensor de CO2.
#define CO2_MAX_SIMULADO 45.0    // CO2 máximo simulado para el sensor de CO2.

#define WIFI_CONNECTION_TIMEOUT 1000    // =1s. Espera entre intentos de conexión WiFi.
#define WIFI_MAX_ATTEMPTS 5             // Cantidad de intentos de conexión a WiFi antes de fallar.

#define MQTT_INTERVALO_PUSH 5000    // =5s. Intervalo de espera entre publicación de tópicos MQTT.
#define MQTT_CONN_DELAY 5000        // =5s. Intervalo de espera entre intentos de conexión a broker MQTT.

#define LOOP_DELAY 1000    // =1s. Intervalo de espera de loop principal de ejecución.

#define MSG_MAX_LENGTH 64    // Longitud máxima de los mensajes que se envían por MQTT.