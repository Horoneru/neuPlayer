#ifndef MOVEANIMATION_H
#define MOVEANIMATION_H

#include <QObject>
#include <QWidget>
#include <QPropertyAnimation>


/*   Convenience class to have a swipe/move animation on any QWidget.
 *   Created by Horoneru the 20/05/15
 *   V1.0.1
*/
class MoveAnimation : public QObject
{
    Q_OBJECT
public:
    enum MoveDuration { Slow = 400, Normal = 200 , Fast = 100 };
    enum Direction { LeftToRight, RightToLeft, BottomToTop, TopToBottom };
    explicit MoveAnimation(QObject *parent = 0);
    MoveAnimation (QWidget *target, MoveAnimation::Direction direction,
                   MoveAnimation::MoveDuration duration = Normal, QObject *parent = 0);
    MoveAnimation (QWidget *target, QObject *parent = 0);
    ~MoveAnimation();

    inline void setDuration(int duration)
    {
        m_duration = duration;
    }

    inline void setDirection (Direction direction)
    {
        m_direction = direction;
    }

    inline void setTarget(QWidget *target)
    {
        m_target = target;
    }

    void setEndPosition(QPoint pos)
    {
        m_anim.setEndValue(pos);
    }

    void setStartPosition(QPoint pos)
    {
        m_anim.setStartValue(pos);
    }

    void start();

private:
    void setupDirection();

    //Attributes
    QWidget *m_target = nullptr;
    int m_duration;
    Direction m_direction;
    QPropertyAnimation m_anim;

signals:
    void finished();

public slots:
    inline void hasFinished()
    {
        emit finished();
    }

};

#endif // MOVEANIMATION_H
