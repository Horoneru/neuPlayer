#include "tagviewer.h"
#include "ui_tagviewer.h"

TagViewer::TagViewer(QList<QString> currentMediaInfo, QPixmap *cover, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TagViewer)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    m_metas = currentMediaInfo;
    setupInfos(cover); //send the only thing missing
}

void TagViewer::setupInfos(QPixmap *cover)
{
    ui->m_num->setText(m_metas.at(0));
    ui->m_title->setText(m_metas.at(1));
    ui->m_title->setToolTip(m_metas.at(1));
    ui->m_artist->setText(m_metas.at(2));
    ui->m_artist->setToolTip(m_metas.at(2));
    ui->m_album->setText(m_metas.at(3));
    ui->m_album->setToolTip(m_metas.at(3));
    ui->m_year->setText(m_metas.at(4));
    ui->m_genre->setText(m_metas.at(5));
    if(!cover->isNull())
        ui->m_coverArt->setPixmap(*cover);
}
TagViewer::~TagViewer()
{
    delete ui;
}
