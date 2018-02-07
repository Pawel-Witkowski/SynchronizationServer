#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QDateTime>
class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(QObject *parent = nullptr);
    QByteArray getByteArray() const;
    void setByteArray(const QByteArray &value);

signals:

public slots:

    qint64 getFileSize();
    QDateTime getCreationTime();
    QDateTime getLastModificationTime();
    QString getFileSuffix();
private:

    int positionSizeOfCreation;
    int positionCreationDate;
    int positionSizeOfModify;
    int positionModifyDate;
    int positionSizeOfSuffix;
    int positionOfSuffix;

    QByteArray byteArray;

    //void setNumberToQByteArrayHex(QByteArray &targetArray, int size, quint64 number);
};

#endif // FILEMANAGER_H
