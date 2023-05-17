
#ifndef LAPPGETMODELFOLDERS_H
#define LAPPGETMODELFOLDERS_H
#include <QDir>
#include <QStringList>

const std::vector<const char*> getFileNamesFromDir(const QString& dirPath)
{
    std::vector<const char*> modelDirs;

    QDir dir(dirPath);
    QStringList  folderNames = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    modelDirs.reserve(folderNames.size());
    for (const QString& fileName : folderNames) {
        QByteArray byteArray = fileName.toUtf8();
        const int bufferSize = byteArray.size() + 1;
        char* cString = new char[bufferSize];
        std::strncpy(cString, byteArray.constData(), bufferSize - 1);
        cString[bufferSize - 1] = '\0';

        modelDirs.push_back(cString);
    }

    return modelDirs;
}

#endif // LAPPGETMODELFOLDERS_H
