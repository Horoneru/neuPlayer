#include "settings.h"
#include "ui_settings.h"

Settings::Settings(Player *Player, QWidget *parent) :
    QDialog(parent), a_isNewPath(false),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    //Make the Player received as an attribute so we can use it
    passerelle = Player;
    //Restore current config from QSettings
    a_settings = new QSettings("neuPlayer.ini", QSettings::IniFormat, this);
    a_settings->beginGroup("Additional_Features");
    ui->a_libraryAtStartupActivate->setChecked(a_settings->value("libraryAtStartup", false).toBool());
    ui->a_refreshWhenNeededActivate->setChecked(a_settings->value("refreshWhenNeeded", true).toBool());
    ui->a_staticLibraryActivate->setChecked(a_settings->value("staticLibrary", false).toBool());
    ui->a_saveIndexActivate->setChecked(a_settings->value("saveTrackIndex", true).toBool());
    ui->a_framelessCheck->setChecked(a_settings->value("framelessWindow", false).toBool());
    a_settings->endGroup();
    a_bgPath = a_settings->value("customimage", "").toString();
    a_opacityValue = a_settings->value("opacity", 100).toReal();
    ui->a_opacitySlide->setValue(a_opacityValue * 100);
    ui->a_valueSlide->setText(QString::number(ui->a_opacitySlide->value()) + "%");
    //Process config into UI
    setupConfig();

    setupConnections();
}
                /* Setup Section */
void Settings::setupConfig()
{
    ui->a_playlistAtStartupCheck->setChecked(a_settings->value("playlistAtStartup", false).toBool());
    ui->a_skinPick->setCurrentIndex(a_settings->value("skin", 1).toInt());
    setSkin(ui->a_skinPick->currentIndex());
    a_isDynamicLibChecked = ui->a_refreshWhenNeededActivate->isChecked();
    a_isLibraryAtStartchecked = ui->a_libraryAtStartupActivate->isChecked();
    a_isStaticLibChecked = ui->a_staticLibraryActivate->isChecked();
    ui->a_pathView->setText(a_settings->value("mediapath", "").toString());
    ui->a_pathView->setToolTip(ui->a_pathView->text());
    if(ui->a_libraryAtStartupActivate->isChecked())
    {
        a_isLibraryAtStartchecked = true;
        ui->label_8->setHidden(false);
        ui->a_refreshWhenNeededActivate->setHidden(false);
        ui->a_staticLibraryActivate->setHidden(false);
        ui->a_saveIndexActivate->setHidden(false);
        ui->label_13->setHidden(false);
        ui->label_9->setHidden(false);
    }

    else
    {
        a_isLibraryAtStartchecked = false;
        ui->label_8->setHidden(true);
        ui->a_refreshWhenNeededActivate->setHidden(true);
        ui->a_staticLibraryActivate->setHidden(true);
        ui->label_9->setHidden(true);
        ui->a_saveIndexActivate->setHidden(true);
        ui->label_13->setHidden(true);
    }
}

void Settings::setupConnections()
{
    /* Connexions */
    connect(ui->a_libraryAtStartupActivate, SIGNAL(clicked()), this, SLOT(enableLibraryAtStartup()));
    connect(ui->a_staticLibraryActivate, SIGNAL(clicked()), this, SLOT(popupStaticlib()));
    connect(ui->a_refreshWhenNeededActivate, SIGNAL(clicked()), this, SLOT(popupRNlib()));
    connect(ui->a_skinPick, SIGNAL(currentIndexChanged(int)), this, SLOT(setSkin(int)));
    connect(ui->a_changeDbpush, SIGNAL(clicked()), this, SLOT(changeMusicPath()));
    connect(ui->a_close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->a_refreshNeededpush, SIGNAL(clicked()), this, SLOT(setLibrary()));
    connect(ui->a_changeBackgroundPush, SIGNAL(clicked()), this, SLOT(changeBg()));
    connect(ui->a_reloadDefaultSkinPush, SIGNAL(clicked()), this, SLOT(reloadDefaultBg()));
    connect(ui->a_opacitySlide, SIGNAL(valueChanged(int)), this, SLOT(updateOpacity(int)));
    connect(ui->a_confirm, SIGNAL(clicked()), this, SLOT(confirm()));
    connect(ui->a_confirm_2, SIGNAL(clicked()), this, SLOT(confirm()));
    connect(ui->a_framelessCheck, SIGNAL(clicked()), this, SLOT(popupFramelessWindow()));
    connect(ui->TEMP, SIGNAL(clicked()), this, SLOT(TEMP()));
}


                /* Library Section */
