#include "listviewdelegate.h"

ChatWidget::ChatWidget(QWidget* parent) : QWidget(parent) {
    listView = new QListView();
//    listView->setStyleSheet("border: none");
    chatModel = new QStandardItemModel();
    textEdit = new QTextEdit();
//    textEdit->setStyleSheet("border: none");
//    textEdit->setMaximumSize(200,450);
    pushButton = new QPushButton(tr("Send"));
    pushButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(pushButton, &QPushButton::clicked, this, &ChatWidget::send);

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
//    layout->addWidget(separator);
    layout->addWidget(textEdit);

    auto buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(pushButton);
    layout->addLayout(buttonLayout);

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
