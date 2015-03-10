#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QMainWindow>
#include <QDebug>
#include "player.h"
#include <QModelIndex>
#include <QListWidgetItem>
namespace Ui {
class Playlist;
}
class Player;
class Playlist : public QMainWindow
{
    Q_OBJECT
public slots:
    //Basic interface with the player
    void playItem(QModelIndex itemIndex);
    void quickUpdate(QList <QUrl> *items, int currentItemPlusOne);
    void updateList(QMediaPlaylist *listeFichiers);
    //Own actions of the playlist interacting with the player
    void setFolder();
    void reloadLibrary();
    void randomModeChanger();
    void loopModeChanger();
    void setShuffle();
    void setSequential();
    void validateSearch();
    //Other
    void saveTheItems();

    void scrollToPlaying();
    void findItemVisibilityHandler();
    void prepareContextMenu();
    void showContextMenu();
    void deleteItem();
    void addItemToQueue();
    void viewInfo();
    //Events
    void closeEvent(QCloseEvent *);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
public:
    explicit Playlist(QMediaPlaylist *liste, int index, Player *player, QPixmap *cover, QString title, bool playingState, QWidget *parent = 0);
    explicit Playlist(QWidget *parent = 0);
    void setupConnections();
    void setupActions();
    void setupMode();
    /*Setters and getters */
    void setCurrentItem(int index, QPixmap *cover, QString title);
    //Icon
    void setToPlaying(int index);
    void setToPaused(int index);
    bool const isPlayingState()
    {
        return a_isPlaying;
    }

    void setHeader(QPixmap *cover, QString title); //Sets the cover art and the title
    int const queuedIndex() //getter
    {
        return ++a_queueIndex;
    }

    ~Playlist();

private:
    Ui::Playlist *ui;
    Player *a_player;
    bool a_isPlaying;
    bool a_isReload;
    QPixmap *a_defaultCover;
    int a_previousIndex;
    int a_currentIndex;
    int a_queueIndex;
    QList <QListWidgetItem*> a_backupItems;
    QAction *a_findItemTrigger; // ctrl-F to search ~
    QAction *a_validateFind; // Pouvoir trigger facilement un search sans farfouiller des events
    QAction *a_easyHideFind; // Pour pouvoir hide avec echap
    QMenu *a_playlistMenu; //This is for the ... button
    //Actions of the playlist menu
    QAction *a_reloadLibrary;
    QAction *a_selectOtherDir;
    QMenu *a_playlistContextMenu; //This is the context menu called by right-click on an item
    //Actions of the context menu
    QAction *a_addToQueue;
    QAction *a_viewInfo;
    QAction *a_deleteItemFromQueue;
    QAction *a_addToFav;
    QSettings *a_settings;
};

#endif // PLAYLIST_H