void Settings::enableLibraryAtStartup()
{
    if(ui->a_libraryAtStartupActivate->isChecked())
    {
        if(a_settings->value("mediapath", "").toString().isEmpty())
            changeMusicPath();
        //Re-check if user set something
        if(a_settings->value("mediapath", "").toString().isEmpty())
            return;
        a_isLibraryAtStartchecked = true;
        ui->label_8->setHidden(false);
        ui->a_refreshWhenNeededActivate->setHidden(false);
        ui->a_refreshWhenNeededActivate->setChecked(true);
        ui->a_staticLibraryActivate->setHidden(false);
        ui->a_saveIndexActivate->setHidden(false);
        ui->label_13->setHidden(false);
        ui->label_9->setHidden(false);
        ui->a_refreshNeededpush->setHidden(false);
        ui->a_changeDbpush->setHidden(false);

    }
    else if(!ui->a_libraryAtStartupActivate->isChecked())
    {
        a_isLibraryAtStartchecked = false;
        ui->label_8->setHidden(true);
        ui->a_refreshWhenNeededActivate->setHidden(true);
        ui->label_9->setHidden(true);
        ui->a_saveIndexActivate->setHidden(true);
        ui->label_13->setHidden(true);
        ui->a_refreshNeededpush->setHidden(true);
        ui->a_changeDbpush->setHidden(true);
    }
}

void Settings::popupRNlib()
{
    if(ui->a_refreshWhenNeededActivate->isChecked())
    {
        ui->a_staticLibraryActivate->setChecked(false);
        a_isStaticLibChecked = false;
        QMessageBox msgbox(QMessageBox::NoIcon, tr("Information sur ce mode"), tr("Ce mode est plus lourd car il nécessite de vérifier à chaque fois si une modification a eu lieu dans le dossier où réside les données musicales, mais il permet d'être tenu au courant lorsque le dossier a été modifié, pour rafraîchir les médias présents"));
        msgbox.exec();
        a_isDynamicLibChecked = true;
        qDebug() << a_isDynamicLibChecked;
    }
    else
        a_isDynamicLibChecked = false;
}

void Settings::popupStaticlib()
{
    if(ui->a_staticLibraryActivate->isChecked())
    {
        ui->a_refreshWhenNeededActivate->setChecked(false);
        a_isDynamicLibChecked = false;
        QMessageBox msgbox(QMessageBox::NoIcon, tr("Information sur ce mode"), tr("Ce mode est plus léger car il ne fait que charger les données musicales. En revanche, lors d'une modification du dossier de données, vous ne serez pas notifié et devrez recharger la playlist"));
        msgbox.exec();
        a_isStaticLibChecked = true;
    }
    else
        a_isStaticLibChecked = false;
}

void Settings::popupFramelessWindow()
{
    if(ui->a_framelessCheck->isChecked())
    {
        QMessageBox msgbox(QMessageBox::NoIcon, tr("neuPlayer"), tr("Vous devez redémarrer pour utiliser le player sans bordures"));
        msgbox.exec();
    }
    else
    {
        QMessageBox msgbox(QMessageBox::NoIcon, tr("neuPlayer"), tr("Vous devez redémarrer pour utiliser le player avec ses bordures"));
        msgbox.exec();
    }
}

void Settings::changeMusicPath()
{
    a_settings->setValue("mediapath", QFileDialog::getExistingDirectory(this, tr("Selectionnez votre répertoire de musique"),"", QFileDialog::DontResolveSymlinks));
    if(a_settings->value("mediapath") == ui->a_pathView->text())
    {
        a_isNewPath = false;
    }
    else
    {
        a_isNewPath = true;
        if(a_isLibraryAtStartchecked && (a_isDynamicLibChecked || a_isStaticLibChecked))
        {
            a_settings->setValue("currentTrack", 0);
            a_settings->setValue("trackPosition", 0);
        }
    }
    ui->a_pathView->setText(a_settings->value("mediapath").toString());
    ui->a_pathView->setToolTip(ui->a_pathView->text());
    if(!ui->a_pathView->text().isEmpty()) //If the user didn't check but selected something, check the library at startup and enable it
        if(!ui->a_libraryAtStartupActivate->isChecked())
        {
            ui->a_libraryAtStartupActivate->setChecked(true);
            enableLibraryAtStartup();
        }
}

                    /* Skin Section */

void Settings::setSkin(int index)
{
    if (index == 0) //Clean Fusion
    {
        ui->a_skinImage->setPixmap(QPixmap(":/Ressources/skinpreviewlight.png"));
        if(a_bgPath.isEmpty())
            ui->a_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewwhite.jpg"));
        else
            ui->a_skinImage_2->setPixmap(QPixmap(a_bgPath));

        ui->a_SkinDescription->setText("Skin light pout neuPlayer");
    }

    if (index == 1) // Holo Fusion
    {
        ui->a_skinImage->setPixmap(QPixmap(":/Ressources/skinpreviewdark.png"));
        if(a_bgPath.isEmpty())
            ui->a_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewdark.png"));
        else
            ui->a_skinImage_2->setPixmap(QPixmap(a_bgPath));

        ui->a_SkinDescription->setText("Skin dark pour neuPlayer");
    }
    if(index == 2) //Sky Fusion
    {
        ui->a_skinImage->setPixmap(QPixmap(":/Ressources/skinpreviewcustomlight.jpg"));
        if(a_bgPath.isEmpty())
            ui->a_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewcustomwhite.jpg"));
        else
            ui->a_skinImage_2->setPixmap(QPixmap(a_bgPath));

        ui->a_SkinDescription->setText("Skin light custom pour neuPlayer");
    }
}


