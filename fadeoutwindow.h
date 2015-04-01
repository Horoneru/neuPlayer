#ifndef FADEOUTWINDOW_H
#define FADEOUTWINDOW_H

#include <QObject>
#include <QDialog>
#include <QMainWindow>
#include <QWindow>
#include <QPropertyAnimation>

/*   Convenience class to fade out easily windows
 *   Created by Horoneru the 30/03/15
*/


class FadeOutWindow : public QObject
{
    Q_OBJECT
public:
    explicit FadeOutWindow(QObject *parent = 0);
    //We're sure to only have windows.
    FadeOutWindow(QDialog *dialogWidget, const int msecs, QObject *parent);
    FadeOutWindow(QMainWindow *mainWindowWidget, const int msecs, QObject *parent);
    FadeOutWindow(QWindow *windowWidget, const int msecs, QObject *parent);
    FadeOutWindow(QDialog *dialogWidget, QObject *parent);
    FadeOutWindow(QMainWindow *mainWindowWidget, QObject *parent);
    FadeOutWindow(QWindow *windowWidget, QObject *parent);

    //Duration of animation
    inline void setDuration(const int duration)
    {
        a_duration = duration;
    }
    //Do we activate the timer to report the animation finished ?
    void setTimerEnabled(const bool enabled)
    {
        a_timerEnabled = enabled;
    }

    void start();

    inline const bool isFinished() const
    {
        return a_finished;
    }

private slots:

    inline void setFinished() //Called when the requested object finishes to animate.
    {
        a_finished = true;
    }

private:
    int a_duration;
    QObject *a_target; //All the windows inherit from QWidget, so to simplify things we're also sure it's a QObject.
    bool a_timerEnabled;
    bool a_finished;
};

#endif // FADEOUTWINDOW_H
