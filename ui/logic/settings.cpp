#include "settings.h"
#include "ui_settings.h"

Settings::Settings(Player *Player, QWidget *parent) :
    QDialog(parent), m_isNewPath(false),
    m_isUpdateHandlerAlreadyCalled(false), m_previousTabId(0),
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
    ui->libraryAtStartupActivate->setChecked(
                m_settings->value("libraryAtStartup", false).toBool());
    ui->refreshWhenNeededActivate->setChecked(
                m_settings->value("refreshWhenNeeded", true).toBool());
    ui->staticLibraryActivate->setChecked(
                m_settings->value("staticLibrary", false).toBool());
    ui->saveIndexActivate->setChecked(
                m_settings->value("saveTrackIndex", true).toBool());
    ui->framelessCheck->setChecked(
                m_settings->value("framelessWindow", false).toBool());
    ui->fadeCheck->setChecked(m_settings->value("audioFade", false).toBool());
    m_settings->endGroup();
    m_bgPath = m_settings->value("customimage", "").toString();
    m_opacityValue = m_settings->value("opacity", 100).toReal();
    ui->opacitySlide->setValue(m_opacityValue * 100);

    on_fadeClicked(ui->fadeCheck->isChecked());
    updateFadeValue(m_settings->value("fadeValue", 1).toInt() / 1000);
    ui->fadeSlide->setValue(m_settings->value("fadeValue", 1).toInt() / 1000);
    ui->valueSlide->setText(QString::number(ui->opacitySlide->value()) + "%");
    //Process config into UI
    setupConfig();
    setupConnections();
    ui->versionLabel->setText("v" +
                              QApplication::applicationVersion());
}
                /* Setup Section */
void Settings::setupConfig()
{
    ui->playlistAtStartupCheck->setChecked(
                m_settings->value("playlistAtStartup", false).toBool());
    ui->skinPick->setCurrentIndex(m_settings->value("skin", 1).toInt());
    setSkin(ui->skinPick->currentIndex());
    m_isDynamicLibChecked = ui->refreshWhenNeededActivate->isChecked();
    m_isLibraryAtStartchecked = ui->libraryAtStartupActivate->isChecked();
    m_isStaticLibChecked = ui->staticLibraryActivate->isChecked();
    ui->pathView->setText(m_settings->value("mediapath", "").toString());
    ui->pathView->setToolTip(ui->pathView->text());
    if(ui->libraryAtStartupActivate->isChecked())
    {
        m_isLibraryAtStartchecked = true;
        ui->label_8->setHidden(false);
        ui->refreshWhenNeededActivate->setHidden(false);
        ui->staticLibraryActivate->setHidden(false);
        ui->saveIndexActivate->setHidden(false);
        ui->label_13->setHidden(false);
        ui->label_9->setHidden(false);
    }

    else
    {
        m_isLibraryAtStartchecked = false;
        ui->label_8->setHidden(true);
        ui->refreshWhenNeededActivate->setHidden(true);
        ui->staticLibraryActivate->setHidden(true);
        ui->label_9->setHidden(true);
        ui->saveIndexActivate->setHidden(true);
        ui->label_13->setHidden(true);
    }
}

