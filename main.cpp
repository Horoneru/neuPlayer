#include "player.h"
#include "initialconfig.h"
#include <QApplication>
#include <QStyleFactory>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QTimer>
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
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(45,45,45));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(30,30,30));
        darkPalette.setColor(QPalette::AlternateBase, QColor(45,45,45));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Button, QColor(45,45,45));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        qApp->setPalette(darkPalette);

        qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #353535; border: 1px solid #53a0ed; }");
    }

    else
    {
        QPalette lightPalette;
        lightPalette.setColor(QPalette::Base, QColor(30,30,30));
        lightPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        qApp->setStyleSheet("QToolTip { border: 1px solid #53a0ed; }");
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
