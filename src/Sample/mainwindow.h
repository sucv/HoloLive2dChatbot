#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QThread>
#include <QActionGroup>
#include "./QtLive2dWidget/src/QLive2dWidget.hpp"
#include "listviewdelegate.h"
#include "hologram.hpp"
#include "editdialog.h"
#include <communicate.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QLive2dWidget *qLive2dWidget;
    QLabel *holoWidget;
    ChatWidget *chatWidget;
    QWidget *toolbarPlaceholder;

    int getModelIndex() const { return index; };

    QString getModelPath();


    void loadModel(int);
    QString currentPrompt;

    QString currentVoiceLanguage;


//    virtual void enterEvent(QEnterEvent * event) override;
//    virtual void leaveEvent(QEvent * event) override;

public slots:
    void chat(QString userInput);
    void updateModelIndex(int);
    void updateVoiceLanguage(QString);
    void getPrompt(QString);


private:

    QSize screenSize;
    QSize modelSize;


//    QToolBar *toolBar;

    QMenu *contextMenu;
    QAction *settingAction, *holoModeAction, *chatModeAction, *widgetModeAction, *talkAction, *closeAction, *restartAction, *helpAction;
    QActionGroup *viewActionGroup;
    QComboBox *comboModelList, *comboPromptList;
    EditDialog *editDialog = nullptr;

    const QString rsrcPath = "Resources";
    void createActions();
    void createContextActions();


    HoloGram holoGram;
    void openSettings();

    int index = 0;

    void keyPressEvent(QKeyEvent *event) override;

    Mailman *mailman;
    QThread *talkThread;
    void toggleTalking();
    bool isTalking = false;


private slots:
    void HoloDisplay();
    void restart();

signals:
    void sendCurrentPromptsAndVoiceLanguage(QString, QString);
    void sendPromptsAndChatInput(QString, QString);
    void stopChatting();
};
#endif // MAINWINDOW_H
