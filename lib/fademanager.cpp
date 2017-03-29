#include "fademanager.h"
#include <QtDebug>


FadeManager::FadeManager(QObject *parent) : //Primarily used for groups
    QObject(parent), m_duration(300), m_deleteWhenFinished(false)
{}

FadeManager::FadeManager(QWidget *target, FadeManager::FadeMode mode,
                         int msecs, FadeManager::AnimationSequenceType sequence, QObject *parent) :
    QObject(parent), m_target(target), m_mode(mode),
    m_duration(msecs), m_sequenceType(sequence), m_deleteWhenFinished(false)
{}

FadeManager::FadeManager(QWidget *target, int msecs,
                         QObject *parent, FadeManager::FadeMode mode) :
    QObject(parent), m_target(target),
    m_duration(msecs), m_mode(mode), m_deleteWhenFinished(false)
{}

FadeManager::FadeManager(QWidget *target, QObject *parent) :
    QObject(parent), m_target(target), m_duration(300),
    m_deleteWhenFinished(false)
{}

FadeManager::FadeManager(FadeManager::FadeMode mode, QObject *parent) :
    QObject(parent), m_mode(mode), m_duration(300),
    m_deleteWhenFinished(false)
{}

//Will set the entire group to fade out
//It is used when using fadeOutGroup();
void FadeManager::setGroupToFadeOut(FadeManager::AnimationSequenceType typeToChange)
{
    if(typeToChange == Sequential)
    {
        const int groupSize = m_sequentialAnimations.animationCount();
        for (int i(0); i < groupSize; i++)
        {
            auto *anim = (QPropertyAnimation*) m_sequentialAnimations.animationAt(i);
            anim->setStartValue(1.0);
            anim->setEndValue(0.0);
        }
    }
    else
    {
        const int groupSize = m_parallelAnimations.animationCount();
        for (int i(0); i < groupSize; i++)
        {
            auto *anim = (QPropertyAnimation*) m_parallelAnimations.animationAt(i);
            anim->setStartValue(1.0);
            anim->setEndValue(0.0);
        }
    }
}

//Will set the entire group to fade in
//It is used when using fadeInGroup();
void FadeManager::setGroupToFadeIn(FadeManager::AnimationSequenceType typeToChange)
{
    if(typeToChange == Sequential)
    {
        const int groupSize = m_sequentialAnimations.animationCount();
        for (int i(0); i < groupSize; i++)
        {
            auto *anim = (QPropertyAnimation*) m_sequentialAnimations.animationAt(i);
            anim->setStartValue(0.0);
            anim->setEndValue(1.0);
        }
    }
    else
    {
        const int groupSize = m_parallelAnimations.animationCount();
        for (int i(0); i < groupSize; i++)
        {
            auto *anim = (QPropertyAnimation*) m_parallelAnimations.animationAt(i);
            anim->setStartValue(0.0);
            anim->setEndValue(1.0);
        }
    }
}

//Used whenever we have to iterate over groups to search
QAbstractAnimation *FadeManager::searchTarget(QWidget *target, AnimationSequenceType inWhichGroup)
{
    if(inWhichGroup == Parallel)
    {
        const int groupSize = m_parallelAnimations.animationCount();
        for (int i(0); i < groupSize; i++)
        {
            if(m_parallelAnimations.animationAt(i)->objectName() == target->objectName())
            {
                return m_parallelAnimations.animationAt(i);
            }
        }
    }
    else
    {
        const int groupSize = m_sequentialAnimations.animationCount();
        for (int i(0); i < groupSize; i++)
        {
            if(m_sequentialAnimations.animationAt(i)->objectName() == target->objectName())
            {
                return m_sequentialAnimations.animationAt(i);
            }
        }
    }
    //We get here only if we didn't find anything
    qWarning() << "Couldn't find target " << target->objectName()
               << "in the specified group !";
    return nullptr;
}

void FadeManager::addToParallel(QAbstractAnimation *animation)
{
    m_parallelAnimations.addAnimation(animation);
}

