#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include "player.h"
#include "skin.h"
#include "updaterhandler.h"
namespace Ui {
class Settings;
}
class Player;
class Settings : public QDialog
{
    Q_OBJECT
private slots:
    void enableLibraryAtStartup();
    void changeMusicPath();
    void on_RNlibActivated();
    void on_staticLibActivated();
    void popupFramelessWindow();
    void setLibrary();
    void setSkin(int index);
    void changeBg();
    void reloadDefaultBg();
    void updateOpacity(int value);
    void confirm(); //Master method : apply everything and close when pressing OK
    void cancel();
    void gotoWorkingDir();
    void checkUpdates();
public:
    explicit Settings(Player *Player, QWidget *parent = 0);
    ~Settings();

private:
    void setupConfig();
    void setupConnections();

    //Attributes
    Ui::Settings *ui;
    QSettings *a_settings;
    Player *a_passerelle;
    QString a_bgPath;
    UpdaterHandler *a_handler;
    bool a_isUpdateHandlerAlreadyCalled;
    bool a_isLibraryAtStartchecked;
    bool a_isDynamicLibChecked;
    bool a_isStaticLibChecked;
    bool a_isNewPath;
    double a_opacityValue;
};

#endif // SETTINGS_H
