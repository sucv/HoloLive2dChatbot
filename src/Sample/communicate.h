#ifndef COMMUNICATE_H
#define COMMUNICATE_H
#include <QApplication>
#include <QJsonObject>

#include <listviewdelegate.h>
#include <nlohmann/json.hpp>
#include <speechapi_cxx.h>


std::string GetEnvironmentVariable(const char* name);

class Mailman : public QObject {
    Q_OBJECT

public:
    Mailman(ChatWidget *chatWidget);
    QString currentPromptFileName;
    nlohmann::json jsonBody;


private:
    std::string expression, motion, content;
    ChatWidget *m_chatWidget = nullptr;
    nlohmann::json getNewMessage(std::string, std::string);
    nlohmann::json initJsonBody(std::string);
    nlohmann::json insertMessage(nlohmann::json, nlohmann::json);
    bool isChatting = false;

public slots:
    void talk(QString, QString);
    void chat(QString, QString);


signals:
//    void finished();
    void sendResponseMove(std::string, std::string);
    void sendMessage(std::string);
    void updateChatModel(nlohmann::json, QString);
};
#endif // COMMUNICATE_H

