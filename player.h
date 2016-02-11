#ifndef PLAYER_H
#define PLAYER_H

#include <QMainWindow>
#include <QtMultimedia/QMediaPlayer>
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
#include <QParallelAnimationGroup>
#include <QMessageBox>
#include <QGraphicsOpacityEffect>
#include <QBitmap>
#include <QPainter>
#include <QRegion>

#ifdef Q_OS_WIN
#include <QtWinExtras/QWinThumbnailToolButton>
#include <QtWinExtras/QWinThumbnailToolBar>
#endif

#include "tagviewer.h"
#include "playlist.h"
#include "slider.h"
#include "settings.h"
#include "neuplaylist.h"
#include "fademanager.h"
#include "fadewindow.h"
#include "scrollinglabel.h"
namespace Ui {
class Player;
}
class Playlist;
class ScrollingLabel;
class Settings;
class Player : public QMainWindow
{
    Q_OBJECT
public slots:
    //Loading library
    void setupNewLibrary();
    void checkForNewMedias();
    void updateLibrary();
    void savePlaylists();
    //Basic Media Controls
    void playMedia();
    void errorHandling(QMediaPlayer::Error error);
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

    void loadSkin();
    //Events
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event); //Autorise le drop
    void dropEvent(QDropEvent *event); //Effectue le traitement du drop
    void delayedClose();

signals:
    void EndOfMedia();
public:
    Ui::Player *ui;
    explicit Player(QWidget *parent = 0);
    void restart();
    void setupPlayMode();
    void loadPlaylist();
    void setLightCSS();
    void setDarkCSS();
    //Do all the check for which label to show and update it.
    void updateLabel(QString &text);
    // Routine to update anim target when necessary
    void updateAnimTarget();
    //Handles the change of the speed at which the info displayed changes
    void updateFadeinSpeed();
    //Méthodes accesseurs et mutateurs

    void setAudioFadeValue(int value)
    {
        m_audioFade->setDuration(value);
    }

    void setAudioFade(bool enabled)
    {
        m_audioFadeActivated = enabled;
    }

    void runAudioFade();
    void setPlaylistOpen(bool open)
    {
        m_isPlaylistOpen = open;
        if(!m_isPlaylistOpen)
            m_playlist = nullptr;
    }
    void setSettingsOpen(bool open)
    {
        m_isSettingsOpen = open;
    }

    inline int getIndexOfThePlayer() //Will always return the correct index regardless of the playlist used
    {
        return neu->playlist()->currentIndex();
    }

    void setIndexOfThePlayer(int index, bool play);
    void addMediasToThePlayer(QList <QUrl> &medias);
    void setPlaylistToBeSaved(bool foo)
    {
        m_hasToSavePlaylistLater = foo;
    }

    void setFavsToBeSaved(bool foo)
    {
        m_hasToSaveFavsLater = foo;
    }

    bool deleteMedia(int index, int tab = 0)
    {
        if(!m_isUsingFavPlaylist)
        {
            if(tab == 0)
            {
                if(m_mediaPlaylist.currentIndex() == index)
                    return false;
            }
        }
        else
        {
            if(tab == 1)
            {
                if(m_favPlaylist.currentIndex() == index)
                    return false;
            }
        }
        m_settings.setValue("trackPosition", neu->position());
        int track = neu->playlist()->currentIndex() - 1;
        if(tab == 0)
            m_mediaPlaylist.removeMedia(index);
        else
            m_favPlaylist.removeMedia(index);
        if(tab == 0 && !m_isUsingFavPlaylist)
        {
            if(index <= track)
            {
                m_deleteTriggered = true;
                m_mediaPlaylist.setCurrentIndex(track);
            }
        }
        else if (tab == 1 && m_isUsingFavPlaylist)
        {
            if(index <= track)
            {
                m_deleteTriggered = true;
                m_favPlaylist.setCurrentIndex(track);
            }
        }
        if(m_hasToSavePlaylistLater != true)
            m_hasToSavePlaylistLater = true;
        //The update of m_hasToSaveFavsLater is handled by the playlist if needed
        return true;
    }

    void addFav(int index);

    void changeToFavPlaylist();
    void changeToDefaultPlaylist();

    void addToQueue(int index, int currentlyPlaying);
    void setOpacity(qreal opacityFromSettings = 0.0);
    void setRandomMode(bool mode);
    void setLoopMode(bool mode);
    bool getRandomState() const
    {
        return m_isRandomMode;
    }
    inline void setUsingFav(bool usingFav)
    {
        m_isUsingFavPlaylist = usingFav;
    }

    inline bool isUsingFav() const
    {
        return m_isUsingFavPlaylist;
    }
    bool getLoopState() const
    {
        return m_isLoopPlaylistMode;
    }
    inline bool canChangeMusic() const
    {
        return m_canChangeMusic;
    }
    bool deleteTriggered() const
    {
        return m_deleteTriggered;
    }
    void updatePlaylistOfThePlayer(const QList <QUrl> &medias, bool play = false);
    void saveBeforeClosing();

    ~Player();


private slots:
    inline void canChangeMusicNow()
    {
        m_canChangeMusic = true;
    }
    inline void canShuffleNow()
    {
        m_canDoShuffleAgain = true;
    }
