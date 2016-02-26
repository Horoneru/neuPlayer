#ifndef FADEWINDOW_H
#define FADEWINDOW_H

#include <QObject>
#include <QDialog>
#include <QMainWindow>
#include <QWindow>
#include <QPropertyAnimation>

/*   Convenience class to fade easily windows
 *   Created by Horoneru the 30/03/15
*/


class FadeWindow : public QObject
{
    Q_OBJECT
public:
    enum FadeMode { FadeOut, FadeIn };
    explicit FadeWindow(QObject *parent = 0);
    ~FadeWindow();
    //We're sure to only have windows.
    FadeWindow(QDialog *dialogWidget, const int msecs, FadeMode = FadeMode::FadeIn, QObject *parent = nullptr);
    FadeWindow(QMainWindow *mainWindowWidget, const int msecs, FadeMode = FadeMode::FadeIn, QObject *parent = nullptr);
    FadeWindow(QWindow *windowWidget, const int msecs, FadeMode = FadeMode::FadeIn, QObject *parent = nullptr);
    FadeWindow(QDialog *dialogWidget, QObject *parent= nullptr);
    FadeWindow(QMainWindow *mainWindowWidget, QObject *parent = nullptr);
    FadeWindow(QWindow *windowWidget, QObject *parent = nullptr);

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

    void start(qreal endValue = 0.0);

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

    void setMode(QPropertyAnimation *anim, FadeMode mode, qreal endValue);
    //Attributes
    int a_duration;
    //All the windows inherit from QWidget, so to simplify things we're also sure it's a QObject.
    QObject *a_target = nullptr;
    FadeMode a_mode;
    bool a_timerEnabled;
    bool a_finished;
};

#endif // FADEWINDOW_H
