#include "player.h"
#include "ui_player.h"
#include <QDebug>
Player::Player(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Player)
{
    /*!
                                            2015 Horoneru                                   1.1 stable 160315 active
      TODO
      à faire : (/ ordre d'importance)
      > add to fav au niveau playlist (coming soon)
      > UPDATE TRANSLATIONS
      - ajouter un "recent bg" à la séléction d'une custom image (coming along soon too ! )
      - (Optional) plugin manager musiques osu! << gérer par delete des filenames
      - Re-design et occuper l'espace alloué par le borderless -> S'occuper des boutons
      - (long-terme) s'occuper de quelques extras win-specific... (peu d'interêt, à voir)
      */
    ui->setupUi(this);
    QApplication::setApplicationVersion("1.1 stable 160315");
    this->setAcceptDrops(true);
    this->setAttribute(Qt::WA_AlwaysShowToolTips);

    setupObjects();

    setupMenus();
    // Bool to control playlist state
    a_isPlaylistOpen = false;

    //Bool to detect a delete
    a_deleteTriggered = false;

    //Bool to do initial shuffle when selecting random play and forwarding
    a_hasToDoInitialShuffle = false;

    //self-explanatory
    a_hasToStartupPlaylist = false;

    //Bool to control whether we have to set the type of label (Scrolling Label or normal)
    a_hasToSetLabelType = false;

    //Bool to indicate to the playlist if the player is playing
    a_isPlaying = false;

    if(a_settings->value("playlistAtStartup").toBool() == true)
        a_hasToStartupPlaylist = true; //Will trigger the playlist startup

    //Bool to recover progress when needed
    a_recoveredProgress = true;

    ui->a_pausebtn->setVisible(false);

    a_secondesPasse = 0;

    setPlaybackRate();

    setupConnections();

    //Paramètres loadés après car les connexions se chargent de traiter les nouvelles données
    setupPlugins();


    ui->a_volumeslider->setValue(a_settings->value("volume", 50).toInt());

    a_alwaysOnTopHandler->setChecked(a_settings->value("visibilite", false).toBool());

    if(a_isRandomMode)
    {
        a_mediaPlaylist.setPlaybackMode(QMediaPlaylist::Random);
        a_hasToDoInitialShuffle = true;
    }
    else
        a_mediaPlaylist.setPlaybackMode(QMediaPlaylist::Sequential);

    setOpacity();
}

    /*///////Plugins & Setup Section///////*/

void Player::setupObjects()
{
    /*///////Setup Objects and Data///////*/
    a_settings = new QSettings("neuPlayer.ini", QSettings::IniFormat, this);
    resize(a_settings->value("size", QSize(400, 47) ).toSize());
    move(a_settings->value("pos", QPoint(200,200)).toPoint());
    a_musicUserPath = a_settings->value("mediapath", "").toString();
    a_playbackState = a_settings->value("playbackrate", 0).toInt();
    a_isRandomMode = a_settings->value("random", false).toBool();
    a_isLoopPlaylistMode = a_settings->value("loop", false).toBool();

    //Prepare animation
    a_titleAnimate = new QPropertyAnimation(ui->a_label, "pos", this);
    a_titleAnimate->setDuration(200); //Always 200 ms
    a_infoFadein = new QGraphicsOpacityEffect(this);
    a_infoFadein->setOpacity(1.0);
    ui->a_label->setGraphicsEffect(a_infoFadein);
    a_infoFadeAnim = new QPropertyAnimation(a_infoFadein, "opacity", this);

    //Prepare the sliding text object
    QFont font;
    font.setFamily(QStringLiteral("Segoe UI"));
    font.setPointSize(10);
    a_scrollingLabel = new ScrollingInfoLabel(this);
    a_scrollingLabel->setFont(font);
    a_scrollingLabel->setGeometry(QRect(100, 0, 181, 31));
    a_scrollingLabel->setMinimumSize(QSize(170, 30));

    //Media Player
    neu = new QMediaPlayer(this);

    //Raccourcis clavier pour parcourir les éléments du player
    a_advance = new QAction(this);
    a_advance->setShortcut(Qt::ALT + Qt::Key_Right);
    a_back = new QAction(this);
    a_back->setShortcut(Qt::ALT + Qt::Key_Left);
    ui->a_volumebtn->setCheckable(true);
    a_volumeUp = new QAction(this);
    a_volumeUp->setShortcut(Qt::CTRL + Qt::Key_Up);
    a_volumeDown = new QAction(this);
    a_volumeDown->setShortcut(Qt::CTRL + Qt::Key_Down);

    this->addAction(a_advance);
    this->addAction(a_back);
    this->addAction(a_volumeUp);
    this->addAction(a_volumeDown);

    //Elements du menu :
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    a_menu = new QMenu(this); //On le crée, mais on va d'abord créer touts ses constituants
    a_openMedia = new QAction(tr("Ouvrir des fichiers"), this);
    a_openMedia->setShortcut(Qt::CTRL + Qt::Key_O);
    a_showPlaylist = new QAction(tr("Ouvrir la liste de lecture"), this);
    a_showPlaylist->setShortcut(Qt::CTRL + Qt::Key_P); //P comme Playlist ?
    a_alwaysOnTopHandler = new QAction(tr("Toujours visible"), this);
    a_alwaysOnTopHandler->setCheckable(true);
    a_alwaysOnTopHandler->setShortcut(Qt::CTRL + Qt::Key_V); // V comme visible :3
    a_accessSettings = new QAction(tr("Paramètres"), this);
    a_accessSettings->setShortcut(Qt::CTRL + Qt::Key_Space);
    a_accessSettings->setShortcutContext(Qt::ApplicationShortcut);
    a_shuffle = new QAction(tr("Mélanger la playlist"), this);
    a_shuffle->setShortcut(Qt::CTRL + Qt::Key_R); // R comme random !
    a_tagViewer = new QAction(tr("Voir les tags"), this);
    a_tagViewer->setShortcut(Qt::ALT + Qt::Key_Return);

    //Timer enabling the scrolling of the label info
    Timer.setInterval(1000);
    //Timer that delays the check of which label type to use
    setTypeTimer.setSingleShot(true);
}


