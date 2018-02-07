#include <QCoreApplication>
#include <QThread>
#include <serverworker.h>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QThread *thread = new QThread();
    ServerWorker *serverWorker = new ServerWorker();

    serverWorker->moveToThread(thread);
    QObject::connect (&a,SIGNAL(aboutToQuit()),serverWorker,SLOT(emitFinished()));
    QObject::connect(thread,SIGNAL(started()),serverWorker,SLOT(startListening()));
    QObject::connect(serverWorker,SIGNAL(finished()),thread,SLOT(quit()));
    QObject::connect(serverWorker,SIGNAL(finished()),serverWorker,SLOT(deleteLater()));
    QObject::connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));
    thread->start();
    return a.exec();
}
