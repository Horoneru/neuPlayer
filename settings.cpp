#include "settings.h"
#include "ui_settings.h"

Settings::Settings(Player *Player, QWidget *parent) :
    QDialog(parent), m_isNewPath(false), m_isUpdateHandlerAlreadyCalled(false), m_previousTabId(0),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);


    //Make the Player received as an attribute so we can use it
    m_passerelle = Player;
    m_moveAnim.setDuration(MoveAnimation::Fast); //Prepare animation
    //Restore current config from QSettings
    m_settings = new QSettings("neuPlayer.ini", QSettings::IniFormat, this);
    m_settings->beginGroup("Additional_Features");
    ui->m_libraryAtStartupActivate->setChecked(
                m_settings->value("libraryAtStartup", false).toBool());
    ui->m_refreshWhenNeededActivate->setChecked(
                m_settings->value("refreshWhenNeeded", true).toBool());
    ui->m_staticLibraryActivate->setChecked(
                m_settings->value("staticLibrary", false).toBool());
    ui->m_saveIndexActivate->setChecked(
                m_settings->value("saveTrackIndex", true).toBool());
    ui->m_framelessCheck->setChecked(
                m_settings->value("framelessWindow", false).toBool());
    ui->m_fadeCheck->setChecked(
                m_settings->value("audioFade", false).toBool());
    m_settings->endGroup();

    m_bgPath = m_settings->value("customimage", "").toString();
    m_opacityValue = m_settings->value("opacity", 100).toReal();
    ui->m_opacitySlide->setValue(m_opacityValue * 100);
    on_fadeClicked(ui->m_fadeCheck->isChecked());
    updateFadeValue(m_settings->value("fadeValue", 1).toInt() / 1000);
    ui->m_fadeSlide->setValue(m_settings->value("fadeValue", 1).toInt() / 1000);
    ui->m_valueSlide->setText(QString::number(ui->m_opacitySlide->value()) + "%");
    ui->groupBox_5->setVisible(false); //Not ready for prime time (yet?)
    //Process config into UI
    setupConfig();
    setupConnections();
}

                /* Setup Section */
void Settings::setupConfig()
{
    ui->m_playlistAtStartupCheck->setChecked(
                m_settings->value("playlistAtStartup", false).toBool());
    ui->m_skinPick->setCurrentIndex(m_settings->value("skin", 1).toInt());
    setSkin(ui->m_skinPick->currentIndex());
    m_isDynamicLibChecked = ui->m_refreshWhenNeededActivate->isChecked();
    m_isLibraryAtStartchecked = ui->m_libraryAtStartupActivate->isChecked();
    m_isStaticLibChecked = ui->m_staticLibraryActivate->isChecked();
    ui->m_pathView->setText(m_settings->value("mediapath", "").toString());
    ui->m_pathView->setToolTip(ui->m_pathView->text());
    if(ui->m_libraryAtStartupActivate->isChecked())
    {
        m_isLibraryAtStartchecked = true;
        ui->label_8->setHidden(false);
        ui->m_refreshWhenNeededActivate->setHidden(false);
        ui->m_staticLibraryActivate->setHidden(false);
        ui->m_saveIndexActivate->setHidden(false);
        ui->label_13->setHidden(false);
        ui->label_9->setHidden(false);
    }

    else
    {
        m_isLibraryAtStartchecked = false;
        ui->label_8->setHidden(true);
        ui->m_refreshWhenNeededActivate->setHidden(true);
        ui->m_staticLibraryActivate->setHidden(true);
        ui->label_9->setHidden(true);
        ui->m_saveIndexActivate->setHidden(true);
        ui->label_13->setHidden(true);
    }
}