void Player::setupMenus()
{

    //Sous-menu playback rate
    a_playbackMenu = new QMenu(tr("Vitesse de lecture"), this);
    normalrate = new QAction("1.0x", this);
    normalrate->setCheckable(true);
    doublerate = new QAction ("2.0x", this);
    doublerate->setCheckable(true);
    halfrate = new QAction ("0.5x", this);
    halfrate->setCheckable(true);
    a_playbackrates = new QActionGroup(this);
    a_playbackrates->addAction(doublerate);
    a_playbackrates->addAction(normalrate);
    a_playbackrates->addAction(halfrate);
    a_playbackrates->setExclusive(true);
    a_playbackMenu->addAction(halfrate);
    a_playbackMenu->addAction(normalrate);
    a_playbackMenu->addAction(doublerate);

    //Setup The menu
    a_menu->addAction(a_showPlaylist);
    a_menu->addAction(a_openMedia);
    a_menu->addAction(a_accessSettings);
    a_menu->addMenu(a_playbackMenu);
    a_menu->addAction(a_alwaysOnTopHandler);
    a_menu->addAction(a_shuffle);
    a_menu->addAction(a_tagViewer);
    this->addAction(a_openMedia);
    this->addAction(a_showPlaylist);
    this->addAction(a_alwaysOnTopHandler);
    this->addAction(a_shuffle);
    this->addAction(a_accessSettings);
    this->addAction(a_tagViewer);

}

void Player::setupConnections()
{
    /*///////Connexions///////*/
    connect(ui->a_playbtn, SIGNAL(clicked()), this, SLOT(playMedia()));
    connect(neu, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(errorHandling(QMediaPlayer::Error)));
    connect(neu, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(statusChanged(QMediaPlayer::MediaStatus)));
    connect(neu, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(stateChanged(QMediaPlayer::State)));
    connect(ui->a_forward, SIGNAL(clicked()), this, SLOT(forwardMedia()));
    connect(ui->a_previous, SIGNAL(clicked()), this, SLOT(previousMedia()));
    connect(ui->a_menubtn, SIGNAL(clicked()), this, SLOT(showMenu()));
    connect(a_advance, SIGNAL(triggered()), this, SLOT(advanceProgress()));
    connect(a_back, SIGNAL(triggered()), this, SLOT(backProgress()));
    connect(&a_mediaPlaylist, SIGNAL(loaded()), this, SLOT(finishingUp()));
    connect(neu, SIGNAL(durationChanged(qint64)), this, SLOT(on_durationChanged(qint64)));
    connect(neu, SIGNAL(positionChanged(qint64)), this, SLOT(UpdateProgress(qint64)));
    connect(ui->a_progressslider, SIGNAL(sliderMoved(int)), this, SLOT(seekProgress(int)));
    connect(ui->a_volumeslider, SIGNAL(valueChanged(int)), this, SLOT(on_volumeChanged(int)));
    connect(ui->a_volumebtn, SIGNAL(clicked()), this, SLOT(setVolumeMuted()));
    connect(a_volumeUp, SIGNAL(triggered()), this, SLOT(volumeUp()));
    connect(a_volumeDown, SIGNAL(triggered()), this, SLOT(volumeDown()));
    connect(neu, SIGNAL(metaDataChanged()), this, SLOT(setMeta()));
    connect(&Timer, SIGNAL(timeout()), this, SLOT(update_info()));
    connect(&setTypeTimer, SIGNAL(timeout()), this, SLOT(setType()));
    connect(this, SIGNAL(EndOfMedia()), this, SLOT(endOfMediaGuard()));
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showMenu()));
    connect(a_openMedia, SIGNAL(triggered()), this, SLOT(openMedia()));
    connect(a_alwaysOnTopHandler, SIGNAL(toggled(bool)), this, SLOT(windowFlagsHandler()));
    connect(a_playbackrates, SIGNAL(triggered(QAction*)), this, SLOT(playbackHandler(QAction*)));
    connect(a_shuffle, SIGNAL(triggered()), this, SLOT(setShuffle()));
    connect(a_showPlaylist, SIGNAL(triggered()), this, SLOT(showPlaylist()));
    connect(a_tagViewer, SIGNAL(triggered()), this, SLOT(showTagViewer()));
    connect(a_accessSettings, SIGNAL(triggered()), this, SLOT(showSettings()));
    connect(&a_mediaPlaylist, SIGNAL(loaded()), this, SLOT(setupNewLibrary()));
}

