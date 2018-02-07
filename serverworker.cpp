#include "serverworker.h"

ServerWorker::ServerWorker(QObject *parent) : QObject(parent)
{
    rootDirectoryPath="C:\\Users\\Pawel\\Desktop";
   // rootDirectoryPath="/Users/p.witkowski/Documents";
    finishFlag = false;
    fileManager = new FileManager(this);
}

void ServerWorker::firstSynchronization()
{
    QDir dir(rootDirectoryPath +"/" + connectionName);
    qDebug() << dir.absolutePath();
    socket->waitForReadyRead();

    QByteArray fileInfo;
    QFileInfo fileToCheck;
    while(!finishFlag){
        socket->waitForReadyRead(3000);
        int isFile = socket->read(sizeof(qint8)).toInt();
        if (isFile==1){
            //is File

            int sizeOfName = socket->read(sizeof(qint32)).toInt(nullptr,16);
            socket->waitForReadyRead(3000);
            QString name = socket->read(sizeOfName);
            QString nameCopy(name);
            qDebug() << "Name of file " + name;

            socket->waitForReadyRead(3000);

            int sizeOfFileInfo = socket->read(sizeof(qint16)).toShort(nullptr,16);
            fileInfo = socket->read(sizeOfFileInfo);
            fileManager->setByteArray(fileInfo);
            socket->waitForReadyRead(3000);
            qint64 sizeOfFile = fileManager->getFileSize();
            fileToCheck.setFile(dir.absolutePath() + "/" + name);
            directoryDataset.remove(fileToCheck.absoluteFilePath());

            if (fileToCheck.exists() && fileToCheck.isFile()){
                if (fileToCheck.size() == sizeOfFile && fileToCheck.suffix() == fileManager->getFileSuffix()){
                    qDebug() << name + " rejected!";
                    socket->write(QByteArray::number(2));
                    socket->waitForBytesWritten(3000);
                    continue;
                }
            }
            qDebug() << name + " accepted!";
            socket->write(QByteArray::number(1));
            socket->waitForBytesWritten(3000);


            if (!nameCopy.remove(QRegExp("[^/]*\\..*$")).isEmpty()){
                dir.mkpath(nameCopy.remove(QRegExp("/$")));
            }
            QFile file(rootDirectoryPath + "/" + connectionName + "/" + name);
            if (!file.open(QIODevice::WriteOnly)){
                qDebug() << rootDirectoryPath + "/" + connectionName + "/" + name + " not opened!";
            }
            while(socket->bytesAvailable() < sizeOfFile){

                if (!socket->waitForReadyRead()){
                    qDebug() << "readRead timeout";
                    emitFinished();
                    break;
                }
            }
            file.write(socket->read(sizeOfFile));
            file.close();


        }else if (isFile==2){
            //is Directory
            socket->waitForReadyRead(3000);
            int sizeOfName = socket->read(sizeof(qint32)).toInt(nullptr,16);
            socket->waitForReadyRead(3000);
            QString name = socket->read(sizeOfName);
            qDebug() << "Creating directory: " + name;
            dir.mkpath(name);
            directoryDataset.remove(dir.absolutePath()+"/"+name);
        }else{
            while(socket->bytesAvailable() == 0){

                if (!socket->waitForReadyRead(5000)){
                    qDebug() << "readyRead timeout";
                    QMapIterator<QString,int> map(directoryDataset);
                    while(map.hasNext()){
                        map.next();
                        qDebug() << map.key();
                        QFileInfo p(map.key());
                        if (p.isDir()){
                            QDir dir(map.key());
                            dir.removeRecursively();
                        }
                        if (p.isFile()){
                            QFile s(p.absoluteFilePath());
                            s.remove();
                        }

                    }
                    emitFinished();
                    finishFlag=true;
                    break;
                }
            }
        }
    }

}

void ServerWorker::startListening()
{
    server = new QTcpServer(this);
    if (!server->listen(QHostAddress::Any,server->serverPort())){
        qDebug() << "couldn't establish connection";
    }else{
        qDebug() << "connection established";
        connect(server,SIGNAL(newConnection()),this,SLOT(connectNewClient()));
        QString ipAddress;
        QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
        // use the first non-localhost IPv4 address
        for (int i = 0; i < ipAddressesList.size(); ++i) {
            if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
                ipAddressesList.at(i).toIPv4Address()) {
                ipAddress = ipAddressesList.at(i).toString();
                break;
            }
        }
        qDebug() <<  "IP:     " + ipAddress;
        qDebug() <<  "PORT:   " + QString::number(server->serverPort());

    }
}

void ServerWorker::connectNewClient()
{
    socket = server->nextPendingConnection();
    qDebug() << "Connection with " + socket->peerAddress().toString();
    connect(socket,SIGNAL(disconnected()),this,SLOT(emitFinished()));
    connectionName = socket->peerAddress().toString().remove(QRegExp("^.*:.*:.*:"));
        QDir rootDirectory(rootDirectoryPath);
        rootDirectory.mkdir(connectionName);

    QDirIterator it (rootDirectoryPath +"/" +connectionName, QDir::AllEntries | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while(it.hasNext()){
        it.next();
        directoryDataset[it.filePath()] = 1;
    }
    firstSynchronization();
}

void ServerWorker::emitFinished()
{
    qDebug() << "finished job!";
    emit finished();
}
