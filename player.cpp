#include "player.h"
#include "ui_player.h"
#include <QDebug>
Player::Player(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Player),
    /* Background ressources */
    m_neuDarkBg(":/Ressources/neudarkbg.png"), m_neuLightBg(":/Ressources/neubackgroundwhite.jpg"), m_neuLightCustombg(":/Ressources/neucustombackgroundwhite.jpg"),
    m_neuDarkCustombg(":/Ressources/neucustombackgroundblack.jpg"),
    /* Volume icons ressources */
    m_volumeIcon(":/Ressources/volumebtn.png"), m_volumeDarkIcon(":/Ressources/volumedarkbtn.png"), m_volumeMutedIcon(":/Ressources/volumebtn_onPressed.png"),
    m_volumeMutedDarkIcon(":/Ressources/volumedarkbtn_onPressed.png"), m_volumeLowIcon(":/Ressources/volumebtn2.png"), m_volumeLowDarkIcon(":/Ressources/volumedarkbtn2.png"),
    /* No CSS controls ressources */
    m_previousIcon(":/Ressources/previousbtn.png"), m_previousDarkIcon(":/Ressources/previousdarkbtn.png"),
    m_forwardIcon(":/Ressources/forwardbtn.png"), m_forwardDarkIcon(":/Ressources/forwarddarkbtn.png"),
    /* Actions */
    m_accessSettings(tr("Paramètres"), this), m_alwaysOnTopHandler(tr("Toujours visible"), this), m_advance(this), m_back(this),
    m_doublerate("2.0x", this), m_halfrate("0.5x", this), m_normalrate("1.0x", this), m_playbackrates(this),
    m_openMedia(tr("Ouvrir des fichiers"), this),  m_showPlaylist(tr("Ouvrir la liste de lecture"), this), m_shuffle(tr("Mélanger la playlist"), this), m_tagViewer(tr("Voir les tags"), this), m_volumeDown(this), m_volumeUp(this),
    /* Menus */
    m_playbackMenu (tr("Vitesse de lecture"), this), m_menu (this),
    /* Settings */
    m_settings ("neuPlayer.ini", QSettings::IniFormat, this)
{
    /*!
                                            2015 Horoneru                                   2.0.1 stable 310515 active
      TODO
      à faire : (/ ordre d'importance)
      > Indexing music infos somehow or another
      - Further skinning options ! (Coming later maybe)
      > UPDATE TRANSLATIONS
      - (Optional) plugin manager musiques osu! << gérer par delete des filenames
      */
    ui->setupUi(this);
    QApplication::setApplicationVersion("2.1.0");
    this->setAcceptDrops(true);
    this->setAttribute(Qt::WA_AlwaysShowToolTips);

    setupObjects();

    setupMenus();
    // Bool to control playlist state
    m_isPlaylistOpen = false;

    //Same logic
    m_isSettingsOpen = false;

    //Bool to detect a delete. Used to manage a possible problem after a delete
    m_deleteTriggered = false;

    //Bool to do initial shuffle when selecting random play and forwarding
    m_hasToDoInitialShuffle = false;

    //self-explanatory
    m_hasToStartupPlaylist = false;

    //Bool to control whether we have to set the type of label (Scrolling Label or normal)
    m_hasToSetLabelType = false;

    //Bool to indicate to the playlist if the player is playing
    m_isPlaying = false;

    //Bool to indicate that we were using the previous button. It'll launch the animation when receiving meta-data
    m_wasPrevious = false;

    //Bool to recover progress when needed
    m_recoveredProgress = true;

    //Bool to control if we have to animate the window when closing
    m_canClose = true;

    //Bool to avoid the user to go too fast.
    m_canChangeMusic = true;

    //Bool to avoid the user going too fast again when shuffling the playlist
    m_canDoShuffleAgain = true;

    //Bool that is used to know if we're starting up
    m_isStarting = true;

    //Bool to indicate the playlist needs to be refreshed on the disk
    m_hasToSavePlaylistLater = false;

    ui->m_pausebtn->setVisible(false);

    if(m_settings.value("Additional_Features/framelessWindow", false).toBool() == true)
    {
        m_isFrameless = true;
        setFramelessButtonsVisibility(true);
        m_canClose = false;
    }
    else
    {
        m_isFrameless = false;
        setFramelessButtonsVisibility(false);
    }
    m_audioFade = new QPropertyAnimation(neu, "volume", this);
    m_audioFade->setStartValue(0);
    m_audioFade->setDuration(m_settings.value("fadeValue").toInt());
    if(m_settings.value("Additional_Features/audioFade", false).toBool() == true)
        setAudioFade(true);

    setPlaybackRate();

    setupConnections();

    //Paramètres loadés après car les connexions se chargent de traiter les nouvelles données
    setupPlugins();

    setupPlayMode();

    m_volumeSlider->setValue(m_settings.value("volume", 50).toInt());

    m_alwaysOnTopHandler.setChecked(m_settings.value("visibilite", false).toBool());

    if(m_settings.value("playlistAtStartup").toBool() == true)
        m_hasToStartupPlaylist = true; //Will trigger the playlist startup

    setOpacity();

    m_isStarting = false;    //Done !
}

    /*///////Plugins & Setup Section///////*/

void Player::setupObjects()
{
    /*///////Setup Objects and Data///////*/
    resize(m_settings.value("size", QSize(400, 47) ).toSize());
    move(m_settings.value("pos", QPoint(200,200)).toPoint());
    m_playbackState = m_settings.value("playbackrate", 0).toInt();
    m_isRandomMode = m_settings.value("random", false).toBool();
    m_isLoopPlaylistMode = m_settings.value("loop", false).toBool();

    //Media Player
    neu = new QMediaPlayer(this);

    //Prepare custom Sliders
    m_progressSlider = new Slider(ui->centralWidget);
    m_progressSlider->setGeometry(QRect(132, 26, 110, 20));
    m_progressSlider->setOrientation(Qt::Horizontal);
    m_progressSlider->setInvertedAppearance(false);
    m_progressSlider->setInvertedControls(false);
    m_progressSlider->setTickPosition(QSlider::NoTicks);
    m_progressSlider->setTickInterval(10);
    m_progressSlider->setToolTip(tr("Alt + droite ou gauche pour parcourir"));


    m_volumeSlider = new Slider(ui->centralWidget);
    m_volumeSlider->setGeometry(QRect(310, 28, 60, 16));
    m_volumeSlider->setMaximum(99);
    m_volumeSlider->setValue(50);
    m_volumeSlider->setOrientation(Qt::Horizontal);
    m_volumeSlider->setToolTip(tr("Volume (Ctrl haut ou bas) "));

    //Prepare animation
    m_titleAnimate = new QPropertyAnimation(ui->m_label, "pos", this);
    m_titleAnimate->setDuration(200); //Always 200 ms
    m_infoFadein = new QGraphicsOpacityEffect(this);
    m_infoFadein->setOpacity(1.0);
    ui->m_label->setGraphicsEffect(m_infoFadein);
    m_infoFadeAnim = new QPropertyAnimation(m_infoFadein, "opacity", this);

    //Prepare the sliding text object
    QFont font;
    font.setFamily(QStringLiteral("Segoe UI"));
    font.setPointSize(10);
    m_scrollingLabel.setParent(this);
    m_scrollingLabel.setFont(font);
    m_scrollingLabel.setGeometry(QRect(100, 0, 175, 31));
    m_scrollingLabel.setMinimumSize(QSize(170, 30));

    //Raccourcis clavier pour parcourir les éléments du player
    m_advance.setShortcut(Qt::ALT + Qt::Key_Right);
    m_back.setShortcut(Qt::ALT + Qt::Key_Left);
    ui->m_volumebtn->setCheckable(true);
    m_volumeUp.setShortcut(Qt::CTRL + Qt::Key_Up);
    m_volumeDown.setShortcut(Qt::CTRL + Qt::Key_Down);

    this->addAction(&m_advance);
    this->addAction(&m_back);
    this->addAction(&m_volumeUp);
    this->addAction(&m_volumeDown);

    //Elements du menu :
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    m_openMedia.setShortcut(Qt::CTRL + Qt::Key_O);
    m_showPlaylist.setShortcut(Qt::CTRL + Qt::Key_P); //P comme Playlist ?
    m_alwaysOnTopHandler.setCheckable(true);
    m_alwaysOnTopHandler.setShortcut(Qt::CTRL + Qt::Key_V); // V comme visible :3
    m_accessSettings.setShortcut(Qt::CTRL + Qt::Key_Space);
    m_accessSettings.setShortcutContext(Qt::ApplicationShortcut);
    m_shuffle.setShortcut(Qt::CTRL + Qt::Key_R); // R comme random !
    m_tagViewer.setShortcut(Qt::ALT + Qt::Key_Return);

    //Timer enabling the scrolling of the label info
    Timer.setInterval(1000);
    //Timer that delays the check of which label type to use
    setTypeTimer.setSingleShot(true);
    //Timer that delays When the user can change music again (avoid reading null data when going too fast)
    grantChangeTimer.setSingleShot(true);
    //Timer that delays the time when the user can shuffle again
    grantShuffleAgainTimer.setSingleShot(true);

}