void Player::setupPlugins()
{
    loadSkin();
    if(a_settings->value("Additional_Features/libraryAtStartup", false).toBool() == true)
    {
        //Modes de chargement de la librairie

        ui->a_label->setText("<No Media>");
        if(a_settings->value("Additional_Features/refreshWhenNeeded", false).toBool() == true)
        {
            QFile fileHandler(".configdone");
            if(!fileHandler.exists())
            {
                updateLibrary();
                fileHandler.open(QFile::ReadWrite); //Create it and I'm done
                if(a_hasToSavePlaylistLater)
                    close(); //Will be restarted either way. This helps to not show the player if the playlist at startup is checked at setup.
            }
            else
                checkForNewMedias();
            a_mediaPlaylist.load(QUrl::fromLocalFile("neuLibrary.m3u8"), "m3u8");
        }
        if(a_settings->value("Additional_Features/staticLibrary", false).toBool() == true)
        {
            a_mediaPlaylist.load(QUrl::fromLocalFile("neuLibrary.m3u8"), "m3u8");
        }
    }
    QFile fileHandler(".configdone");
    fileHandler.open(QFile::ReadWrite); //Create it and I'm done
}

void Player::setOpacity(qreal opacityFromSettings)
{
    if(opacityFromSettings != 0) //This is used for live preview from settings. Its default value is 0 and it's an optional parameter
    {
        this->setWindowOpacity(opacityFromSettings);
        return;
    }

    if(a_settings->value("opacity", 1.0).toReal() <= 0) //Because I know someone will somehow set a 0 opacity...
    {
        this->setWindowOpacity(1.0);
        return;
    }
    if(a_settings->value("opacity", 1.0).toReal() < 1.0)
    {
        this->setWindowOpacity(a_settings->value("opacity").toReal());
    }
}

//Did a work-around and works
void Player::loadSkin()
{
    //Skin support avec un ID. Tant qu'on est pas en release, toujours Fusion Dark
    a_idSkin = a_settings->value("skin", 1).toInt();

    if(a_idSkin == 1) //The dark skin is the default, so we don't have to change the initial stylesheet
        if(!QFile(a_settings->value("customimage").toString()).exists()) //We won't charge something which doesn't exists
            ui->a_image->setPixmap(QPixmap(":/Ressources/neudarkbg.png"));
        else
        {
            ui->a_image->setPixmap(QPixmap(a_settings->value("customimage").toString()));
        }

    if(a_idSkin == 0)
    {
    ui->a_playbtn->setStyleSheet("background-image: url(:/Ressources/Playdarkbtn.png);}"
                                 ":hover{background-image: url(:/Ressources/Playdarkbtn_onHover.png);}");
    ui->a_pausebtn->setStyleSheet("QPushButton#a_pausebtn{background-image: url(:/Ressources/Pausedarkbtn.png);}"
                                  "QPushButton#a_pausebtn:hover{background-image: url(:/Ressources/Pausedarkbtn_onHover.png);}");
    ui->a_menubtn->setStyleSheet("QPushButton#a_menubtn{background-image: url(:/Ressources/roundedmenudarkbtn.png);}"
                                 "QPushButton#a_menubtn:hover{background-image: url(:/Ressources/roundedmenudarkbtn_onHover.png);}");

    ui->a_progressslider->setStyleSheet("QSlider::handle:horizontal {image: url(:/Ressources/handledark.png);}");
    ui->a_volumeslider->setStyleSheet("QSlider::handle:horizontal {image: url(:/Ressources/handledark.png);}");

    //Stylize the rest by putting icons...
    ui->a_forward->setIcon(QIcon(":/Ressources/forwarddarkbtn.png"));
    ui->a_previous->setIcon(QIcon(":/Ressources/previousdarkbtn.png"));
    ui->a_volumebtn->setIcon(QIcon(":/Ressources/volumedarkbtn.png"));

    if(!QFile(a_settings->value("customimage").toString()).exists())
        ui->a_image->setPixmap(QPixmap(":/Ressources/neubackgroundwhite.jpg"));
    else
        ui->a_image->setPixmap(QPixmap(a_settings->value("customimage").toString()));
    }
}

void Player::checkForNewMedias()
{
    QFileInfo currentInfo (a_settings->value("mediapath").toString());
    if(a_settings->value("libModified").toULongLong() < currentInfo.lastModified().toMSecsSinceEpoch())
    {
        int reponse = QMessageBox::information(this, "neuPlayer", tr("Des changements ont été detectés dans votre base depuis la dernière mise à jour de celle-ci.\nVoulez-vous l'actualiser ?"), QMessageBox::Yes, QMessageBox::No, QMessageBox::Ignore);
        if(reponse == QMessageBox::Yes)
        {
            a_settings->setValue("libModified", currentInfo.lastModified().toMSecsSinceEpoch());
            updateLibrary();
        }
        if(reponse == QMessageBox::Ignore)
            //"Don't bug me for this change"
            a_settings->setValue("libModified", currentInfo.lastModified().toMSecsSinceEpoch());
    }
}

void Player::updateLibrary()
{
    QStringList listFilter;
    listFilter << "*.wav";
    listFilter << "*.mp3";
    listFilter << "*.mp4";
    listFilter << "*.m4a";
    QDirIterator dirIterator(a_settings->value("mediapath", "").toString(), listFilter ,QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    QFile fileHandler("neuLibrary.m3u8");
    if(fileHandler.exists())
        fileHandler.remove();
    if(!fileHandler.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, tr("Erreur ! "), tr("Le fichier de playlist n'a pas pu être ouvert"));
        return;
    }
    QTextStream out(&fileHandler);
    out.setCodec("UTF-8");
    a_mediaPlaylist.clear();
    ui->a_label->setText(tr("Mise à jour librairie..."));
    while(dirIterator.hasNext())
    {
        out << dirIterator.next().prepend("file:///").append("\n").toUtf8(); //adding the common structure of a m3u file to the URL
        a_mediaPlaylist.addMedia(QUrl(dirIterator.filePath())); //load only the neccesary
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    if(a_mediaPlaylist.isEmpty())
        ui->a_label->setText(tr("Aucun média trouvé"));
    fileHandler.close();
}

//Have to delay it
void Player::finishingUp()
{
    if(a_hasToStartupPlaylist)
    {
        a_hasToStartupPlaylist = false; //No more
        showPlaylist();
    }
}

void Player::saveCurrentPlaylist()
{
    QFile fileHandler("neuLibrary.m3u8");
    if(fileHandler.exists())
        fileHandler.remove();
    if(!fileHandler.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, tr("Erreur ! "), tr("Le fichier de playlist n'a pas pu être ouvert"));
        return;
    }
    QTextStream out(&fileHandler);
    out.setCodec("UTF-8");
    int const mediaCount = a_mediaPlaylist.mediaCount();
    for(int i (0); i < mediaCount; i++ )
    {
        out << a_mediaPlaylist.media(i).canonicalUrl().path().append("\n").toUtf8(); //adding the common structure of a m3u file to the URL
    }
    fileHandler.close();
}

