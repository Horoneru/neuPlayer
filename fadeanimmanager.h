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
 *   V0.8 (updated 04/04/15), already implemented throughout the player.
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
    fadeAnimManager(QWidget *target, int msecs, QObject *parent);
    fadeAnimManager(QWidget *target, int msecs, QObject *parent, FadeMode mode = FadeMode::FadeIn);
    fadeAnimManager(QWidget *target, QObject *parent);
    fadeAnimManager(FadeMode mode, QObject *parent);

    inline const bool isFinished() const
    {
        return a_finished;
    }

    inline void setDefaultDuration(int msecs)
    {
        a_duration = msecs;
    }

    inline const int getDefaultDuration()
    {
        return a_duration;
    }

    void addParallelAnim(QAbstractAnimation *animation);
    void addSequentialAnim(QAbstractAnimation *animation);
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

    void start(); //Start one animation with the arguments specified on constructor
    void startGroup(AnimationSequenceType typeToStart, bool deleteWhenFinished = true); //Starts the animations in the group started
    void fadeOutGroup(AnimationSequenceType typeToStart, bool deleteWhenFinished = true); //Sets the entire group to FadeOut
    void fadeInGroup(AnimationSequenceType typeToStart, bool deleteWhenFinished = true); //Sets the entire group to FadeIn

signals:
    //TODO : may emit a signal when animations finishes to animate

public slots:

    inline void setFinished() //Called when the requested object finishes to animate if timer is enabled. TODO : actually implement everything correctly
    {
        a_finished = true;
    }

private:
    void setMode(QPropertyAnimation *anim, FadeMode mode, QGraphicsOpacityEffect *container);
    void initGroups();
    void setGroupToFadeOut(AnimationSequenceType typeToChange);
    void setGroupToFadeIn(AnimationSequenceType typeToChange);
    QAbstractAnimation* searchTarget(QWidget *target, AnimationSequenceType inWhichGroup);

    //Attributes
    int a_duration; //Duration in ms
    QWidget *a_target; //Used when starting only one animation
    QGraphicsOpacityEffect *a_effectContainer; //Used when starting only one animation
    QSequentialAnimationGroup *a_sequentialAnimations;
    QParallelAnimationGroup *a_parallelAnimations;
    bool a_timerEnabled;
    bool a_finished;
    bool a_groupAlreadyInit;
    FadeMode a_mode; //FadeIn or FadeOut
    AnimationSequenceType a_sequenceType; //Parallel or Sequential
};

#endif // FADEANIMMANAGER_H