void Player::setupMenus()
{

    //Sous-menu playback rate
    m_normalrate.setCheckable(true);
    m_doublerate.setCheckable(true);
    m_halfrate.setCheckable(true);
    m_playbackrates.addAction(&m_doublerate);
    m_playbackrates.addAction(&m_normalrate);
    m_playbackrates.addAction(&m_halfrate);
    m_playbackrates.setExclusive(true);
    m_playbackMenu.addAction(&m_halfrate);
    m_playbackMenu.addAction(&m_normalrate);
    m_playbackMenu.addAction(&m_doublerate);

    //Setup The menu
    m_menu.addAction(&m_showPlaylist);
    m_menu.addAction(&m_openMedia);
    m_menu.addAction(&m_accessSettings);
    m_menu.addMenu(&m_playbackMenu);
    m_menu.addAction(&m_alwaysOnTopHandler);
    m_menu.addAction(&m_shuffle);
    m_menu.addAction(&m_tagViewer);
    this->addAction(&m_openMedia);
    this->addAction(&m_showPlaylist);
    this->addAction(&m_alwaysOnTopHandler);
    this->addAction(&m_shuffle);
    this->addAction(&m_accessSettings);
    this->addAction(&m_tagViewer);

}

void Player::setupPlayMode()
{
    if(m_isRandomMode)
    {

        m_mediaPlaylist.setPlaybackMode(QMediaPlaylist::Random);
        m_favPlaylist.setPlaybackMode(QMediaPlaylist::Random);
        m_hasToDoInitialShuffle = true;
    }
    else
    {
        m_mediaPlaylist.setPlaybackMode(QMediaPlaylist::Sequential);
        m_favPlaylist.setPlaybackMode(QMediaPlaylist::Sequential);
    }
}


void Player::setupConnections()
{
    /*///////Connexions///////*/
    connect(ui->m_playbtn, SIGNAL(clicked()), this, SLOT(playMedia()));
    connect(neu, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(errorHandling(QMediaPlayer::Error)));
    connect(neu, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(statusChanged(QMediaPlayer::MediaStatus)));
    connect(neu, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(stateChanged(QMediaPlayer::State)));
    connect(ui->m_forward, SIGNAL(clicked()), this, SLOT(forwardMedia()));
    connect(ui->m_previous, SIGNAL(clicked()), this, SLOT(previousMedia()));
    connect(ui->m_menubtn, SIGNAL(clicked()), this, SLOT(showMenu()));
    connect(&m_advance, SIGNAL(triggered()), this, SLOT(advanceProgress()));
    connect(&m_back, SIGNAL(triggered()), this, SLOT(backProgress()));
    connect(&m_mediaPlaylist, SIGNAL(loaded()), this, SLOT(finishingUp()));
    connect(neu, SIGNAL(durationChanged(qint64)), this, SLOT(on_durationChanged(qint64)));
    connect(neu, SIGNAL(positionChanged(qint64)), this, SLOT(UpdateProgress(qint64)));
    connect(m_progressSlider, SIGNAL(sliderMoved(int)), this, SLOT(seekProgress(int)));
    connect(m_volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(on_volumeChanged(int)));
    connect(ui->m_volumebtn, SIGNAL(clicked()), this, SLOT(setVolumeMuted()));
    connect(&m_volumeUp, SIGNAL(triggered()), this, SLOT(volumeUp()));
    connect(&m_volumeDown, SIGNAL(triggered()), this, SLOT(volumeDown()));
    connect(neu, SIGNAL(metaDataChanged()), this, SLOT(setMeta()));
    connect(&Timer, SIGNAL(timeout()), this, SLOT(update_info()));
    connect(&setTypeTimer, SIGNAL(timeout()), this, SLOT(setType()));
    connect(&grantChangeTimer, SIGNAL(timeout()), this, SLOT(canChangeMusicNow()));
    connect(&grantShuffleAgainTimer, SIGNAL(timeout()), this, SLOT(canShuffleNow()));
    connect(this, SIGNAL(EndOfMedia()), this, SLOT(endOfMediaGuard()));
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showMenu()));
    connect(&m_openMedia, SIGNAL(triggered()), this, SLOT(openMedia()));
    connect(&m_alwaysOnTopHandler, SIGNAL(toggled(bool)), this, SLOT(windowFlagsHandler()));
    connect(&m_playbackrates, SIGNAL(triggered(QAction*)), this, SLOT(playbackHandler(QAction*)));
    connect(&m_shuffle, SIGNAL(triggered()), this, SLOT(setShuffle()));
    connect(&m_showPlaylist, SIGNAL(triggered()), this, SLOT(showPlaylist()));
    connect(&m_tagViewer, SIGNAL(triggered()), this, SLOT(showTagViewer()));
    connect(&m_accessSettings, SIGNAL(triggered()), this, SLOT(showSettings()));
    connect(&m_mediaPlaylist, SIGNAL(loaded()), this, SLOT(setupNewLibrary()));
    connect(ui->m_closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->m_hideButton, SIGNAL(clicked()), this, SLOT(showMinimized()));

#ifdef Q_OS_WIN
    connect(&m_thumbActionButton, SIGNAL(clicked()), this, SLOT(playMedia())); //Base connect
    connect(&m_thumbForwardButton, SIGNAL(clicked()), this, SLOT(forwardMedia()));
    connect(&m_thumbPreviousButton, SIGNAL(clicked()), this, SLOT(previousMedia()));
