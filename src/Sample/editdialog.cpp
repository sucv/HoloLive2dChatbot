#include "QtLive2dWidget/src/LAppDefine.hpp"
#include "editdialog.h"
#include <Type/CubismBasicType.hpp>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTextDocumentWriter>
#include <QGridLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QSettings>
#include "mainwindow.h"

using namespace LAppDefine;

EditDialog::EditDialog(MainWindow *mainWindow, QWidget *parent)
    : QDialog(parent), m_mainWindow(mainWindow)
{
    modelComboBox = new QComboBox(this);
    connect(modelComboBox, &QComboBox::activated, m_mainWindow, &MainWindow::loadModel);

    promptComboBox = new QComboBox(modelComboBox);
    connect(modelComboBox, SIGNAL(currentIndexChanged(int)), m_mainWindow, SLOT(updateModelIndex(int)));
    connect(modelComboBox, SIGNAL(currentIndexChanged(int)), promptComboBox, SLOT(clear()));
    connect(modelComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(populateComboBox()));

    connect(promptComboBox, &QComboBox::activated, this, &EditDialog::loadPrompts);

    languageComboBox = new QComboBox(this);

    connect(languageComboBox, &QComboBox::currentTextChanged, m_mainWindow, &MainWindow::updateVoiceLanguage);
    initAzureSetting();

    modelLabel = new QLabel(tr("&Model"));
    modelLabel->setBuddy(modelComboBox);

    promptLabel = new QLabel(tr("&Prompt"));
    promptLabel->setBuddy(promptComboBox);

    languageLabel = new QLabel(tr("Voice Language"));
    languageLabel->setBuddy(languageComboBox);

    promptGroupBox = new QGroupBox(tr("Prompt Editing"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, [&]() {
        if (promptTextEdit->document()->isModified()) { // If there are changes in the text

            QMessageBox::StandardButton reply = QMessageBox::question(this, "Save changes?", "Do you want to save the changes to a new text file?",
                                                                      QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) { // If the user wants to save the changes
                // Save the text to a new file
                QString modelPath = m_mainWindow->getModelPath();
                QString fileName = QFileDialog::getSaveFileName(this, "Save Text File",  modelPath, "Text files (*.txt)");
                if (!fileName.isEmpty()) { // If the user has selected a valid file name
                    QFile file(fileName);
                    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                        QTextStream stream(&file);
                        stream << promptTextEdit->toPlainText();
                        file.close();
                    }
                }
            }
        }

        emit sendPrompt(promptTextEdit->toPlainText());
        promptComboBox->clear();
        populateComboBox();

        this->accept(); // Accept the dialog
    });
    connect(buttonBox, &QDialogButtonBox::rejected, this, [&](){
        loadPrompts(0);
        this->reject();
    });

    promptTextEdit = new QTextEdit(this);
    QVBoxLayout *promptLayout = new QVBoxLayout(promptGroupBox);
    promptLayout->addWidget(promptTextEdit);

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(modelLabel, 0, 0);
    mainLayout->addWidget(modelComboBox, 0, 1);
    mainLayout->addWidget(promptLabel, 1, 0);
    mainLayout->addWidget(promptComboBox, 1, 1);
    mainLayout->addWidget(languageLabel, 2, 0);
    mainLayout->addWidget(languageComboBox, 2, 1);
    mainLayout->addWidget(promptGroupBox, 3, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 4, 0, 1, 2);

    setWindowTitle(tr("Open Application Settings"));
    resize(650, 400);

    for (int i=0; i < ModelDirSize; ++i)
    {
        QString modelName = ModelDir[i];
        //        modelComboBox->setCurrentIndex(i);
        modelComboBox->addItem(modelName);
    }
    modelComboBox->setCurrentIndex(0);
}

void EditDialog::initAzureSetting()
{
    QStringList defaultVoices = {
        "en-US-JennyNeural",
        "zh-CN-XiaoyiNeural",
        "zh-TW-HsiaoYuNeural",
        "es-ES-AbrilNeural",
        "ja-JP-AoiNeural",
        "fr-FR-EloiseNeural",
        "de-DE-AmalaNeural",
        "ru-RU-SvetlanaNeural",
        "pt-BR-LeticiaNeural",
        "it-IT-PierinaNeural",
        "ko-KR-SoonBokNeural"
        // Add more languages and voices as needed
    };

    QSettings settings("./languages.ini", QSettings::IniFormat);

    // Iterate through the default voices and check if there is a stored voice for each
    for (int i = 0; i < defaultVoices.size(); ++i) {
        QString voice = defaultVoices.at(i);
        QString groupId = QString::number(i + 1); // Generate a unique ID for the group

        if (!settings.contains(groupId)) {
            // Voice not stored in the INI file, so create a new entry
            settings.beginGroup(groupId);
            settings.setValue("voice", voice);
            settings.endGroup();
        }
    }

    // Read all the values from the INI file
    QStringList savedVoices = settings.childGroups();

    // Convert the group IDs to integers and sort them
    QVector<int> sortedGroupIds;
    for (const QString& groupId : savedVoices) {
        int id = groupId.toInt();
        sortedGroupIds.append(id);
    }
    std::sort(sortedGroupIds.begin(), sortedGroupIds.end());

    // Iterate through the sorted group IDs and retrieve the voice value for each
    for (int id : sortedGroupIds) {
        QString groupId = QString::number(id);
        settings.beginGroup(groupId);
        QString voice = settings.value("voice").toString();
        // Use the retrieved voice value as needed
        languageComboBox->addItem(voice);
        settings.endGroup();
    }
}

