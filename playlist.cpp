#include "playlist.h"
#include "ui_playlist.h"
#include <QFileDialog>
#include <QDirIterator>

Playlist::Playlist(neuPlaylist &liste, neuPlaylist &favs, Player *player,
                   QPixmap *cover, QString title, bool playingState, QWidget *parent) :
    QMainWindow(parent), ui(new Ui::Playlist),
    m_settings("neuPlayer.ini", QSettings::IniFormat, this)
{
    ui->setupUi(this);
    ui->m_searchLine->setHidden(true);
    this->setAcceptDrops(true);
    this->setAttribute(Qt::WA_DeleteOnClose);

    resize(m_settings.value("Playlist/size", QSize(265, 300)).toSize());
    ui->m_playlistWidget->setDragEnabled(false); //I can't handle it well as of now :/
    ui->m_playlistFavWidget->setDragEnabled(false);
    m_defaultCover.load(":/Ressources/noCoverHeader.png");
    //On reçoit le player, il faut maintenant le mettre en tant qu'attribut pour l'utiliser partout
    m_player = player;
    //De même, on met la playlist en tant qu'attribut pour l'utiliser plus tard.
    m_playlist = &liste;
    m_favPlaylist = &favs; //Et l'autre
    m_previousIndex = 99999; //Pretty sure nobody would reach that value
    m_queueIndex = 0; //used to determine where to place the song you want to queue
    //Prepare fade manager animations
    m_fadeManager.addTarget(ui->m_cover, FadeManager::FadeIn, 400, FadeManager::Parallel);
    m_fadeManager.addTarget(ui->m_titleHeader, FadeManager::FadeIn, 300, FadeManager::Parallel);
    //Prepare tab animations
    m_moveAnim.setDuration(MoveAnimation::Fast);
    setupActions();
    //Setup list
    updateList(&liste);
    if(!player->isUsingFav())
    {
        if(!liste.isEmpty()) //If it's okay
        {
            if(liste.currentIndex() == -1) //If it's stopped or something weird is going on
                ui->m_titleHeader->setText(tr("Séléctionnez une musique à lire"));
            else //Okay, gogo
            {
                ui->m_playlistWidget->scrollToItem(ui->m_playlistWidget->item(
                                                       liste.currentIndex()),
                                                       QAbstractItemView::PositionAtTop);

                setCurrentItem(liste.currentIndex(), cover, title, playingState);
                //Update the icon if the player is paused or stopped
            }
        }
        else //If not, shows there's nothing
        {
            ui->m_playlistWidget->addItem(tr("Aucune musique"));
            ui->m_playlistWidget->item(0)->setTextAlignment(Qt::AlignCenter);
        }
    }
    else
    {
        updateFavs(&favs);
        if(!favs.isEmpty()) //If it's okay
        {
            if(favs.currentIndex() != -1) //Okay, gogo
            {
                ui->m_playlistFavWidget->scrollToItem(ui->m_playlistFavWidget->item(
                                                          favs.currentIndex()),
                                                          QAbstractItemView::PositionAtTop);
                setCurrentItem(favs.currentIndex(), cover, title, playingState);
                //Update the icon if the player is paused or stopped
            }
        }
        setTab(1);
    }

    setupConnections();
    setupMode();
    if(player->y() > 600)
        this->move(player->x() + 60, player->y() - 200);
}

