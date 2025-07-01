const CO2_ACTUAL = 5.3;
const CO2_ANTERIOR = 0;
const KG_PAPA_SANA = 5_000;
const KG_PAPA_PODRIDA = 0;
const M3 = 8;
const LOOP_MS = 1_000;

const FACTOR_ESCALA = 1;

const CO2_PPM_KG_H_PAPA_SANA = 11 * FACTOR_ESCALA;
const CO2_PPM_KG_H_PAPA_PODRIDA = 65 * FACTOR_ESCALA;

const CO2_PAPA_SANA_LOOP = CO2_PPM_KG_H_PAPA_SANA / 3600 / 1000 * LOOP_MS;
const CO2_PAPA_PODRIDA_LOOP = CO2_PPM_KG_H_PAPA_PODRIDA / 3600 / 1000 * LOOP_MS;

const CO2_ESPERADO = CO2_ANTERIOR + CO2_PAPA_SANA_LOOP * KG_PAPA_SANA;
const CO2_EXTRA = CO2_ACTUAL - CO2_ESPERADO;

let kgPodridosNuevos = (CO2_EXTRA > 0) ? Math.min(CO2_EXTRA / CO2_PAPA_PODRIDA_LOOP, KG_PAPA_SANA) : 0;

console.log(`CO2 esperado: ${CO2_ESPERADO.toFixed(4)}`);
console.log(`CO2 actual: ${CO2_ACTUAL.toFixed(4)}`);
console.log(`CO2 extra: ${CO2_EXTRA.toFixed(4)}`);
console.log(`Kg papas podridas nuevas: ${kgPodridosNuevos.toFixed(4)}`);
console.log(`Kg papas sanas nuevas: ${(KG_PAPA_SANA - kgPodridosNuevos).toFixed(4)}`);
console.log(`Kg papas podridas nuevas totales: ${(KG_PAPA_PODRIDA + kgPodridosNuevos).toFixed(4)}`);