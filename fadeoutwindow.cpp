#include "fadeoutwindow.h"

/* Armada of constructors...
   Those are so this class can receive any type of Window widgets available.
*/
FadeOutWindow::FadeOutWindow(QObject *parent) :
    QObject(parent), a_finished(false), a_timerEnabled(false)
{}
FadeOutWindow::FadeOutWindow(QDialog *dialogWidget, QObject *parent) :
    a_target(dialogWidget), a_duration(200),
    a_finished(false), a_timerEnabled(false), QObject(parent)
{}

FadeOutWindow::FadeOutWindow(QDialog *dialogWidget, int msecs, QObject *parent) :
    a_target(dialogWidget), a_duration(msecs),
    a_finished(false), a_timerEnabled(false), QObject(parent)
{}

FadeOutWindow::FadeOutWindow(QWindow *windowWidget, QObject *parent) :
    a_target(windowWidget), a_duration(200),
    a_finished(false), a_timerEnabled(false), QObject(parent)
{}

FadeOutWindow::FadeOutWindow(QWindow *windowWidget, int msecs, QObject *parent) :
    a_target(windowWidget), a_duration(msecs),
    a_finished(false), a_timerEnabled(false), QObject(parent)
{}

FadeOutWindow::FadeOutWindow(QMainWindow *mainWindowWidget, int msecs, QObject *parent) :
    a_target(mainWindowWidget), a_duration(msecs), a_finished(false),
    a_timerEnabled(false), QObject(parent)
{}

FadeOutWindow::FadeOutWindow(QMainWindow *mainWindowWidget, QObject *parent) :
    a_target(mainWindowWidget), a_duration(200), a_finished(false),
    a_timerEnabled(false), QObject(parent)
{}

//Actual method

void FadeOutWindow::start()
{
    QPropertyAnimation *animation;
    animation = new QPropertyAnimation(nullptr);
    animation->setTargetObject(a_target);
    animation->setDuration(a_duration);
    animation->setPropertyName("windowOpacity");
    animation->setEndValue(0.0);
    animation->start();
    //If the timer is enabled, the user will be able to be notified by checking via getter if the animation is finished.
    if(a_timerEnabled)
        connect(animation, SIGNAL(finished()), this, SLOT(setFinished()));
}

