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
    //Master method : apply everything and close when pressing OK
    void confirm();
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
<<<<<<< HEAD
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
=======
    QSettings *a_settings = nullptr;
    Player *a_passerelle = nullptr;
    MoveAnimation a_moveAnim;
    QString a_bgPath;
    UpdaterHandler *a_handler = nullptr;
    bool a_isUpdateHandlerAlreadyCalled;
    bool a_isLibraryAtStartchecked;
    bool a_isDynamicLibChecked;
    bool a_isStaticLibChecked;
    bool a_isNewPath;
    int a_previousTabId;
    int a_fadeValue;
    double a_opacityValue;
>>>>>>> 8be90d0feae33059fced48e5c9cbf5b95bc64903
};

#endif // SETTINGS_H
