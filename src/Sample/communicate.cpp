#include <QJsonDocument>
#include <QThread>
#include <QMessageBox>
#include "OpenAI/openai.hpp"
#include "communicate.h"

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;

Mailman::Mailman(ChatWidget *chatWidget) : m_chatWidget(chatWidget) {
    // Save the chat history to Qt Standard model.
    connect(this, SIGNAL(updateChatModel(nlohmann::json,QString)), m_chatWidget, SLOT(update(nlohmann::json,QString)));
}


void Mailman::talk(QString prompts, QString azureSetting) {

    // Get the current prompts.
    static QRegularExpression fingNewLineChar("[\\n]");
    std::string cleanprompt = prompts.remove(fingNewLineChar).toStdString();

    // Format the prompts following OPENAI's standard.
    jsonBody = initJsonBody(cleanprompt);

    // Read the variables from System Setting.
    auto speechKey = GetEnvironmentVariable("SPEECH_KEY");
    auto speechRegion = GetEnvironmentVariable("SPEECH_REGION");

    // If the variable is not set, then quit the thread.
    if ((std::size(speechKey) == 0) || (std::size(speechRegion) == 0)) {
        std::cout << "Please set both SPEECH_KEY and SPEECH_REGION environment variables." << std::endl;
        QMessageBox msgBox;
        msgBox.setWindowTitle("Error");
        msgBox.setText("Cannot find SPEECH_KEY and SPEECH_REGION environment variables. Check your system environment variable setting!");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.addButton("OK", QMessageBox::AcceptRole);

        QThread::currentThread()->quit();
        QThread::currentThread()->wait();
    }
    auto speechConfig = SpeechConfig::FromSubscription(speechKey, speechRegion);

    // Get the preferred voice.
    QString language = azureSetting.split("-")[0] + "-" + azureSetting.split("-")[1];
    QString voice = azureSetting;
    speechConfig->SetSpeechRecognitionLanguage(language.toStdString());
    speechConfig->SetSpeechSynthesisVoiceName(voice.toStdString());

    // Initialize streaming...
    auto audioConfig = AudioConfig::FromDefaultMicrophoneInput();
    auto recognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);
    auto speechSynthesizer = SpeechSynthesizer::FromConfig(speechConfig);

    // Start streaming...
    while (!QThread::currentThread()->isInterruptionRequested())
    {
        auto voiceInput = recognizer->RecognizeOnceAsync().get();

        if (voiceInput->Reason == ResultReason::RecognizedSpeech)
        {

            std::string userInput = voiceInput->Text;

            nlohmann::json userInputJson;

            // The json contains Expression and Motion to animate the Live2D model and Content for conversation.
            userInputJson["Expression"] = "";
            userInputJson["Motion"] = "";
            userInputJson["Content"] = userInput;

            // Save the json to Qt StandardItemModel. So that the chat bubble can be filled in with words.
            emit updateChatModel(userInputJson, "Outgoing");

            // Format the user input following OpenAI's standard.
            nlohmann::json userMessage = getNewMessage("user", userInput);

            // Stitch it with the main json.
            jsonBody = insertMessage(jsonBody, userMessage);

            // Send POST request to OpenAI.
            openai::start();
            nlohmann::json completion = openai::chat().create(jsonBody);

            // Get the response from OpenAI. Always use the top-1 response.
            std::string gptResponse = completion["choices"][0]["message"]["content"];

            // Parse the returned data as if it is json. Sometime the data may not be json, so we manually make sure it is.
            nlohmann::json gptResponseJson = parseJsonString(gptResponse);
            expression = gptResponseJson.contains("Expression") ? gptResponseJson["Expression"] : "";
            motion = gptResponseJson.contains("Motion") ? gptResponseJson["Motion"] : "";
            content = gptResponseJson.contains("Content") ? gptResponseJson["Content"].get<std::string>() : gptResponse;

            nlohmann::json reconstructedJson;
            reconstructedJson["Expression"] = expression;
            reconstructedJson["Motion"] = motion;
            reconstructedJson["Content"] = content;

            // Send the expression and motion to animate the Live2D model.
            emit sendResponseMove(expression, motion);

            // Save the json to Qt StandardItemModel.
            emit updateChatModel(reconstructedJson, "Incoming");

            // Stitch the json with the main json.
            nlohmann::json gptMessage = getNewMessage("assistant", reconstructedJson.dump());
            jsonBody = insertMessage(jsonBody, gptMessage);

            // Voice the reply.
            auto chatGptVoiceOutput = speechSynthesizer->SpeakTextAsync(content).get();}
       }

    // Stop the thread.
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
    // This member is largely the same as talk(), but it is only a one-turn process and has no while loop.
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
    // Stitch the reply from ChatGPT so that it can track the context. Without stitching, the prompt will be forgotten in no time.
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

