#include "filemanager.h"

FileManager::FileManager(QObject *parent) : QObject(parent)
{

}
QByteArray FileManager::getByteArray() const
{

    return byteArray;
}

void FileManager::setByteArray(const QByteArray &value)
{
    positionSizeOfCreation = 16;
    positionCreationDate = positionSizeOfCreation + 2;
    positionSizeOfModify= positionCreationDate + value.mid(positionSizeOfCreation,2).toInt(nullptr,16);

    int sizeOfModificateDate = value.mid(positionSizeOfModify,2).toInt(nullptr,16);
    positionModifyDate = positionSizeOfModify + 2;

    positionSizeOfSuffix = positionModifyDate + sizeOfModificateDate;

    //int sizeOfSuffix = value.mid(positionSizeOfSuffix,2).toInt(nullptr,16);
    positionOfSuffix = positionSizeOfSuffix + 2;

    byteArray = value;
}



qint64 FileManager::getFileSize()
{
    return byteArray.mid(0,16).toInt(nullptr,16);
}

QDateTime FileManager::getCreationTime()
{
    int sizeOfCreateDate = byteArray.mid(positionSizeOfCreation,2).toInt(nullptr,16);
    QString stringData = QString::fromUtf8(byteArray.mid(positionCreationDate,sizeOfCreateDate));
    return QDateTime::fromString(stringData, "dd.MM.yyyy hh:mm:ss");
}

QDateTime FileManager::getLastModificationTime()
{
    int sizeOfModificateDate = byteArray.mid(positionSizeOfModify,2).toInt(nullptr,16);
    QString stringData = QString::fromUtf8(byteArray.mid(positionModifyDate,sizeOfModificateDate));
    return QDateTime::fromString(stringData, "dd.MM.yyyy hh:mm:ss");
}

QString FileManager::getFileSuffix()
{
   // int sizeOfSuffix = byteArray.mid(positionSizeOfSuffix,2).toInt(nullptr,16);
    QString suffix = QString::fromUtf8(byteArray.mid(16));
    return suffix;
}


