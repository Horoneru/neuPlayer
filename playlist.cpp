#include "playlist.h"
#include "ui_playlist.h"
#include <QFileDialog>
#include <QDirIterator>

Playlist::Playlist(neuPlaylist *liste, int index, Player *player, QPixmap *cover, QString title, bool playingState, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Playlist), a_isReload (false) //That bool can result in unexpected behaviour if it's not initialized
{
    ui->setupUi(this);
    ui->a_searchLine->setHidden(true);
    /*!
      TODO :
       #Ajouter un addtoFav au contextmenu et implanter des onglets différents playlist/favoris << Soon after some internal optimizations overall
    */
    this->setAcceptDrops(true);
    ui->a_playlistWidget->setDragEnabled(false); //I can't handle it well as of now :/
    a_settings = new QSettings("neuPlayer.ini", QSettings::IniFormat, this);
    a_defaultCover = new QPixmap(":/Ressources/noCoverHeader.png");
    //On reçoit le player, il faut maintenant le mettre en tant qu'attribut pour l'utiliser partout
    a_player = player;
    //De même, on met la playlist en tant qu'attribut pour l'utiliser plus tard.
    a_playlist = liste;
    a_previousIndex = 99999; //Pretty sure nobody would reach that value
    a_queueIndex = 0; //used to determine where to place the song you want to queue
    setupActions();

    //Setup list

    updateList(liste);

    if(!liste->isEmpty()) //If it's okay
    {
        ui->a_playlistWidget->scrollToItem(ui->a_playlistWidget->item(index));
        setCurrentItem(index, cover, title);
        //Update the icon if the player is paused or stopped
        if(!playingState)
            setToPaused(index);
    }
    else //If not, shows there's nothing
    {
        ui->a_playlistWidget->addItem("Aucune musique");
        ui->a_playlistWidget->item(0)->setTextAlignment(Qt::AlignCenter);
    }

    setupConnections();

    setupMode();
}

void Playlist::setupActions()
{
    //Actions & Playlist Menu
    a_selectOtherDir = new QAction(tr("Sélectionner un dossier à lire"), this);
    a_selectOtherDir->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_O);
    a_reloadLibrary = new QAction(tr("Recharger la bibliothèque"), this);
    a_reloadLibrary->setShortcut(Qt::Key_F5);
    a_playlistMenu = new QMenu(this);
    a_playlistMenu->addAction(a_selectOtherDir);
    a_playlistMenu->addAction(a_reloadLibrary);
    ui->a_tools->setMenu(a_playlistMenu);
    //Actions & Context menu
    ui->a_playlistWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    a_findItemTrigger = new QAction(this);
    a_findItemTrigger->setShortcut(Qt::CTRL + Qt::Key_F);
    a_findItemTrigger->setCheckable(true);
    a_deleteItemFromQueue = new QAction(tr("Supprimer de la file d'attente"), this);
    a_deleteItemFromQueue->setShortcut(Qt::Key_Delete);
    a_addToQueue = new QAction(tr("Ajouter à la file d'attente"), this);
    a_addToQueue->setShortcut(Qt::CTRL + Qt::Key_Plus);
    a_viewInfo = new QAction(tr("Voir les informations"), this);
    a_viewInfo->setShortcut(Qt::ALT + Qt::Key_Return);
    a_easyHideFind = new QAction(this);
    a_easyHideFind->setShortcut(Qt::Key_Escape);
    a_validateFind = new QAction(this);
    a_validateFind->setShortcut(Qt::Key_Return); //Action so I don't bother with events to validate

    this->addAction(a_addToQueue);
    this->addAction(a_findItemTrigger);
    this->addAction(a_viewInfo);
    this->addAction(a_deleteItemFromQueue);
    //Setup menu
    a_playlistContextMenu = new QMenu(this);
    a_playlistContextMenu->addAction(a_addToQueue);
    a_playlistContextMenu->addAction(a_deleteItemFromQueue);
    a_playlistContextMenu->addAction(a_viewInfo);
}


void Playlist::setupConnections()
{
    /* Connexions */
    connect(a_selectOtherDir, SIGNAL(triggered()), this, SLOT(setFolder()));
    connect(a_reloadLibrary, SIGNAL(triggered()), this, SLOT(reloadLibrary()));
    connect(ui->a_switchRandomPlay, SIGNAL(clicked()), this, SLOT(randomModeChanger()));
    connect(ui->a_switchLoopPlay, SIGNAL(clicked()), this, SLOT(loopModeChanger()));
    connect(ui->a_playlistWidget, SIGNAL(activated(QModelIndex)), this, SLOT(playItem(QModelIndex)));
    //This one below is commented out because it doesn't work yet. Need help as to why it crashes
//    connect(a_findItemTrigger, SIGNAL(triggered()), this, SLOT(findItemVisibilityHandler()));
    connect(ui->a_playlistWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu()));
    connect(a_playlistContextMenu, SIGNAL(aboutToShow()), this, SLOT(prepareContextMenu()));
    connect(a_deleteItemFromQueue, SIGNAL(triggered()), this, SLOT(deleteItem()));
    connect(a_addToQueue, SIGNAL(triggered()), this, SLOT(addItemToQueue()));
    connect(a_viewInfo, SIGNAL(triggered()), this, SLOT(viewInfo()));
    connect(ui->a_titleHeader, SIGNAL(clicked()), this, SLOT(scrollToPlaying()));
    connect(a_validateFind, SIGNAL(triggered()), this, SLOT(validateSearch()));
}

