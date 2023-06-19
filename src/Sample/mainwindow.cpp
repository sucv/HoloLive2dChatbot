#include "mainwindow.h"
#include "QtLive2dWidget/src/QLive2dWidget.hpp"
#include "QtLive2dWidget/src/LAppLive2DManager.hpp"
#include "QtLive2dWidget/src/LAppDefine.hpp"
#include <QLabel>

#include <QFile>
#include <QMessageBox>
#include <QApplication>
#include <QScreen>
#include <QMenuBar>
#include <QToolBar>
#include <QStandardItemModel>
#include <QListView>
#include "listviewdelegate.h"
#include "editdialog.h"

using namespace LAppDefine;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    setWindowTitle(QCoreApplication::applicationName());

    //    Set the size for the model.
    screenSize = qApp->primaryScreen()->size();
    modelSize = QSize(screenSize.height() / 2.15, screenSize.height() / 2.15);
    resize(modelSize);
    setMinimumSize(modelSize / 2);

    //    Create the three widgets, the qLive2dWidget, holoWidget, and chatWidget.
    //    And also the layout.
    centralwidget = new QWidget(this);
    centralwidget->setObjectName("centralwidget");

    horizontalLayout = new QHBoxLayout(centralwidget);
    horizontalLayout->setObjectName("horizontalLayout");
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    qLive2dWidget = new QLive2dWidget(this);
    qLive2dWidget->setObjectName("qLive2dWidget");

    holoWidget = new QLabel();
    holoWidget->setObjectName("holoWidget");

    holoWidget->hide();

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());

    qLive2dWidget->setSizePolicy(sizePolicy);
    holoWidget->setSizePolicy(sizePolicy);

    horizontalLayout->addWidget(qLive2dWidget);
    horizontalLayout->addWidget(holoWidget);

    chatWidget = new ChatWidget(this);

    horizontalLayout->addWidget(chatWidget);
    chatWidget->hide();
    setCentralWidget(centralwidget);

    //    Create a new thread, and put the communication with Azure/OpenAI API to it.
    mailman = new Mailman(chatWidget);
    talkThread = new QThread(this);

    // Move worker to thread.
    mailman->moveToThread(talkThread);

    connect(this, &MainWindow::sendCurrentPromptsAndVoiceLanguage, mailman, &Mailman::talk);
    connect(chatWidget, &ChatWidget::sendUserInput, this, &MainWindow::chat);
    connect(this, &MainWindow::sendPromptsAndChatInput, mailman, &Mailman::chat);
    connect(mailman, &Mailman::sendResponseMove, qLive2dWidget, &QLive2dWidget::react);

    //    Create all the actions.
    createActions();
    createContextActions();


    // Initialize the default prompt settings
    editDialog = new EditDialog(this);
    connect(editDialog, &EditDialog::sendPrompt, this, &MainWindow::getPrompt);
    editDialog->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    //    Transparent the window
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::WindowStaysOnTopHint);
    setWindowFlag(Qt::FramelessWindowHint);

}

MainWindow::~MainWindow()
{
    talkThread->terminate();
}


void MainWindow::updateVoiceLanguage(QString locale)
{
    currentVoiceLanguage = locale;
}

void MainWindow::toggleTalking()
{
    if (!isTalking)
    {
        talkAction->setChecked(true);
        isTalking = true;
        settingAction->setDisabled(true);
        talkThread->start();
        emit sendCurrentPromptsAndVoiceLanguage(currentPrompt, currentVoiceLanguage);

    }
    else{
        talkAction->setChecked(false);
        isTalking = false;
        talkThread->requestInterruption();
        settingAction->setEnabled(true);
    }

}

void MainWindow::getPrompt(QString prompt)
{
    currentPrompt = prompt;
}

void MainWindow::chat(QString userInput)
{
    talkThread->start();
    emit sendPromptsAndChatInput(currentPrompt, userInput);
}


