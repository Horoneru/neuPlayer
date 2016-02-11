#ifndef TAGVIEWER_H
#define TAGVIEWER_H

#include <QDialog>

namespace Ui {
class TagViewer;
}

class TagViewer : public QDialog
{
    Q_OBJECT
public:
    explicit TagViewer(QList <QString> currentMediaInfo, QPixmap *cover,
                       QWidget *parent = 0);
    ~TagViewer();

private:
    void setupInfos(QPixmap *cover);

    //Attributes
    Ui::TagViewer *ui = nullptr;
<<<<<<< HEAD
    QList <QString> m_metas;
=======
    QList <QString> a_metas;
>>>>>>> 8be90d0feae33059fced48e5c9cbf5b95bc64903
};

#endif // TAGVIEWER_H
