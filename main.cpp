#include "player.h"
#include "skin.h"
#include "initialconfig.h"
#include <QApplication>
#include <QStyleFactory>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QFile>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSettings a_settings("neuPlayer.ini", QSettings::IniFormat);

    // Init Trad engine
    QString locale = QLocale::system().name().section('_', 0, 0);
    QTranslator translator;
    //Load correct translation based on system locale
    translator.load("neuplayer_" + locale);
    a.installTranslator(&translator);
    //Setup actual window
    qApp->setStyle(QStyleFactory::create("Fusion")); //base theme fusion

    if(a_settings.value("skin", 1).toInt() == 1)
    {
        Skin HoloFusion(1, nullptr);
        HoloFusion.load();
    }

    else
    {
        Skin CleanFusion(0, nullptr);
        CleanFusion.load();
    }
    //Ready to show

    QFile fileHandler(".configdone"); //So you config before anything else
    if(!fileHandler.exists()) //If there is the config to do, the config will launch the app when done
    {
        InitialConfig *config = new InitialConfig();
        config->show();
    }
    else
    {
        Player *neuPlayer = new Player();
        neuPlayer->show();
    }
    fileHandler.close();

    return a.exec();
}