void Playlist::setupActions()
{
    //Actions & Playlist Menu
    m_selectOtherDir = new QAction(tr("Sélectionner un dossier à lire"), this);
    m_selectOtherDir->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_O);
    m_reloadLibrary = new QAction(tr("Recharger la bibliothèque"), this);
    m_reloadLibrary->setShortcut(Qt::Key_F5);
    m_playlistMenu = new QMenu(this);
    m_playlistMenu->addAction(m_selectOtherDir);
    m_playlistMenu->addAction(m_reloadLibrary);
    ui->m_tools->setMenu(m_playlistMenu);

    //Actions & Context menu
    ui->m_playlistWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->m_playlistFavWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    m_findItemTrigger = new QAction(this);
    m_findItemTrigger->setShortcut(Qt::CTRL + Qt::Key_F);
    m_findItemTrigger->setCheckable(true);
    m_deleteItemFromQueue = new QAction(tr("Supprimer de la file d'attente"), this);
    m_deleteItemFromQueue->setShortcut(Qt::Key_Delete);
    m_addToQueue = new QAction(tr("Ajouter à la file d'attente"), this);
    m_addToQueue->setShortcut(Qt::CTRL + Qt::Key_Plus);
    m_addToFav = new QAction(tr("Ajouter aux favoris"), this);
    m_addToFav->setShortcut(QKeySequence("CTRL + *"));
    m_viewInfo = new QAction(tr("Voir les informations"), this);
    m_viewInfo->setShortcut(Qt::ALT + Qt::Key_Return);
    m_easyHideFind = new QAction(this);
    m_easyHideFind->setShortcut(Qt::Key_Escape);
    m_validateFind = new QAction(this);
    m_validateFind->setShortcut(Qt::Key_Return); //Action so I don't bother with events to validate

    this->addAction(m_addToQueue);
    this->addAction(m_findItemTrigger);
    this->addAction(m_viewInfo);
    this->addAction(m_deleteItemFromQueue);

    //Setup menu
    m_playlistContextMenu = new QMenu(this);
    m_playlistContextMenu->addAction(m_addToQueue);
    m_playlistContextMenu->addAction(m_deleteItemFromQueue);
    m_playlistContextMenu->addAction(m_viewInfo);
    m_playlistContextMenu->addAction(m_addToFav);
}


void Playlist::setupConnections()
{
    /* Connexions */
    connect(m_selectOtherDir, SIGNAL(triggered()), this, SLOT(setFolder()));
    connect(m_reloadLibrary, SIGNAL(triggered()), this, SLOT(reloadLibrary()));
    connect(ui->m_switchRandomPlay, SIGNAL(clicked()), this, SLOT(randomModeChanger()));
    connect(ui->m_switchLoopPlay, SIGNAL(clicked()), this, SLOT(loopModeChanger()));
    connect(ui->m_playlistWidget, SIGNAL(activated(QModelIndex)),
            this, SLOT(playItem(QModelIndex)));
    connect(ui->m_playlistFavWidget, SIGNAL(activated(QModelIndex)),
            this, SLOT(playItem(QModelIndex)));
    //This one below is commented out because it doesn't work yet. Need help as to why it crashes
//    connect(m_findItemTrigger, SIGNAL(triggered()), this, SLOT(findItemVisibilityHandler()));
    connect(ui->m_playlistWidget, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showContextMenu()));
    connect(ui->m_playlistFavWidget, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showContextMenu()));
    connect(m_playlistContextMenu, SIGNAL(aboutToShow()),
            this, SLOT(prepareContextMenu()));
    connect(m_deleteItemFromQueue, SIGNAL(triggered()), this, SLOT(deleteItem()));
    connect(m_addToQueue, SIGNAL(triggered()), this, SLOT(addItemToQueue()));
    connect(m_viewInfo, SIGNAL(triggered()), this, SLOT(viewInfo()));
    connect(m_addToFav, SIGNAL(triggered()), this, SLOT(addToFavContext()));
    connect(ui->m_addFavStar, SIGNAL(clicked()), this, SLOT(on_starClicked()));
    connect(ui->m_titleHeader, SIGNAL(clicked()), this, SLOT(scrollToPlaying()));
    connect(ui->m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(setTab(int)));
    connect(m_validateFind, SIGNAL(triggered()), this, SLOT(validateSearch()));
}