QString MainWindow::getModelPath()
{
    QString model = ModelDir[index];
    QString modelPath = ResourcesPath + model + "/";
    return modelPath;
}

void MainWindow::loadModel(int modelIndex)
{
    LAppLive2DManager::GetInstance()->ChangeScene(modelIndex);
    index = modelIndex;
}

void MainWindow::updateModelIndex(int modelIndex)
{
    index = modelIndex;
}


void MainWindow::openSettings()
{
    if (!editDialog)
        editDialog = new EditDialog(this);

    if (editDialog->exec() != QDialog::Accepted)
        return;
}


void MainWindow::createActions()
{
//    toolBar = addToolBar(tr("Actions"));

    closeAction = new QAction(QIcon(rsrcPath + "/switch.png"),  tr("&Exit"), this);
    connect(closeAction, &QAction::triggered, this, &MainWindow::close);
//    toolBar->addAction(closeAction);


    restartAction = new QAction(QIcon(rsrcPath + "/restart.png"),  tr("&Clear Histroy"), this);
    connect(restartAction, &QAction::triggered, this, &MainWindow::restart);
//    toolBar->addAction(restartAction);

//    toolBar->addSeparator();


    viewActionGroup = new QActionGroup(this);
    viewActionGroup->setExclusive(true);


    widgetModeAction = new QAction(QIcon(rsrcPath + "/minimize.png"),  tr("&Widget Mode"), this);
    connect(widgetModeAction, &QAction::triggered, this, [&](){
        resize(modelSize);

        holoWidget->hide();
        qLive2dWidget->show();
        chatWidget->hide();
        move(screenSize.width() - width(), screenSize.height() - height() - height() * 0.05);

    });

    widgetModeAction->setCheckable(true);
    widgetModeAction->setChecked(true);
//    toolBar->addAction(widgetModeAction);

    chatModeAction = new QAction(QIcon(rsrcPath + "/chat.png"), tr("&Chat Mode"), this);
    connect(chatModeAction, &QAction::triggered, this, [&](){

        resize(modelSize);
        holoWidget->hide();
        qLive2dWidget->show();
        chatWidget->show();
    });


    chatModeAction->setCheckable(true);
//    toolBar->addAction(chatModeAction);


    holoModeAction = new QAction(QIcon(rsrcPath + "/hologram.png"),  tr("&Holo Mode"), this);
    connect(holoModeAction, &QAction::triggered, this, [&](){
        HoloDisplay();
    });

    holoModeAction->setCheckable(true);
//    toolBar->addAction(holoModeAction);

    viewActionGroup->addAction(widgetModeAction);
    viewActionGroup->addAction(chatModeAction);
    viewActionGroup->addAction(holoModeAction);


//    toolBar->addSeparator();

    talkAction = new QAction(QIcon(rsrcPath + "/microphone.png"),  tr("&Talk"), this);
    connect(talkAction, &QAction::triggered, this, &MainWindow::toggleTalking);
    talkAction->setCheckable(true);
//    toolBar->addAction(talkAction);

//    toolBar->addSeparator();

    settingAction = new QAction(QIcon(rsrcPath + "/gear.png"), tr("Settings"), this);
    connect(settingAction, &QAction::triggered, this, [&](){
        openSettings();
    });

//    toolBar->addAction(settingAction);
    helpAction = new QAction(QIcon(rsrcPath + "/help.png"), tr("Help"), this);
    connect(helpAction, &QAction::triggered, this, [&](){
        // Create the message box
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Control");
        msgBox.setTextFormat(Qt::RichText);
//        msgBox.setText("This is a rush instruction on key bindings.");

        QString htmlContent;
        QFile file(rsrcPath + "/help.html");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            htmlContent = file.readAll();
            file.close();
        }
        else
        {
            htmlContent = "<table style=\"margin-left:auto; margin-right:auto; text-align:center;\" border=\"1\">"
                                  "<tr>"
                                  "<th>Action</th>"
                                  "<th>Key</th>"
                                  "</tr>"
                                  "<tr>"
                                  "<td>Interaction</td>"
                                  "<td>Mouse Left</td>"
                                  "</tr>"
                                  "<tr>"
                                  "<td>Move</td>"
                                  "<td>Mouse Middle</td>"
                                  "</tr>"
                                  "<tr>"
                                  "<td>Menu</td>"
                                  "<td>Mouse Right</td>"
                                  "</tr>"
                                  "<tr>"
                                  "<td>Resize</td>"
                                  "<td>Mouse Wheel</td>"
                                  "</tr>"
                                  "<tr>"
                                  "<td>Exit Holo Mode</td>"
                                  "<td>ESC</td>"
                                  "</tr>"
                                  "</table>";
        }


        // Set the informative text with a paragraph
        msgBox.setInformativeText(htmlContent);
        // Show the message box
        msgBox.exec();
    });