void Playlist::setupMode()
{
    //Random mode setup
    bool isRandomMode = a_player->getRandomState();
    if(isRandomMode)
        ui->a_switchRandomPlay->setChecked(true);
    else
        ui->a_switchRandomPlay->setChecked(false);

    //Loop mode setup
    bool isLoopingPlaylistMode = a_player->getLoopState();
    if(isLoopingPlaylistMode)
        ui->a_switchLoopPlay->setChecked(true);
    else
        ui->a_switchLoopPlay->setChecked(false);
}

        /* Update section */

void Playlist::updateList(neuPlaylist*listeFichiers)
{
    ui->a_playlistWidget->clear();
    int const mediaCount = listeFichiers->mediaCount();
    for(int i(0); i < mediaCount; i++)
    {
        QString filepath = listeFichiers->media(i).canonicalUrl().toString();
        ui->a_playlistWidget->addItem(QFileInfo(filepath).fileName());
        ui->a_playlistWidget->item(i)->setData(Qt::ToolTipRole, filepath);
        ui->a_playlistWidget->item(i)->setData(Qt::WhatsThisRole, i);
    }

}

void Playlist::quickUpdate(QList<QUrl> *items, int currentItemPlusOne)
{
    unsigned int const numberItems = items->size();
    for(int i(0); i < numberItems; i++, currentItemPlusOne++)
    {
        ui->a_playlistWidget->insertItem(currentItemPlusOne, items->at(i).fileName());
        ui->a_playlistWidget->item(currentItemPlusOne)->setData(Qt::ToolTipRole, QUrl(items->at(i)));
        ui->a_playlistWidget->item(currentItemPlusOne)->setData(Qt::WhatsThisRole, currentItemPlusOne);
    }
}
        /* State section */
//Those are handled by the player

void::Playlist::setToPlaying(int index)
{
    ui->a_playlistWidget->item(index)->setIcon(QIcon(":/Ressources/playingState_icon.png"));
    a_isPlaying = true;
}


void Playlist::setToPaused(int index)
{
   ui->a_playlistWidget->item(index)->setIcon(QIcon(":/Ressources/pausedState-icon2.png"));
   a_isPlaying = false;
}


void Playlist::setCurrentItem(int index, QPixmap *cover, QString title)
{
    if(a_previousIndex != 99999) //Default value of a_previousIndex
    {
        ui->a_playlistWidget->item(a_previousIndex)->setIcon(QIcon());
    }
    a_queueIndex = 0;
    a_previousIndex = index;
    setToPlaying(index);
    a_currentIndex = index;
    ui->a_playlistWidget->setCurrentRow(index);
    //Use data received
    ui->a_cover->setPixmap(*cover);
    if(ui->a_cover->pixmap()->isNull())
        ui->a_cover->setPixmap(*a_defaultCover);
    ui->a_titleHeader->setText(title);
    ui->a_titleHeader->setToolTip(title);
    if(!a_player->deleteTriggered())
    {
        fadeAnimManager animManager(this);
        animManager.addTarget(ui->a_cover, fadeAnimManager::FadeIn, 500, fadeAnimManager::Parallel);
        animManager.addTarget(ui->a_titleHeader, fadeAnimManager::FadeIn, 350, fadeAnimManager::Parallel);
        animManager.startGroup(fadeAnimManager::Parallel, true);
    }
}

        /* Playlist functionality */

void Playlist::setFolder()
{
    QString selectDir;
    // On sélectionne le répertoire à partir duquel on va rechercher les fichiers que le player peut lire
    if(!a_isReload)
        selectDir = (QFileDialog::getExistingDirectory (this,tr("Ouvrir un répertoire"), "", QFileDialog::DontResolveSymlinks));
    else if(!a_settings->value("mediapath").toString().isEmpty())
        selectDir =  a_settings->value("mediapath", "").toString();

    if(!selectDir.isEmpty())
    {
        if(a_currentIndex != 0) //Helps not to crash. Each index is reset to 0 so there isn't any out of bound.
        {
            a_player->setIndexOfThePlayer(0, false);
            ui->a_playlistWidget->setCurrentRow(0);
        }
        QList <QUrl> urlList = a_playlist->update(selectDir);
        if(!urlList.empty()) //Don't set anything if there wasn't any content
            a_player->updatePlaylistOfThePlayer(urlList, true);
     }

}

//Proxy method that uses setFolder();
void Playlist::reloadLibrary()
{
    a_isReload = true;
    setFolder();
    a_isReload = false;
}

void Playlist::randomModeChanger()
{
    if(ui->a_switchRandomPlay->isChecked())
        setShuffle();
    else
        setSequential();
    this->setFocus(); //évite d'avoir le focus sur le bouton
}