void Playlist::setupMode()
{
    //Random mode setup
    bool isRandomMode = m_player->getRandomState();
    if(isRandomMode)
        ui->m_switchRandomPlay->setChecked(true);
    else
        ui->m_switchRandomPlay->setChecked(false);

    //Loop mode setup
    bool isLoopingPlaylistMode = m_player->getLoopState();
    if(isLoopingPlaylistMode)
        ui->m_switchLoopPlay->setChecked(true);
    else
        ui->m_switchLoopPlay->setChecked(false);
}

        /* Update section */

void Playlist::updateList(neuPlaylist *listeFichiers, bool setZeroIndex)
{
    //Helps not to crash.
    //Each index is reset to 0 so there isn't any out of bound
    //but only when we're not starting up
    if(m_currentIndex != 0 && setZeroIndex)
    {
        m_player->setIndexOfThePlayer(0, false);
        ui->m_playlistWidget->setCurrentRow(0);
    }
    if(ui->m_tabWidget->currentIndex() != 0)
        ui->m_tabWidget->setCurrentIndex(0);
    ui->m_playlistWidget->clear();
    int const mediaCount = listeFichiers->mediaCount();
    for(int i(0); i < mediaCount; i++)
    {
        QString filepath = listeFichiers->media(i).canonicalUrl().toString();
        ui->m_playlistWidget->addItem(QFileInfo(filepath).fileName());
    }
}

//Called when the tab is set to favs, and it wasn't initially called
void Playlist::updateFavs(neuPlaylist *favPlaylist)
{
    ui->m_playlistFavWidget->clear();
    int const mediaCount = favPlaylist->mediaCount();
    for(int i(0); i < mediaCount; i++)
    {
        QString filepath = favPlaylist->media(i).canonicalUrl().toString();
        ui->m_playlistFavWidget->addItem(QFileInfo(filepath).fileName());
    }
    m_favsNotLoadedYet = false;
}

//Updates the list very fast. Used when adding an element
void Playlist::quickUpdate(QList<QUrl> *items, int currentItemPlusOne)
{
    unsigned int const numberItems = items->size();
    if(!m_player->isUsingFav())
    {
        for(unsigned int i(0); i < numberItems; i++, currentItemPlusOne++)
        {
            ui->m_playlistWidget->insertItem(currentItemPlusOne, items->at(i).fileName());
        }
    }
    else
    {
        if(m_favsNotLoadedYet) //Loads everything
            updateFavs(m_favPlaylist);
        else //Loads only what's necessary, the rest is already there
        {
            for(unsigned int i(0); i < numberItems; i++, currentItemPlusOne++)
            {
                ui->m_playlistFavWidget->insertItem(currentItemPlusOne, items->at(i).fileName());
            }
        }
    }
}
        /* State section */
//Those are handled by the player

void::Playlist::setToPlaying(int index)
{
    if(!m_player->isUsingFav())
        ui->m_playlistWidget->item(index)->setIcon(QIcon(":/Ressources/playingState_icon.png"));
    else
        ui->m_playlistFavWidget->item(index)->setIcon(QIcon(":/Ressources/playingState_icon.png"));
    m_isPlaying = true;
}


void Playlist::setToPaused(int index)
{
    if(!m_player->isUsingFav())
        ui->m_playlistWidget->item(index)->setIcon(QIcon(":/Ressources/pausedState-icon2.png"));
    else
        ui->m_playlistFavWidget->item(index)->setIcon(QIcon(":/Ressources/pausedState-icon2.png"));
    m_isPlaying = false;
}