#endif
}

void Player::setupPlugins()
{
    loadSkin();
    if(m_settings.value("Additional_Features/libraryAtStartup", false).toBool() == true)
    {
        //Modes de chargement de la librairie

        ui->m_label->setText("<No Media>");
        if(m_settings.value("Additional_Features/refreshWhenNeeded", false).toBool() == true)
        {
            QFile fileHandler(".configdone");
            if(!fileHandler.exists())
            {
                updateLibrary();
                if(m_settings.value("playlistAtStartup", false).toBool() == true)
                    deleteLater();
            }
            else
                checkForNewMedias();
        }

        loadPlaylist(); // We will load the library even if it isn't in refreshWhenNeeded
    }
    QFile fileHandler(".configdone");
    fileHandler.open(QFile::ReadWrite); //Create it and I'm done
    fileHandler.close();
}

void Player::loadPlaylist()
{
    if(m_settings.value("usingFavorites", false).toBool() == true) //Are we on the favorites playlist ?
    {
        m_isUsingFavPlaylist = true;
        disconnect(&m_mediaPlaylist, SIGNAL(loaded()), this, SLOT(setupNewLibrary()));
        connect(&m_favPlaylist, SIGNAL(loaded()), this, SLOT(setupNewLibrary()));

    }
    else
        setUsingFav(false);
    m_mediaPlaylist.load(QUrl::fromLocalFile("neuLibrary.m3u8"), "m3u8");  //We load both anyways
    m_favPlaylist.load(QUrl::fromLocalFile("favorites.m3u8"), "m3u8");
}

void Player::setOpacity(qreal opacityFromSettings)
{
    if(opacityFromSettings != 0) //This is used for live preview from settings. Its default value is 0 and it's an optional parameter
    {
        this->setWindowOpacity(opacityFromSettings);
        if(opacityFromSettings != 1.0)
            m_canClose = false; //Handle the fade out anim
        else
            if(!m_isFrameless) //Frameless also isn't handled by the system anymore
                m_canClose = true;
        return;
    }

    if(m_settings.value("opacity", 1.0).toReal() <= 0) //Because I know someone will somehow set a 0 opacity...
    {
        this->setWindowOpacity(1.0);
        return;
    }
    if(m_settings.value("opacity", 1.0).toReal() < 1.0)
    {
        this->setWindowOpacity(m_settings.value("opacity").toReal());
        m_canClose = false; //Handle the fade out anim
    }
}

void Player::setFramelessButtonsVisibility(bool visible)
{
    ui->m_closeButton->setVisible(visible);
    ui->m_hideButton->setVisible(visible);
}

void Player::loadSkin()
{
    m_idSkin = m_settings.value("skin", 1).toInt();
    if(m_idSkin == 1 || m_idSkin == 3) //Dark skin
    {
        if(!m_isStarting)
            setDarkCSS(); //Because the stylesheet is already set as default
        if(m_idSkin == 1)
        {
            if(!QFile(m_settings.value("customimage").toString()).exists()) //We won't charge something which doesn't exists
                ui->m_image->setPixmap(m_neuDarkBg);
            else
                ui->m_image->setPixmap(QPixmap(m_settings.value("customimage").toString()));
        }
        else //Custom bg
        {
            if(!QFile(m_settings.value("customimage").toString()).exists())
                ui->m_image->setPixmap(m_neuDarkCustombg);
            else
                ui->m_image->setPixmap(QPixmap(m_settings.value("customimage").toString()));
        }
    }

    if(m_idSkin == 0 || m_idSkin == 2) //Light skin
    {
        setLightCSS();
        if(m_idSkin == 0)
        {
            if(!QFile(m_settings.value("customimage").toString()).exists())
                ui->m_image->setPixmap(m_neuLightBg);
            else
                ui->m_image->setPixmap(QPixmap(m_settings.value("customimage").toString()));
        }
        else //Custom bg
        {
            if(!QFile(m_settings.value("customimage").toString()).exists())
                ui->m_image->setPixmap(m_neuLightCustombg);
            else
                ui->m_image->setPixmap(QPixmap(m_settings.value("customimage").toString()));
        }
    }
    if(!m_isStarting)
    {
        if(!m_animManager) //If the pointer is null
            m_animManager = new FadeManager(ui->m_image, 1000, this, FadeManager::FadeIn);
        m_animManager->start();
    }
}

void Player::setLightCSS()
{
    ui->m_playbtn->setStyleSheet("QPushButton#m_playbtn{background-image: url(:/Ressources/Playdarkbtn.png);}"
                                 "QPushButton#m_playbtn:hover{background-image: url(:/Ressources/Playdarkbtn_onHover.png);}");
    ui->m_pausebtn->setStyleSheet("QPushButton#m_pausebtn{background-image: url(:/Ressources/Pausedarkbtn.png);}"
                                  "QPushButton#m_pausebtn:hover{background-image: url(:/Ressources/Pausedarkbtn_onHover.png);}");
    ui->m_menubtn->setStyleSheet("QPushButton#m_menubtn{background-image: url(:/Ressources/roundedmenudarkbtn.png);}"
                                 "QPushButton#m_menubtn:hover{background-image: url(:/Ressources/roundedmenudarkbtn_onHover.png);}");

    m_progressSlider->setStyleSheet("Slider::handle:horizontal {image: url(:/Ressources/handledark.png);}");
    m_volumeSlider->setStyleSheet("Slider::handle:horizontal {image: url(:/Ressources/handledark.png);}");
    if(m_isFrameless)
    {
    ui->m_closeButton->setStyleSheet("QPushButton#m_closeButton{background-image: url(:/Ressources/closeButtonDark.png);}"
                                 "QPushButton#m_closeButton:hover{background-image: url(:/Ressources/closeButton_onHover.png);}");
    ui->m_hideButton->setStyleSheet("QPushButton#m_hideButton{background-image: url(:/Ressources/hideButtonDark.png);}"
                                 "QPushButton#m_hideButton:hover{background-image: url(:/Ressources/hideButtonDark_onHover.png);}");
    }

    //Stylize the rest by putting icons...
    ui->m_forward->setIcon(m_forwardDarkIcon);
    ui->m_previous->setIcon(m_previousDarkIcon);
    on_volumeChanged(neu->volume()); //Will reload the correct icon for volume
}

void Player::setDarkCSS()
{
    ui->m_playbtn->setStyleSheet("QPushButton#m_playbtn{background-image: url(:/Ressources/Playbtn.png);}"
                                 "QPushButton#m_playbtn:hover{background-image: url(:/Ressources/Playbtn_onHover.png);}");
    ui->m_pausebtn->setStyleSheet("QPushButton#m_pausebtn{background-image: url(:/Ressources/Pausebtn.png);}"
                                  "QPushButton#m_pausebtn:hover{background-image: url(:/Ressources/Pausebtn_onHover.png);}");
    ui->m_menubtn->setStyleSheet("QPushButton#m_menubtn{background-image: url(:/Ressources/roundedmenubtn.png);}"
                                 "QPushButton#m_menubtn:hover{background-image: url(:/Ressources/roundedmenubtn_onHover.png);}");

    m_progressSlider->setStyleSheet("Slider::handle:horizontal {image: url(:/Ressources/handle.png);}");
    m_volumeSlider->setStyleSheet("Slider::handle:horizontal {image: url(:/Ressources/handle.png);}");

    if(m_isFrameless)
    {
    ui->m_closeButton->setStyleSheet("QPushButton#m_closeButton{background-image: url(:/Ressources/closeButton.png);}"
                                 "QPushButton#m_closeButton:hover{background-image: url(:/Ressources/closeButton_onHover.png);}");
    ui->m_hideButton->setStyleSheet("QPushButton#m_hideButton{background-image: url(:/Ressources/hideButton.png);}"
                                 "QPushButton#m_hideButton:hover{background-image: url(:/Ressources/hideButton_onHover.png);}");
    }
    //Stylize the rest by putting icons...
    ui->m_forward->setIcon(m_forwardIcon);
    ui->m_previous->setIcon(m_previousIcon);
    on_volumeChanged(neu->volume()); //Will reload the correct icon for volume
}

