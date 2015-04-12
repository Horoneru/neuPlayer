#include "fadeanimmanager.h"
#include <QtDebug>

fadeAnimManager::fadeAnimManager(QObject *parent) : //Primarily used for groups
    QObject(parent), a_duration(300), a_timerEnabled(false),
    a_groupAlreadyInit(false), a_finished(false)
{}

fadeAnimManager::fadeAnimManager(QWidget *target, fadeAnimManager::FadeMode mode,
                                 int msecs, fadeAnimManager::AnimationSequenceType sequence, QObject *parent) :
    QObject(parent), a_target(target), a_mode(mode),
    a_duration(msecs), a_sequenceType(sequence), a_timerEnabled(false),
    a_groupAlreadyInit(false), a_finished(false)
{

}

fadeAnimManager::fadeAnimManager(QWidget *target, int msecs, QObject *parent, fadeAnimManager::FadeMode mode) :
    QObject(parent), a_target(target),
    a_duration(msecs), a_mode(mode), a_timerEnabled(false),
    a_groupAlreadyInit(false), a_finished(false)
{

}

fadeAnimManager::fadeAnimManager(QWidget *target, QObject *parent) :
    QObject(parent), a_target(target), a_duration(300),
    a_timerEnabled(false), a_groupAlreadyInit(false), a_finished(false)
{

}

fadeAnimManager::fadeAnimManager(fadeAnimManager::FadeMode mode, QObject *parent) :
    QObject(parent), a_mode(mode), a_duration(300),
    a_timerEnabled(false), a_groupAlreadyInit(false), a_finished(false)
{

}

void fadeAnimManager::initGroups()
{
    a_parallelAnimations = new QParallelAnimationGroup(nullptr);
    a_sequentialAnimations = new QSequentialAnimationGroup(nullptr);
    a_groupAlreadyInit = true;
}


//Will set the entire group to fade out
//It is used when using fadeOutGroup();
void fadeAnimManager::setGroupToFadeOut(fadeAnimManager::AnimationSequenceType typeToChange)
{

    if(typeToChange == Sequential)
    {
        const int groupSize = a_sequentialAnimations->animationCount();
        for (int i(0); i < groupSize; i++)
        {
            auto *anim = (QPropertyAnimation*) a_sequentialAnimations->animationAt(i);
            anim->setStartValue(1.0);
            anim->setEndValue(0.0);
        }
    }
    else
    {
        const int groupSize = a_parallelAnimations->animationCount();
        for (int i(0); i < groupSize; i++)
        {
            auto *anim = (QPropertyAnimation*) a_parallelAnimations->animationAt(i);
            anim->setStartValue(1.0);
            anim->setEndValue(0.0);
        }
    }
}

//Will set the entire group to fade in
//It is used when using fadeInGroup();
void fadeAnimManager::setGroupToFadeIn(fadeAnimManager::AnimationSequenceType typeToChange)
{
    if(typeToChange == Sequential)
    {
        const int groupSize = a_sequentialAnimations->animationCount();
        for (int i(0); i < groupSize; i++)
        {
            auto *anim = (QPropertyAnimation*) a_sequentialAnimations->animationAt(i);
            anim->setStartValue(0.0);
            anim->setEndValue(1.0);
        }
    }
    else
    {
        const int groupSize = a_parallelAnimations->animationCount();
        for (int i(0); i < groupSize; i++)
        {
            auto *anim = (QPropertyAnimation*) a_parallelAnimations->animationAt(i);
            anim->setStartValue(0.0);
            anim->setEndValue(1.0);
        }
    }
}

//Used whenever we have to iterate over groups to search
QAbstractAnimation *fadeAnimManager::searchTarget(QWidget *target, AnimationSequenceType inWhichGroup)
{
    if(inWhichGroup == Parallel)
    {
        const int groupSize = a_parallelAnimations->animationCount();
        for (int i(0); i < groupSize; i++)
        {
            if(a_parallelAnimations->animationAt(i)->objectName() == target->objectName())
            {
                return a_parallelAnimations->animationAt(i);
            }
        }
    }
    else
    {
        const int groupSize = a_sequentialAnimations->animationCount();
        for (int i(0); i < groupSize; i++)
        {
            if(a_sequentialAnimations->animationAt(i)->objectName() == target->objectName())
            {
                return a_sequentialAnimations->animationAt(i);
            }
        }
    }
    //We get here only if we didn't find anything
    qWarning() << "Couldn't find target " << target->objectName() << "in the specified group !";
    return nullptr;
}

void fadeAnimManager::addParallelAnim(QAbstractAnimation *animation)
{
    a_parallelAnimations->addAnimation(animation);
}

void fadeAnimManager::addSequentialAnim(QAbstractAnimation *animation)
{
    a_sequentialAnimations->addAnimation(animation);
}