void Player::setupNewLibrary()
{
    neu->setPlaylist(&a_mediaPlaylist);
    if(a_settings->value("Additional_Features/saveTrackIndex", false).toBool() == true)
    {
        a_recoveredProgress = false;
        a_mediaPlaylist.setCurrentIndex(a_settings->value("currentTrack").toInt());
    }
}

    /*///////MediaPlayer Controls///////*/

void Player::playMedia()
{
    if ((neu->media().isNull()))
    {
        openMedia();
    }
    else //On peut play quelque chose
    {
        neu->play();
        disconnect(ui->a_playbtn, SIGNAL(clicked()), this, SLOT(playMedia()));
        ui->a_playbtn->setVisible(false);
        ui->a_pausebtn->setVisible(true);
        connect(ui->a_pausebtn, SIGNAL(clicked()), this, SLOT(pauseMedia()));
        if(a_isPlaylistOpen)
            playlist->setToPlaying(a_mediaPlaylist.currentIndex());
    }
}

void Player::pauseMedia()
{
    neu->pause();
    disconnect(ui->a_pausebtn, SIGNAL(clicked()), this, SLOT(pauseMedia()));
    ui->a_playbtn->setVisible(true);
    ui->a_pausebtn->setVisible(false);
    connect(ui->a_playbtn, SIGNAL(clicked()), this, SLOT(playMedia()));
    if(a_isPlaylistOpen)
        playlist->setToPaused(a_mediaPlaylist.currentIndex());
}

void Player::forwardMedia()
{
    a_lastIndex = a_mediaPlaylist.currentIndex();
    a_mediaPlaylist.next();
    if(a_mediaPlaylist.currentMedia().isNull())
    {
        if(a_isLoopPlaylistMode)
        {
            a_mediaPlaylist.setCurrentIndex(0);
            playMedia();
        }
        else
        {
            emit EndOfMedia();
            return;
        }
    }
    if(a_hasToDoInitialShuffle) //For true randomness
    {
        qsrand(QTime::currentTime().msec());
        a_mediaPlaylist.setCurrentIndex(qrand() % a_mediaPlaylist.mediaCount());
        a_hasToDoInitialShuffle = false;
    }

    forwardAnim();
}

void Player::forwardAnim()
{
    a_titleAnimate->setStartValue(QPoint(130, 0));
    a_titleAnimate->setEndValue(QPoint(110, 0));
    a_titleAnimate->start();
}

void Player::previousMedia()
{
    a_lastIndex = a_mediaPlaylist.currentIndex();
    a_mediaPlaylist.previous();
    if(a_mediaPlaylist.currentMedia().isNull())
    {
        emit EndOfMedia();
        return;
    }
    if(a_hasToDoInitialShuffle)
    {
        qsrand(QTime::currentTime().msec());
        a_mediaPlaylist.setCurrentIndex(qrand() % a_mediaPlaylist.mediaCount());
        a_hasToDoInitialShuffle = false;
    }
    previousAnim();
}


void Player::previousAnim()
{
    a_titleAnimate->setStartValue(ui->a_label->pos().operator +=(QPoint(ui->a_label->width() - 200, 0)));
    a_titleAnimate->setEndValue(QPoint(110, 0));
    a_titleAnimate->start();
}

QMediaPlayer::Error Player::errorHandling(QMediaPlayer::Error error)
{
    int current;
    switch (error) {
    case QMediaPlayer::NoError:
        break;
    case QMediaPlayer::ResourceError:
        ui->a_label->setText(tr("erreur : Unresolved Resource"));
        a_mediaPlaylist.previous();
        current = a_mediaPlaylist.currentIndex();
        qDebug() << a_mediaPlaylist.media(current).canonicalUrl();
        a_mediaPlaylist.removeMedia(current);
        if(a_isPlaylistOpen)
            playlist->updateList(&a_mediaPlaylist);
        a_mediaPlaylist.setCurrentIndex(current - 2);
        a_hasToSavePlaylistLater = true; //Prevent error from coming back.
        break;
    case QMediaPlayer::FormatError :
        ui->a_label->setText(tr("erreur : Format non supporté"));
        pauseMedia();
        neu->stop();
        a_mediaPlaylist.clear();
        break;
    case QMediaPlayer::NetworkError :
        ui->a_label->setText(tr("erreur : Network error"));
        break;
    case QMediaPlayer::AccessDeniedError :
        QMessageBox::critical(this, tr("Erreur ! "), tr("Vous n'avez pas les droits pour accéder à ce fichier"));
        break;
    case QMediaPlayer::ServiceMissingError :
        QMessageBox::critical(this, tr("Erreur Critique !"), tr("Le service du player est indisponible. Impossible de lire le fichier !"));
        break;
    }
    a_isPlaying = false;
    return error;
}