void Playlist::setCurrentItem(int index, QPixmap *cover, QString title, bool playing)
{
    if(m_previousIndex != 99999) //Default value of m_previousIndex
    {
        //Then it obviously exists within the list
        if(ui->m_playlistWidget->count() > m_previousIndex)
            ui->m_playlistWidget->item(m_previousIndex)->setIcon(QIcon());
        if(ui->m_playlistFavWidget->count() > m_previousIndex)
            ui->m_playlistFavWidget->item(m_previousIndex)->setIcon(QIcon());
    }
    m_queueIndex = 0;
    m_previousIndex = index;
    if(playing)
        setToPlaying(index);
    else
        setToPaused(index);
    m_currentIndex = index;
    if(!m_player->isUsingFav())
    {
        ui->m_playlistWidget->setCurrentRow(index);
        if(checkIfNotFav(index))
            makeStarFull();
        else
            makeStarEmpty();
    }
    else
    {
        ui->m_playlistFavWidget->setCurrentRow(index);
        makeStarFull();
    }
    scrollToPlaying();
    //Use data received
    updateInfoHeader(title, *cover);
}


void Playlist::updateInfoHeader(QString &title, QPixmap &cover)
{
    ui->m_cover->setPixmap(cover);
    if(ui->m_cover->pixmap()->isNull())
        ui->m_cover->setPixmap(m_defaultCover);
    ui->m_titleHeader->setText(title);
    ui->m_titleHeader->setToolTip(title);

    //Won't start the animation if we're actually not changing the item played
    if(!m_player->deleteTriggered())
        m_fadeManager.startGroup(FadeManager::Parallel, false);
}

        /* Playlist functionality */

void Playlist::setFolder()
{
    QString selectDir;

    if(!m_isReload)
        selectDir = (QFileDialog::getExistingDirectory
                     (this, tr("Ouvrir un répertoire"),
                      "", QFileDialog::DontResolveSymlinks));
    else if(!m_settings.value("mediapath").toString().isEmpty())
        selectDir =  m_settings.value("mediapath", "").toString();

    if(!selectDir.isEmpty())
    {
        setCursor(Qt::BusyCursor);
        //Helps not to crash
        //the index is reset to 0 so there isn't any out of bound.
        if(m_currentIndex != 0)
        {
            m_player->setIndexOfThePlayer(0, false);
            ui->m_playlistWidget->setCurrentRow(0);
        }
        QList <QUrl> urlList = m_playlist->update(selectDir);
        if(!urlList.empty()) //Don't set anything if there wasn't any content
            m_player->updatePlaylistOfThePlayer(urlList, true);
        setCursor(Qt::ArrowCursor);
    }
}

void Playlist::on_starClicked()
{
    if(!m_player->isUsingFav())
    {
        if(!checkIfNotFav(m_currentIndex))
            addToFav();
        else
        {
            deleteFav(m_currentIndex);
            makeStarEmpty();
        }
    }
    else //It's already in favorite, so...
    {
        deleteFav(m_currentIndex);
        makeStarEmpty();
    }
}

//Proxy method that uses setFolder();
void Playlist::reloadLibrary()
{
    m_isReload = true;
    setFolder();
    m_isReload = false;
}

void Playlist::randomModeChanger()
{
    if(ui->m_switchRandomPlay->isChecked())
        setShuffle();
    else
        setSequential();
    this->setFocus(); //évite d'avoir le focus sur le bouton
}

void Playlist::loopModeChanger()
{
    if(ui->m_switchLoopPlay->isChecked())
        m_player->setLoopMode(true);
    else
        m_player->setLoopMode(false);
    this->setFocus();
}

void Playlist::setSequential()
{
    m_player->setRandomMode(false); // 0 = No random = Sequential
}

void Playlist::setShuffle()
{
    m_player->setRandomMode(true); // 1 = random
}

void Playlist::playItem(QModelIndex itemIndex)
{
    if(!m_player->canChangeMusic())
        return;
    if(ui->m_tabWidget->currentIndex() == 0 &&
       ui->m_playlistWidget->currentItem()->textAlignment() == Qt::AlignCenter)
    {
        setFolder();
        return;
    }
    if(ui->m_tabWidget->currentIndex() == 1 &&
       ui->m_playlistFavWidget->currentItem()->textAlignment() == Qt::AlignCenter)
        return;
    if(!m_player->isUsingFav() && ui->m_tabWidget->currentIndex() == 1)
        m_player->changeToFavPlaylist();
    else if(ui->m_tabWidget->currentIndex() == 0 && m_player->isUsingFav())
        m_player->changeToDefaultPlaylist();
    m_player->setIndexOfThePlayer(itemIndex.row(), true);
    m_currentIndex = itemIndex.row();
}

