#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QMainWindow>
#include <QDebug>
#include <QModelIndex>
#include <QListWidgetItem>
#include <QPointer>
#include <QToolTip>

#include "neuplaylist.h"
#include "fademanager.h"
#include "moveanimation.h"
#include "player.h"
namespace Ui {
class Playlist;
}
class Player;
class neuPlaylist;
class Playlist : public QMainWindow
{
    Q_OBJECT
public slots:
    //Basic interface with the player
    void playItem(QModelIndex itemIndex);
    void quickUpdate(QList <QUrl> *items, int currentItemPlusOne);
    void updateList(neuPlaylist *listeFichiers, bool setZeroIndex = false);
    //Other
    void setTab(int tabId); //Called by :  user, programmatically, and by player when needed.
public:
    explicit Playlist(neuPlaylist &liste, neuPlaylist &favs, Player *player,
                      QPixmap *cover, QString title, bool playingState, QWidget *parent = 0);
    explicit Playlist(QWidget *parent = 0);
    void setupConnections();
    void setupActions();
    void setupMode();
    /*Setters and getters */
    void setCurrentItem(int index, QPixmap *cover, QString title, bool playing);
    //Icon
    void setToPlaying(int index);
    void setToPaused(int index);
    void deleteItem(int index);
    bool isPlayingState() const
    {
        return m_isPlaying;
    }

    void updateFavs(neuPlaylist *favPlaylist);
    void setHeader(QPixmap *cover, QString title); //Sets the cover art and the title
    int queuedIndex() //getter
    {
        return ++m_queueIndex;
    }

    ~Playlist();

private slots:
    void scrollToPlaying();
    void findItemVisibilityHandler();
    void prepareContextMenu();
    void showContextMenu() const;
    void deleteItem();
    void addItemToQueue();
    void addToFavContext();
    void addToFav();
    void viewInfo();
    void sendNewInfos();
    //Own actions of the playlist interacting with the player
    void setFolder();
    void on_starClicked();

    void reloadLibrary();
    void randomModeChanger();
    void loopModeChanger();
    void setShuffle();
    void setSequential();
    void validateSearch();
    void saveTheItems();

private:
    void updateInfoHeader(QString &title, QPixmap &cover);
    bool checkIfNotFav(int index);
    void deleteFav(int index);
    void makeStarFull();
    void makeStarEmpty();
    //Events
    void closeEvent(QCloseEvent *);

    //Attributes

    Ui::Playlist *ui = nullptr;
    Player *m_player = nullptr;
    neuPlaylist *m_playlist = nullptr;
    neuPlaylist *m_favPlaylist = nullptr;
    bool m_isPlaying;
    bool m_isReload;
    bool m_favsNotLoadedYet;
    QPixmap m_defaultCover;
    FadeManager m_fadeManager;
    MoveAnimation m_moveAnim;
    int m_previousIndex;
    int m_previousTab;
    int m_currentIndex;
    int m_queueIndex;
    QList <QListWidgetItem*> m_backupItems;
    QAction *m_findItemTrigger = nullptr;
    QAction *m_validateFind = nullptr; // Pouvoir trigger facilement un search sans farfouiller des events
    QAction *m_easyHideFind = nullptr; // Pour pouvoir hide avec echap
    QMenu *m_playlistMenu = nullptr; //This is for the ... button
    //Actions of the playlist menu
    QAction *m_reloadLibrary = nullptr;
    QAction *m_selectOtherDir = nullptr;
    QMenu *m_playlistContextMenu = nullptr; //This is the context menu called by right-click on an item
    //Actions of the context menu
    QAction *m_addToQueue = nullptr;
    QAction *m_viewInfo = nullptr;
    QAction *m_deleteItemFromQueue = nullptr;
    QAction *m_addToFav = nullptr;
    QSettings m_settings;
    QPointer <QMediaPlayer> m_tempPlayer;
};

#endif // PLAYLIST_H
