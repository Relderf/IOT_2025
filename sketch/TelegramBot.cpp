#include "TelegramBot.h"
#include "WifiConn.h"

TelegramBot::TelegramBot(String telegramBotToken, WiFiClientSecure& clienteWifi,
                         int intervalo, std::function<bool()> isWifiConnected)
        : bot(telegramBotToken, clienteWifi), isWifiConnected(isWifiConnected) {
    clienteWifi.setCACert(TELEGRAM_CERTIFICATE_ROOT);
    ultimoChequeoMensajesMs = 0;
    intervaloChequeoMensajesMs = intervalo;
}

void TelegramBot::setChatIdsValidos(const std::vector<String>& chatIds) {
    chatIdsValidos = chatIds;
}

bool TelegramBot::esChatValido(String chatId) {
    for (const String& idPermitido : chatIdsValidos) {
        if (idPermitido == chatId) {
            return true;
        }
    }
    return false;
}

void TelegramBot::registerCommand(const String& command, std::function<void(String)> handler) {
    commandHandlers[command] = handler;
}

void TelegramBot::sendMessage(String chatId, String message) {
    if (!isWifiConnected()) {
        Serial.println("No hay conexión WiFi. Cancelando envío.");
        return;
    }
    Serial.print("Enviando mensaje a ");
    Serial.print(chatId);
    Serial.print(": ");
    Serial.println(message);
    bool ok = bot.sendMessage(chatId, message, "");
    Serial.print("Resultado Telegram: ");
    Serial.println(ok ? "Éxito" : "FALLO");
}

void TelegramBot::checkMensajes() {
    if (millis() > ultimoChequeoMensajesMs + intervaloChequeoMensajesMs)  {
        Serial.println("TelegramBot: verificando mensajes...");
        int cantNuevosMensajes = bot.getUpdates(bot.last_message_received + 1);
        Serial.print("TelegramBot: Mensajes recibidos. Cantidad: ");
        Serial.print(cantNuevosMensajes);
        Serial.println(".");
        recibirMensajes(cantNuevosMensajes);
        ultimoChequeoMensajesMs = millis();
    }
}

void TelegramBot::recibirMensajes(int cantNuevosMensajes) {
    Serial.print("TelegramBot: Recibiendo mensajes. Cantidad: ");
    Serial.print(cantNuevosMensajes);
    Serial.println(".");

    for (int i=0; i<cantNuevosMensajes; i++) {
        Serial.print("TelegramBot: Mensaje " + String(i) + ". Chat ID: ");
        String chatId = String(bot.messages[i].chat_id);
        Serial.print(chatId);
        Serial.println(".");

        if (!esChatValido(chatId)) {
            Serial.println("TelegramBot: Mensaje " + String(i) + " rechazado.");
            bot.sendMessage(chatId, "Acceso denegado. Usuario no autorizado.", "");
            continue;
        }

        String texto = bot.messages[i].text;
        Serial.println("Mensaje " + String(i) + ": '" + texto + "'.");

        auto it = commandHandlers.find(texto);
        if (it != commandHandlers.end()) {
            it->second(chatId);
        } else {
            Serial.println("TelegramBot: Mensaje " + String(i) + " ignorado.");
            sendMessage(chatId, "Comando no encontrado.");
        }
    }
}