void Playlist::prepareContextMenu()
{
    // 0 : add to queue
    // 1 : delete from queue
    // 2 : view info
    // 3 : add to fav
    if(ui->m_tabWidget->currentIndex() == 0 &&
       m_currentIndex != ui->m_playlistWidget->currentRow())
    {
        m_deleteItemFromQueue->setText(tr("Supprimer de la file d'attente"));
        m_playlistContextMenu->actions().at(3)->setVisible(true);
        m_playlistContextMenu->actions().at(2)->setVisible(true);
        m_playlistContextMenu->actions().at(1)->setVisible(true);
        m_playlistContextMenu->actions().at(0)->setVisible(true);
    }
    else if(ui->m_tabWidget->currentIndex() == 0 &&
            m_currentIndex == ui->m_playlistWidget->currentRow())
    {
        m_playlistContextMenu->actions().at(3)->setVisible(true);
        m_playlistContextMenu->actions().at(2)->setVisible(true);
        m_playlistContextMenu->actions().at(1)->setVisible(false);
        m_playlistContextMenu->actions().at(0)->setVisible(false);
    }
    //if no music on playlist
    if(ui->m_tabWidget->currentIndex() == 0 &&
       ui->m_playlistWidget->currentItem()->textAlignment() == Qt::AlignCenter)
    {
        m_playlistContextMenu->actions().at(3)->setVisible(false);
        m_playlistContextMenu->actions().at(2)->setVisible(false);
        m_playlistContextMenu->actions().at(1)->setVisible(false);
        m_playlistContextMenu->actions().at(0)->setVisible(false);
    }
    if(m_player->isUsingFav() &&
       m_currentIndex == ui->m_playlistFavWidget->currentRow() &&
       ui->m_tabWidget->currentIndex() == 1)
    {
        m_playlistContextMenu->actions().at(3)->setVisible(false);
        m_playlistContextMenu->actions().at(2)->setVisible(true);
        m_playlistContextMenu->actions().at(1)->setVisible(false);
        m_playlistContextMenu->actions().at(0)->setVisible(false);
    }
}

//Activated by the clicked() signal of the m_titleHeader UI component
void Playlist::scrollToPlaying()
{
    if(!m_player->isUsingFav())
    {
        if(ui->m_tabWidget->currentIndex() != 0)
            ui->m_tabWidget->setCurrentIndex(0);
        ui->m_playlistWidget->setCurrentRow(m_currentIndex);
        ui->m_playlistWidget->scrollTo(ui->m_playlistWidget->currentIndex(),
                                       QAbstractItemView::PositionAtTop);
    }
    else
    {
        if(ui->m_tabWidget->currentIndex() != 1)
            ui->m_tabWidget->setCurrentIndex(1);
        ui->m_playlistFavWidget->setCurrentRow(m_currentIndex);
        ui->m_playlistFavWidget->scrollTo(ui->m_playlistFavWidget->currentIndex(),
                                          QAbstractItemView::PositionAtTop);
    }
}

void Playlist::setTab(int tabId)
{
    if(tabId == 1) //Favorites
    {
        m_deleteItemFromQueue->setText(tr("Supprimer des favoris"));
        m_playlistContextMenu->actions().at(3)->setVisible(false);
        m_playlistContextMenu->actions().at(2)->setVisible(true);
        m_playlistContextMenu->actions().at(1)->setVisible(true);
        m_playlistContextMenu->actions().at(0)->setVisible(false);
        m_moveAnim.setTarget(ui->m_playlistFavWidget);
        m_moveAnim.setDirection(MoveAnimation::LeftToRight);
        m_moveAnim.start();
        if(m_favsNotLoadedYet)
            updateFavs(m_favPlaylist);
    }
    else
    {
        m_moveAnim.setTarget(ui->m_playlistWidget);
        m_moveAnim.setDirection(MoveAnimation::RightToLeft);
        m_moveAnim.start();
    }
    m_previousTab = tabId;
    if(ui->m_tabWidget->currentIndex() != tabId) //If called
        ui->m_tabWidget->setCurrentIndex(tabId);
}


