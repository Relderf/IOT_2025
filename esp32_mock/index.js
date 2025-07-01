const mqtt = require('mqtt');
const brokerUrl = 'mqtt://localhost:1883';
const client = mqtt.connect(brokerUrl);
const CO2_PPM_KG_S = 10 / 3_600;
const KG_PAPAS_INICIAL = 5_000;
const M3 = 8;

let kgPapasSanas = KG_PAPAS_INICIAL;
let kgPapasPodridas = 0;
let ventilacionPrendida = false;
let co2 = 0;

function startMock() {
    setInterval(() => {
        console.log(`Papas Sanas/Podridas (KG): ${kgPapasSanas.toFixed(2)}/${kgPapasPodridas.toFixed(2)}`);
        console.log(`Ventilación: ${ventilacionPrendida && "ON" || "OFF"}`);
        console.log(`CO2: ${co2.toFixed(2)}`);
        const payload = JSON.stringify({
            co2: co2.toFixed(2)
        });
        client.publish("camara/01/co2", payload, {qos: 0}, (err) => {
            if (err) {
                console.error(`Error al publicar CO2:`, err);
            } else {
                console.log(`Publicado CO2: ${co2.toFixed(2)}`);
            }
        });

        co2 = Math.max(co2
            + ((CO2_PPM_KG_S * kgPapasSanas) + (CO2_PPM_KG_S * 2 * kgPapasPodridas)) / M3
            - (ventilacionPrendida ? 150 : 0), 0);
        const KG_PAPAS_PODRIDAS_EN_LOOP = KG_PAPAS_INICIAL * (Math.random() * 0.02);
        kgPapasSanas = Math.max(kgPapasSanas - KG_PAPAS_PODRIDAS_EN_LOOP, 0);
        kgPapasPodridas = Math.min(kgPapasPodridas + KG_PAPAS_PODRIDAS_EN_LOOP, KG_PAPAS_INICIAL);
    }, 1_000);
}

client.on('connect', () => {
    console.log('Conectado al broker MQTT.');
    client.subscribe('camara/01/#', (err) => {
        if (err) {
            console.error('Error al suscribirse:', err);
        } else {
            console.log('Subscripción MQTT correcta.');
        }
    });
    startMock();
});

client.on('error', (err) => {
    console.error('Error en la conexión MQTT:', err);
});

client.on('message', (topic, message) => {
    if (topic === 'camara/01/ventilacion') {
        ventilacionPrendida = (message === "prender");
        if (ventilacionPrendida) {
            console.log('Ventilación PRENDIDA.');
        } else {
            console.log('Ventilación APAGADA.');
        }
    }
});