void Player::statusChanged(QMediaPlayer::MediaStatus status)
{
    // handle status message and cursor
    switch (status) {
    case QMediaPlayer::LoadingMedia:
        this->setCursor(Qt::BusyCursor);
        break;
    case QMediaPlayer::StalledMedia:
        ui->a_label->setText("Media stalled");
        a_isPlaying = false;
        break;
    case QMediaPlayer::LoadedMedia:
        this->setCursor(Qt::ArrowCursor);
        if(!a_recoveredProgress && a_settings->value("Additional_Features/saveTrackIndex", false).toBool() == true
                || a_deleteTriggered)
        {
            a_deleteTriggered = false; // no more
            seekProgress(a_settings->value("trackPosition").toInt());
            a_recoveredProgress = true;
        }
        break;
    case QMediaPlayer::EndOfMedia:
        a_isPlaying = false;
        forwardMedia();
    }
}

void Player::stateChanged(QMediaPlayer::State state)
{
    //Handle the update of the bool a_isPlaying
    //Permits an up-to-date icon in the playlist
    switch(state)
    {
    case QMediaPlayer::PlayingState:
        a_isPlaying = true;
        break;
    case QMediaPlayer::PausedState:
        a_isPlaying = false;
        break;
    case QMediaPlayer::StoppedState:
        a_isPlaying = false;
        break;
    }
}

void Player::showMenu()
{
    a_menu->exec(QCursor::pos()); //simplest method pls
}

void Player::openMedia()
{
    if(!a_musicUserPath.isEmpty())
    {
        a_files = QFileDialog::getOpenFileUrls(this, tr("Sélectionnez des médias à lire"), a_musicUserPath, tr("Flux audios (*.mp3 *.mp4 *.m4a *.wav)"));
    }
    else
    {
        a_files = QFileDialog::getOpenFileUrls(this, tr("Sélectionnez des médias à lire"), (QStandardPaths::locate(QStandardPaths::MusicLocation, QString(), QStandardPaths::LocateDirectory)) , tr("Flux audios (*.mp3 *.mp4 *.m4a *.wav)"));
    }
    if(a_files.isEmpty()) //It was a mistake I guess, so don't do anything
        return;
    //Clear before processing
    a_mediaPlaylist.clear();
    unsigned int const fileNumber = a_files.size();
    for(unsigned int i (0); i < fileNumber; i++ )
    {
        a_mediaPlaylist.addMedia(a_files.at(i));
    }
    if(!a_mediaPlaylist.isEmpty())
    {
        neu->setPlaylist(&a_mediaPlaylist);
        playMedia();

        forwardAnim();

        if(a_isPlaylistOpen)
        {
            a_mediaPlaylist.setCurrentIndex(0);
            playlist->setCurrentItem(0, &a_coverArt, a_titre);
            playlist->updateList(&a_mediaPlaylist);
        }
    }
}

void Player::setMeta()
{
    //New track, we start at 0 secs !
    a_secondesPasse = 0; //For infos
    a_currentTrackTime = 0, a_currentTrackMinutes = 0; //For progress display
    // On check pour savoir si les metas sont nulles ou avec une chaine vide pour avoir une donnée correcte à afficher
    if(!neu->metaData("Title").isNull() && !neu->metaData("Title").operator ==(""))
        a_titre = neu->metaData("Title").toString();
    else if(neu->metaData("Title").isNull())
        a_titre = neu->currentMedia().canonicalUrl().fileName();
    if(a_titre.isEmpty())
        a_titre = neu->currentMedia().canonicalUrl().fileName();
    if(!neu->metaData("ContributingArtist").isNull() && !neu->metaData("ContributingTitle").operator ==(""))
        a_artiste = neu->metaData("ContributingArtist").toString();
    else
        a_artiste = tr("Artiste Inconnu");
    if(!neu->metaData("AlbumTitle").isNull() && !neu->metaData("AlbumTitle").operator ==(""))
        a_album = neu->metaData("AlbumTitle").toString();
    else
        a_album = tr("Album Inconnu");

    //Si c'est une vidéo ou un flux sans header
    if( (neu->metaData("Title").isNull())
         && (neu->metaData("ContributingArtist").isNull())
         && (neu->metaData("AlbumTitle").isNull()) )
    {
        a_artiste = neu->currentMedia().canonicalUrl().fileName();
        a_titre = neu->currentMedia().canonicalUrl().fileName();
        a_album = neu->currentMedia().canonicalUrl().fileName();
    }

    if(!ui->a_label->isVisible())
    {
        a_scrollingLabel->hide();
        ui->a_label->setVisible(true);
    }
    a_hasToSetLabelType = true;
    ui->a_label->setText(a_titre);
    ui->a_label->setToolTip(a_titre);
    setTypeTimer.start(230); //Set the type 230 ms later

    a_coverArt = QPixmap::fromImage(QImage(neu->metaData("ThumbnailImage").value<QImage>()));
    if(a_coverArt.isNull())
        a_coverArt = QPixmap::fromImage(QImage(neu->metaData("CoverArtImage").value<QImage>()));

    if(a_settings->value("Additional_Features/saveTrackIndex", false).toBool() == true){
        a_lastIndex = a_mediaPlaylist.currentIndex();
        a_settings->setValue("currentTrack", a_lastIndex);
         }
    if(a_isPlaylistOpen)
        playlist->setCurrentItem(a_mediaPlaylist.currentIndex(), &a_coverArt, a_titre);
    updateFadeinSpeed();
    Timer.start();
}