void Settings::setupConnections()
{
    /* Connexions */
    connect(ui->libraryAtStartupActivate, SIGNAL(clicked()),
            this, SLOT(enableLibraryAtStartup()));
    connect(ui->staticLibraryActivate, SIGNAL(clicked()),
            this, SLOT(on_staticLibActivated()));
    connect(ui->refreshWhenNeededActivate, SIGNAL(clicked()),
            this, SLOT(on_RNlibActivated()));
    connect(ui->fadeCheck, SIGNAL(clicked(bool)), this, SLOT(on_fadeClicked(bool)));
    connect(ui->skinPick, SIGNAL(currentIndexChanged(int)), this, SLOT(setSkin(int)));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(on_changeTab(int)));
    connect(ui->changeDbpush, SIGNAL(clicked()), this, SLOT(changeMusicPath()));
    connect(ui->close_2, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(ui->close, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(ui->close_3, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(ui->refreshNeededpush, SIGNAL(clicked()), this, SLOT(setLibrary()));
    connect(ui->changeBackgroundPush, SIGNAL(clicked()), this, SLOT(changeBg()));
    connect(ui->reloadDefaultSkinPush, SIGNAL(clicked()), this, SLOT(reloadDefaultBg()));
    connect(ui->opacitySlide, SIGNAL(valueChanged(int)), this, SLOT(updateOpacity(int)));
    connect(ui->fadeSlide, SIGNAL(valueChanged(int)), this, SLOT(updateFadeValue(int)));
    connect(ui->confirm, SIGNAL(clicked()), this, SLOT(confirm()));
    connect(ui->confirm_2, SIGNAL(clicked()), this, SLOT(confirm()));
    connect(ui->confirm_3, SIGNAL(clicked()), this, SLOT(confirm()));
    connect(ui->framelessCheck, SIGNAL(clicked()), this, SLOT(popupFramelessWindow()));
    connect(ui->gotoWorkingDir, SIGNAL(clicked()), this, SLOT(gotoWorkingDir()));
    connect(ui->checkUpdatesBtn, SIGNAL(clicked()), this, SLOT(checkUpdates()));
    connect(ui->issueLink, SIGNAL(clicked()), this, SLOT(gotoIssues()));
}


                /* Library Section */
void Settings::enableLibraryAtStartup()
{
    if(ui->libraryAtStartupActivate->isChecked())
    {
        if(m_settings->value("mediapath", "").toString().isEmpty())
            changeMusicPath();
        //Re-check if user set something
        if(m_settings->value("mediapath", "").toString().isEmpty())
            return;
        m_isLibraryAtStartchecked = true;
        ui->label_8->setHidden(false);
        ui->refreshWhenNeededActivate->setHidden(false);
        ui->refreshWhenNeededActivate->setChecked(true);
        ui->staticLibraryActivate->setHidden(false);
        ui->saveIndexActivate->setHidden(false);
        ui->label_13->setHidden(false);
        ui->label_9->setHidden(false);
        ui->refreshNeededpush->setHidden(false);
        ui->changeDbpush->setHidden(false);

    }
    else if(!ui->libraryAtStartupActivate->isChecked())
    {
        m_isLibraryAtStartchecked = false;
        ui->label_8->setHidden(true);
        ui->refreshWhenNeededActivate->setHidden(true);
        ui->label_9->setHidden(true);
        ui->saveIndexActivate->setHidden(true);
        ui->label_13->setHidden(true);
        ui->refreshNeededpush->setHidden(true);
        ui->changeDbpush->setHidden(true);
    }
}

void Settings::on_RNlibActivated()
{
    if(ui->refreshWhenNeededActivate->isChecked())
    {
        ui->staticLibraryActivate->setChecked(false);
        m_isStaticLibChecked = false;
        m_isDynamicLibChecked = true;
    }
    else
        m_isDynamicLibChecked = false;
}

void Settings::on_staticLibActivated()
{
    if(ui->staticLibraryActivate->isChecked())
    {
        ui->refreshWhenNeededActivate->setChecked(false);
        m_isDynamicLibChecked = false;
        m_isStaticLibChecked = true;
    }
    else
        m_isStaticLibChecked = false;
}

void Settings::popupFramelessWindow()
{
    if(ui->framelessCheck->isChecked())
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
    m_moveAnim.setTarget(ui->tabWidget->currentWidget());
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
        if(path == ui->pathView->text())
        {
            m_isNewPath = false;
        }
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
        ui->pathView->setText(path);
        ui->pathView->setToolTip(path);
        //If the user didn't check but selected something...
        if(!path.isEmpty() && !ui->libraryAtStartupActivate->isChecked())
        {
            ui->libraryAtStartupActivate->setChecked(true); //...Check the library at startup and...
            enableLibraryAtStartup(); //...Enable it
        }
    }
}

                    /* Skin Section */

void Settings::setSkin(int index)
{
    if (index == 0) //Clean Fusion
    {
        ui->skinImage->setPixmap(QPixmap(":/skinpreviewlight.png"));
        if(m_bgPath.isEmpty())
            ui->skinImage_2->setPixmap(QPixmap(":/backgroundpreviewwhite.jpg"));
        else
            ui->skinImage_2->setPixmap(QPixmap(m_bgPath));

        ui->SkinDescription->setText("Skin light pout neuPlayer");
    }

    if (index == 1) // Holo Fusion
    {
        ui->skinImage->setPixmap(QPixmap(":/skinpreviewdark.png"));
        if(m_bgPath.isEmpty())
            ui->skinImage_2->setPixmap(QPixmap(":/backgroundpreviewdark.png"));
        else
            ui->skinImage_2->setPixmap(QPixmap(m_bgPath));

        ui->SkinDescription->setText("Skin dark pour neuPlayer");
    }
    if(index == 2) //Sky Fusion
    {
        ui->skinImage->setPixmap(QPixmap(":/skinpreviewcustomlight.jpg"));
        if(m_bgPath.isEmpty())
            ui->skinImage_2->setPixmap(QPixmap(":/backgroundpreviewcustomwhite.jpg"));
        else
            ui->skinImage_2->setPixmap(QPixmap(m_bgPath));

        ui->SkinDescription->setText("Skin light 2 pour neuPlayer");
    }

    if(index == 3) //Night Fusion
    {
        ui->skinImage->setPixmap(QPixmap(":/skinpreviewcustomdark.png"));
        if(m_bgPath.isEmpty())
            ui->skinImage_2->setPixmap(QPixmap(":/backgroundpreviewcustomdark.jpg"));
        else
            ui->skinImage_2->setPixmap(QPixmap(m_bgPath));

        ui->SkinDescription->setText("Skin dark 2 pour neuPlayer");
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
        ui->skinImage_2->setPixmap(QPixmap(m_bgPath));
    }
}


void Settings::reloadDefaultBg()
{
    m_bgPath = "";
    if (ui->skinPick->currentIndex() == 0) //Clean Fusion
        ui->skinImage_2->setPixmap(QPixmap(":/backgroundpreviewwhite.jpg"));

    if (ui->skinPick->currentIndex() == 1) // Holo Fusion
        ui->skinImage_2->setPixmap(QPixmap(":/backgroundpreviewdark.png"));

    if(ui->skinPick->currentIndex() == 2) //Sky Fusion
        ui->skinImage_2->setPixmap(QPixmap(":/backgroundpreviewcustomwhite.jpg"));

    if(ui->skinPick->currentIndex() == 3) //Night Fusion
        ui->skinImage_2->setPixmap(QPixmap(":/backgroundpreviewcustomdark.jpg"));
}

void Settings::updateOpacity(int value)
{
    m_opacityValue = value / 100.0;
    ui->valueSlide->setText(QString::number(value) + "%");
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

            QFileInfo info (ui->pathView->text());
            qDebug() << info.lastModified().toMSecsSinceEpoch();
            m_settings->setValue("libModified", info.lastModified().toMSecsSinceEpoch());
        }
        else if(!m_isDynamicLibChecked)
        {
            m_settings->setValue("Additional_Features/refreshWhenNeeded", false);
        }
        if(ui->saveIndexActivate->isChecked())
            m_settings->setValue("Additional_Features/saveTrackIndex", true);

        else if (!ui->saveIndexActivate->isChecked())
            m_settings->setValue("Additional_Features/saveTrackIndex", false);
        m_settings->setValue("Additional_Features/libraryAtStartup", true);

    }
    else if (!m_isLibraryAtStartchecked) {
        m_settings->setValue("Additional_Features/libraryAtStartup", false);
    }
        /* Other Plugins */
    if(ui->playlistAtStartupCheck->isChecked())
        m_settings->setValue("playlistAtStartup", true);
    else
        m_settings->setValue("playlistAtStartup", false);
    if(ui->framelessCheck->isChecked())
        m_settings->setValue("Additional_Features/framelessWindow", true);
    else
        m_settings->setValue("Additional_Features/framelessWindow", false);

    /* Skin section */

    int currentSkin = m_settings->value("skin").toInt(); //Backup to test

    //Si un élément de skin a changé
    if(currentSkin != ui->skinPick->currentIndex() ||
       m_settings->value("customimage").toString() != m_bgPath )
    {
        m_settings->setValue("skin", ui->skinPick->currentIndex());
        m_settings->setValue("customimage", m_bgPath);
        Skin skin(ui->skinPick->currentIndex(), this);
        skin.load();
        m_passerelle->loadSkin();
        m_passerelle->update();
    }
    m_settings->setValue("opacity", m_opacityValue);
    m_settings->setValue("Additional_Features/audioFade", ui->fadeCheck->isChecked());
    m_settings->setValue("fadeValue", m_fadeValue);
    m_passerelle->setAudioFade(ui->fadeCheck->isChecked());
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
    if(!ui->pathView->text().isEmpty())
    {
        ui->confirm->setText(tr("Sauvegarde..."));
        neuPlaylist playlist(this);
        QList <QUrl> medias = playlist.setLibrary(ui->pathView->text());
        if(!medias.isEmpty())
            m_passerelle->updatePlaylistOfThePlayer(medias);
        ui->confirm->setText ("OK");
        m_settings->setValue("trackPosition", 0);
        //The Qt playlist loader will be fooled and will load the playlist smoothly as if it was saved by save();
     }
     else
     {
        m_settings->setValue("Additional_Features/staticLibrary", false);
        m_settings->setValue("Additional_Features/refreshWhenNeeded", false);
     }
}

void Settings::gotoWorkingDir()
{
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
    ui->fadeSlide->setVisible(enabled); ui->valueFadeSlide->setVisible(enabled);
}

void Settings::updateFadeValue(int value)
{
    m_fadeValue = value * 1000;
    if(value == 1)
        ui->valueFadeSlide->setText(QString::number(value) + " sec");
    else
        ui->valueFadeSlide->setText(QString::number(value) + " secs");
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