void Settings::changeBg()
{
    QString tempPath = QFileDialog::getOpenFileName(this, tr("Sélectionnez un fond pour le player"),QString(), tr("Images (*.jpg *.png)"));
    if(!tempPath.isEmpty())
    {
        a_bgPath = tempPath;
        ui->a_skinImage_2->setPixmap(QPixmap(a_bgPath));
    }
}


void Settings::reloadDefaultBg()
{
    a_bgPath = "";
    if (ui->a_skinPick->currentIndex() == 0) //Clean Fusion
    {
        ui->a_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewwhite.jpg"));
    }

    if (ui->a_skinPick->currentIndex() == 1) // Holo Fusion
    {
        ui->a_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewdark.png"));
    }
    if(ui->a_skinPick->currentIndex() == 2) //Sky Fusion
    {
        ui->a_skinImage_2->setPixmap(QPixmap(":/Ressources/backgroundpreviewcustomlight.jpg"));
    }
}

void Settings::updateOpacity(int value)
{
    a_opacityValue = value / 100.0;
    ui->a_valueSlide->setText(QString::number(value) + "%");
    passerelle->setOpacity(a_opacityValue);
}

void Settings::confirm()
{
        /* Library Section */
    if(a_isLibraryAtStartchecked)
    {
        if(a_isStaticLibChecked)
        {
            a_settings->setValue("Additional_Features/staticLibrary", true);
            if(a_isNewPath)
            /* Now saving playlist ! */
            setLibrary();
        }
        else if(!a_isStaticLibChecked)
        {
            a_settings->setValue("Additional_Features/staticLibrary", false);
        }
        if(a_isDynamicLibChecked)
        {
            a_settings->setValue("Additional_Features/refreshWhenNeeded", true);
            if(a_isNewPath)
                setLibrary();

            QFileInfo info (ui->a_pathView->text());
            qDebug() << info.lastModified().toMSecsSinceEpoch();
            a_settings->setValue("libModified", info.lastModified().toMSecsSinceEpoch());
        }
        else if(!a_isDynamicLibChecked)
        {
            a_settings->setValue("Additional_Features/refreshWhenNeeded", false);
        }
        if(ui->a_saveIndexActivate->isChecked())
            a_settings->setValue("Additional_Features/saveTrackIndex", true);

        else if (!ui->a_saveIndexActivate->isChecked())
            a_settings->setValue("Additional_Features/saveTrackIndex", false);
        a_settings->setValue("Additional_Features/libraryAtStartup", true);

    }
    else if (!a_isLibraryAtStartchecked) {
        a_settings->setValue("Additional_Features/libraryAtStartup", false);
    }
        /* Other Plugins */
    if(ui->a_playlistAtStartupCheck->isChecked())
        a_settings->setValue("playlistAtStartup", true);
    else
        a_settings->setValue("playlistAtStartup", false);
    if(ui->a_framelessCheck->isChecked())
        a_settings->setValue("Additional_Features/framelessWindow", true);
    else
        a_settings->setValue("Additional_Features/framelessWindow", false);

    /* Skin section */
    int currentSkin = a_settings->value("skin").toInt(); //Backup to test
    if(currentSkin != ui->a_skinPick->currentIndex() || a_settings->value("customimage").toString() != a_bgPath ) //Si un élément de skin a changé
    {
        a_settings->setValue("skin", ui->a_skinPick->currentIndex());
        a_settings->setValue("customimage", a_bgPath);
        Skin skin(ui->a_skinPick->currentIndex(), this);
        skin.load();
        passerelle->loadSkin();
        passerelle->update();
    }
    a_settings->setValue("opacity", a_opacityValue);
    this->close();
}

void Settings::setLibrary()
{
    if(!ui->a_pathView->text().isEmpty())
    {
        ui->a_confirm->setText(tr("Sauvegarde..."));
        neuPlaylist playlist(this);
        QList <QUrl> medias = playlist.setLibrary(ui->a_pathView->text());
        if(!medias.isEmpty())
            passerelle->updatePlaylistOfThePlayer(medias);
        ui->a_confirm->setText ("OK");
        a_settings->setValue("trackPosition", 0);
        //The Qt playlist loader will be fooled and will load the playlist smoothly as if it was saved by save();
     }
     else
     {
        a_settings->setValue("Additional_Features/staticLibrary", false);
        a_settings->setValue("Additional_Features/refreshWhenNeeded", false);
     }
}

void Settings::TEMP()
{
    QString workingdir = QDir::currentPath();
    QUrl path(workingdir);
    QDesktopServices::openUrl(path);
}

Settings::~Settings()
{
    delete ui;
    delete a_settings;
}