void Player::setType()
{
        updateLabel(a_titre); //Will set the good type
        a_hasToSetLabelType = false; //It's been done !
        finishingUp();
}

//Update and set type of label
void Player::updateLabel(QString &text)
{
    if(text.size() < 28)
    {
        a_isScrollingText = false;
        if(!ui->a_label->isVisible())
            ui->a_label->show();
        if(a_scrollingLabel->isVisible())
            a_scrollingLabel->hide();

        if(!a_hasToSetLabelType) //Doesn't need to set them if that's the case
        {
            ui->a_label->setText(text);
            ui->a_label->setToolTip(text);
        }
    }
    else
    {
        a_isScrollingText = true;
        if(ui->a_label->isVisible())
            ui->a_label->hide();
        a_scrollingLabel->setText(text);
        a_scrollingLabel->setToolTip(text);
        a_scrollingLabel->show();
    }
}

//This method keeps the fade in speed variable by the info label's length
//It is a workaround for very long info labels that don't have the time to scroll all the way
void Player::updateFadeinSpeed()
{
    /*                                      How it works
     * A case is the number of seconds an info type will be displayed till we switch over to the next info
     * As they're dependent to the previous one, we always add the seconds of the case before
     * ex : if each info is less/equal to 15 chars, it'll be case 3,6,9 so that it switches each 3 seconds
     * */
    if(a_titre.size() <= 17) //17 so it won't go to the else and generate 3 when the default would be 4 (it's an int, it floors)
        a_titleCase = 4;
    else
        a_titleCase = (a_titre.size() / 10) * 3; //3 is a magic number, and /10 is to have seconds

    if(a_artiste.size() <= 17)
        a_artistCase = a_titleCase + 4;
    else
        a_artistCase = a_titleCase + ((a_artiste.size() / 10) * 3);
    if(a_album.size() <= 17)
        a_albumCase = a_artistCase + 4;
    else
        a_albumCase = a_artistCase + ((a_album.size() / 10) * 3);
}

//Does the animation between each info
void Player::fadeInLabel()
{
    //This changes the target if we're using the scrolling label
    if(a_isScrollingText)
        a_scrollingLabel->setGraphicsEffect(a_infoFadein);
    else
        ui->a_label->setGraphicsEffect(a_infoFadein);

    a_infoFadeAnim->setTargetObject(a_infoFadein);
    a_infoFadeAnim->setPropertyName("opacity");
    a_infoFadeAnim->setDuration(700);
    a_infoFadeAnim->setStartValue(0);
    a_infoFadeAnim->setEndValue(1.0);
    a_infoFadeAnim->start();
}

void Player::update_info()
{
    //update à chaque seconde
    a_secondesPasse++;

    /*Permet de gérer les informations à afficher en fonction du temps
     * Fades each time
     * The cases are named after the item which is displayed BEFORE going to the specific case
     * It might seems unclear at first but I believe it's better this way
    */

    //Switches over to normal if's because switch case can't evaluate non-constant expression
    if(a_secondesPasse == a_titleCase)
    {
        updateLabel(a_artiste);
        fadeInLabel();
    }
    if(a_secondesPasse == a_artistCase)
    {
        updateLabel(a_album);
        fadeInLabel();
    }
    if(a_secondesPasse == a_albumCase)
    {
        a_secondesPasse = 0;
        updateLabel(a_titre);
        fadeInLabel();
    }

    if(a_currentTrackTime == 60)
    {
        a_currentTrackTime = 0;
        a_currentTrackMinutes++;
        //Pour avoir ce fichu 0 en plus quand il n'y a pas encore de dizaine !
        if(a_currentTrackTime < 10)
            ui->a_currenttime->setText(QString::number(a_currentTrackMinutes) + ":0" + QString::number(a_currentTrackTime));
        else
            ui->a_currenttime->setText(QString::number(a_currentTrackMinutes) + ":" + QString::number(a_currentTrackTime));
    }

    if(a_isPlaying) //The update_info is now called even when paused so it's a guard
        a_currentTrackTime++;
}
    /*///////SliderBar Section///////*/
void Player::UpdateProgress(qint64 pos)
{
    ui->a_progressslider->setValue(pos);
    pos /= 1000;
    a_currentTrackMinutes = 0;
    // cf setMeta()
    while (pos % 60 != pos)
    {
        pos -= 60;
        a_currentTrackMinutes++;
    }
    a_currentTrackTime = pos;
    if(a_currentTrackTime < 10)
        ui->a_currenttime->setText(QString::number(a_currentTrackMinutes) + ":0" + QString::number(a_currentTrackTime));
    else
        ui->a_currenttime->setText(QString::number(a_currentTrackMinutes) + ":" + QString::number(a_currentTrackTime));
}

//Produit grâce à un signal EndOfMedia();
void Player::endOfMediaGuard()
{
    Timer.stop(); //There should be no more info to display
    ui->a_pausebtn->setVisible(false);
    ui->a_playbtn->setVisible(true);
    disconnect(ui->a_pausebtn, SIGNAL(clicked()), this, SLOT(pauseMedia()));
    connect(ui->a_playbtn, SIGNAL(clicked()), this, SLOT(playMedia()));
    ui->a_label->setText(tr("Stoppé"));
}
void Player::seekProgress(int pos)
{
    neu->setPosition(pos);
    if(a_recoveredProgress)
    {
        neu->play();
        disconnect(ui->a_playbtn, SIGNAL(clicked()), this, SLOT(playMedia()));
        ui->a_playbtn->setVisible(false);
        ui->a_pausebtn->setVisible(true);
        connect(ui->a_pausebtn, SIGNAL(clicked()), this, SLOT(pauseMedia()));

    }
    if(a_isPlaylistOpen && !playlist->isPlayingState())
        playlist->setToPlaying(a_mediaPlaylist.currentIndex());
    if(pos > neu->duration())
        forwardMedia();
}

