#include "updaterhandler.h"
#include <QApplication>
UpdaterHandler::UpdaterHandler(QObject *parent) :
    QObject(parent)
{
}

void UpdaterHandler::start(QString appName, QString currentVersion,
                           QString urlLatestVersion, QString urlLatestExe,
                           QString showMessageIfNoUpdate)
{
    QStringList arguments;
    arguments << appName
              << currentVersion
              << urlLatestVersion
              << urlLatestExe
              << showMessageIfNoUpdate;

    a_updateprocess.start("Updater.exe", arguments);
    connect(&a_updateprocess, SIGNAL(readyReadStandardOutput()), this, SLOT(on_newInfosFromProc()));
}

void UpdaterHandler::on_newInfosFromProc()
{
    while (a_updateprocess.canReadLine())
    {
        //Signal that we must shut down the player now
        if(a_updateprocess.readLine().contains("downloading"))
            qApp->closeAllWindows();
    }
}