void FadeManager::addToSequential(QAbstractAnimation *animation)
{
    m_sequentialAnimations.addAnimation(animation);
}

void FadeManager::addTarget(QWidget *target, FadeManager::FadeMode mode)
{
    QGraphicsOpacityEffect *effectContainer;
    effectContainer = new QGraphicsOpacityEffect(this);
    QPropertyAnimation *anim;
    anim = new QPropertyAnimation(effectContainer, "opacity", this);
    target->setGraphicsEffect(effectContainer);
    anim->setDuration(m_duration);
    setMode(anim, mode, effectContainer);

    anim->setObjectName(target->objectName()); //To find them later on by set or delete
    m_parallelAnimations.addAnimation(anim);

}

void FadeManager::addTarget(QWidget *target)
{
    QGraphicsOpacityEffect *effectContainer;

    effectContainer = new QGraphicsOpacityEffect(this);
    QPropertyAnimation *anim;
    anim = new QPropertyAnimation(effectContainer, "opacity", this);
    target->setGraphicsEffect(effectContainer);
    anim->setDuration(m_duration);
    setMode(anim, FadeIn, effectContainer);

    anim->setObjectName(target->objectName()); //To find them later on by set or delete
    m_parallelAnimations.addAnimation(anim);
}

void FadeManager::addTarget(QWidget *target, FadeManager::FadeMode mode,
                                FadeManager::AnimationSequenceType sequence)
{
    QGraphicsOpacityEffect *effectContainer;
    effectContainer = new QGraphicsOpacityEffect(this);
    QPropertyAnimation *anim;
    anim = new QPropertyAnimation(effectContainer, "opacity", this);
    target->setGraphicsEffect(effectContainer);
    anim->setDuration(m_duration);
    setMode(anim, mode, effectContainer);

    anim->setObjectName(target->objectName()); //To find them later on by set or delete
    if(sequence == Parallel)
        m_parallelAnimations.addAnimation(anim);
    else
        m_sequentialAnimations.addAnimation(anim);

}

void FadeManager::addTarget(QWidget *target, int msecs)
{
    QGraphicsOpacityEffect *effectContainer;
    effectContainer = new QGraphicsOpacityEffect(this);
    QPropertyAnimation *anim;
    anim = new QPropertyAnimation(effectContainer, "opacity", this);
    target->setGraphicsEffect(effectContainer);
    anim->setDuration(msecs);
    setMode(anim, FadeIn, effectContainer);
    anim->setObjectName(target->objectName()); //To find them later on by set or delete
    m_parallelAnimations.addAnimation(anim);
}

void FadeManager::addTarget(QWidget *target, FadeManager::FadeMode mode,
                            int msecs, FadeManager::AnimationSequenceType sequence)
{
    QGraphicsOpacityEffect *effectContainer;
    effectContainer = new QGraphicsOpacityEffect(this);
    QPropertyAnimation *anim;
    anim = new QPropertyAnimation(effectContainer, "opacity", this);
    target->setGraphicsEffect(effectContainer);
    anim->setDuration(msecs);
    setMode(anim, mode, effectContainer);

    anim->setObjectName(target->objectName()); //To find them later on by set or delete
    if(sequence == Parallel)
        m_parallelAnimations.addAnimation(anim);
    else
        m_sequentialAnimations.addAnimation(anim);
}

void FadeManager::deleteTarget(QWidget *targetToDelete,
                               FadeManager::AnimationSequenceType fromWhichGroup)
{
    if(fromWhichGroup == Parallel)
    {
        auto animToRemove =  searchTarget(targetToDelete, Parallel);
        if(animToRemove == nullptr)
            return;
        m_parallelAnimations.removeAnimation(animToRemove);
    }
    else
    {
        auto animToRemove =  searchTarget(targetToDelete, Sequential);
        if(animToRemove == nullptr)
            return;
        m_sequentialAnimations.removeAnimation(animToRemove);
    }
}