void Playlist::showContextMenu() const
{
    m_playlistContextMenu->exec(QCursor::pos());
}

void Playlist::addItemToQueue()
{
    m_player->addToQueue(ui->m_playlistWidget->currentRow(), m_currentIndex);
}

void Playlist::addToFavContext()
{
    if(!checkIfNotFav(ui->m_playlistWidget->currentRow()))
        m_player->addFav(ui->m_playlistWidget->currentRow());
    if(ui->m_tabWidget->currentIndex() == 0 &&
       m_currentIndex == ui->m_playlistWidget->currentRow())
        makeStarFull();
}

void Playlist::addToFav()
{
    if(!checkIfNotFav(m_currentIndex)) //If not there already
    {
        m_player->addFav(m_currentIndex);
        makeStarFull();
    }
}

bool Playlist::checkIfNotFav(int index)
{
    QString fileAdded;
    fileAdded = m_playlist->media(index).canonicalUrl().fileName(); //We extract the filename
    unsigned int mediaCount = m_favPlaylist->mediaCount();
    for(unsigned int i(0); i < mediaCount; i++)
    {
        if(m_favPlaylist->media(i).canonicalUrl().fileName() == fileAdded)
            return true; //It already exists, do not add it
    }
    return false;
}

void Playlist::deleteFav(int index)
{
    QString fileAdded;
    if(!m_player->isUsingFav())
        fileAdded = m_playlist->media(index).canonicalUrl().fileName();
    else
        fileAdded = m_favPlaylist->media(index).canonicalUrl().fileName();
    unsigned int mediaCount = m_favPlaylist->mediaCount();
    for(unsigned int i(0); i < mediaCount; i++)
    {
        if(m_favPlaylist->media(i).canonicalUrl().fileName() == fileAdded)
        {
            if(m_player->isUsingFav())
                m_favPlaylist->setCurrentIndex(i - 1);
            m_favPlaylist->removeMedia(i); //Remove media in fav list, index i
            ui->m_playlistFavWidget->takeItem(i);
        }
    }
}

void Playlist::makeStarFull()
{
    ui->m_addFavStar->setIcon(QIcon(":/Ressources/star_full.png"));
    ui->m_addFavStar->setToolTip(tr("Cliquez pour enlever des favoris"));
}

void Playlist::makeStarEmpty()
{
    ui->m_addFavStar->setIcon(QIcon(":/Ressources/star.png"));
    ui->m_addFavStar->setToolTip(tr("Cliquez pour ajouter aux favoris"));
}

void Playlist::deleteItem()
{
    int index;
    /* Protections against the shortcut */
    if(ui->m_tabWidget->currentIndex() == 0)
        index = ui->m_playlistWidget->currentRow();
    else
        index = ui->m_playlistFavWidget->currentRow();

    if(m_player->deleteMedia(index, ui->m_tabWidget->currentIndex()))
    {
        //So if we're deleting something that was added, we update the queue index
        if(index <= m_queueIndex + m_currentIndex)
            m_queueIndex--;
        if(ui->m_tabWidget->currentIndex() == 0)
            ui->m_playlistWidget->takeItem(index);
        else
        {
            ui->m_playlistFavWidget->takeItem(index);
            m_player->setFavsToBeSaved(true);
        }
    }
}