void EditDialog::populateComboBox()
{
//    index = modelIndex;
    QString modelPath = m_mainWindow->getModelPath();
    int modelIndex = m_mainWindow->getModelIndex();

    QDir directory(modelPath);
    QStringList txtFiles = directory.entryList(QStringList() << "*.txt", QDir::Files, QDir::Time);

    initPrompts(modelIndex);
    txtFiles = directory.entryList(QStringList() << "*.txt", QDir::Files, QDir::Time);


    for (int i = 0; i < txtFiles.size(); ++i)
    {
        promptComboBox->addItem(txtFiles[i]);
    }
    loadPrompts(0);
}

QString EditDialog::parsePrompt(QString txtFileName)
{
    QFile txt(txtFileName);
    QString text = "";
    if (txt.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QTextStream stream(&txt);
        text = stream.readAll();
    }
    txt.close();

    return text;
}

void EditDialog::loadPrompts(int promptIndex)
{
    QString modelPath = m_mainWindow->getModelPath();
    QDir directory(modelPath);
    QStringList txtFiles = directory.entryList(QStringList() << "*.txt", QDir::Files, QDir::Time);
    QString promptFileName = txtFiles.at(promptIndex);
    QString text = parsePrompt(modelPath + promptFileName);

    promptTextEdit->clear();
    promptTextEdit->setText(text);
}

void EditDialog::initPrompts(int modelIndex)
{

    QJsonObject jsonObject, motions, expressions;

    QList<QString> expressionList, motionList, rules;


    QString model = ModelDir[modelIndex];

//    QString model = ModelDir[modelIndex];
    QString modelPath = ResourcesPath + model + "/";
    QString modelName = ModelDir[modelIndex];
    QString modelJsonFileName = modelName + ".model3.json";

    QString defaultPromptFileName = modelPath + "prompts_default.txt";

    QFile defaultPromptFile(defaultPromptFileName);

    QFile modelJsonFile(modelPath + modelJsonFileName);
    if (!modelJsonFile.open(QIODevice::ReadWrite)) {
        qWarning("Couldn't open file.");
        return;
    }

    QByteArray jsonData = modelJsonFile.readAll();
    QJsonDocument document = QJsonDocument::fromJson(jsonData);

    if (document.isNull()) {
        qWarning("Couldn't create JSON document.");
        return;
    }

    jsonObject = document.object();
    if (jsonObject.contains("FileReferences"))
    {
        jsonObject = jsonObject["FileReferences"].toObject();

        if (jsonObject.contains("Motions"))
        {
            QJsonObject motionInfo = jsonObject["Motions"].toObject();
            motionList = motionInfo.keys();
        }

        if (jsonObject.contains("Expressions"))
        {

            int j = 0;
            while (jsonObject["Expressions"][j].toObject().size())
            {
                QJsonObject expressionInfo = jsonObject["Expressions"][j].toObject();
                QString expression = expressionInfo.value("Name").toString();
                expressionList.append(expression);
                ++j;
            }
        }
    }

    QString str;
    rules.append(" ### Instruction ### \nYou are Albert Einstein and you know everything.  \n");

    str = "Your reply must be in JSON format with the following keys: Expression, Motion, Content \n";
    rules.append(str);

    if (!expressionList.isEmpty())
    {
        str = "In Expression, you must use one word as its value. The value can only be either one of the following words: ";
        QString keyString = expressionList.join(", ");
        str += keyString + ".\n";

    }
    else
    {
        str = "In Expression, you must always set it to ''. \n ";
    }
    rules.append(str);

    if (!motionList.isEmpty())
    {
        str = "In Motion, you must use one word as its value. The value can only be either one of the following words: ";
        QString keyString = motionList.join(", ");
        str += keyString + ".\n";
    }
    else
    {
        str = "In Motion, you must always set it to ''. \n ";
    }
    rules.append(str);

    str = "In Content, you must use Chinese.\n";
    rules.append(str);

    QString defaultPrompt = rules.join("\n");
    QTextDocument textDocument;
    QTextDocumentWriter writer(defaultPromptFileName);
    textDocument.setPlainText(defaultPrompt);
    writer.write(&textDocument);

}