//    toolBar->addAction(helpAction);
//    toolBar->addSeparator();

//    toolBar->hide();
//    toolBar->setStyleSheet("QToolBar { border-top: 0px; border-bottom: 0px; }");

}

void MainWindow::createContextActions()
{
    contextMenu = new QMenu(this);
    contextMenu->addAction(closeAction);
    contextMenu->addAction(restartAction);
    contextMenu->addSeparator();
    contextMenu->addAction(widgetModeAction);
    contextMenu->addAction(chatModeAction);
    contextMenu->addAction(holoModeAction);
    contextMenu->addSeparator();
    contextMenu->addAction(talkAction);
    contextMenu->addAction(settingAction);
    contextMenu->addAction(helpAction);


    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, [=](const QPoint &pos) {
        // Show the context menu at the requested position
        contextMenu->exec(mapToGlobal(pos));
    });
}

void MainWindow::restart()
{
    chatWidget->chatModel->clear();
}

void MainWindow::HoloDisplay()
{

    hide();
    setAttribute(Qt::WA_TranslucentBackground, 0);
    show();

//    toolBar->hide();

    qLive2dWidget->resize(modelSize);
    qLive2dWidget->hide();
    chatWidget->hide();

    holoWidget->show();


    QRect rect = QGuiApplication::primaryScreen()->geometry();
    holoWidget->setGeometry(rect);
    setGeometry(rect);

    while (holoWidget->isVisible()) {

        // Capture the frame and transform it
        QImage frame = qLive2dWidget->grabFramebuffer();
        cv::Mat mat(frame.height(), frame.width(), CV_8UC4, frame.bits(), frame.bytesPerLine());
        cv::flip(mat, mat, 0);
        cv::Mat holoMat = HoloGram::holoTransform(mat, 1, 2.2, 0, screenSize);
        QImage image(holoMat.data, holoMat.cols, holoMat.rows, holoMat.step, QImage::Format_RGB32);
        QPixmap pixmap = QPixmap::fromImage(image);

        holoWidget->setPixmap(pixmap);
        qApp->processEvents(); // Process events to update the display

    }

    widgetModeAction->setChecked(true);
    hide();
    setAttribute(Qt::WA_TranslucentBackground, 1);
    show();
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape and holoWidget->isVisible()) {
        resize(modelSize);

        holoWidget->hide();
        qLive2dWidget->show();
        chatWidget->hide();
        move(screenSize.width() - width(), screenSize.height() - height());
    }
    else
    {
        // handle other key press events
        QWidget::keyPressEvent(event); // let the base class handle the event
    }
}


//void MainWindow::enterEvent(QEnterEvent* event)
//{
//    // Show the toolbar when the mouse enters the window

//    if (!holoWidget->isVisible())
//    {
////        toolBar->show();
//    }
//    QMainWindow::enterEvent(event);
//}

//void MainWindow::leaveEvent(QEvent* event)
//{
//    // Hide the toolbar when the mouse leaves the window
////    toolBar->hide();
//    QMainWindow::leaveEvent(event);
//}
