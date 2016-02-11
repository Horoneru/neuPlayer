#include "moveanimation.h"

MoveAnimation::MoveAnimation(QObject *parent) :
    QObject(parent), m_direction(RightToLeft), m_duration(Normal)
{
}

MoveAnimation::MoveAnimation(QWidget *target, MoveAnimation::Direction direction, MoveAnimation::MoveDuration duration, QObject *parent) :
        QObject(parent), m_target(target), m_direction(direction), m_duration(duration)
{
}

MoveAnimation::MoveAnimation(QWidget *target, QObject *parent) :
    QObject(parent), m_target(target), m_direction(RightToLeft), m_duration(Normal)
{
}

void MoveAnimation::setupDirection()
{
    int startPosition[2];
    switch (m_direction)
    {
    case LeftToRight:
        startPosition[0] = m_target->x() + 50;
        startPosition[1] = m_target->y();
        break;
    case RightToLeft:
        startPosition[0] = m_target->x() - 50;
        startPosition[1] = m_target->y();
        break;
    case BottomToTop:
        startPosition[0] = m_target->x();
        startPosition[1] = m_target->y() + 50;
        break;
    case TopToBottom:
        startPosition[0] = m_target->x();
        startPosition[1] = m_target->y() - 50;
        break;
    }
    m_anim.setStartValue(QPoint(startPosition[0], startPosition[1]));
}

void MoveAnimation::start()
{
    m_anim.setDuration(m_duration);
    if(m_anim.endValue().isNull()) //If it hasn't been set yet
        setupDirection();
    m_anim.setTargetObject(m_target);
    m_anim.setPropertyName("pos");
    m_anim.setEndValue(QPoint(m_target->x(), m_target->y()));
    if(m_anim.state() == QPropertyAnimation::Running)
        m_anim.stop();
    m_anim.start();
    connect(&m_anim, SIGNAL(finished()), this, SLOT(hasFinished()));
}

MoveAnimation::~MoveAnimation()
{
    m_target = nullptr;
}