//For use when the player is in panic mode.
void Playlist::deleteItem(int index)
{
    if(m_player->isUsingFav())
        ui->m_playlistFavWidget->takeItem(index);
    else
        ui->m_playlistWidget->takeItem(index);
    //The rest is handled by the errorhandling in player
}

void Playlist::viewInfo()
{
    //If you're selecting the currently playing music
    if(ui->m_tabWidget->currentIndex() == 0 &&
       ui->m_playlistWidget->currentRow() == m_currentIndex)
        m_player->showTagViewer();
    else if (ui->m_tabWidget->currentIndex() == 1 &&
             ui->m_playlistFavWidget->currentRow() == m_currentIndex)
        m_player->showTagViewer();

    else //Let's find a workaround to give you info Qt shouldn't let me get !
    {
        m_tempPlayer = new QMediaPlayer(this);
        connect(m_tempPlayer, SIGNAL(metaDataChanged()), this, SLOT(sendNewInfos()));
        m_tempPlayer->setMuted(true); //Don't let the user know that this ugly code is happening
        if(ui->m_tabWidget->currentIndex() == 0)
            m_tempPlayer->setMedia(m_playlist->media(ui->m_playlistWidget->currentRow()));
        else
            m_tempPlayer->setMedia(m_favPlaylist->media(ui->m_playlistFavWidget->currentRow()));
        m_tempPlayer->stop();
    }

}

void Playlist::sendNewInfos()
{
    QList<QString> metaDatas;
    metaDatas.append(m_tempPlayer->metaData("TrackNumber").toString());
    metaDatas.append(m_tempPlayer->metaData("Title").toString());
    metaDatas.append(m_tempPlayer->metaData("ContributingArtist").toString());
    metaDatas.append(m_tempPlayer->metaData("AlbumTitle").toString());
    metaDatas.append(m_tempPlayer->metaData("Year").toString());
    metaDatas.append(m_tempPlayer->metaData("Genre").toString());

    //Load it now !
    QPixmap coverArt = QPixmap::fromImage(QImage(m_tempPlayer->metaData("ThumbnailImage").value<QImage>()));
    QPointer<TagViewer> TagWindow = new TagViewer(metaDatas, &coverArt, this);
    TagWindow->show();
    m_tempPlayer->deleteLater();
}

//TODO : may rework on it
void Playlist::findItemVisibilityHandler()
{
    if(!m_findItemTrigger->isChecked())
    {
        ui->m_searchLine->setHidden(true);
        this->removeAction(m_validateFind);
        ui->m_playlistWidget->clear();
        for(int i(0); i < m_backupItems.size(); i++)
        {
            ui->m_playlistWidget->addItem(m_backupItems.at(10));

        }
    }

    else
    {
        ui->m_searchLine->setVisible(true);
        this->addAction(m_validateFind);
    }
}

void Playlist::saveTheItems()
{
    ui->m_playlistWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->m_playlistWidget->selectAll();
    QList <QListWidgetItem *> list = ui->m_playlistWidget->selectedItems();
    for(int i(0); i < list.size(); i++)
    {
        m_backupItems.append(list.at(i));
    }
}

void Playlist::validateSearch()
{
    saveTheItems();
    QList <QListWidgetItem*> itemsReturned = ui->m_playlistWidget->findItems(
                ui->m_searchLine->text(), Qt::MatchContains);
    ui->m_playlistWidget->clear();
    for(int i(0); i < itemsReturned.size(); i++)
    {
        ui->m_playlistWidget->addItem(itemsReturned.at(i)->text());
        qDebug() << itemsReturned.at(i)->text();
    }
    qDebug() << itemsReturned.size() << m_backupItems.size();
}

void Playlist::closeEvent(QCloseEvent *)
{
    m_player->setPlaylistOpen(false);
    m_settings.setValue("Playlist/size", this->size());
}

Playlist::~Playlist()
{
    delete ui;
    m_player = nullptr;
    m_playlist = nullptr;
    m_favPlaylist = nullptr;
}