void fadeAnimManager::addTarget(QWidget *target, fadeAnimManager::FadeMode mode)
{
    QGraphicsOpacityEffect *effectContainer;
    effectContainer = new QGraphicsOpacityEffect(nullptr);
    QPropertyAnimation *anim;
    anim = new QPropertyAnimation(effectContainer, "opacity", nullptr);
    target->setGraphicsEffect(effectContainer);
    anim->setDuration(a_duration);
    setMode(anim, mode, effectContainer);

    anim->setObjectName(target->objectName()); //To find them later on by set or delete
    if(!a_groupAlreadyInit)
        initGroups(); //Instantiate the groups if they aren't already
    a_parallelAnimations->addAnimation(anim);

}

void fadeAnimManager::addTarget(QWidget *target)
{
    QGraphicsOpacityEffect *effectContainer;

    effectContainer = new QGraphicsOpacityEffect(nullptr);
    QPropertyAnimation *anim;
    anim = new QPropertyAnimation(effectContainer, "opacity", nullptr);
    target->setGraphicsEffect(effectContainer);
    anim->setDuration(a_duration);
    setMode(anim, FadeIn, effectContainer);

    anim->setObjectName(target->objectName()); //To find them later on by set or delete
    if(!a_groupAlreadyInit)
        initGroups();
    a_parallelAnimations->addAnimation(anim);
}

void fadeAnimManager::addTarget(QWidget *target, fadeAnimManager::FadeMode mode,
                                fadeAnimManager::AnimationSequenceType sequence)
{
    QGraphicsOpacityEffect *effectContainer;
    effectContainer = new QGraphicsOpacityEffect(nullptr);
    QPropertyAnimation *anim;
    anim = new QPropertyAnimation(effectContainer, "opacity", nullptr);
    target->setGraphicsEffect(effectContainer);
    anim->setDuration(a_duration);
    setMode(anim, mode, effectContainer);

    anim->setObjectName(target->objectName()); //To find them later on by set or delete
    if(!a_groupAlreadyInit)
        initGroups();
    if(sequence == Parallel)
        a_parallelAnimations->addAnimation(anim);
    else
        a_sequentialAnimations->addAnimation(anim);

}

void fadeAnimManager::addTarget(QWidget *target, int msecs)
{
    QGraphicsOpacityEffect *effectContainer;
    effectContainer = new QGraphicsOpacityEffect(nullptr);
    QPropertyAnimation *anim;
    anim = new QPropertyAnimation(effectContainer, "opacity", nullptr);
    target->setGraphicsEffect(effectContainer);
    anim->setDuration(msecs);
    setMode(anim, FadeIn, effectContainer);
    anim->setObjectName(target->objectName()); //To find them later on by set or delete
    if(!a_groupAlreadyInit)
        initGroups();
    a_parallelAnimations->addAnimation(anim);
}

void fadeAnimManager::addTarget(QWidget *target, fadeAnimManager::FadeMode mode,
                                int msecs, fadeAnimManager::AnimationSequenceType sequence)
{
    QGraphicsOpacityEffect *effectContainer;
    effectContainer = new QGraphicsOpacityEffect(nullptr);
    QPropertyAnimation *anim;
    anim = new QPropertyAnimation(effectContainer, "opacity", nullptr);
    target->setGraphicsEffect(effectContainer);
    anim->setDuration(msecs);
    setMode(anim, mode, effectContainer);

    anim->setObjectName(target->objectName()); //To find them later on by set or delete
    if(!a_groupAlreadyInit)
        initGroups();
    if(sequence == Parallel)
        a_parallelAnimations->addAnimation(anim);
    else
        a_sequentialAnimations->addAnimation(anim);
}

void fadeAnimManager::deleteTarget(QWidget *targetToDelete, fadeAnimManager::AnimationSequenceType fromWhichGroup)
{
    if(fromWhichGroup == Parallel)
    {
        auto animToRemove =  searchTarget(targetToDelete, Parallel);
        if(animToRemove == nullptr)
            return;
        a_parallelAnimations->removeAnimation(animToRemove);
    }
    else
    {
        auto animToRemove =  searchTarget(targetToDelete, Sequential);
        if(animToRemove == nullptr)
            return;
        a_sequentialAnimations->removeAnimation(animToRemove);
    }
}

void fadeAnimManager::editTarget(QWidget *targetToModify, fadeAnimManager::FadeMode mode, fadeAnimManager::AnimationSequenceType fromWhichGroup)
{
    if(fromWhichGroup == Parallel)
    {
        auto animToModify =  searchTarget(targetToModify, Parallel);
        if(animToModify == nullptr)
            return;
        auto *anim = (QPropertyAnimation*) animToModify;
        auto *container = (QGraphicsOpacityEffect*) anim->targetObject();
        setMode(anim, mode, container);
    }
    else
    {
        auto animToModify =  searchTarget(targetToModify, Sequential);
        if(animToModify == nullptr)
            return;
        auto *anim = (QPropertyAnimation*) animToModify;
        auto *container = (QGraphicsOpacityEffect*) anim->targetObject();
        setMode(anim, mode, container);
    }
}