void Playlist::loopModeChanger()
{
    if(ui->a_switchLoopPlay->isChecked())
        a_player->setLoopMode(true);
    else
        a_player->setLoopMode(false);
    this->setFocus();
}

void Playlist::setSequential()
{
    a_player->setRandomMode(false); // 0 = No random = Sequential
}

void Playlist::setShuffle()
{
    a_player->setRandomMode(true); // 1 = random
}

void Playlist::playItem(QModelIndex itemIndex)
{
    while(!a_player->canChangeMusic())
        QApplication::processEvents(QEventLoop::AllEvents);
    if(a_previousIndex != 99999)
    {
        ui->a_playlistWidget->item(a_previousIndex)->setIcon(QIcon(":/Ressources/blank.png)"));
    }
    a_previousIndex = itemIndex.row();
    if(ui->a_playlistWidget->currentItem()->textAlignment() == Qt::AlignCenter)
    {
        setFolder();
        return;
    }
    setToPlaying(itemIndex.row());
    a_currentIndex = itemIndex.row();
    a_player->setIndexOfThePlayer(itemIndex.row(), true);
}

void Playlist::prepareContextMenu()
{
    if(a_currentIndex != ui->a_playlistWidget->currentRow())
    {
        a_playlistContextMenu->actions().at(2)->setVisible(false);
        a_playlistContextMenu->actions().at(1)->setVisible(true);
        a_playlistContextMenu->actions().at(0)->setVisible(true);
    }
    else
    {
        a_playlistContextMenu->actions().at(2)->setVisible(true);
        a_playlistContextMenu->actions().at(1)->setVisible(false);
        a_playlistContextMenu->actions().at(0)->setVisible(false);
    }
    if(ui->a_playlistWidget->currentItem()->textAlignment() == Qt::AlignCenter) //if no music...
    {
        a_playlistContextMenu->actions().at(2)->setVisible(false);
        a_playlistContextMenu->actions().at(1)->setVisible(false);
        a_playlistContextMenu->actions().at(0)->setVisible(false);
    }

}

//Activated by the clicked() signal of the a_titleHeader UI component
void Playlist::scrollToPlaying()
{
    ui->a_playlistWidget->setCurrentRow(a_currentIndex);
    ui->a_playlistWidget->scrollTo(ui->a_playlistWidget->currentIndex());
}

void Playlist::showContextMenu() const
{
    a_playlistContextMenu->exec(QCursor::pos());
}

void Playlist::addItemToQueue()
{
    a_player->addToQueue(ui->a_playlistWidget->currentRow(), a_currentIndex);
}

void Playlist::deleteItem()
{
    if(a_currentIndex != ui->a_playlistWidget->currentRow()) //Who knows, might be called by shortcut and BAM
    {
        int index = ui->a_playlistWidget->currentRow();
        a_player->deleteMedia(index);
        ui->a_playlistWidget->takeItem(index);
    }
}

void Playlist::viewInfo()
{
    scrollToPlaying();
    a_player->showTagViewer();
}

void Playlist::findItemVisibilityHandler()
{
    if(!a_findItemTrigger->isChecked())
    {
        qDebug() << "I leave";
        ui->a_searchLine->setHidden(true);
        this->removeAction(a_validateFind);
        ui->a_playlistWidget->clear();
        for(int i(0); i < a_backupItems.size(); i++)
        {
            ui->a_playlistWidget->addItem(a_backupItems.at(10));

        }
    }

    else
    {
        qDebug() << "I enter";
        ui->a_searchLine->setVisible(true);
        this->addAction(a_validateFind);
    }
}

void Playlist::saveTheItems()
{
    ui->a_playlistWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->a_playlistWidget->selectAll();
    QList <QListWidgetItem *> list = ui->a_playlistWidget->selectedItems();
    for(int i(0); i < list.size(); i++)
    {
        a_backupItems.append(list.at(i));
    }
}

void Playlist::validateSearch()
{
    saveTheItems();
    QList <QListWidgetItem*> itemsReturned = ui->a_playlistWidget->findItems(ui->a_searchLine->text(), Qt::MatchContains);
    ui->a_playlistWidget->clear();
    for(int i(0); i < itemsReturned.size(); i++)
    {
        ui->a_playlistWidget->addItem(itemsReturned.at(i)->text());
        qDebug() << itemsReturned.at(i)->text();
    }
    qDebug() << itemsReturned.size() << a_backupItems.size();
}

void Playlist::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
            event->acceptProposedAction();
}

void Playlist::dropEvent(QDropEvent *event)
{
    this->setCursor(Qt::BusyCursor);
    a_player->addMediasToThePlayer(event->mimeData()->urls());
    //Should be finished then
    this->setCursor(Qt::ArrowCursor);
    a_player->setPlaylistToBeSavedOrNot(true);
}

void Playlist::closeEvent(QCloseEvent *)
{
    a_player->setPlaylistOpen(false);
    this->deleteLater();
}

Playlist::~Playlist()
{
    delete ui;
}
