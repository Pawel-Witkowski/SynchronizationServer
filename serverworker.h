#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QtNetwork>
#include <QThread>
#include <QDir>
#include <QMap>
#include <QDirIterator>
#include <QMapIterator>
#include <filemanager.h>
class ServerWorker : public QObject
{
    Q_OBJECT
public:
    explicit ServerWorker(QObject *parent = nullptr);

    void firstSynchronization();
signals:
    void finished();
public slots:
    void startListening();
private slots:
    void connectNewClient();
    void emitFinished();
private:
    QTcpServer *server;
    QTcpSocket *socket;
    FileManager *fileManager;
    QString rootDirectoryPath;
    QString connectionName;
    QMap <QString,int> directoryDataset;

    bool finishFlag;

};

#endif // SERVERWORKER_H