void Player::on_durationChanged(qint64 pos)
{
    ui->a_progressslider->setMaximum(pos);
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
        ui->a_duration->setText(QString::number(minutes) + ":0" + QString::number(secondes));
    else
        ui->a_duration->setText(QString::number(minutes) + ":" + QString::number(secondes));
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
        ui->a_volumebtn->setChecked(true);
        setVolumeMuted();
        return;
    }
    if(pos <= 50)
    {
        if(a_idSkin == 1)
            ui->a_volumebtn->setIcon(QIcon(":/Ressources/volumebtn2.png"));
        else
            ui->a_volumebtn->setIcon(QIcon(":/Ressources/volumedarkbtn2.png"));
    }
    else
    {
        if(a_idSkin == 1)
            ui->a_volumebtn->setIcon(QIcon(":/Ressources/volumebtn.png"));
        else
            ui->a_volumebtn->setIcon(QIcon(":/Ressources/volumedarkbtn.png"));
    }
    ui->a_volumebtn->setChecked(false);
    neu->setMuted(false);

}

void Player::setVolumeMuted()
{
    if(ui->a_volumebtn->isChecked())
    {
        a_volumeBeforeMute = neu->volume();
        disconnect(ui->a_volumeslider, SIGNAL(valueChanged(int)), this, SLOT(on_volumeChanged(int)));
        neu->setMuted(true);
        ui->a_volumeslider->setValue(0);
        connect(ui->a_volumeslider, SIGNAL(valueChanged(int)), this, SLOT(on_volumeChanged(int)));
        if(a_idSkin == 1)
            ui->a_volumebtn->setIcon(QIcon(":/Ressources/volumebtn_onPressed.png"));
        if(a_idSkin == 0)
            ui->a_volumebtn->setIcon(QIcon(":/Ressources/volumedarkbtn_onPressed.png"));
    }
    else
    {
        neu->setMuted(false);
        ui->a_volumeslider->setValue(a_volumeBeforeMute); //On laisse le _onvolumeChanged() faire le boulot de remettre les bonnes icones
    }
}

void Player::volumeUp()
{
    ui->a_volumeslider->setSliderPosition(neu->volume() + 10);
}

void Player::volumeDown()
{
    ui->a_volumeslider->setSliderPosition(neu->volume() - 10);
}

    /*///////Menu Actions Section///////*/

void Player::setShuffle()
{
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
        int index = qrand() % a_mediaPlaylist.mediaCount();
        a_mediaPlaylist.setCurrentIndex(index);
        //Plus le shuffle, qui crée un rand en prenant comme valeur le média actuel
        a_mediaPlaylist.shuffle();
        a_mediaPlaylist.setCurrentIndex(0); //So it looks better this way
        if(!a_isRandomMode)
            a_mediaPlaylist.setPlaybackMode(QMediaPlaylist::Sequential); //Default : random. We want it back to Sequential if we were on sequential before
        //On a maintenant un double rand : un rand purement aléatoire, et un rand basé sur le media actuel

        forwardAnim(); //Showing it advanced

        if(a_settings->value("Additional_Features/libraryAtStartup", false).toBool() == true)
            a_hasToSavePlaylistLater = true; // Will update playlist to load the correct one when booting up later on
        if(a_isPlaylistOpen)
            playlist->updateList(&a_mediaPlaylist);
    }
}

void Player::windowFlagsHandler()
{
    if(a_alwaysOnTopHandler->isChecked())
        this->setWindowFlags(Qt::WindowStaysOnTopHint);
    else
        this->setWindowFlags(Qt::Window);
    this->show();
}

void Player::playbackHandler(QAction *playbackSelected)
{
    if(playbackSelected->text() == "1.0x")
        a_playbackState = 0;
    else if (playbackSelected->text() == "0.5x")
        a_playbackState = 1;
    else if (playbackSelected->text() == "2.0x")
        a_playbackState = 2;
    setPlaybackRate();
}

void Player::setPlaybackRate()
{
    //Setup playback rate
    //Pause pour forcer de rappuyer sur lecture à nouveau, de façon à ce que le buffer soit déjà à nouveau à peu près rempli
    pauseMedia();
    if(a_playbackState == 0)
    {
        neu->setPlaybackRate(1);
        normalrate->setChecked(true);
    }
    else if (a_playbackState == 1)
    {
        neu->setPlaybackRate(0.5);
        halfrate->setChecked(true);
    }
    else if (a_playbackState == 2)
    {
        neu->setPlaybackRate(2);
        doublerate->setChecked(true);
    }
}

void Player::showPlaylist()
{
    if(!a_isPlaylistOpen)
    {
        a_isPlaylistOpen = true;
        if(a_mediaPlaylist.mediaCount()!= 0 )
            playlist = new Playlist(&a_mediaPlaylist, a_mediaPlaylist.currentIndex(), this, &a_coverArt, a_titre, a_isPlaying, this);
        else if(ui->a_label->text() != "Stoppé" || ui->a_label->text() == "Stopped")
            playlist = new Playlist(&a_mediaPlaylist, -1, this, &a_coverArt, a_titre, a_isPlaying, this);

        playlist->show();
    }
    else
    {
        playlist->showNormal();
        playlist->activateWindow();
    }
}

