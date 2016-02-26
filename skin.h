#ifndef SKIN_H
#define SKIN_H

#include <QObject>
#include <QStyle>
#include <QApplication>
#include <QSettings>

class Skin : public QObject
{
    Q_OBJECT
public:
    explicit Skin(QObject *parent = 0);
    Skin (int skinId, QObject *parent);
    ~Skin();

    void setSkin(int id)
    {
        m_idSkin = id;
    }

    void load(); //Used when loaded (For both startup and runtime)
    //Use update(); on your objects or a container to apply the changes if loaded at runtime

private:
    void initSkinPalettes();
    //Attributes
    int m_idSkin;
    QPalette m_darkPalette;
    QString m_darkPaletteCSS;
    QPalette m_lightPalette;
    QString m_lightPaletteCSS;

};

#endif // SKIN_H
