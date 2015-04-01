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
    //All of the opacity effects. They need to be setup one by one...
    QGraphicsOpacityEffect *a_effectEnter0;
    QGraphicsOpacityEffect *a_effectEnter1;
    QGraphicsOpacityEffect *a_effectEnterEnter2;
    QGraphicsOpacityEffect *a_effectEnter3;
    QGraphicsOpacityEffect *a_effectShowButtonNext;
    QGraphicsOpacityEffect *a_effectLeaveButton0;
    QGraphicsOpacityEffect *a_effectLeavebutton1;
    QGraphicsOpacityEffect *a_effectNewDescription;
    QParallelAnimationGroup *a_enterAnimations;
    QParallelAnimationGroup *a_leaveButtonsAnimations;
    QPropertyAnimation *a_anim0;
    QPropertyAnimation *a_anim1;
    QPropertyAnimation *a_anim2;
    QPropertyAnimation *a_anim3;

    //Others
    bool a_canClose; //To setup an anim before closing
    QString a_mediaPath;
    QTimer *a_timer;
    int a_page; //"Where are we ?"
    double a_slideValue;



};

#endif // INITIALCONFIG_H
