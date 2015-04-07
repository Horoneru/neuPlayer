#include "initialconfig.h"
#include "ui_initialconfig.h"
#include <QDebug>

InitialConfig::InitialConfig(QWidget *parent) :
    QDialog(parent), a_page(1),
    a_canClose(false), ui(new Ui::InitialConfig)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowOpacity(100);
    a_settings = new QSettings("neuPlayer.ini", QSettings::IniFormat, this);

    setupObjects();

    setupWidgetsInitialVisibility();

    setupConnections();

    entranceAnimations(); //It's time for an appearance ~
}

void InitialConfig::setupObjects()
{
    animManager = new fadeAnimManager(this);
}


void InitialConfig::setupWidgetsInitialVisibility()
{
    ui->a_nextBtn->setVisible(false);
    ui->a_finishButton->setVisible(false);
    ui->a_libraryAtStartupCheck->setVisible(false);
    ui->a_playlistAtStartupCheck->setVisible(false);
    ui->a_label->setVisible(false);
    ui->a_opacitySlide->setVisible(false);
    ui->a_opacityLabel->setVisible(false);
    ui->a_libraryAtStartupCheck_2->setVisible(false);
}

void InitialConfig::setupConnections()
{
    connect(ui->a_recommendedBtn, SIGNAL(clicked()), this, SLOT(presetWizard()));
    connect(ui->a_configureBtn, SIGNAL(clicked()), this, SLOT(customWizard()));
    connect(ui->a_nextBtn, SIGNAL(clicked()), this, SLOT(advanceWizard()));
    connect(ui->a_finishButton, SIGNAL(clicked()), this, SLOT(finishWizard()));
}

void InitialConfig::entranceAnimations()
{
    /* By default this class will make make the target fade in for 300 ms and add it to a parallel animation group */
    animManager->setDefaultDuration(400); //The animation will be more visible that way
    animManager->addTarget(ui->a_welcome);
    animManager->addTarget(ui->a_description);
    animManager->addTarget(ui->a_recommendedBtn);
    animManager->addTarget(ui->a_configureBtn);
    animManager->startGroup(fadeAnimManager::Parallel, false);
}

void InitialConfig::customWizard()
{
    ui->a_configureBtn->setVisible(false);
    ui->a_recommendedBtn->setVisible(false);
    //On peut maintenant atteindre la prochaine page
    a_page++;
    advanceWizard();
}

//View that's displayed when selecting the recommended option
void InitialConfig::presetWizard()
{
    this->setCursor(Qt::BusyCursor);
    ui->a_configureBtn->setVisible(false);
    ui->a_recommendedBtn->setText(tr("Chargement..."));
    presetConfig();
    ui->a_recommendedBtn->setVisible(false);
    finalPage();
}

//Do the work of configuring the player's data when choosing recommended option
void InitialConfig::presetConfig()
{
    a_settings->beginGroup("Additional_Features");
    a_settings->setValue("libraryAtStartup", true);
    a_settings->setValue("refreshWhenNeeded", true);
    a_settings->setValue("saveTrackIndex", true);
    a_settings->endGroup();
    a_settings->setValue("playlistAtStartup", false);
    a_settings->setValue("mediapath", QStandardPaths::standardLocations(QStandardPaths::MusicLocation));
    qDebug() << QStandardPaths::standardLocations(QStandardPaths::MusicLocation);
    QFileInfo info (a_settings->value("mediapath").toString());
    a_settings->setValue("libModified", info.lastModified().toMSecsSinceEpoch());
    neuPlayer = new Player(); //The player will update itself, and write that the config has been done after that
    neuPlayer->hide(); //The update will force-show, but I don't want to show the player to the user yet ;)
}

