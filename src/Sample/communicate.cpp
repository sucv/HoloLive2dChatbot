#include <QJsonDocument>
#include <QThread>

#include "OpenAI/openai.hpp"
#include "communicate.h"

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;

Mailman::Mailman(ChatWidget *chatWidget) : m_chatWidget(chatWidget) {
    connect(this, SIGNAL(updateChatModel(nlohmann::json,QString)), m_chatWidget, SLOT(update(nlohmann::json,QString)));
}


void Mailman::talk(QString prompts, QString azureSetting) {
    static QRegularExpression fingNewLineChar("[\\n]");
    std::string cleanprompt = prompts.remove(fingNewLineChar).toStdString();

    jsonBody = initJsonBody(cleanprompt);

    auto speechKey = GetEnvironmentVariable("SPEECH_KEY");
    auto speechRegion = GetEnvironmentVariable("SPEECH_REGION");

    if ((std::size(speechKey) == 0) || (std::size(speechRegion) == 0)) {
        std::cout << "Please set both SPEECH_KEY and SPEECH_REGION environment variables." << std::endl;
    }

    auto speechConfig = SpeechConfig::FromSubscription(speechKey, speechRegion);

    QString language = azureSetting.split("-")[0] + "-" + azureSetting.split("-")[1];
    QString voice = azureSetting;
    speechConfig->SetSpeechRecognitionLanguage(language.toStdString());
    speechConfig->SetSpeechSynthesisVoiceName(voice.toStdString());


    auto audioConfig = AudioConfig::FromDefaultMicrophoneInput();
    auto recognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    auto speechSynthesizer = SpeechSynthesizer::FromConfig(speechConfig);


    while (!QThread::currentThread()->isInterruptionRequested())
    {
        auto voiceInput = recognizer->RecognizeOnceAsync().get();

        if (voiceInput->Reason == ResultReason::RecognizedSpeech)
        {

            std::string userInput = voiceInput->Text;

            nlohmann::json userInputJson;

            // Add key-value pairs to the JSON object
            userInputJson["Expression"] = "";
            userInputJson["Motion"] = "";
            userInputJson["Content"] = userInput;
            emit updateChatModel(userInputJson, "Outgoing");

            nlohmann::json userMessage = getNewMessage("user", userInput);
            jsonBody = insertMessage(jsonBody, userMessage);

            openai::start();
            nlohmann::json completion = openai::chat().create(jsonBody);

            std::string gptResponse = completion["choices"][0]["message"]["content"];
            nlohmann::json gptResponseJson = parseJsonString(gptResponse);

            expression = gptResponseJson.contains("Expression") ? gptResponseJson["Expression"] : "";
            motion = gptResponseJson.contains("Motion") ? gptResponseJson["Motion"] : "";
            content = gptResponseJson.contains("Content") ? gptResponseJson["Content"].get<std::string>() : gptResponse;

            nlohmann::json reconstructedJson;
            reconstructedJson["Expression"] = expression;
            reconstructedJson["Motion"] = motion;
            reconstructedJson["Content"] = content;

            emit sendResponseMove(expression, motion);
            emit updateChatModel(reconstructedJson, "Incoming");

            nlohmann::json gptMessage = getNewMessage("assistant", reconstructedJson.dump());
            jsonBody = insertMessage(jsonBody, gptMessage);

            auto chatGptVoiceOutput = speechSynthesizer->SpeakTextAsync(content).get();}
       }


    QThread::currentThread()->quit();
    QThread::currentThread()->wait();
}


nlohmann::json Mailman::getNewMessage(std::string role, std::string content)
{
    nlohmann::json message =
        {
            {"role", role},
            {"content", content}
        };
    return message;
}

void Mailman::chat(QString prompts, QString userInput) {
    static QRegularExpression fingNewLineChar("[\\n]");
    std::string cleanprompt = prompts.remove(fingNewLineChar).toStdString();

    jsonBody = initJsonBody(cleanprompt);

    nlohmann::json userInputJson;

    userInputJson["Expression"] = "";
    userInputJson["Motion"] = "";
    userInputJson["Content"] = userInput.toStdString();
    emit updateChatModel(userInputJson, "Outgoing");

    std::string cleanUserInput = userInput.remove(fingNewLineChar).toStdString();
    nlohmann::json userMessage = getNewMessage("user", cleanUserInput);
    jsonBody = insertMessage(jsonBody, userMessage);

    openai::start();
    nlohmann::json completion = openai::chat().create(jsonBody);

    std::string gptResponse = completion["choices"][0]["message"]["content"];
    nlohmann::json gptResponseJson = parseJsonString(gptResponse);

    expression = gptResponseJson.contains("Expression") ? gptResponseJson["Expression"] : "";
    motion = gptResponseJson.contains("Motion") ? gptResponseJson["Motion"] : "";
    content = gptResponseJson.contains("Content") ? gptResponseJson["Content"].get<std::string>() : gptResponse;

    nlohmann::json reconstructedJson;
    reconstructedJson["Expression"] = expression;
    reconstructedJson["Motion"] = motion;
    reconstructedJson["Content"] = content;

    emit sendResponseMove(expression, motion);
    emit updateChatModel(reconstructedJson, "Incoming");

    nlohmann::json gptMessage = getNewMessage("assistant", reconstructedJson.dump());
    jsonBody = insertMessage(jsonBody, gptMessage);

}


nlohmann::json Mailman::insertMessage(nlohmann::json json, nlohmann::json newJson)
{
    if (newJson.size())
    {
             if(newJson.is_array())
             {
                    json["messages"].insert(json["messages"].end(), newJson.begin(), newJson.end());
             }
             else{
                    json["messages"].push_back(newJson);
             }
    }
    return json;
}

nlohmann::json Mailman::initJsonBody(std::string prompts)
{

    nlohmann::json systemMessage = getNewMessage("system", prompts);

    nlohmann::json requestJson =
        {
            {"model", "gpt-3.5-turbo-0613"},
            {"max_tokens", 2000},
            {"temperature", 0.7},
            {"messages", nlohmann::json::array()},
    };

    requestJson["messages"][0] = systemMessage;
    nlohmann::json chatMessages = m_chatWidget->itemToJsonArray();
    requestJson = insertMessage(requestJson, chatMessages);
    return requestJson;
}


nlohmann::json Mailman::parseJsonString(const std::string& inputString) {
    try {
             nlohmann::json parsedJson = nlohmann::json::parse(inputString);
             return parsedJson;
    }
    catch (const nlohmann::json::parse_error&) {
             // If parsing fails, return an empty JSON object
             return nlohmann::json();
    }
}

std::string GetEnvironmentVariable(const char* name)
{
    #if defined(_MSC_VER)
        size_t requiredSize = 0;
        (void)getenv_s(&requiredSize, nullptr, 0, name);
        if (requiredSize == 0)
        {
            return "";
        }
        auto buffer = std::make_unique<char[]>(requiredSize);
        (void)getenv_s(&requiredSize, buffer.get(), requiredSize, name);
        return buffer.get();
    #else
        auto value = getenv(name);
        return value ? value : "";
    #endif
}

