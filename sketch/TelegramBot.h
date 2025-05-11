#ifndef TELEGRAMBOT_H
#define TELEGRAMBOT_H

#include <map>
#include <functional>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>

class TelegramBot {
private:
    UniversalTelegramBot bot;
    unsigned long ultimoChequeoMensajesMs;
    unsigned long intervaloChequeoMensajesMs;
    std::vector<String> chatIdsValidos;
    std::map<String, std::function<void(String)>> commandHandlers;

    void recibirMensajes(int cantNuevosMensajes);
    bool esChatValido(String chatId);

public:
    TelegramBot(String telegramBotToken, WiFiClientSecure& clienteWifi, int intervaloChequeoMensajesMs);
    void setChatIdsValidos(const std::vector<String>& chatIds);
    void registerCommand(const String& command, std::function<void(String)> handler);
    void checkMensajes();
    void sendMessage(String chatId, String message);
};

#endif //TELEGRAMBOT_H
