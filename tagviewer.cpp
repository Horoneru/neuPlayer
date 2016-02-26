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
    ui->num->setText(m_metas.at(0));
    ui->title->setText(m_metas.at(1));
    ui->title->setToolTip(m_metas.at(1));
    ui->artist->setText(m_metas.at(2));
    ui->artist->setToolTip(m_metas.at(2));
    ui->album->setText(m_metas.at(3));
    ui->album->setToolTip(m_metas.at(3));
    ui->year->setText(m_metas.at(4));
    ui->genre->setText(m_metas.at(5));
    if(!cover->isNull())
        ui->coverArt->setPixmap(*cover);
}
TagViewer::~TagViewer()
{
    delete ui;
}