//Slot that handles the next button action. The master method of the next button
void InitialConfig::advanceWizard()
{
    /* Structure :
     * config with given parameters from previous page
     * visibility
     * others (effects, anims, connections...)
     * */
    if(a_page == 2)
    {
        ui->a_description->setText(tr("Playlist"));
        ui->a_playlistAtStartupCheck->setVisible(true);
        ui->a_nextBtn->setVisible(true);

        animManager->setDefaultDuration(300);
//        animManager->clearGroup(fadeAnimManager::Parallel);
        animManager->addTarget(ui->a_description);
        animManager->addTarget(ui->a_playlistAtStartupCheck);
        animManager->addTarget(ui->a_nextBtn);
        animManager->startGroup(fadeAnimManager::Parallel, false);
    }
    if(a_page == 3)
    {
        a_settings->setValue("playlistAtStartup", ui->a_playlistAtStartupCheck->isChecked());
        ui->a_playlistAtStartupCheck->setVisible(false);
        ui->a_description->setText(tr("Opacité"));
        ui->a_label->setText(tr("Laissez à 100% si vous ne souhaitez\npas de transparence"));
        ui->a_label->setVisible(true);
        ui->a_opacityLabel->setVisible(true);
        ui->a_opacitySlide->setVisible(true);
        updateSlideValue(100);

        animManager->clearGroup(fadeAnimManager::Parallel);
        animManager->addTarget(ui->a_description);
        animManager->addTarget(ui->a_label);
        animManager->addTarget(ui->a_opacityLabel);
        animManager->addTarget(ui->a_opacitySlide);
        animManager->startGroup(fadeAnimManager::Parallel, false);

        connect(ui->a_opacitySlide, SIGNAL(valueChanged(int)), this, SLOT(updateSlideValue(int)));
    }

    if(a_page == 4)
    {
        a_settings->setValue("opacity", a_slideValue);
        ui->a_opacityLabel->setVisible(false);
        ui->a_opacitySlide->setVisible(false);
        ui->a_libraryAtStartupCheck->setVisible(true);
        ui->a_libraryAtStartupCheck_2->setVisible(true);
        ui->a_playlistAtStartupCheck->setVisible(false);
        ui->a_label->setVisible(false);
        ui->a_description->setText(tr("Démarrage avec librairie"));

        animManager->clearGroup(fadeAnimManager::Parallel);
        animManager->addTarget(ui->a_libraryAtStartupCheck);
        animManager->addTarget(ui->a_libraryAtStartupCheck_2);
        animManager->addTarget(ui->a_description);
        animManager->startGroup(fadeAnimManager::Parallel, false);

        connect(ui->a_libraryAtStartupCheck, SIGNAL(clicked(bool)), this, SLOT(setLibrary(bool)));
        connect(ui->a_libraryAtStartupCheck_2, SIGNAL(clicked(bool)), this, SLOT(setDefault(bool)));
    }
    if(a_page == 5)
    {
        this->setCursor(Qt::BusyCursor); //Showing to the user processing is being done
        a_settings->setValue("Additional_Features/libraryAtStartup", ui->a_libraryAtStartupCheck->isChecked());
        a_settings->setValue("Additional_Features/refreshWhenNeeded", ui->a_libraryAtStartupCheck->isChecked());
        a_settings->setValue("saveTrackIndex", true);
        QFileInfo info (a_settings->value("mediapath").toString());
        a_settings->setValue("libModified", info.lastModified().toMSecsSinceEpoch());
        ui->a_libraryAtStartupCheck->setVisible(false);
        ui->a_libraryAtStartupCheck_2->setVisible(false);

        neuPlayer = new Player(); //The player will update itself, and write that the config has been done after that
        neuPlayer->hide(); //The update will force-show, but I don't want to show the player to the user yet ;)
        finalPage();
    }
    a_page++;
}

void InitialConfig::updateSlideValue(int value)
{
    a_slideValue = value /100.0;
    ui->a_opacityLabel->setText(QString::number(value) + "%");
    this->setWindowOpacity(a_slideValue);
}


void InitialConfig::setDefault(bool isActivated)
{
    if(isActivated)
    {
        a_settings->setValue("mediapath", QStandardPaths::standardLocations(QStandardPaths::MusicLocation));
        ui->a_libraryAtStartupCheck->setChecked(true);
    }
}

//Handle les states de la checkbox library
void InitialConfig::setLibrary(bool isActivated)
{
    if(isActivated)
    {
        QString tempPath = QFileDialog::getExistingDirectory(this, tr("Selectionnez votre répertoire de musique"),"", QFileDialog::DontResolveSymlinks);
        if(tempPath.isEmpty())
        {
            ui->a_libraryAtStartupCheck_2->setChecked(true);
            a_settings->setValue("mediapath", QStandardPaths::standardLocations(QStandardPaths::MusicLocation));
            QMessageBox::information(this, tr("Information"), "default folder : " + a_settings->value("mediapath").toString()); //On get depuis le QSettings pour avoir un string normal affichable
        }
        else //Sinon on a de quoi remplir la var
        {
            ui->a_libraryAtStartupCheck_2->setChecked(false);
            a_mediaPath = tempPath;
            a_settings->setValue("mediapath", a_mediaPath);
        }
    }
}

void InitialConfig::finalPage()
{
    a_page = 5;
    ui->a_description->setText(tr("Vous êtes prêt à utiliser le lecteur !"));
    ui->a_welcome->setText(tr("Terminé !"));
    ui->a_label->setVisible(true);
    ui->a_label->setText(tr("Si le lecteur n'affiche pas vos musiques\nconfigurez-le manuellement dans les paramètres"));
    ui->a_finishButton->setVisible(true);
    ui->a_finishButton->setFocus(); //So the user can hit enter and finish right away

    animManager->clearGroup(fadeAnimManager::Parallel);
    animManager->addTarget(ui->a_welcome);
    animManager->addTarget(ui->a_label);
    animManager->addTarget(ui->a_description);
    animManager->addTarget(ui->a_finishButton);
    animManager->startGroup(fadeAnimManager::Parallel); //Will delete the animation. We're done with it !

    this->setCursor(Qt::ArrowCursor);
}

void InitialConfig::finishWizard()
{
    a_page = 10;
    if(a_settings->value("playlistAtStartup").toBool() == true)
        neuPlayer = new Player();
    neuPlayer->show();
    neuPlayer->setIndexOfThePlayer(0, false);
    close(); //Will call closeEvent
}

void InitialConfig::delayedClose()
{
    a_canClose = true;
    close();
}

void InitialConfig::closeEvent(QCloseEvent *event = 0)
{
    if(a_canClose)
    {
        QFile fileHandler(".configdone");
        fileHandler.open(QFile::ReadWrite); //Create it and I'm done
        if(a_page <= 5) //Create it if it hasn't been configured yet
        {
            neuPlayer = new Player();
        }
        //else, simply use the object created
        neuPlayer->show();
        event->accept();
    }
    else
    {
        event->ignore();
        FadeOutWindow fadeOut(this, 450, this);
        fadeOut.start();
        a_timer = new QTimer(this);
        a_timer->start(650);
        connect(a_timer, SIGNAL(timeout()), this, SLOT(delayedClose()));
    }
}

InitialConfig::~InitialConfig()
{
    delete ui;
}