private:
    //Méthodes de constructeur
    void setupObjects();
    void setupMenus();
    void setupConnections();
    //Load plugins selected
    void setupPlugins();
    void setFramelessButtonsVisibility(bool visible);

    void forwardAnim(); //Created as a separate module because we call it at several places
    void previousAnim(); //Created for consistency with forwardAnim();
    void fadeInLabel(); //Convenience function
    void updateProgressDisplay(qint64 pos, bool isProgress); //Used to update the display of both duration (isProgress = false) and progress (isProgress = true)

    //Attributs
    QMediaPlayer *neu = nullptr; //Dat media player
<<<<<<< HEAD
    Slider *m_progressSlider = nullptr;
    Slider *m_volumeSlider = nullptr;
    QPointer <Playlist> m_playlist = nullptr;
    QPointer <Settings> m_settingsForm = nullptr;
=======
    Slider *a_progressSlider = nullptr;
    Slider *a_volumeSlider = nullptr;
    QPointer <Playlist> a_playlist = nullptr;
    QPointer <Settings> a_settingsForm = nullptr;
>>>>>>> 8be90d0feae33059fced48e5c9cbf5b95bc64903

    /* Extras for win */
#ifdef Q_OS_WIN
    QWinThumbnailToolButton m_thumbActionButton;
    QWinThumbnailToolButton m_thumbPreviousButton;
    QWinThumbnailToolButton m_thumbForwardButton;
    QWinThumbnailToolBar m_thumbnailToolbar;
#endif

    QSettings m_settings; //Contient les settings de l'application
    neuPlaylist m_mediaPlaylist;
    neuPlaylist m_favPlaylist;
    QPointer<FadeManager> m_animManager;
    int m_idSkin; // 0 : Light | 1 : Dark | n : custom theme
    int m_previousIndex;
    bool m_isFrameless;
    bool m_hasToSavePlaylistLater;
    bool m_hasToSaveFavsLater;
    bool m_wasPrevious;
    bool m_hasToStartupPlaylist;
    bool m_deleteTriggered;
    bool m_recoveredProgress;
    bool m_isSettingsOpen;
    bool m_isPlaylistOpen;
    bool m_isRandomMode;
    bool m_isLoopPlaylistMode;
    bool m_hasToDoInitialShuffle;
    bool m_isPlaying;
    bool m_isScrollingText;
    bool m_hasToSetLabelType;
    bool m_canClose;
    bool m_canChangeMusic;
    bool m_canDoShuffleAgain;
    bool m_isStarting;
    bool m_isUsingFavPlaylist;
    bool m_audioFadeActivated;
    bool m_no_meta;
    int m_secondesPasse;
    int m_volumeBeforeMute;
    //The three int delays at which we switch over
    int m_titleCase;
    int m_artistCase;
    int m_albumCase;
    //Meta datas
    QString m_titre, m_artiste, m_album;
    /* Icons stored on attributes to avoid reloading each time */
    const QIcon m_forwardIcon, m_forwardDarkIcon;
    const QIcon m_previousIcon, m_previousDarkIcon;
    const QIcon m_volumeIcon, m_volumeDarkIcon;
    const QIcon m_volumeLowIcon, m_volumeLowDarkIcon;
    const QIcon m_volumeMutedIcon, m_volumeMutedDarkIcon;
    const QPixmap m_neuDarkBg, m_neuLightBg, m_neuLightCustombg, m_neuDarkCustombg;

<<<<<<< HEAD
    ScrollingLabel m_scrollingLabel; //Custom label
    QPixmap m_coverArt;
    QPropertyAnimation *m_titleAnimate = nullptr;
    QPropertyAnimation *m_audioFade = nullptr;
    QGraphicsOpacityEffect *m_infoFadein = nullptr;
    QPropertyAnimation *m_infoFadeAnim = nullptr;
=======
    ScrollingLabel a_scrollingLabel; //Custom label
    QPixmap a_coverArt;
    QPropertyAnimation *a_titleAnimate = nullptr;
    QPropertyAnimation *a_audioFade = nullptr;
    QGraphicsOpacityEffect *a_infoFadein = nullptr;
    QPropertyAnimation *a_infoFadeAnim = nullptr;
>>>>>>> 8be90d0feae33059fced48e5c9cbf5b95bc64903
    //Regular timer, updated each second
    QTimer Timer;
    //The timer which sets the type when it timeouts
    QTimer setTypeTimer;
    //The timers slowing down operations so there's no crash
    QTimer grantChangeTimer;
    QTimer grantShuffleAgainTimer;
    /*          Permet l'utilisation de raccourcis clavier
     * qui interagissent directement avec le player sans souris */
    QAction m_advance;
    QAction m_back;
    QAction m_volumeUp;
    QAction m_volumeDown;
    //Menu et ses actions
    QMenu m_playbackMenu;
    QActionGroup m_playbackrates;
    QAction m_normalrate;
    QAction m_halfrate;
    QAction m_doublerate;
    int m_playbackState;
    QMenu m_menu;
    QAction m_openMedia;
    QAction m_accessSettings ;
    QAction m_tagViewer;
    QAction m_showPlaylist;
    QAction m_alwaysOnTopHandler; //Set les flags pour always on top ou non
    QAction m_shuffle;
};

#endif // PLAYER_H