void Player::showSettings()
{
    Settings *settings = new Settings(this, this);
    settings->show();
}

void Player::showTagViewer()
{
    if(neu->media().isNull())
        return;

    /* Add meta datas to a list which will be treated by the tageditor constructor */
    QList<QString> metaDatas;
    metaDatas.append(neu->metaData("TrackNumber").toString());
    metaDatas.append(a_titre);
    metaDatas.append(a_artiste);
    metaDatas.append(a_album);
    metaDatas.append(neu->metaData("Year").toString());
    metaDatas.append(neu->metaData("Genre").toString());
    //Load it now !
    TagViewer *TagWindow = new TagViewer(metaDatas, &a_coverArt , this);
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
    a_mediaPlaylist.clear();
    pauseMedia();
    ui->a_label->setText("Loading...");
    this->setCursor(Qt::BusyCursor);
    unsigned int const numberUrls = event->mimeData()->urls().size();
    for (unsigned int i(0); i < numberUrls; i++)
    {
        a_mediaPlaylist.addMedia(event->mimeData()->urls().at(i));
        //Permet de toujours rendre l'application plus ou moins utilisable
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    neu->setPlaylist(&a_mediaPlaylist);
    if(a_isPlaylistOpen)
        playlist->updateList(&a_mediaPlaylist);

    playMedia();
    this->setCursor(Qt::ArrowCursor);
}

void Player::closeEvent(QCloseEvent *)
{
    if(a_hasToSavePlaylistLater && a_settings->value("Additional_Features/libraryAtStartup").toBool() == true)
    {
        saveCurrentPlaylist();
    }
    a_settings->setValue("pos", pos());
    a_settings->setValue("size", size());
    a_settings->setValue("volume", ui->a_volumeslider->value());
    a_settings->setValue("visibilite", a_alwaysOnTopHandler->isChecked());
    a_settings->setValue("playbackrate", a_playbackState);
    a_settings->setValue("random", a_isRandomMode);
    a_settings->setValue("loop", a_isLoopPlaylistMode);
    if(a_settings->value("Additional_Features/saveTrackIndex", false).toBool() == true)
        a_settings->setValue("trackPosition", neu->position());
    qApp->closeAllWindows();
}

    /*///////Playlist Interface Section///////*/

void Player::setRandomMode(bool mode)
{
    bool prev = a_isRandomMode;
    if(prev != mode) //On change que s'il y a besoin
    {
        a_isRandomMode = mode;
        if(a_isRandomMode)
        {
            a_mediaPlaylist.setPlaybackMode(QMediaPlaylist::Random);
            a_hasToDoInitialShuffle = true;
        }
        else
        {
            a_mediaPlaylist.setPlaybackMode(QMediaPlaylist::Sequential);
            if(a_hasToDoInitialShuffle)
                a_hasToDoInitialShuffle = false;
        }
    }
}

void Player::setLoopMode(bool mode)
{
    bool prev = a_isLoopPlaylistMode;
    if(prev != mode) //On change que s'il y a besoin
    {
        a_isLoopPlaylistMode = mode;
    }
}

//Called when playlist wants to play something
void Player::setIndexOfThePlayer(int index, bool play)
{
    a_mediaPlaylist.setCurrentIndex(index);
    if(play)
    {
        playMedia();
        forwardAnim(); //For lack of a better animation, this one suits most cases
    }
    else
        pauseMedia();
}

//Called when dealing with a drag & drop
void Player::addMediasToThePlayer(QList<QUrl> &medias)
{
    int insertTo = a_mediaPlaylist.currentIndex() + 1;
    unsigned int const mediaNumber = medias.size();
    for(unsigned int i(0); i < mediaNumber; i++, insertTo++)
    {
        a_mediaPlaylist.insertMedia(insertTo, QUrl(medias.at(i)));
    }
    if(a_isPlaylistOpen)
        playlist->quickUpdate(&medias, a_mediaPlaylist.currentIndex() + 1);
    if(a_hasToSavePlaylistLater != true)
        a_hasToSavePlaylistLater = true;
}

//When added to queue
void Player::addToQueue(int index, int currentlyPlaying)
{
    int insertTo = currentlyPlaying + playlist->queuedIndex(); //if you add one music, it'll place the next one next to it, ect...
    QMediaContent media(a_mediaPlaylist.media(index)); //We copy the media
    a_mediaPlaylist.insertMedia(insertTo, media);
    QList <QUrl> temp;
    temp.append(a_mediaPlaylist.media(insertTo).canonicalUrl());
    playlist->quickUpdate(&temp, insertTo); //The quick update will place the media at insertTo index without reloading the whole playlist
    if(a_hasToSavePlaylistLater != true)
        a_hasToSavePlaylistLater = true;
}

//Called when setting a folder from playlist
void Player::setPlaylistOfThePlayer(QList<QUrl> &medias)
{
    neu->stop();
    a_mediaPlaylist.clear();
    unsigned int const mediaNumber = medias.size();
    for(unsigned int i (0); i < mediaNumber; i++ )
    {
        a_mediaPlaylist.addMedia(medias.at(i));
    }
    if(!a_mediaPlaylist.isEmpty())
    {
        neu->setPlaylist(&a_mediaPlaylist);
        if(a_isPlaylistOpen)
            playlist->updateList(&a_mediaPlaylist);
        if(a_hasToSavePlaylistLater != true)
            a_hasToSavePlaylistLater = true;
    }
}

//Destructor
Player::~Player()
{
    delete ui;
}