void FadeManager::editTarget(QWidget *targetToModify, FadeManager::FadeMode mode,
                             FadeManager::AnimationSequenceType fromWhichGroup)
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

void FadeManager::changeTargetGroup(QWidget *targetToModify,
                                    FadeManager::AnimationSequenceType fromWhichGroup)
{
    if(fromWhichGroup == Parallel)
    {
        auto animToModify =  searchTarget(targetToModify, Parallel);
        if(animToModify == nullptr)
            return;
        auto *anim = (QPropertyAnimation*) animToModify;
        m_parallelAnimations.removeAnimation(anim);
        m_sequentialAnimations.addAnimation(anim);

    }
    else
    {
        auto animToModify =  searchTarget(targetToModify, Sequential);
        if(animToModify == nullptr)
            return;
        auto *anim = (QPropertyAnimation*) animToModify;
        m_sequentialAnimations.removeAnimation(anim);
        m_parallelAnimations.addAnimation(anim);
    }
}

void FadeManager::clearGroup(FadeManager::AnimationSequenceType whichGroup)
{
    if(whichGroup == Parallel)
        m_parallelAnimations.clear();
    else
        m_sequentialAnimations.clear();
}

void FadeManager::editTarget(QWidget *targetToModify, int msecs,
                             FadeManager::AnimationSequenceType fromWhichGroup)
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

void FadeManager::editTarget(QWidget *targetToModify, FadeManager::FadeMode mode,
                             int msecs, FadeManager::AnimationSequenceType fromWhichGroup)
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

void FadeManager::start(bool deleteWhenFinished)
{
    //TODO : Clean up ressources when finished and add similiar logic to groups
    QPropertyAnimation *anim;
    m_effectContainer = new QGraphicsOpacityEffect(this);
    anim = new QPropertyAnimation(m_effectContainer, "opacity", this);
    m_target->setGraphicsEffect(m_effectContainer);
    anim->setDuration(m_duration);
    setMode(anim, m_mode, m_effectContainer);
    if(deleteWhenFinished)
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    else
        anim->start();
}

void FadeManager::setMode(QPropertyAnimation *anim, FadeMode mode,
                          QGraphicsOpacityEffect *container)
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

void FadeManager::startGroup(FadeManager::AnimationSequenceType typeToStart,
                             bool deleteWhenFinished)
{
    if(typeToStart == Sequential)
    {
        if(m_sequentialAnimations.state() == QAbstractAnimation::Running)
            m_sequentialAnimations.stop();
        if(deleteWhenFinished)
            m_deleteWhenFinished = true;
        connect(&m_sequentialAnimations, SIGNAL(finished()), this, SLOT(sequentialFinished()));
        m_sequentialAnimations.start();
    }
    else
    {
        if(m_parallelAnimations.state() == QAbstractAnimation::Running)
            m_parallelAnimations.stop();
        if(deleteWhenFinished)
            m_deleteWhenFinished = true;
        connect(&m_parallelAnimations, SIGNAL(finished()), this, SLOT(parallelFinished()));
        m_parallelAnimations.start();
    }
}

void FadeManager::fadeOutGroup(FadeManager::AnimationSequenceType typeToStart,
                               bool deleteWhenFinished)
{
    setGroupToFadeOut(typeToStart);
    startGroup(typeToStart, deleteWhenFinished);
}

void FadeManager::fadeInGroup(FadeManager::AnimationSequenceType typeToStart,
                              bool deleteWhenFinished)
{
    setGroupToFadeIn(typeToStart);
    startGroup(typeToStart, deleteWhenFinished);
}

FadeManager::~FadeManager()
{
    m_target = nullptr;
    if(m_parallelAnimations.animationCount()) //If it's not 0 (false), then there's something
        clearGroup(Parallel);
    if(m_sequentialAnimations.animationCount())
        clearGroup(Sequential);
    if(!m_effectContainer) //if pointer not null
        delete m_effectContainer;
    else
        m_effectContainer = nullptr;
}
