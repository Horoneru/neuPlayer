#include "fadeanimmanager.h"

fadeAnimManager::fadeAnimManager(QObject *parent) :
    QObject(parent), a_duration(300),
    a_sequenceType(AnimationSequenceType::NoSequence), a_timerEnabled(false)
{}

fadeAnimManager::fadeAnimManager(QWidget *target, fadeAnimManager::FadeMode mode,
                                 int msecs, fadeAnimManager::AnimationSequenceType sequence, QObject *parent) :
    QObject(parent), a_target(target),
    a_mode(mode), a_duration(msecs), a_sequenceType(sequence)
{

}

fadeAnimManager::fadeAnimManager(QWidget *target, int msecs,
                                 QObject *parent) :
    QObject(parent), a_target(target),
    a_duration(msecs)
{
}

fadeAnimManager::fadeAnimManager(QWidget *target, int msecs, QObject *parent, fadeAnimManager::FadeMode mode) :
    QObject(parent), a_target(target),
    a_duration(msecs), a_mode(mode)
{

}

fadeAnimManager::fadeAnimManager(QWidget *target, QObject *parent)
{

}

fadeAnimManager::fadeAnimManager(fadeAnimManager::FadeMode mode, QObject *parent)
{

}

void fadeAnimManager::addParallelAnim(const QAbstractAnimation *animation)
{

}

void fadeAnimManager::addSequentialAnim(const QAbstractAnimation *animation)
{

}

void fadeAnimManager::addTarget(QWidget *target, fadeAnimManager::FadeMode mode)
{

}

void fadeAnimManager::addTarget(QWidget *target)
{

}

void fadeAnimManager::addTarget(QWidget *target, fadeAnimManager::FadeMode mode,
                                fadeAnimManager::AnimationSequenceType sequence)
{

}

void fadeAnimManager::addTarget(QWidget *target, fadeAnimManager::FadeMode mode,
                                int msecs, fadeAnimManager::AnimationSequenceType sequence)
{
    QGraphicsOpacityEffect effectContainer(nullptr);
    QPropertyAnimation anim(&effectContainer, "opacity", nullptr);
    anim.setDuration(msecs);
    if(mode == FadeIn)
    {
        effectContainer.setOpacity(0.0);
        anim.setStartValue(0.0);
        anim.setEndValue(1.0);
    }
    else
    {
        effectContainer.setOpacity(1.0);
        anim.setStartValue(1.0);
        anim.setEndValue(0.0);
    }
    if(sequence == Parallel)
        a_parallelAnimations->addAnimation(&anim);
    else
        a_sequentialAnimations->addAnimation(&anim);

}

void fadeAnimManager::start()
{
    QPropertyAnimation *anim;
    a_effectContainer = new QGraphicsOpacityEffect(nullptr);
    anim = new QPropertyAnimation(a_effectContainer, "opacity", nullptr);
    a_target->setGraphicsEffect(a_effectContainer);
    anim->setDuration(a_duration);
    if(a_mode == FadeIn)
    {
        a_effectContainer->setOpacity(0.0);
        anim->setStartValue(0.0);
        anim->setEndValue(1.0);
    }
    else //Fade out
    {
        a_effectContainer->setOpacity(1.0);
        anim->setStartValue(1.0);
        anim->setEndValue(0.0);
    }
    anim->start();
}

void fadeAnimManager::startGroup(fadeAnimManager::AnimationSequenceType typeToStart)
{
    if(typeToStart == Sequential)
        a_sequentialAnimations->start();
    else
        a_parallelAnimations->start();
    //TODO : Find a way to instantiate all objects required whatever the size of the given group is.
    //We need to keep a QGraphicsOpacityEffect container alive at bare minimum.
}
