
#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QSettings>

class MainWindow;

class EditDialog : public QDialog
{
    Q_OBJECT

public:
   explicit  EditDialog(MainWindow *mainWindow, QWidget *parent = nullptr);

    QLabel *modelLabel, *promptLabel, *languageLabel;
    QComboBox *modelComboBox, *promptComboBox, *languageComboBox;

    QGroupBox *promptGroupBox;
    QTextEdit *promptTextEdit;
    QDialogButtonBox *buttonBox;


    void initPrompts(int);
    QString parsePrompt(QString);
    void loadPrompts(int);
    void saveNewPrompts();

private:
    MainWindow *m_mainWindow;
    int promptIndex = 0;
    void initAzureSetting();

signals:
    QString sendPrompt(const QString);

public slots:
    void populateComboBox();
};


#endif // EDITDIALOG_H