void Player::checkForNewMedias()
{
    QFileInfo currentInfo (m_settings.value("mediapath").toString());
    if(m_settings.value("libModified").toLongLong() < currentInfo.lastModified().toMSecsSinceEpoch())
    {
        int reponse = QMessageBox::information(this, "neuPlayer", tr("Des changements ont été detectés dans votre base depuis la dernière mise à jour de celle-ci.\nVoulez-vous l'actualiser ?"), QMessageBox::Yes, QMessageBox::No, QMessageBox::Ignore);
        if(reponse == QMessageBox::Yes)
        {
            m_settings.setValue("libModified", currentInfo.lastModified().toMSecsSinceEpoch());
            updateLibrary();
        }
        if(reponse == QMessageBox::Ignore)
            //"Don't bug me for this change"
            m_settings.setValue("libModified", currentInfo.lastModified().toMSecsSinceEpoch());
    }
}

void Player::updateLibrary()
{
    ui->m_label->setText(tr("Mise à jour librairie..."));
    m_mediaPlaylist.save();
    if(m_mediaPlaylist.isEmpty())
        ui->m_label->setText(tr("Aucun média trouvé"));
    else
        m_mediaPlaylist.load(QUrl::fromLocalFile("neuLibrary.m3u8"), "m3u8");
}

//Have to delay it
void Player::finishingUp()
{
    if(m_hasToStartupPlaylist)
    {
        m_hasToStartupPlaylist = false; //No more
        showPlaylist();
    }
#ifdef Q_OS_WIN
    //TODO : Create icons, set them, and link buttons to play/pause/forward/previous slots.
    //prepare buttons
//    m_thumbPlayButton.setDismissOnClick(true);
//    m_thumbPauseButton.setDismissOnClick(true);
    m_thumbActionButton.setIcon(QIcon(":/Ressources/play_thumbnailButton.png"));
    m_thumbForwardButton.setIcon(QIcon(":/Ressources/forward_thumbnailButton.png"));
    m_thumbPreviousButton.setIcon(QIcon(":/Ressources/previous_thumbnailButton.png"));

    //Prepare and set buttons on thumbnailtoolbar
    m_thumbnailToolbar.addButton(&m_thumbPreviousButton);
    m_thumbnailToolbar.addButton(&m_thumbActionButton);
    m_thumbnailToolbar.addButton(&m_thumbForwardButton);

    m_thumbnailToolbar.setWindow(this->windowHandle());
#endif
}

void Player::savePlaylists()
{
    if(m_hasToSaveFavsLater)
        m_favPlaylist.saveFromPlaylist("favorites.m3u8");
    if(m_settings.value("Additional_Features/libraryAtStartup").toBool() == true)
    {
        if(m_hasToSavePlaylistLater)
            m_mediaPlaylist.saveFromPlaylist(); //Defaults to normal library
    }
}

void Player::setupNewLibrary()
{
    if(!m_isUsingFavPlaylist)
        neu->setPlaylist(&m_mediaPlaylist); //The m_isUsingFavPlaylist was already updated before in the loadPlaylist() method
    else
        neu->setPlaylist(&m_favPlaylist); // ^^^^^^^^^
    if(m_settings.value("Additional_Features/saveTrackIndex", false).toBool() == true)
    {
        m_recoveredProgress = false;
        neu->playlist()->setCurrentIndex(m_settings.value("currentTrack").toInt());
    }
}

    /*///////MediaPlayer Controls///////*/

void Player::playMedia()
{
    if(m_isStarting)
        return; // Not ready yet !
    /* Those silly disconnect-reconnect are made so the openMedia(); method isn't called multiple times.
     * For whatever reasons if I don't do it, it will be called about 2 to 4 times ! */

    disconnect(ui->m_playbtn, SIGNAL(clicked()), this, SLOT(playMedia()));
    if ((neu->media().isNull()))
        openMedia();
    else //On peut play quelque chose
    {
        if(m_audioFadeActivated) //If it was instantiated, it means It's activated
            runAudioFade();
        neu->play();
        ui->m_playbtn->setVisible(false);
        ui->m_pausebtn->setVisible(true);

#ifdef Q_OS_WIN
        m_thumbActionButton.setIcon(QIcon(":/Ressources/pause_thumbnailButton.png"));
        connect(&m_thumbActionButton, SIGNAL(clicked()), this, SLOT(pauseMedia()));
#endif

        connect(ui->m_pausebtn, SIGNAL(clicked()), this, SLOT(pauseMedia()));
        if(m_isPlaylistOpen)
        {
            m_playlist->setToPlaying(neu->playlist()->currentIndex());
        }
        return; //We don't want to go to that last line
    }
    connect(ui->m_playbtn, SIGNAL(clicked()), this, SLOT(playMedia()));
}

void Player::pauseMedia()
{
    neu->pause();
    disconnect(ui->m_pausebtn, SIGNAL(clicked()), this, SLOT(pauseMedia()));
    ui->m_playbtn->setVisible(true);
    ui->m_pausebtn->setVisible(false);

#ifdef Q_OS_WIN
    m_thumbActionButton.setIcon(QIcon(":/Ressources/play_thumbnailButton.png"));
    connect(&m_thumbActionButton, SIGNAL(clicked()), this, SLOT(playMedia()));
#endif

    connect(ui->m_playbtn, SIGNAL(clicked()), this, SLOT(playMedia()));
    if(m_isPlaylistOpen)
    {
        m_playlist->setToPaused(neu->playlist()->currentIndex());
    }
}

void Player::forwardMedia()
{
    if(!m_canChangeMusic || m_mediaPlaylist.mediaCount() == 0 )
        return;
    neu->playlist()->next();
    if(m_audioFadeActivated) //If it was instantiated, it means It's activated
        runAudioFade();
    if(neu->playlist()->currentMedia().isNull())
    {
        if(m_isLoopPlaylistMode)
        {
            neu->playlist()->setCurrentIndex(0);
            playMedia();
        }
        else
        {
            emit EndOfMedia();
            return;
        }
    }
    if(m_hasToDoInitialShuffle) //For true randomness
    {
        qsrand(QTime::currentTime().msec());
        neu->playlist()->setCurrentIndex(qrand() % neu->playlist()->mediaCount());
        m_hasToDoInitialShuffle = false;
    }
    m_wasPrevious = false;
}

void Player::forwardAnim()
{
    m_titleAnimate->setStartValue(QPoint(130, 0));
    m_titleAnimate->setEndValue(QPoint(110, 0));
    m_titleAnimate->start();
}

