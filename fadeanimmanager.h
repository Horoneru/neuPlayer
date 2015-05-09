#ifndef FADEANIMMANAGER_H
#define FADEANIMMANAGER_H

#include <QObject>
#include <QWidget>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>


/*   Convenience class to fade in or out any QWidget,
 *   The class manages internally groups for you, making code cleaner.
 *   Created by Horoneru the 31/03/15
 *   V1.1 (updated 09/05/15), implemented throughout the player.
*/


class fadeAnimManager : public QObject
{
    Q_OBJECT
public:
    enum FadeMode { FadeOut, FadeIn };
    enum AnimationSequenceType { Parallel, Sequential };
    //Constructors
    explicit fadeAnimManager(QObject *parent = 0);
    fadeAnimManager(QWidget *target, FadeMode mode , int msecs, AnimationSequenceType sequence, QObject *parent);
    fadeAnimManager(QWidget *target, int msecs, QObject *parent, FadeMode mode = FadeMode::FadeIn);
    fadeAnimManager(QWidget *target, QObject *parent);
    fadeAnimManager(FadeMode mode, QObject *parent);
    ~fadeAnimManager();

    inline void setDefaultDuration(int msecs)
    {
        a_duration = msecs;
    }

    inline const int getDefaultDuration()
    {
        return a_duration;
    }

    void addToParallel(QAbstractAnimation *animation);
    void addToSequential(QAbstractAnimation *animation);
    void addTarget(QWidget *target);
    void addTarget(QWidget *target, FadeMode mode);
    void addTarget(QWidget *target, FadeMode mode,
                   AnimationSequenceType sequence);
    void addTarget(QWidget *target, int msecs);
    void addTarget(QWidget *target, FadeMode mode,
                   int msecs, AnimationSequenceType sequence);

    /* Those Use the target names and calls searchTarget(); to iterate */
    void deleteTarget(QWidget *targetToDelete, AnimationSequenceType fromWhichGroup);

    void editTarget(QWidget *targetToModify, FadeMode mode, AnimationSequenceType fromWhichGroup = Parallel);
    void editTarget(QWidget *targetToModify, int msecs, AnimationSequenceType fromWhichGroup = Parallel);
    void editTarget(QWidget *targetToModify, FadeMode mode, int msecs, AnimationSequenceType fromWhichGroup = Parallel);

    void changeTargetGroup(QWidget *targetToModify, AnimationSequenceType fromWhichGroup = Parallel);

    void clearGroup(AnimationSequenceType whichGroup);

    void start(bool deleteWhenFinished = true); //Start one animation with the arguments specified on constructor
    void startGroup(AnimationSequenceType typeToStart, bool deleteWhenFinished = true); //Starts the animations in the group started
    void fadeOutGroup(AnimationSequenceType typeToStart, bool deleteWhenFinished = true); //Sets the entire group to FadeOut
    void fadeInGroup(AnimationSequenceType typeToStart, bool deleteWhenFinished = true); //Sets the entire group to FadeIn

signals:
    void finished(fadeAnimManager::AnimationSequenceType); //When finished, you will be notified, and know which group finished.

public slots:

    inline void parallelFinished()
    {
        if(a_deleteWhenFinished)
        {
            a_parallelAnimations.clear();
            a_deleteWhenFinished = false;
        }
        emit finished(fadeAnimManager::Parallel);
    }

    inline void sequentialFinished()
    {
        if(a_deleteWhenFinished)
        {
            a_sequentialAnimations.clear();
            a_deleteWhenFinished = false;
        }
        emit finished(fadeAnimManager::Sequential);
    }

private:
    void setMode(QPropertyAnimation *anim, FadeMode mode, QGraphicsOpacityEffect *container);
    void setGroupToFadeOut(AnimationSequenceType typeToChange);
    void setGroupToFadeIn(AnimationSequenceType typeToChange);
    QAbstractAnimation* searchTarget(QWidget *target, AnimationSequenceType inWhichGroup);

    //Attributes
    int a_duration; //Duration in ms
    QWidget *a_target; //Used when starting only one animation
    QGraphicsOpacityEffect *a_effectContainer; //Used when starting only one animation
    QSequentialAnimationGroup a_sequentialAnimations;
    QParallelAnimationGroup a_parallelAnimations;
    bool a_deleteWhenFinished;
    FadeMode a_mode; //FadeIn or FadeOut
    AnimationSequenceType a_sequenceType; //Parallel or Sequential
};

#endif // FADEANIMMANAGER_H
