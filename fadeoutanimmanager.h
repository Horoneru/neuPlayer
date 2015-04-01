#ifndef FADEOUTANIMMANAGER_H
#define FADEOUTANIMMANAGER_H

#include <QObject>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class fadeOutAnimManager : public QObject
{
    Q_OBJECT
public:
    explicit fadeOutAnimManager(QObject *parent = 0);
//    fadeOutAnimManager(QObject *target, FadeMode mode , int msecs, QObject *parent);
//    fadeOutAnimManager(QObject *target, int msecs, QObject *parent);
//    fadeOutAnimManager(QObject *target, QObject *parent);
//    fadeOutAnimManager(QObject *target, FadeMode mode, QObject *parent);
//    fadeOutAnimManager(QObject *target, FadeMode mode , int msecs, QObject *parent);
//    enum FadeMode { FadeOut, FadeIn };
signals:

public slots:


private:
    int a_duration; //Duration in ms
    QObject *a_target; //Used when starting only one animation
    QGraphicsOpacityEffect *a_effectContainer; //Used when starting only one animation
    QSequentialAnimationGroup *a_sequentialAnimations;
    QParallelAnimationGroup *a_parallelAnimations;
};

#endif // FADEOUTANIMMANAGER_H