void Player::previousMedia()
{
    if(!m_canChangeMusic || m_mediaPlaylist.mediaCount() == 0)
        return;
    neu->playlist()->previous();
    if(m_audioFadeActivated)
        runAudioFade();
    if(neu->playlist()->currentMedia().isNull())
    {
        emit EndOfMedia();
        return;
    }
    if(m_hasToDoInitialShuffle)
    {
        qsrand(QTime::currentTime().msec());
        neu->playlist()->setCurrentIndex(qrand() % neu->playlist()->mediaCount());
        m_hasToDoInitialShuffle = false;
    }
    m_wasPrevious = true;
}


void Player::previousAnim()
{
    m_titleAnimate->setEndValue(QPoint(110, 0));
    m_titleAnimate->start();
}

void Player::errorHandling(QMediaPlayer::Error error)
{
    int current;
    bool stateBefore = m_isPlaying;
    switch (error) {
    case QMediaPlayer::NoError:
        break;
    case QMediaPlayer::ResourceError:
        ui->m_label->setText(tr("erreur : Unresolved Resource"));
        neu->playlist()->previous();
        current = neu->playlist()->currentIndex();
        qDebug() << neu->playlist()->media(current).canonicalUrl();
        neu->playlist()->removeMedia(current);
        if(m_isPlaylistOpen)
            m_playlist->deleteItem(current);
        neu->setPlaylist(neu->playlist()); // I do this to ensure stability, because we're in a panic case
        if(!m_wasPrevious)
            neu->playlist()->setCurrentIndex(current - 2);
        else
            neu->playlist()->setCurrentIndex(current - 1);
        m_isPlaying = stateBefore;
        if(m_isPlaying)
            playMedia();
        if(!m_isUsingFavPlaylist)
            m_hasToSavePlaylistLater = true; //Prevent error from coming back.
        else
            m_hasToSaveFavsLater = true;
        break;
    case QMediaPlayer::FormatError :
        ui->m_label->setText(tr("erreur : Format non supporté"));
        pauseMedia(); // Total panic
        neu->stop();
        neu->playlist()->clear();// so we have to discard the content
        break;
    case QMediaPlayer::NetworkError :
        ui->m_label->setText(tr("erreur : Network error"));
        break;
    case QMediaPlayer::AccessDeniedError :
        QMessageBox::critical(this, tr("Erreur ! "), tr("Vous n'avez pas les droits pour accéder à ce fichier"));
        break;
    case QMediaPlayer::ServiceMissingError :
        QMessageBox::critical(this, tr("Erreur Critique !"), tr("Le service du player est indisponible. Impossible de lire le fichier !"));
        break;
    }
    m_isPlaying = false;
}

void Player::statusChanged(QMediaPlayer::MediaStatus status)
{
    // handle status message and cursor
    switch (status) {
    case QMediaPlayer::LoadingMedia:
        this->setCursor(Qt::BusyCursor);
        break;
    case QMediaPlayer::StalledMedia:
        ui->m_label->setText("Media stalled");
        m_isPlaying = false;
        break;
    case QMediaPlayer::LoadedMedia:
        this->setCursor(Qt::ArrowCursor);
        if(!m_recoveredProgress && m_settings.value("Additional_Features/saveTrackIndex", false).toBool() == true
                || m_deleteTriggered)
        {
            m_deleteTriggered = false; // no more
            seekProgress(m_settings.value("trackPosition").toInt());
            m_recoveredProgress = true;
        }
        break;
    case QMediaPlayer::EndOfMedia:
        forwardMedia();
    }
}

void Player::stateChanged(QMediaPlayer::State state)
{
    //Handle the update of the bool m_isPlaying
    //Permits an up-to-date icon in the playlist and an info on what is the player's playing state
    switch(state)
    {
    case QMediaPlayer::PlayingState:
        m_isPlaying = true;
        break;
    case QMediaPlayer::PausedState:
        m_isPlaying = false;
        break;
    case QMediaPlayer::StoppedState:
        m_isPlaying = false;
        break;
    }
}

void Player::showMenu()
{
    m_menu.exec(QCursor::pos()); //simplest method pls
}

void Player::openMedia()
{
    QList <QUrl> files = QFileDialog::getOpenFileUrls(this, tr("Sélectionnez des médias à lire"), (QStandardPaths::locate(QStandardPaths::MusicLocation, QString(), QStandardPaths::LocateDirectory)) , tr("Flux audios (*.mp3 *.mp4 *.m4a *.wav)"));
    if(files.isEmpty()) //It was a mistake I guess, so don't do anything
        return;
    //Clear before processing
    m_mediaPlaylist.clear();
    unsigned int const fileNumber = files.size();
    for(unsigned int i (0); i < fileNumber; i++ )
    {
        m_mediaPlaylist.addMedia(files.at(i));
    }
    if(!m_mediaPlaylist.isEmpty())
    {
        changeToDefaultPlaylist();
        playMedia();
        if(m_isPlaylistOpen)
        {
            m_mediaPlaylist.setCurrentIndex(0);
            m_playlist->updateList(&m_mediaPlaylist, true);
        }
    }
}

void Player::setMeta()
{
    //New track, we start at 0 secs !
    m_secondesPasse = 0; //For infos
    // On check pour savoir si les metas sont nulles ou avec une chaine vide pour avoir une donnée correcte à afficher
    if(!neu->metaData("Title").isNull() && !neu->metaData("Title").operator ==(""))
        m_titre = neu->metaData("Title").toString();
    else
        m_titre = neu->currentMedia().canonicalUrl().fileName();
    if(!neu->metaData("ContributingArtist").isNull() && !neu->metaData("ContributingArtist").operator ==(""))
        m_artiste = neu->metaData("ContributingArtist").toString();
    else
        m_artiste = tr("Artiste Inconnu");
    if(!neu->metaData("AlbumTitle").isNull() && !neu->metaData("AlbumTitle").operator ==(""))
        m_album = neu->metaData("AlbumTitle").toString();
    else
        m_album = tr("Album Inconnu");

    //Si c'est une vidéo ou un flux sans header
    if( (neu->metaData("Title").isNull())
         && (neu->metaData("ContributingArtist").isNull())
         && (neu->metaData("AlbumTitle").isNull()) )
    {
        m_artiste = neu->currentMedia().canonicalUrl().fileName();
        m_titre = neu->currentMedia().canonicalUrl().fileName();
        m_album = neu->currentMedia().canonicalUrl().fileName();
        m_no_meta = true;
    }
    else
        m_no_meta = false;

    m_coverArt = QPixmap::fromImage(QImage(neu->metaData("ThumbnailImage").value<QImage>()));
    if(m_coverArt.isNull())
        m_coverArt = QPixmap::fromImage(QImage(neu->metaData("CoverArtImage").value<QImage>()));

    if(!ui->m_label->isVisible())
    {
        m_scrollingLabel.hide();
        ui->m_label->setVisible(true);
    }
    m_hasToSetLabelType = true;
    ui->m_label->setText(m_titre);
    ui->m_label->setToolTip(m_titre);
    if(!m_deleteTriggered) //To avoid animation, because the user wouldn't expect it
    {
        if(m_wasPrevious)
            previousAnim();
        else
            forwardAnim();
    }

    m_canChangeMusic = false;
    setTypeTimer.start(300); //Set the type 300 ms later
    grantChangeTimer.start(200);


    m_previousIndex = neu->playlist()->currentIndex();

    if(m_settings.value("Additional_Features/saveTrackIndex", false).toBool() == true)
        m_settings.setValue("currentTrack", m_previousIndex);
    if(m_isPlaylistOpen)
        m_playlist->setCurrentItem(neu->playlist()->currentIndex(), &m_coverArt, m_titre, m_isPlaying);

    updateFadeinSpeed();
    Timer.start();
}