void Settings::setupConnections()
{
    /* Connexions */
    connect(ui->m_libraryAtStartupActivate, SIGNAL(clicked()),
            this, SLOT(enableLibraryAtStartup()));
    connect(ui->m_staticLibraryActivate, SIGNAL(clicked()),
            this, SLOT(on_staticLibActivated()));
    connect(ui->m_refreshWhenNeededActivate, SIGNAL(clicked()),
            this, SLOT(on_RNlibActivated()));
    connect(ui->m_fadeCheck, SIGNAL(clicked(bool)), this, SLOT(on_fadeClicked(bool)));
    connect(ui->m_skinPick, SIGNAL(currentIndexChanged(int)), this, SLOT(setSkin(int)));
    connect(ui->m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(on_changeTab(int)));
    connect(ui->m_changeDbpush, SIGNAL(clicked()), this, SLOT(changeMusicPath()));
    connect(ui->m_close_2, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(ui->m_close, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(ui->m_close_3, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(ui->m_refreshNeededpush, SIGNAL(clicked()), this, SLOT(setLibrary()));
    connect(ui->m_changeBackgroundPush, SIGNAL(clicked()), this, SLOT(changeBg()));
    connect(ui->m_reloadDefaultSkinPush, SIGNAL(clicked()), this, SLOT(reloadDefaultBg()));
    connect(ui->m_opacitySlide, SIGNAL(valueChanged(int)), this, SLOT(updateOpacity(int)));
    connect(ui->m_fadeSlide, SIGNAL(valueChanged(int)), this, SLOT(updateFadeValue(int)));
    connect(ui->m_confirm, SIGNAL(clicked()), this, SLOT(confirm()));
    connect(ui->m_confirm_2, SIGNAL(clicked()), this, SLOT(confirm()));
    connect(ui->m_confirm_3, SIGNAL(clicked()), this, SLOT(confirm()));
    connect(ui->m_framelessCheck, SIGNAL(clicked()), this, SLOT(popupFramelessWindow()));
    connect(ui->m_gotoWorkingDir, SIGNAL(clicked()), this, SLOT(gotoWorkingDir()));
    connect(ui->m_checkUpdatesBtn, SIGNAL(clicked()), this, SLOT(checkUpdates()));
    connect(ui->m_issueLink, SIGNAL(clicked()), this, SLOT(gotoIssues()));
}


                /* Library Section */
void Settings::enableLibraryAtStartup()
{
    if(ui->m_libraryAtStartupActivate->isChecked())
    {
        if(m_settings->value("mediapath", "").toString().isEmpty())
            changeMusicPath();
        //Re-check if user set something
        if(m_settings->value("mediapath", "").toString().isEmpty())
            return;
        m_isLibraryAtStartchecked = true;
        ui->label_8->setHidden(false);
        ui->m_refreshWhenNeededActivate->setHidden(false);
        ui->m_refreshWhenNeededActivate->setChecked(true);
        ui->m_staticLibraryActivate->setHidden(false);
        ui->m_saveIndexActivate->setHidden(false);
        ui->label_13->setHidden(false);
        ui->label_9->setHidden(false);
        ui->m_refreshNeededpush->setHidden(false);
        ui->m_changeDbpush->setHidden(false);

    }
    else if(!ui->m_libraryAtStartupActivate->isChecked())
    {
        m_isLibraryAtStartchecked = false;
        ui->label_8->setHidden(true);
        ui->m_refreshWhenNeededActivate->setHidden(true);
        ui->label_9->setHidden(true);
        ui->m_saveIndexActivate->setHidden(true);
        ui->label_13->setHidden(true);
        ui->m_refreshNeededpush->setHidden(true);
        ui->m_changeDbpush->setHidden(true);
    }
}

void Settings::on_RNlibActivated()
{
    if(ui->m_refreshWhenNeededActivate->isChecked())
    {
        ui->m_staticLibraryActivate->setChecked(false);
        m_isStaticLibChecked = false;
        m_isDynamicLibChecked = true;
    }
    else
        m_isDynamicLibChecked = false;
}

void Settings::on_staticLibActivated()
{
    if(ui->m_staticLibraryActivate->isChecked())
    {
        ui->m_refreshWhenNeededActivate->setChecked(false);
        m_isDynamicLibChecked = false;
        m_isStaticLibChecked = true;
    }
    else
        m_isStaticLibChecked = false;
}

void Settings::popupFramelessWindow()
{
    if(ui->m_framelessCheck->isChecked())
    {
        QMessageBox msgbox(QMessageBox::NoIcon, tr("neuPlayer"),
                           tr("Vous devez redémarrer pour utiliser le player sans bordures"));
        msgbox.setWindowFlags(Qt::WindowStaysOnTopHint);
        msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::Ignore);
        msgbox.setButtonText(QMessageBox::Yes, tr("Redémarrer"));
        msgbox.setButtonText(QMessageBox::Ignore, tr("Plus tard"));
        int result = msgbox.exec();
        if(result == QMessageBox::Yes)
        {
            confirm();
            m_passerelle->saveBeforeClosing();
            qApp->quit();
            QProcess::startDetached("neuPlayer.exe");
        }
    }
    else
    {
        QMessageBox msgbox(QMessageBox::NoIcon, tr("neuPlayer"),
                           tr("Vous devez redémarrer pour utiliser le player avec ses bordures"));
        msgbox.setWindowFlags(Qt::WindowStaysOnTopHint);
        msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::Ignore);
        msgbox.setButtonText(QMessageBox::Yes, tr("Redémarrer"));
        msgbox.setButtonText(QMessageBox::Ignore, tr("Plus tard"));
        msgbox.raise();
        int result = msgbox.exec();
        if(result == QMessageBox::Yes)
        {
            confirm();
            m_passerelle->restart();
        }
    }
}

void Settings::on_changeTab(int tabId)
{
    m_moveAnim.setTarget(ui->m_tabWidget->currentWidget());
    if(tabId < m_previousTabId)
        m_moveAnim.setDirection(MoveAnimation::RightToLeft);
    else
        m_moveAnim.setDirection(MoveAnimation::LeftToRight);
    m_moveAnim.start();
    m_previousTabId = tabId;
}

void Settings::changeMusicPath()
{
    QString path = QFileDialog::getExistingDirectory(this,
                                                     tr("Selectionnez votre répertoire de musique"),
                                                     "",
                                                     QFileDialog::DontResolveSymlinks);
    if (path.isEmpty())
        return;
    else
    {
        if(path == ui->m_pathView->text())
            m_isNewPath = false;
        else
        {
            m_isNewPath = true;
            m_settings->setValue("mediapath", path);
            if(m_isLibraryAtStartchecked &&
              (m_isDynamicLibChecked || m_isStaticLibChecked))
            {
                m_settings->setValue("currentTrack", 0);
                m_settings->setValue("trackPosition", 0);
            }
        }
        ui->m_pathView->setText(path);
        ui->m_pathView->setToolTip(path);
        //If the user didn't check but selected something...
        if(!path.isEmpty() && !ui->m_libraryAtStartupActivate->isChecked())
        {
            ui->m_libraryAtStartupActivate->setChecked(true); //...Check the library at startup and...
            enableLibraryAtStartup(); //...Enable it
        }
    }
}

                    /* Skin Section */

void Settings::setSkin(int index)
{
    if (index == 0) //Clean Fusion
    {
        ui->m_skinImage->setPixmap(QPixmap(":/Ressources/skinpreviewlight.png"));
        if(m_bgPath.isEmpty())
            ui->m_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewwhite.jpg"));
        else
            ui->m_skinImage_2->setPixmap(QPixmap(m_bgPath));

        ui->m_SkinDescription->setText("Skin light pout neuPlayer");
    }

    if (index == 1) // Holo Fusion
    {
        ui->m_skinImage->setPixmap(QPixmap(":/Ressources/skinpreviewdark.png"));
        if(m_bgPath.isEmpty())
            ui->m_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewdark.png"));
        else
            ui->m_skinImage_2->setPixmap(QPixmap(m_bgPath));

        ui->m_SkinDescription->setText("Skin dark pour neuPlayer");
    }
    if(index == 2) //Sky Fusion
    {
        ui->m_skinImage->setPixmap(QPixmap(":/Ressources/skinpreviewcustomlight.jpg"));
        if(m_bgPath.isEmpty())
            ui->m_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewcustomwhite.jpg"));
        else
            ui->m_skinImage_2->setPixmap(QPixmap(m_bgPath));

        ui->m_SkinDescription->setText("Skin light 2 pour neuPlayer");
    }
    if(index == 3) //Night Fusion
    {
        ui->m_skinImage->setPixmap(QPixmap(":/Ressources/skinpreviewcustomdark.png"));
        if(m_bgPath.isEmpty())
            ui->m_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewcustomdark.jpg"));
        else
            ui->m_skinImage_2->setPixmap(QPixmap(m_bgPath));

        ui->m_SkinDescription->setText("Skin dark 2 pour neuPlayer");
    }
}


void Settings::changeBg()
{
    QString tempPath = QFileDialog::getOpenFileName(this,
                                                    tr("Sélectionnez un fond pour le player"),
                                                    QString(),
                                                    tr("Images (*.jpg *.png)"));
    if(!tempPath.isEmpty())
    {
        m_bgPath = tempPath;
        ui->m_skinImage_2->setPixmap(QPixmap(m_bgPath));
    }
}


void Settings::reloadDefaultBg()
{
    m_bgPath = "";
    if (ui->m_skinPick->currentIndex() == 0) //Clean Fusion
        ui->m_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewwhite.jpg"));

    if (ui->m_skinPick->currentIndex() == 1) // Holo Fusion
        ui->m_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewdark.png"));

    if(ui->m_skinPick->currentIndex() == 2) //Sky Fusion
        ui->m_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewcustomwhite.jpg"));

    if(ui->m_skinPick->currentIndex() == 3) //Night Fusion
        ui->m_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewcustomdark.jpg"));
}

void Settings::updateOpacity(int value)
{
    m_opacityValue = value / 100.0;
    ui->m_valueSlide->setText(QString::number(value) + "%");
    m_passerelle->setOpacity(m_opacityValue);
}

void Settings::confirm()
{
        /* Library Section */
    if(m_isLibraryAtStartchecked)
    {
        if(m_isStaticLibChecked)
        {
            m_settings->setValue("Additional_Features/staticLibrary", true);
            if(m_isNewPath)
            /* Now saving playlist ! */
            setLibrary();
        }
        else if(!m_isStaticLibChecked)
        {
            m_settings->setValue("Additional_Features/staticLibrary", false);
        }
        if(m_isDynamicLibChecked)
        {
            m_settings->setValue("Additional_Features/refreshWhenNeeded", true);
            if(m_isNewPath)
                setLibrary();

            QFileInfo info (ui->m_pathView->text());
            qDebug() << info.lastModified().toMSecsSinceEpoch();
            m_settings->setValue("libModified",
                                 info.lastModified().toMSecsSinceEpoch());
        }
        else if(!m_isDynamicLibChecked)
        {
            m_settings->setValue("Additional_Features/refreshWhenNeeded", false);
        }
        if(ui->m_saveIndexActivate->isChecked())
            m_settings->setValue("Additional_Features/saveTrackIndex", true);

        else if (!ui->m_saveIndexActivate->isChecked())
            m_settings->setValue("Additional_Features/saveTrackIndex", false);
        m_settings->setValue("Additional_Features/libraryAtStartup", true);

    }
    else if (!m_isLibraryAtStartchecked) {
        m_settings->setValue("Additional_Features/libraryAtStartup", false);
    }
        /* Other Plugins */
    if(ui->m_playlistAtStartupCheck->isChecked())
        m_settings->setValue("playlistAtStartup", true);
    else
        m_settings->setValue("playlistAtStartup", false);
    if(ui->m_framelessCheck->isChecked())
        m_settings->setValue("Additional_Features/framelessWindow", true);
    else
        m_settings->setValue("Additional_Features/framelessWindow", false);

    /* Skin section */
    int currentSkin = m_settings->value("skin").toInt(); //Backup to test
    //Si un élément de skin a changé
    if(currentSkin != ui->m_skinPick->currentIndex() ||
       m_settings->value("customimage").toString() != m_bgPath )
    {
        m_settings->setValue("skin", ui->m_skinPick->currentIndex());
        m_settings->setValue("customimage", m_bgPath);
        Skin skin(ui->m_skinPick->currentIndex(), this);
        skin.load();
        m_passerelle->loadSkin();
        m_passerelle->update();
    }
    m_settings->setValue("opacity", m_opacityValue);
    m_settings->setValue("Additional_Features/audioFade", ui->m_fadeCheck->isChecked());
    m_settings->setValue("fadeValue", m_fadeValue);
    m_passerelle->setAudioFade(ui->m_fadeCheck->isChecked());
    m_passerelle->setAudioFadeValue(m_fadeValue);
    this->cancel();
}

void Settings::cancel()
{
    m_passerelle->setOpacity(m_settings->value("opacity").toReal());
    close();
}

void Settings::setLibrary()
{
    if(!ui->m_pathView->text().isEmpty())
    {
        ui->m_confirm->setText(tr("Sauvegarde..."));
        neuPlaylist playlist(this);
        QList <QUrl> medias = playlist.setLibrary(ui->m_pathView->text());
        if(!medias.isEmpty())
            m_passerelle->updatePlaylistOfThePlayer(medias);
        ui->m_confirm->setText ("OK");
        m_settings->setValue("trackPosition", 0);
     }
     else
     {
        m_settings->setValue("Additional_Features/staticLibrary", false);
        m_settings->setValue("Additional_Features/refreshWhenNeeded", false);
     }
}

void Settings::gotoWorkingDir()
{
    //Cross-platform solution, always works.
    QDesktopServices::openUrl(QUrl::fromLocalFile(""));
}

void Settings::checkUpdates()
{
    if(!m_isUpdateHandlerAlreadyCalled)
    {
        m_handler = new UpdaterHandler(nullptr);
        m_isUpdateHandlerAlreadyCalled = true;
    }
    m_handler->start("neuPlayer", QApplication::applicationVersion(),
                     "http://sd-2.archive-host.com/membres/up/16630996856616518/version.txt",
                     "http://sd-2.archive-host.com/membres/up/16630996856616518/neuPlayer.exe",
                     "show");
}

void Settings::on_fadeClicked(bool enabled)
{
    ui->m_fadeSlide->setVisible(enabled); ui->m_valueFadeSlide->setVisible(enabled);
}

void Settings::updateFadeValue(int value)
{
    m_fadeValue = value * 1000;
    if(value == 1)
        ui->m_valueFadeSlide->setText(QString::number(value) + " sec");
    else
        ui->m_valueFadeSlide->setText(QString::number(value) + " secs");
}

void Settings::gotoIssues()
{
    QDesktopServices::openUrl(QUrl("https://github.com/Horoneru/neuPlayer/issues"));
}

Settings::~Settings()
{
    m_passerelle->setSettingsOpen(false);
    m_passerelle = nullptr;
    delete ui;
    delete m_settings;
    m_settings = nullptr;
}
