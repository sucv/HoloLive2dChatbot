#include "listviewdelegate.h"

// Use this custom QTextEdit class to capture the Enter key and shift enter key.
// It improves the keyboard-based chat experience a lot.
ChatTextEdit::ChatTextEdit(QWidget *parent) : QTextEdit(parent){}

void ChatTextEdit::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        if (event->modifiers() == Qt::ShiftModifier)
        {
            // Shift+Enter pressed, handle custom behavior here
            // For example, add a new line without emitting the enterKeyPressed() signal
            QTextEdit::keyPressEvent(event); // Let the base class handle the key event
        }
        else
        {
            // Enter pressed, emit the enterKeyPressed() signal
            emit enterKeyPressed();
            setFocus();
        }
    }
    else
    {
        QTextEdit::keyPressEvent(event); // Let the base class handle the key event
    }
}


ChatWidget::ChatWidget(QWidget* parent) : QWidget(parent) {
    listView = new QListView();
    chatModel = new QStandardItemModel();
    textEdit = new ChatTextEdit();
//    connect(textEdit, &ChatTextEdit::enterKeyPressed, pushButton, &QPushButton::click);
    connect(textEdit, &ChatTextEdit::enterKeyPressed, this, &ChatWidget::send);

    listView->setModel(chatModel);

    // create a view and set our data
    listView->setResizeMode(QListView::Adjust);
    listView->setWordWrap(true);
    listView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    listView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listView->setModel(chatModel);

    listView->setMinimumSize(200,450);

    // NOW tell the view to rely on out custom delegate for drawing
    listView->setItemDelegate(new ListViewDelegate());

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(listView);
    layout->addWidget(textEdit);
    setLayout(layout);
    connect(chatModel, &QStandardItemModel::rowsInserted, listView, &QListView::scrollToBottom);
}

 nlohmann::json ChatWidget::itemToJsonArray()
{
    nlohmann::json chatArray = nlohmann::json::array();

    // iterate over the model's items
    for(int i = 0; i < chatModel->rowCount(); ++i) {
        QStandardItem *item = chatModel->item(i);

        // create a JSON object for the chat messages
        nlohmann::json chatObject;

        chatObject["content"] = item->text().toStdString();
        std::string role = item->data(Qt::UserRole + 1).toString().toStdString();
        if(role == "Incoming") {
            chatObject["role"] = "assistant";
        } else if(role == "Outgoing") {
            chatObject["role"] = "user";
        }

        // add the chat message object to the chat array
        chatArray.push_back(chatObject);
    }
    return chatArray;
}

void ChatWidget::send()
{
    QString userInput = textEdit->toPlainText();
    if (userInput != "")
    {
        emit sendUserInput(userInput);
        textEdit->setFocus();
    }
}

void ChatWidget::update(nlohmann::json input, QString role)
{
    QString inputString = QString::fromStdString(input.dump());
    QStandardItem *item = new QStandardItem(inputString);
    item->setData(role, Qt::UserRole + 1);
    chatModel->appendRow(item);
    QString text = textEdit->toPlainText();
    if (!text.isEmpty()) textEdit->clear();
}