void Player::setType()
{
        updateLabel(m_titre); //Will set the good type
        m_hasToSetLabelType = false; //It's been done !
        finishingUp();
}

//Update and set type of label
void Player::updateLabel(QString &text)
{
    if(text.size() < 25)
    {
        m_isScrollingText = false;
        if(!ui->m_label->isVisible())
            ui->m_label->show();
        if(m_scrollingLabel.isVisible())
            m_scrollingLabel.hide();

        if(!m_hasToSetLabelType) //Doesn't need to set them if that's the case
        {
            ui->m_label->setText(text);
            ui->m_label->setToolTip(text);
        }
    }
    else
    {
        m_isScrollingText = true;
        if(ui->m_label->isVisible())
            ui->m_label->hide();
        m_scrollingLabel.setText(text);
        m_scrollingLabel.setToolTip(text);
        m_scrollingLabel.show();
    }
}

//This method keeps the fade in speed variable by the info label's length
//It is a workaround for very long info labels that don't have the time to scroll all the way
void Player::updateFadeinSpeed()
{
    /*                                      How it works
     * A case is the number of seconds an info type will be displayed till we switch over to the next info
     * As they're dependent from the previous one, we always add the seconds of the case before
     * ex : if each info is less/equal to 17 chars, it'll be case 3,6,9 so that it switches each 3 seconds
     * */
    if(m_titre.size() <= 17) //17 so it won't go to the else and generate 3 when the default would be 4 (it's an int, it floors)
        m_titleCase = 4;
    else
        m_titleCase = (m_titre.size() / 10) * 3; //3 is a magic number, and /10 is to have seconds

    if(m_artiste.size() <= 17)
        m_artistCase = m_titleCase + 4;
    else
        m_artistCase = m_titleCase + ((m_artiste.size() / 10) * 3);
    if(m_album.size() <= 17)
        m_albumCase = m_artistCase + 4;
    else
        m_albumCase = m_artistCase + ((m_album.size() / 10) * 3);
}

void Player::runAudioFade()
{
    m_audioFade->setEndValue(m_volumeSlider->value());
    if(m_audioFade->state() == QPropertyAnimation::Running)
        m_audioFade->stop();
    m_audioFade->start();
}

//Does the animation between each info
void Player::fadeInLabel()
{
    //This changes the target if we're using the scrolling label
    if(m_isScrollingText)
        m_scrollingLabel.setGraphicsEffect(m_infoFadein);
    else
        ui->m_label->setGraphicsEffect(m_infoFadein);

    m_infoFadeAnim->setTargetObject(m_infoFadein);
    m_infoFadeAnim->setPropertyName("opacity");
    m_infoFadeAnim->setDuration(700);
    m_infoFadeAnim->setStartValue(0);
    m_infoFadeAnim->setEndValue(1.0);
    m_infoFadeAnim->start();
}

void Player::update_info()
{
    //update à chaque seconde
    m_secondesPasse++;

    /*Permet de gérer les informations à afficher en fonction du temps
     * Fades each time
     * The cases are named after the item which is displayed BEFORE going to the specific case
     * It might seems unclear at first but I believe it's better this way
    */

    //Switches over to normal if's because switch case can't evaluate non-constant expression
    if(m_secondesPasse == m_titleCase)
    {
        updateLabel(m_artiste);
        if(!m_no_meta)
            fadeInLabel();
    }
    if(m_secondesPasse == m_artistCase)
    {
        updateLabel(m_album);
        if(!m_no_meta)
            fadeInLabel();
    }
    if(m_secondesPasse == m_albumCase)
    {
        m_secondesPasse = 0;
        updateLabel(m_titre);
        if(!m_no_meta)
            fadeInLabel();
    }
    if(!m_menu.isVisible() &&  m_alwaysOnTopHandler.isChecked()) //If we're always on top, this assure that we're always on top, even if on Windows explorer gets on top
        if(!m_isPlaylistOpen && !m_isSettingsOpen) //Raising also raises over the widgets of these windows... Can be weird if you're using it's context menus...
            this->raise();
}
    /*///////SliderBar Section///////*/
void Player::UpdateProgress(qint64 pos)
{
    if(pos < 0)
        pos = 0; //Will bring back bizarre values to saner levels
    m_progressSlider->setValue(pos);
    updateProgressDisplay(pos, true);

}

//This method updates the seconds and minutes displayed when playing and if isProgress is false it will update the duration (on_durationChanged event)
void Player::updateProgressDisplay(qint64 pos, bool isProgress)
{
    pos /= 1000;
    int minutes = 0;
    // Fait en sorte d'update le label de temps actuel
    while (pos % 60 != pos)
    {
        pos -= 60;
        minutes++;
    }
    //duration vaut maintenant les secondes restantes...
    int secondes = pos;
    if(secondes < 10)
    {
        if(isProgress)
            ui->m_currenttime->setText(QString::number(minutes) + ":0" + QString::number(secondes));
        else
            ui->m_duration->setText(QString::number(minutes) + ":0" + QString::number(secondes));
    }
    else
    {
        if(isProgress)
            ui->m_currenttime->setText(QString::number(minutes) + ":" + QString::number(secondes));
        else
            ui->m_duration->setText(QString::number(minutes) + ":" + QString::number(secondes));
    }
}

//Produit grâce à un signal EndOfMedia();
void Player::endOfMediaGuard()
{
    Timer.stop(); //There should be no more info to display
    ui->m_pausebtn->setVisible(false);
    ui->m_playbtn->setVisible(true);
    disconnect(ui->m_pausebtn, SIGNAL(clicked()), this, SLOT(pauseMedia()));
    connect(ui->m_playbtn, SIGNAL(clicked()), this, SLOT(playMedia()));
    ui->m_label->setText(tr("Stoppé"));
}
void Player::seekProgress(int pos)
{
    neu->setPosition(pos);
    if(m_recoveredProgress)
    {
        neu->play();
        disconnect(ui->m_playbtn, SIGNAL(clicked()), this, SLOT(playMedia()));
        ui->m_playbtn->setVisible(false);
        ui->m_pausebtn->setVisible(true);
        connect(ui->m_pausebtn, SIGNAL(clicked()), this, SLOT(pauseMedia()));
    }
    if(m_isPlaylistOpen && !m_playlist->isPlayingState())
        m_playlist->setToPlaying(neu->playlist()->currentIndex());
    if(pos > neu->duration())
        forwardMedia();
}

void Player::on_durationChanged(qint64 pos)
{
    m_progressSlider->setMaximum(pos);
    updateProgressDisplay(pos, false);
}

//Accessible par raccourci clavier ALT + Right
void Player::advanceProgress()
{
    int advanceTo = neu->position() + 15000;
    seekProgress(advanceTo);
}

