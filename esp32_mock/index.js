const mqtt = require('mqtt');
const brokerUrl = 'mqtt://localhost:1883';
const client = mqtt.connect(brokerUrl);

/** Cantidad promedio de PPM de CO₂ que genera 1KG de papa sana en 1 hora en una sala hermética de 1M3. */
const CO2_PPM_KG_H_PAPA_SANA = (11 / 3_600) * 10_000; // Real: 11PPM/1H = 11PPM/3600S --> Aumentado x10.000.
/** Cantidad promedio de PPM de CO₂ que genera 1KG de papa podrida en 1 hora en una sala hermética de 1M3. */
const CO2_PPM_KG_H_PAPA_PODRIDA = (65 / 3_600) * 10_000; // Real: 65PPM/1H = 65PPM/3600S --> Aumentado x10.000.
/** Porcentaje de papa sana que se pudre en 1 segundo. */
const KG_PAPA_PODRIDOS_POR_S = (100 / 720 / 3_600) * 2; // Real: 100%/720H/3600S (720H en pudrirse 100%) --> Aumentado x2
/** Cantidad de PPM de CO₂ ventilados en 1 segundo. */
const PPM_VENTILADOS_S = 150;
/** KG de papa sana inicial en la cámara. */
const KG_PAPAS_INICIAL = 5_000;
/** Dimensión en M3 de la cámara de papas. */
const M3 = 8;

let ventilacionPrendida = false;

function startMock() {
    const LOOP_MS = 1_000;
    const CO2_PAPA_SANA_LOOP = CO2_PPM_KG_H_PAPA_SANA / M3 / 3_600 / 1_000 * LOOP_MS;
    const CO2_PAPA_PODRIDA_LOOP = CO2_PPM_KG_H_PAPA_PODRIDA / M3 / 3_600 / 1_000 * LOOP_MS;
    const PPM_VENTILADOS_LOOP = PPM_VENTILADOS_S / 1_000 * LOOP_MS;

    let kgPapasSanas = KG_PAPAS_INICIAL;
    let kgPapasPodridas = 0;
    let co2 = 0;
    let loopCount = 0;

    setInterval(() => {
        console.log(`--- Loop: ${loopCount++} ---`)
        console.log(`\tPapas Sanas/Podridas (KG): ${kgPapasSanas.toFixed(2)}/${kgPapasPodridas.toFixed(2)}`);
        console.log(`\tVentilación: ${ventilacionPrendida && "ON" || "OFF"}`);
        const co2_to_publish = parseFloat(co2.toFixed(2));
        console.log(`\tCO2: ${co2_to_publish}`);
        console.log('---')
        client.publish("camara/01/co2", JSON.stringify({
            co2: co2_to_publish
        }), {qos: 0}, (err) => {
            if (err) {
                console.error(`Error al publicar CO2:`, err);
            } else {
                console.log(`Publicado CO2: ${co2_to_publish}`);
            }
        });

        co2 = Math.max(0, co2
            + (CO2_PAPA_SANA_LOOP * kgPapasSanas)
            + (CO2_PAPA_PODRIDA_LOOP * kgPapasPodridas)
            - (ventilacionPrendida ? PPM_VENTILADOS_LOOP : 0));
        const KG_PAPAS_PODRIDAS_EN_LOOP = KG_PAPAS_INICIAL * (Math.random() * KG_PAPA_PODRIDOS_POR_S / 1_000 * LOOP_MS);
        kgPapasSanas = Math.max(kgPapasSanas - KG_PAPAS_PODRIDAS_EN_LOOP, 0);
        kgPapasPodridas = Math.min(kgPapasPodridas + KG_PAPAS_PODRIDAS_EN_LOOP, KG_PAPAS_INICIAL);
    }, LOOP_MS);
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
        ventilacionPrendida = (message.toString() === "prender");
        if (ventilacionPrendida) {
            console.log('Ventilación PRENDIDA.');
        } else {
            console.log('Ventilación APAGADA.');
        }
    }
});
