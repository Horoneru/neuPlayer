#include "fadewindow.h"

/* Armada of constructors...
   Those are so this class can receive any type of Window widgets available.
*/

FadeWindow::FadeWindow(QObject *parent) :
    QObject(parent), m_finished(false), m_timerEnabled(false)
{}
FadeWindow::FadeWindow(QDialog *dialogWidget, QObject *parent) :
    m_target(dialogWidget), m_duration(200),
    m_finished(false), m_timerEnabled(false), QObject(parent)
{}

FadeWindow::FadeWindow(QDialog *dialogWidget, int msecs,
                       FadeWindow::FadeMode mode, QObject *parent) :
    m_target(dialogWidget), m_duration(msecs),
    m_finished(false), m_timerEnabled(false), QObject(parent),
    m_mode(mode)
{}

FadeWindow::FadeWindow(QWindow *windowWidget, QObject *parent) :
    m_target(windowWidget), m_duration(200),
    m_finished(false), m_timerEnabled(false), QObject(parent)
{}

FadeWindow::FadeWindow(QWindow *windowWidget, int msecs,
                       FadeWindow::FadeMode mode, QObject *parent) :
    m_target(windowWidget), m_duration(msecs),
    m_finished(false), m_timerEnabled(false), QObject(parent),
    m_mode(mode)
{}

FadeWindow::FadeWindow(QMainWindow *mainWindowWidget, int msecs,
                       FadeWindow::FadeMode mode, QObject *parent) :
    m_target(mainWindowWidget), m_duration(msecs), m_finished(false),
    m_timerEnabled(false), QObject(parent), m_mode(mode)
{}

FadeWindow::FadeWindow(QMainWindow *mainWindowWidget, QObject *parent) :
    m_target(mainWindowWidget), m_duration(200), m_finished(false),
    m_timerEnabled(false), QObject(parent)
{}


//Helper method
void FadeWindow::setMode(QPropertyAnimation *anim,
                         FadeWindow::FadeMode mode, qreal endValue)
{
    if(mode == FadeIn)
    {
        anim->setStartValue(0.0);
        if(endValue != 1.0)
            anim->setEndValue(endValue);
        else
            anim->setEndValue(1.0);
    }
    else //Fade out
    {
        anim->setStartValue(1.0);
        if(endValue != 0.0)
            anim->setEndValue(endValue);
        else
            anim->setEndValue(0.0);
    }
}

//Actual method

void FadeWindow::start(qreal endValue)
{
    QPropertyAnimation *animation;
    animation = new QPropertyAnimation(this);
    animation->setTargetObject(m_target);
    animation->setDuration(m_duration);
    setMode(animation, m_mode, endValue);
    animation->setPropertyName("windowOpacity");
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    //If the timer is enabled, the user will be able to be notified by checking via getter if the animation is finished.
    if(m_timerEnabled)
        connect(animation, SIGNAL(finished()), this, SLOT(setFinished()));
}

FadeWindow::~FadeWindow()
{
    m_target = nullptr;
}
