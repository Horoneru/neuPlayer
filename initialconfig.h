#ifndef INITIALCONFIG_H
#define INITIALCONFIG_H

#include <QDialog>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QTimer>
#include <QSettings>
#include <QStandardPaths>

#include "moveanimation.h"
#include "framelesshelper.h"
#include "player.h"
#include "fadewindow.h"
#include "fademanager.h"
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
public:
    explicit InitialConfig(QWidget *parent = 0);
private:
    ~InitialConfig();
    void setupWidgetsInitialVisibility();
    void setupConnections();

    void entranceAnimations();
    void finalPage();
    void presetConfig();

    void closeEvent(QCloseEvent *event);

    //Attributes
    Ui::InitialConfig *ui;
    QSettings m_settings;
    Player *neuPlayer;
    //We only need this manager now !
    FadeManager m_animManager;

    //Others
    bool m_canClose; //To setup an anim before closing
    bool m_willRestart;
    QString m_mediaPath;
    QTimer m_timer;
    int m_page; //"Where are we ?"
    double m_slideValue;
};

#endif // INITIALCONFIG_H
