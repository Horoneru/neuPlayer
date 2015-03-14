#ifndef PLAYER_H
#define PLAYER_H

#include <QMainWindow>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>
#include <QtMultimedia/QMediaMetaData>
#include <QFileDialog>
#include <QFile>
#include <QSettings>
#include <QMenu>
#include <QTimer>
#include <QTime>
#include <QAction>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDropEvent>
#include <QDesktopServices>
#include <QFileInfo>
#include <QPropertyAnimation>
#include <QMessageBox>
#include <QGraphicsOpacityEffect>
#include <QThread>

#include "tagviewer.h"
#include "playlist.h"
#include "settings.h"
#include "scrollinginfolabel.h"
namespace Ui {
class Player;
}
class Playlist;
class ScrollingInfoLabel;
class Player : public QMainWindow
{
    Q_OBJECT
public slots:
    //Loading library
    void setupNewLibrary();
    void checkForNewMedias();
    void updateLibrary();
    void saveCurrentPlaylist();
    //Basic Media Controls
    void playMedia();
    QMediaPlayer::Error errorHandling(QMediaPlayer::Error error);
    void statusChanged(QMediaPlayer::MediaStatus status);
    void stateChanged(QMediaPlayer::State state);
    void setType();
    void finishingUp();
    void pauseMedia();
    void forwardMedia();
    void previousMedia();
    void endOfMediaGuard();
    //Show un QMenu...
    void showMenu();
    //Ouvre un file dialog natif pour choisir quoi lire
    void openMedia();
    //Quand on le lit et il avance...
    void UpdateProgress(qint64 pos);
    //Utilisateur bougeant le slide...
    void seekProgress(int pos);
    //Quand on charge le fichier...
    void on_durationChanged(qint64 pos);
    void advanceProgress();
    void backProgress();
    void setMeta();
    void update_info();
    //Volume
    void on_volumeChanged(int pos);
    void setVolumeMuted();
    void volumeUp();
    void volumeDown();
    //éléments menu
    void windowFlagsHandler();
    void setShuffle();
    void playbackHandler(QAction *playbackSelected);
    void setPlaybackRate();
    void showSettings();
    void showPlaylist();
    void showTagViewer(); //Uses QList <QString> for the sake of consistency
    //Load plugins selected
    void setupPlugins();
    void loadSkin();
    //Events
    void closeEvent(QCloseEvent *);
    void dragEnterEvent(QDragEnterEvent *event); //Autorise le drop
    void dropEvent(QDropEvent *event); //Effectue le traitement du drop

    //Accesseurs, mutateurs et senders pour interfacer avec la playlist
    void setPlaylistOpen(bool foo)
    {
        a_isPlaylistOpen = foo;
    }

    void setIndexOfThePlayer(int index, bool play);
    void addMediasToThePlayer(QList <QUrl> &medias);
    void setPlaylistToBeSavedOrNot(bool foo)
    {
        a_hasToSavePlaylistLater = foo;
    }
    void deleteMedia(int index)
    {
        a_deleteTriggered = true;
        a_settings->setValue("trackPosition", neu->position());
        int track = a_mediaPlaylist.currentIndex() - 1;
        a_mediaPlaylist.removeMedia(index);
        if(index < track)
            a_mediaPlaylist.setCurrentIndex(track);
        if(a_hasToSavePlaylistLater != true)
            a_hasToSavePlaylistLater = true;
    }

    void addToQueue(int index, int currentlyPlaying);

signals:
    void EndOfMedia();
public:
    Ui::Player *ui;
    explicit Player(QWidget *parent = 0);
    //Méthodes de constructeur
    void setupObjects();
    void setupMenus();
    void setupConnections();

    void forwardAnim(); //Created as a separate module because we call it at several places
    void previousAnim(); //Created for consistency with forwardAnim();
    //Convenience function
    void fadeInLabel();
    //Do all the check for which label to show and update it.
    void updateLabel(QString &text);
    // Routine to update anim target when necessary
    void updateAnimTarget();
    //Handles the change of the speed at which the info displayed changes
    void updateFadeinSpeed();
    //Méthodes accesseurs et mutateurs
    void setOpacity(qreal opacityFromSettings = 0.0);
    void setRandomMode(bool mode);
    void setLoopMode(bool mode);
    bool const getRandomState()
    {
        return a_isRandomMode;
    }
    bool const getLoopState()
    {
        return a_isLoopPlaylistMode;
    }

    void setPlaylistOfThePlayer(QList <QUrl> &medias);
    ~Player();

private:
    QMediaPlayer *neu; //Dat media player
    Playlist *playlist;
    QSettings *a_settings; //Contient les settings de l'application
    QString a_musicUserPath; // Contient un string de l'url du dossier musical de l'utilisateur
    QList <QUrl> a_files; //Conteneur pour récupérer l'url des fichiers audio séléctionnés par explorer
    QMediaPlaylist a_mediaPlaylist;
    int a_idSkin; // 0 : Light | 1 : Dark | n : custom theme
    bool a_hasToSavePlaylistLater;
    bool a_hasToStartupPlaylist;
    bool a_deleteTriggered;
    bool a_recoveredProgress;
    bool a_isPlaylistOpen;
    bool a_isRandomMode;
    bool a_isLoopPlaylistMode;
    bool a_hasToDoInitialShuffle;
    bool a_isPlaying;
    bool a_isScrollingText;
    bool a_hasToSetLabelType;
    int a_lastIndex;
    int a_secondesPasse;
    int a_volumeBeforeMute;
    int a_currentTrackTime;
    int a_currentTrackMinutes;
    //The three int delays at which we switch over
    int a_titleCase;
    int a_artistCase;
    int a_albumCase;
    QString a_titre, a_artiste, a_album;
    ScrollingInfoLabel *a_scrollingLabel;
    QPixmap a_coverArt;
    QPropertyAnimation *a_titleAnimate;
    QGraphicsOpacityEffect *a_infoFadein;
    QPropertyAnimation *a_infoFadeAnim;
    //Regular timer, updated each second
    QTimer Timer;
    //The timer which sets the type when it timeouts
    QTimer setTypeTimer;
    /*          Permet l'utilisation de raccourcis clavier
     * qui interagissent directement avec le player sans souris */
    QAction *a_advance;
    QAction *a_back;
    QAction *a_volumeUp;
    QAction *a_volumeDown;
    //Menu et ses actions
    QMenu *a_playbackMenu;
    QActionGroup *a_playbackrates;
    QAction *normalrate;
    QAction *halfrate;
    QAction *doublerate;
    int a_playbackState;
    QList <QAction> actions;
    QMenu *a_menu;
    QAction *a_openMedia;
    QAction *a_accessSettings;
    QAction *a_tagViewer;
    QAction *a_showPlaylist;
    QAction *a_alwaysOnTopHandler; //Set les flags pour always on top ou non
    QAction *a_shuffle;
};
#endif // PLAYER_H