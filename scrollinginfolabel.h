#ifndef SCROLLTEXT_H
#define SCROLLTEXT_H

#include <QWidget>
#include <QLabel>
#include <QStaticText>
#include <QTimer>


/* This class is a slightly modified version of a class contributed by leemes of StackOverflow
 *
 *                              Please thank him for this great class !
 *
 * */

class ScrollingInfoLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QString separator READ separator WRITE setSeparator)

public:
    explicit ScrollingInfoLabel(QWidget *parent = 0);

public slots:
    QString text() const;
    void setText(QString &text);

    QString separator() const;
    void setSeparator(QString separator);

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);

private:
    void updateText();
    QString _text;
    QString _separator;
    QStaticText staticText;
    int singleTextWidth;
    QSize wholeTextSize;
    int leftMargin;
    bool scrollEnabled;
    int scrollPos;
    QImage alphaChannel;
    QImage buffer;
    QTimer timer;

private slots:
    virtual void timer_timeout();
};

#endif // SCROLLTEXT_H
