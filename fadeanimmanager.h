#ifndef FADEANIMMANAGER_H
#define FADEANIMMANAGER_H

#include <QObject>
#include <QWidget>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>


//TODO : Get this class up and running for implementation


/*   Convenience class to fade in or out any QObject, including letting the class manage internally groups.
 *   NOTE:  Should be able to manage the fade in and out of InitialConfig. If it can't, drop the file out this project but keep it for later adjustments.
 *   Created by Horoneru the 31/03/15
*/


class fadeAnimManager : public QObject
{
    Q_OBJECT
public:
    enum FadeMode { FadeOut, FadeIn };
    enum AnimationSequenceType { NoSequence, Parallel, Sequential };
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

    void addParallelAnim(const QAbstractAnimation *animation);
    void addSequentialAnim(const QAbstractAnimation *animation);
    void addTarget(QWidget *target, FadeMode mode = FadeMode::FadeIn);
    void addTarget(QWidget *target);
    void addTarget(QWidget *target, FadeMode mode = FadeMode::FadeIn,
                   AnimationSequenceType sequence = AnimationSequenceType::NoSequence);
    void addTarget(QWidget *target, FadeMode mode = FadeMode::FadeIn,
                   int msecs = 300, AnimationSequenceType sequence = AnimationSequenceType::NoSequence);

    void start();
    void startGroup(AnimationSequenceType typeToStart);
signals:

public slots:

    inline void setFinished() //Called when the requested object finishes to animate.
    {
        a_finished = true;
    }

private:
    int a_duration; //Duration in ms
    QWidget *a_target; //Used when starting only one animation
    QPropertyAnimation *a_anim;
    QGraphicsOpacityEffect *a_effectContainer; //Used when starting only one animation
    QSequentialAnimationGroup *a_sequentialAnimations;
    QParallelAnimationGroup *a_parallelAnimations;
    bool a_timerEnabled;
    bool a_finished;
    FadeMode a_mode;
    AnimationSequenceType a_sequenceType;

};

#endif // FADEANIMMANAGER_H