//Accessible par raccourci clavier ALT + Left
void Player::backProgress()
{
    int goBackTo = neu->position() - 15000;
    seekProgress(goBackTo);
}

//handler du volume
void Player::on_volumeChanged(int pos)
{
    neu->setVolume(pos);
    if(pos == 0)
    {
        ui->m_volumebtn->setChecked(true);
        setVolumeMuted();
        return;
    }
    if(pos <= 50)
    {
        if(m_idSkin == 1 || m_idSkin == 3)
            ui->m_volumebtn->setIcon(m_volumeLowIcon);
        else
            ui->m_volumebtn->setIcon(m_volumeLowDarkIcon);
    }
    else
    {
        if(m_idSkin == 1 || m_idSkin == 3)
            ui->m_volumebtn->setIcon(m_volumeIcon);
        else
            ui->m_volumebtn->setIcon(m_volumeDarkIcon);
    }
    ui->m_volumebtn->setChecked(false);
    neu->setMuted(false);
}

void Player::setVolumeMuted()
{
    if(ui->m_volumebtn->isChecked())
    {
        m_volumeBeforeMute = neu->volume();
        disconnect(m_volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(on_volumeChanged(int)));
        neu->setMuted(true);
        m_volumeSlider->setValue(0);
        connect(m_volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(on_volumeChanged(int)));
        if(m_idSkin == 1 || m_idSkin == 3)
            ui->m_volumebtn->setIcon(m_volumeMutedIcon);
        if(m_idSkin == 0 ||  m_idSkin == 2)
            ui->m_volumebtn->setIcon(m_volumeMutedDarkIcon);
    }
    else
    {
        neu->setMuted(false);
        m_volumeSlider->setValue(m_volumeBeforeMute); //On laisse le _onvolumeChanged() faire le boulot de remettre les bonnes icones
    }
}

void Player::volumeUp()
{
    m_volumeSlider->setSliderPosition(m_volumeSlider->value() + 10);
}

void Player::volumeDown()
{
    m_volumeSlider->setSliderPosition(m_volumeSlider->value() - 10);
}

    /*///////Menu Actions Section///////*/

void Player::setShuffle()
{
    if(!m_canDoShuffleAgain)
        return;
    if(neu->media().isNull()) //On évite de planter en essayant de play quelque chose qui n'existe pas
    {
        openMedia(); //On demande à l'utilisateur de séléctionner quelque chose
        if(!neu->media().isNull()) //On vérifie qu'il y a maintenant des musiques à jouer
            setShuffle(); //On repasse. Ici il passera donc dans le !isNull(); et procédera au shuffle mode
        }
    else if(!neu->media().isNull())
    {
        /* On seed un générateur de nombres pour avoir un vrai rand
                    généré au moment de l'activation              */
        qsrand(QTime::currentTime().msec());
        QMediaPlaylist *playlist (neu->playlist()); //We retrieve a QMediaPlaylist because we don't want another cast to be done

        int index = qrand() % playlist->mediaCount();
        playlist->setCurrentIndex(index);

        //Plus le shuffle, qui crée un rand en prenant comme valeur le média actuel
        playlist->shuffle();
        playlist->setCurrentIndex(0); //So it looks better this way
        if(!m_isRandomMode)
            playlist->setPlaybackMode(QMediaPlaylist::Sequential); //Default : random. We want it back to Sequential if we were on sequential before
        //On a maintenant un double rand : un rand purement aléatoire, et un rand basé sur le media actuel
        if(m_settings.value("Additional_Features/libraryAtStartup", false).toBool() == true)
            m_hasToSavePlaylistLater = true; // Will update playlist to load the correct one when booting up later on
        if(m_isPlaylistOpen && !m_isUsingFavPlaylist)
            m_playlist->updateList(&m_mediaPlaylist, true);
        else if(m_isPlaylistOpen)
            m_playlist->updateFavs(&m_favPlaylist);
        m_canDoShuffleAgain = false;
        grantShuffleAgainTimer.start(500);
        playlist = nullptr;
    }
}

void Player::windowFlagsHandler()
{
    if(m_alwaysOnTopHandler.isChecked())
        this->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    else
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint);
    QPointer <FadeWindow> fadeIn = new FadeWindow(this, 200, FadeWindow::FadeIn, this);
    fadeIn->start(m_settings.value("opacity").toReal());

    this->show();
}

void Player::playbackHandler(QAction *playbackSelected)
{
    if(playbackSelected->text() == "1.0x")
        m_playbackState = 0;
    else if (playbackSelected->text() == "0.5x")
        m_playbackState = 1;
    else if (playbackSelected->text() == "2.0x")
        m_playbackState = 2;
    setPlaybackRate();
}

void Player::setPlaybackRate()
{
    //Setup playback rate
    //Pause pour forcer de rappuyer sur lecture à nouveau, de façon à ce que le buffer soit déjà à nouveau à peu près rempli
    pauseMedia();
    if(m_playbackState == 0)
    {
        neu->setPlaybackRate(1);
        m_normalrate.setChecked(true);
    }
    else if (m_playbackState == 1)
    {
        neu->setPlaybackRate(0.5);
        m_halfrate.setChecked(true);
    }
    else if (m_playbackState == 2)
    {
        neu->setPlaybackRate(2);
        m_doublerate.setChecked(true);
    }
}

void Player::showPlaylist()
{
    if(!m_isPlaylistOpen)
    {
        m_isPlaylistOpen = true;
        if(m_mediaPlaylist.mediaCount()!= 0)
            m_playlist = new Playlist(m_mediaPlaylist, m_favPlaylist, this, &m_coverArt, m_titre, m_isPlaying, this);
        m_playlist->show();
    }
    else
    {
        m_playlist->showNormal();
        m_playlist->activateWindow();
    }
}

void Player::showSettings()
{
    if(!m_isSettingsOpen)
    {
        m_isSettingsOpen = true;
        m_settingsForm = new Settings(this, this);
        m_settingsForm->show();
    }
    else
    {
        m_settingsForm->show();
        m_settingsForm->activateWindow();
    }
}

void Player::showTagViewer()
{
    if(neu->media().isNull())
        return;

    /* Add meta datas to a list which will be treated by the tagViewer constructor */
    QList<QString> metaDatas;
    metaDatas.append(neu->metaData("TrackNumber").toString());
    metaDatas.append(m_titre);
    metaDatas.append(m_artiste);
    metaDatas.append(m_album);
    metaDatas.append(neu->metaData("Year").toString());
    metaDatas.append(neu->metaData("Genre").toString());
    //Load it now !
    QPointer <TagViewer> TagWindow = new TagViewer(metaDatas, &m_coverArt, this);
    TagWindow->exec();
}

    /*///////Events Section///////*/

void Player::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
            event->acceptProposedAction();
}

