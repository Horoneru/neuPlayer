#include "skin.h"

Skin::Skin(QObject *parent) :
    QObject(parent)
{
    initSkinPalettes();
}
Skin::Skin(int skinId, QObject *parent) :
    QObject(parent), m_idSkin(skinId)
{
    initSkinPalettes();
}

void Skin::initSkinPalettes()
{
    //Dark Palette
    m_darkPalette.setColor(QPalette::Window, QColor(45,45,45));
    m_darkPalette.setColor(QPalette::WindowText, Qt::white);
    m_darkPalette.setColor(QPalette::Base, QColor(30,30,30));
    m_darkPalette.setColor(QPalette::AlternateBase, QColor(45,45,45));
    m_darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    m_darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    m_darkPalette.setColor(QPalette::Button, QColor(45,45,45));
    m_darkPalette.setColor(QPalette::ButtonText, Qt::white);
    m_darkPalette.setColor(QPalette::BrightText, Qt::red);
    m_darkPalette.setColor(QPalette::Text, Qt::white);
    m_darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

    m_darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    m_darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    m_darkPaletteCSS = "QToolTip { color: #ffffff; background-color: #353535; border: 1px solid #53a0ed; }";

    //Light palette
    m_lightPalette.setColor(QPalette::Base, QColor(220,220,220));
    m_lightPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    m_lightPalette.setColor(QPalette::Window, QColor(220,220,220));
    m_lightPalette.setColor(QPalette::WindowText, Qt::black);
    m_lightPalette.setColor(QPalette::AlternateBase, QColor(30,30,30));
    m_lightPalette.setColor(QPalette::ToolTipBase, Qt::black);
    m_lightPalette.setColor(QPalette::ToolTipText, Qt::black);
    m_lightPalette.setColor(QPalette::Button, QColor(220,220,220));
    m_lightPalette.setColor(QPalette::ButtonText, Qt::black);
    m_lightPalette.setColor(QPalette::BrightText, Qt::red);
    m_lightPalette.setColor(QPalette::Text, Qt::black);

    m_lightPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    m_lightPalette.setColor(QPalette::HighlightedText, Qt::white);
    m_lightPaletteCSS = "QToolTip { border: 1px solid #53a0ed; background-color: #f0f0f0}";
}

void Skin::load()
{
    if(m_idSkin == 0 || m_idSkin == 2)
    {
        qApp->setPalette(m_lightPalette);
        qApp->setStyleSheet(m_lightPaletteCSS);
    }
    else if(m_idSkin == 1  || m_idSkin == 3)
    {
        qApp->setPalette(m_darkPalette);
        qApp->setStyleSheet(m_darkPaletteCSS);
    }
}
Skin::~Skin()
{}
