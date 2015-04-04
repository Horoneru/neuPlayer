#ifndef INITIALCONFIG_H
#define INITIALCONFIG_H

#include <QDialog>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QTimer>
#include <QSettings>
#include <QStandardPaths>

#include "player.h"
#include "fadeoutwindow.h"
#include "fadeanimmanager.h"
namespace Ui {
class InitialConfig;
}

class Player;
class InitialConfig : public QDialog
{
    Q_OBJECT

private slots:

    void presetWizard();
    void customWizard();
    void advanceWizard();
    void updateSlideValue(int value);
    void setLibrary(bool isActivated);
    void setDefault(bool isActivated);
    void finishWizard();
    void delayedClose();

    void closeEvent(QCloseEvent *event);
public:
    explicit InitialConfig(QWidget *parent = 0);
private:
    void setupObjects();
    void setupWidgetsInitialVisibility();
    void setupConnections();

    void entranceAnimations();
    void finalPage();
    void presetConfig();

    ~InitialConfig();

    //Attributes
    Ui::InitialConfig *ui;
    QSettings *a_settings;
    Player *neuPlayer;
    //We only need this manager now !
    fadeAnimManager *animManager;

    //Others
    bool a_canClose; //To setup an anim before closing
    QString a_mediaPath;
    QTimer *a_timer;
    int a_page; //"Where are we ?"
    double a_slideValue;



};

#endif // INITIALCONFIG_H
