#define MOTOR_PIN 2                   // Pin de conexión del motor.
#define MOTOR_DEFAULT_ESTADO false    // Estado inicial del motor. TRUE = encendido; FALSE = apagado.
#define MOTOR_DURACION_MS 10000       // =10s. Tiempo que tarda el motor en abrir/cerrar las ventanas.

#define VENTANAS_DEFAULT_ESTADO false     // Estado inicial de las ventanas. TRUE = abiertas; FALSE = cerradas.
#define VENTANAS_MODO_AUTOMATICO false    // Estado inicial del modo automático. TRUE = activado; FALSE = desactivado.

#define M2 8.0                    // Superficie en metros cuadrados de la habitación.
#define KGS_PAPAS 10.0            // Cantidad de kilos de papas almacenados (por defecto 10kg).

// Valores de referencia para CO2 en ppm (partes por millón) para un frigorífico con papas:
// CO2 normal: < 1000 ppm. CO2 > 2000 ppm indica posible pudrición.
#define CO2_BAJO (400.0 * KGS_PAPAS / 1.0 * (M2 / 8.0))   // Valor base ajustado por cantidad de papas y superficie.
#define CO2_ALTO (1200.0 * KGS_PAPAS / 1.0 * (M2 / 8.0))  // Valor crítico ajustado por cantidad de papas y superficie.
#define CO2_SENSOR_PIN 15        // Pin de conexión del sensor de CO2.
#define CO2_SENSOR_DELAY 2000    // =2s. Espera desde la última lectura de CO2 para tomar el valor actual.
#define CO2_MIN_SIMULADO 350.0  // CO2 mínimo simulado para el sensor de CO2 (aire limpio).
#define CO2_MAX_SIMULADO 3000.0 // CO2 máximo simulado para el sensor de CO2 (pudrición severa).

#define WIFI_CONNECTION_TIMEOUT 1000    // =1s. Espera entre intentos de conexión WiFi.
#define WIFI_MAX_ATTEMPTS 5             // Cantidad de intentos de conexión a WiFi antes de fallar.

#define MQTT_INTERVALO_PUSH 5000    // =5s. Intervalo de espera entre publicación de tópicos MQTT.
#define MQTT_CONN_DELAY 5000        // =5s. Intervalo de espera entre intentos de conexión a broker MQTT.

#define LOOP_DELAY 1000    // =1s. Intervalo de espera de loop principal de ejecución.

#define MSG_MAX_LENGTH 64    // Longitud máxima de los mensajes que se envían por MQTT.