void fadeAnimManager::changeTargetGroup(QWidget *targetToModify, fadeAnimManager::AnimationSequenceType fromWhichGroup)
{
    if(fromWhichGroup == Parallel)
    {
        auto animToModify =  searchTarget(targetToModify, Parallel);
        if(animToModify == nullptr)
            return;
        auto *anim = (QPropertyAnimation*) animToModify;
        a_parallelAnimations->removeAnimation(anim);
        a_sequentialAnimations->addAnimation(anim);

    }
    else
    {
        auto animToModify =  searchTarget(targetToModify, Sequential);
        if(animToModify == nullptr)
            return;
        auto *anim = (QPropertyAnimation*) animToModify;
        a_sequentialAnimations->removeAnimation(anim);
        a_parallelAnimations->addAnimation(anim);
    }
}

void fadeAnimManager::clearGroup(fadeAnimManager::AnimationSequenceType whichGroup)
{
    if(whichGroup == Parallel)
        a_parallelAnimations->clear();
    else
        a_sequentialAnimations->clear();
}

void fadeAnimManager::editTarget(QWidget *targetToModify, int msecs, fadeAnimManager::AnimationSequenceType fromWhichGroup)
{
    if(fromWhichGroup == Parallel)
    {
        auto animToModify =  searchTarget(targetToModify, Parallel);
        if(animToModify == nullptr)
            return;
        auto *anim = (QPropertyAnimation*) animToModify;
        anim->setDuration(msecs);
    }
    else
    {
        auto animToModify =  searchTarget(targetToModify, Sequential);
        if(animToModify == nullptr)
            return;
        auto *anim = (QPropertyAnimation*) animToModify;
        anim->setDuration(msecs);
    }
}

void fadeAnimManager::editTarget(QWidget *targetToModify, fadeAnimManager::FadeMode mode, int msecs, fadeAnimManager::AnimationSequenceType fromWhichGroup)
{
    if(fromWhichGroup == Parallel)
    {
        auto animToModify =  searchTarget(targetToModify, Parallel);
        if(animToModify == nullptr)
            return;
        auto *anim = (QPropertyAnimation*) animToModify;
        auto *container = (QGraphicsOpacityEffect*) anim->targetObject();
        setMode(anim, mode, container);
        anim->setDuration(msecs);
    }
    else
    {
        auto animToModify =  searchTarget(targetToModify, Sequential);
        if(animToModify == nullptr)
            return;
        auto *anim = (QPropertyAnimation*) animToModify;
        auto *container = (QGraphicsOpacityEffect*) anim->targetObject();
        setMode(anim, mode, container);
        anim->setDuration(msecs);
    }
}

void fadeAnimManager::start(bool deleteWhenFinished)
{
    QPropertyAnimation *anim;
    a_effectContainer = new QGraphicsOpacityEffect(nullptr);
    anim = new QPropertyAnimation(a_effectContainer, "opacity", nullptr);
    a_target->setGraphicsEffect(a_effectContainer);
    anim->setDuration(a_duration);
    setMode(anim, a_mode, a_effectContainer);
    if(deleteWhenFinished)
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    else
        anim->start();
}

void fadeAnimManager::setMode(QPropertyAnimation *anim, FadeMode mode, QGraphicsOpacityEffect *container)
{
    if(mode == FadeIn)
    {
        container->setOpacity(0.0);
        anim->setStartValue(0.0);
        anim->setEndValue(1.0);
    }
    else //Fade out
    {
        container->setOpacity(1.0);
        anim->setStartValue(1.0);
        anim->setEndValue(0.0);
    }
}

void fadeAnimManager::startGroup(fadeAnimManager::AnimationSequenceType typeToStart, bool deleteWhenFinished)
{
    if(typeToStart == Sequential)
    {
        if(a_sequentialAnimations->state() == QAbstractAnimation::Running)
            a_sequentialAnimations->stop();
        if(deleteWhenFinished)
            a_sequentialAnimations->start(QAbstractAnimation::DeleteWhenStopped);
        else
            a_sequentialAnimations->start();
    }
    else
    {
        if(a_parallelAnimations->state() == QAbstractAnimation::Running)
            a_parallelAnimations->stop();
        if(deleteWhenFinished)
            a_parallelAnimations->start(QAbstractAnimation::DeleteWhenStopped);
        else
            a_parallelAnimations->start();
    }
}

void fadeAnimManager::fadeOutGroup(fadeAnimManager::AnimationSequenceType typeToStart, bool deleteWhenFinished)
{
    setGroupToFadeOut(typeToStart);
    startGroup(typeToStart, deleteWhenFinished);
}

void fadeAnimManager::fadeInGroup(fadeAnimManager::AnimationSequenceType typeToStart, bool deleteWhenFinished)
{
    setGroupToFadeIn(typeToStart);
    startGroup(typeToStart, deleteWhenFinished);
}

fadeAnimManager::~fadeAnimManager()
{
}