void Player::dropEvent(QDropEvent *event)
{
    ui->m_label->setText("Loading...");
    neu->stop();
    Timer.stop(); //We don't want to fade in while we're loading, right ?
    this->setCursor(Qt::BusyCursor);
    unsigned int const numberUrls = event->mimeData()->urls().size();
    QList <QUrl> listUrls;
    for (unsigned int i(0); i < numberUrls; i++)
    {
        listUrls.append(event->mimeData()->urls().at(i));
        //Permet de toujours rendre l'application plus ou moins utilisable
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    updatePlaylistOfThePlayer(listUrls, true);
}

void Player::closeEvent(QCloseEvent *event)
{
    if(m_canClose)
    {
        saveBeforeClosing();
        qApp->closeAllWindows();
    }
    else
    {
        event->ignore();
        Timer.setSingleShot(true);
        //Trigger the delayed close with animation
        Timer.start(400);
        connect(&Timer, SIGNAL(timeout()), this, SLOT(delayedClose()));
        QPointer <FadeWindow> fadeOut = new FadeWindow(this, 200, FadeWindow::FadeOut, this);
        if(m_isPlaying)
        {
            QPointer <QPropertyAnimation> audioFadeOut = new QPropertyAnimation(neu, "volume", this);
            audioFadeOut->setEndValue(0);
            audioFadeOut->setStartValue(m_volumeSlider->value());
            audioFadeOut->setDuration(250);
            audioFadeOut->start(QAbstractAnimation::DeleteWhenStopped);
        }
        fadeOut->start();
    }
}

void Player::saveBeforeClosing()
{
    savePlaylists();
    if(m_settings.value("Additional_Features/framelessWindow").toBool())
        m_settings.setValue("pos", QPoint(this->x() - 8, this->y() - 31)); //Little hack to set the value correctly
    else
        m_settings.setValue("pos", pos());
    m_settings.setValue("size", size());
    m_settings.setValue("volume", m_volumeSlider->value());
    m_settings.setValue("visibilite", m_alwaysOnTopHandler.isChecked());
    m_settings.setValue("playbackrate", m_playbackState);
    m_settings.setValue("random", m_isRandomMode);
    m_settings.setValue("loop", m_isLoopPlaylistMode);
    m_settings.setValue("usingFavorites", m_isUsingFavPlaylist); //If this is on, we're currently using the fav playlist. So it shall be set at next run
    if(m_settings.value("Additional_Features/saveTrackIndex", false).toBool() == true)
        m_settings.setValue("trackPosition", neu->position());
}

void Player::delayedClose()
{
    m_canClose = true;
    close(); //Now the event will be accepted
}

    /*///////Playlist Interface Section///////*/

void Player::setRandomMode(bool mode)
{
    bool prev = m_isRandomMode;
    if(prev != mode) //On change que s'il y a besoin
    {
        m_isRandomMode = mode;
        if(m_isRandomMode)
        {
            neu->playlist()->setPlaybackMode(QMediaPlaylist::Random);
            m_hasToDoInitialShuffle = true;
        }
        else
        {
            neu->playlist()->setPlaybackMode(QMediaPlaylist::Sequential);
            if(m_hasToDoInitialShuffle)
                m_hasToDoInitialShuffle = false;
        }
    }
}

void Player::setLoopMode(bool mode)
{
    bool prev = m_isLoopPlaylistMode;
    if(prev != mode) //On change que s'il y a besoin
    {
        m_isLoopPlaylistMode = mode;
    }
}

//Called when playlist wants to play something
void Player::setIndexOfThePlayer(int index, bool play)
{
    m_wasPrevious = false; //To launch anim
    if(!m_isUsingFavPlaylist)
        m_mediaPlaylist.setCurrentIndex(index);
    else
        m_favPlaylist.setCurrentIndex(index);
    if(play)
        playMedia();
}

//Called when dealing with a drag & drop
void Player::addMediasToThePlayer(QList<QUrl> &medias)
{
    int insertTo = neu->playlist()->currentIndex() + 1;
    unsigned int const mediaNumber = medias.size();
    for(unsigned int i(0); i < mediaNumber; i++, insertTo++)
    {
        neu->playlist()->insertMedia(insertTo, QUrl(medias.at(i)));
    }
    if(m_isPlaylistOpen)
        m_playlist->quickUpdate(&medias, insertTo - 1);
    if(!m_isUsingFavPlaylist)
        if(m_hasToSavePlaylistLater != true)
            m_hasToSavePlaylistLater = true;
    else
        if(m_hasToSaveFavsLater != true)
            m_hasToSaveFavsLater = true;
}

void Player::addFav(int index)
{
    QMediaContent media(m_mediaPlaylist.media(index)); //We copy the media
    m_favPlaylist.addMedia(media);
    QList <QUrl> temp;
    temp.append(media.canonicalUrl());
    bool previousState = m_isUsingFavPlaylist;
    m_isUsingFavPlaylist = true; //So the quickUpdate knows what to update
    m_playlist->quickUpdate(&temp, m_favPlaylist.mediaCount()); //The quick update will place the media at the end of fav without reloading the whole playlist
    if(previousState == false)
        m_isUsingFavPlaylist = false; //We're finished
    if(m_hasToSaveFavsLater != true)
        m_hasToSaveFavsLater = true;

}

void Player::changeToFavPlaylist()
{
    m_shuffle.setVisible(false); // Keep your favorites clean.
    neu->setPlaylist(&m_favPlaylist);
    m_isUsingFavPlaylist = true;
}

void Player::changeToDefaultPlaylist()
{
    m_shuffle.setVisible(true); // We can reload the original from disk anyway
    neu->setPlaylist(&m_mediaPlaylist);
    m_isUsingFavPlaylist = false;
}

//When added to queue
void Player::addToQueue(int index, int currentlyPlaying)
{
    int insertTo = currentlyPlaying + m_playlist->queuedIndex(); //if you add one music, it'll place the next one next to it, ect...
    QMediaContent media(m_mediaPlaylist.media(index)); //We copy the media
    deleteMedia(index);
    if(index < currentlyPlaying)
        insertTo--;
    m_mediaPlaylist.insertMedia(insertTo, media);
    QList <QUrl> temp;
    temp.append(m_mediaPlaylist.media(insertTo).canonicalUrl());
    m_playlist->deleteItem(index);
    m_playlist->quickUpdate(&temp, insertTo); //The quick update will place the media at insertTo index without reloading the whole playlist
    if(m_hasToSavePlaylistLater != true)
        m_hasToSavePlaylistLater = true;
}

//Called when setting a folder from playlist
void Player::updatePlaylistOfThePlayer(const QList<QUrl> &medias, bool play)
{
    m_mediaPlaylist.clear();
    setCursor(Qt::BusyCursor);
    unsigned int const mediaNumber = medias.size();
    for(unsigned int i (0); i < mediaNumber; i++ )
    {
        m_mediaPlaylist.addMedia(medias.at(i));
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    changeToDefaultPlaylist(); //Re-set to update...
    if(m_isPlaylistOpen)
        m_playlist->updateList(&m_mediaPlaylist);
    if(m_hasToSavePlaylistLater != true)
        m_hasToSavePlaylistLater = true;
    if(play)
        playMedia();
    else
    {
        disconnect(ui->m_pausebtn, SIGNAL(clicked()), this, SLOT(pauseMedia()));
        ui->m_playbtn->setVisible(true);
        ui->m_pausebtn->setVisible(false);
        connect(ui->m_playbtn, SIGNAL(clicked()), this, SLOT(playMedia()));
    }
    setCursor(Qt::ArrowCursor);
}

void Player::restart()
{
    saveBeforeClosing();
    qApp->quit();
    QProcess::startDetached("neuPlayer.exe");
}
//Destructor
Player::~Player()
{
    delete ui;
}
