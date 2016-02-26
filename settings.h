#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include "player.h"
#include "skin.h"
#include "updaterhandler.h"
#include "moveanimation.h"
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
    void on_changeTab(int tabId);
    void setLibrary();
    void setSkin(int index);
    void changeBg();
    void reloadDefaultBg();
    void updateOpacity(int value);
    void confirm(); //Master method : apply everything and close when pressing OK
    void cancel();
    void gotoWorkingDir();
    void checkUpdates();
    void on_fadeClicked(bool enabled);
    void updateFadeValue(int value);
    void gotoIssues();
public:
    explicit Settings(Player *Player, QWidget *parent = 0);
    ~Settings();

private:
    void setupConfig();
    void setupConnections();

    //Attributes
    Ui::Settings *ui = nullptr;
    QSettings *m_settings = nullptr;
    Player *m_passerelle = nullptr;
    MoveAnimation m_moveAnim;
    QString m_bgPath;
    UpdaterHandler *m_handler = nullptr;
    bool m_isUpdateHandlerAlreadyCalled;
    bool m_isLibraryAtStartchecked;
    bool m_isDynamicLibChecked;
    bool m_isStaticLibChecked;
    bool m_isNewPath;
    int m_previousTabId;
    int m_fadeValue;
    double m_opacityValue;
};

#endif // SETTINGS_H
