#ifndef FADEMANAGER_H
#define FADEMANAGER_H

#include <QObject>
#include <QWidget>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>


/*   Convenience class to fade in or out any QWidget,
 *   The class manages internally groups for you, making code cleaner.
 *   Created by Horoneru the 31/03/15
 *   V1.1.3 (updated 13/05/15)
*/


class FadeManager : public QObject
{
    Q_OBJECT
public:
    enum FadeMode { FadeOut, FadeIn };
    enum AnimationSequenceType { Parallel, Sequential };
    //Constructors
    explicit FadeManager(QObject *parent = 0);
    FadeManager(QWidget *target, FadeMode mode, int msecs,
                AnimationSequenceType sequence, QObject *parent);
    FadeManager(QWidget *target, int msecs, QObject *parent,
                FadeMode mode = FadeMode::FadeIn);
    FadeManager(QWidget *target, QObject *parent);
    FadeManager(FadeMode mode, QObject *parent);
    ~FadeManager();

    inline void setDefaultDuration(int msecs)
    {
        m_duration = msecs;
    }

    inline const int getDefaultDuration()
    {
        return m_duration;
    }

    void addToParallel(QAbstractAnimation *animation);
    void addToSequential(QAbstractAnimation *animation);

    void setSingleTarget(QWidget *target)
    {
        m_target = target;
    }

    void addTarget(QWidget *target);
    void addTarget(QWidget *target, FadeMode mode);
    void addTarget(QWidget *target, FadeMode mode,
                   AnimationSequenceType sequence);
    void addTarget(QWidget *target, int msecs);
    void addTarget(QWidget *target, FadeMode mode,
                   int msecs, AnimationSequenceType sequence);

    /* Those Use the target names and calls searchTarget(); to iterate */
    void deleteTarget(QWidget *targetToDelete, AnimationSequenceType fromWhichGroup);

    void editTarget(QWidget *targetToModify, FadeMode mode,
                    AnimationSequenceType fromWhichGroup = Parallel);
    void editTarget(QWidget *targetToModify, int msecs,
                    AnimationSequenceType fromWhichGroup = Parallel);
    void editTarget(QWidget *targetToModify, FadeMode mode, int msecs,
                    AnimationSequenceType fromWhichGroup = Parallel);

    void changeTargetGroup(QWidget *targetToModify,
                           AnimationSequenceType fromWhichGroup = Parallel);

    void clearGroup(AnimationSequenceType whichGroup);

    void start(bool deleteWhenFinished = true); //Start one animation with the arguments specified on constructor
    //Starts the animations in the group started
    void startGroup(AnimationSequenceType typeToStart, bool deleteWhenFinished = true);
    //Sets the entire group to FadeOut
    void fadeOutGroup(AnimationSequenceType typeToStart, bool deleteWhenFinished = true);
    //Sets the entire group to FadeIn
    void fadeInGroup(AnimationSequenceType typeToStart, bool deleteWhenFinished = true);

signals:
    //When finished, you will be notified, and know which group finished.
    void finished(FadeManager::AnimationSequenceType);

public slots:

    inline void parallelFinished()
    {
        if(m_deleteWhenFinished)
        {
            m_parallelAnimations.clear();
            m_deleteWhenFinished = false;
        }
        emit finished(FadeManager::Parallel);
    }

    inline void sequentialFinished()
    {
        if(m_deleteWhenFinished)
        {
            m_sequentialAnimations.clear();
            m_deleteWhenFinished = false;
        }
        emit finished(FadeManager::Sequential);
    }

private:
    void setMode(QPropertyAnimation *anim, FadeMode mode, QGraphicsOpacityEffect *container);
    void setGroupToFadeOut(AnimationSequenceType typeToChange);
    void setGroupToFadeIn(AnimationSequenceType typeToChange);
    QAbstractAnimation* searchTarget(QWidget *target, AnimationSequenceType inWhichGroup);

    //Attributes
    int m_duration; //Duration in ms
    QWidget *m_target = nullptr; //Used when starting only one animation
    QGraphicsOpacityEffect *m_effectContainer = nullptr; //Used when starting only one animation
    QSequentialAnimationGroup m_sequentialAnimations;
    QParallelAnimationGroup m_parallelAnimations;
    bool m_deleteWhenFinished;
    FadeMode m_mode; //FadeIn or FadeOut
    AnimationSequenceType m_sequenceType; //Parallel or Sequential
};

#endif // FADEMANAGER
