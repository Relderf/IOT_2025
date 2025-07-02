#define CAMARA_ID "01"          // Identificador de la cámara frigorífica.
#define M3 8.0                    // Volumen en metros cúbicos de la cámara (por defecto 2x2x2).
#define KGS_PAPAS 5000.0           // Cantidad deakilos de papas almacenados (por defecto 5000kg).
#define VENTILACION 150.0         // Reducción de CO2 por ventilación (por defecto 150ppm por vuelta de loop).
#define CO2_PPM_H 11.0           // CO2 en ppm por hora generado por un kilo de papas (por defecto 10ppm/hora).
#define CO2_PPM_S (CO2_PPM_H / 3600.0) // CO2 en ppm por segundo generado por un kilo de papas (10ppm/hora / 3600 segundos).
#define FACTOR_CO2_PODRIDAS 5.9 // Factor de aumento de CO2 por segundo cuando las papas se pudren (por defecto seis veces lo normal).
#define CHANCE_PUDRICION 2.0  // Por vuelta del loop se pudren entre 0% y 2% de las papas totales (por defecto 2%).

// Valores de referencia para CO2 en ppm por m3 para un frigorífico con papas:
// CO2 normal: < 800 ppm. CO2 > 1000 ppm indica posible pudrición.
#define CO2_BAJO (800.0 * KGS_PAPAS * M3)   // Valor base ajustado por cantidad de papas y volumen.
#define CO2_ALTO (1400.0 * KGS_PAPAS * M3)  // Valor crítico ajustado por cantidad de papas y volumen.
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