#include "initialconfig.h"
#include "ui_initialconfig.h"
#include <QDebug>

InitialConfig::InitialConfig(QWidget *parent) :
    QDialog(parent), m_page(1), m_willRestart(false),
    m_canClose(false), ui(new Ui::InitialConfig),
    m_settings ("neuPlayer.ini", QSettings::IniFormat, this)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setupWidgetsInitialVisibility();

    setupConnections();

    entranceAnimations(); //It's time for an appearance ~
}

void InitialConfig::setupWidgetsInitialVisibility()
{
    ui->nextBtn->setVisible(false);
    ui->finishButton->setVisible(false);
    ui->libraryAtStartupCheck->setVisible(false);
    ui->playlistAtStartupCheck->setVisible(false);
    ui->label->setVisible(false);
    ui->opacitySlide->setVisible(false);
    ui->opacityLabel->setVisible(false);
    ui->libraryAtStartupCheck_2->setVisible(false);
}

void InitialConfig::setupConnections()
{
    connect(ui->recommendedBtn, SIGNAL(clicked()), this, SLOT(presetWizard()));
    connect(ui->configureBtn, SIGNAL(clicked()), this, SLOT(customWizard()));
    connect(ui->nextBtn, SIGNAL(clicked()), this, SLOT(advanceWizard()));
    connect(ui->finishButton, SIGNAL(clicked()), this, SLOT(finishWizard()));
}

void InitialConfig::entranceAnimations()
{
    QPointer<MoveAnimation> welcomeMove =
            new MoveAnimation(ui->welcome, MoveAnimation::TopToBottom,
                              MoveAnimation::Normal, this);
    QPointer<MoveAnimation> button1move =
            new MoveAnimation(ui->recommendedBtn, MoveAnimation::TopToBottom,
                              MoveAnimation::Normal, this);
    QPointer<MoveAnimation> button2move =
            new MoveAnimation(ui->configureBtn, MoveAnimation::TopToBottom,
                              MoveAnimation::Normal, this);
    QPointer<MoveAnimation> descriptionMove =
            new MoveAnimation(ui->description, MoveAnimation::TopToBottom,
                              MoveAnimation::Normal, this);

    /* By default this class will make make the target fade in for 300 ms and add it to a parallel animation group */
    m_animManager.setDefaultDuration(400); //The animation will be more visible that way
    m_animManager.addTarget(ui->welcome);
    m_animManager.addTarget(ui->description);
    m_animManager.addTarget(ui->recommendedBtn);
    m_animManager.addTarget(ui->configureBtn);
    m_animManager.startGroup(FadeManager::Parallel, false);
    welcomeMove->start();
    descriptionMove->start();
    button1move->start();
    button2move->start();
}

void InitialConfig::customWizard()
{
    ui->configureBtn->setVisible(false);
    ui->recommendedBtn->setVisible(false);
    //On peut maintenant atteindre la prochaine page
    m_page++;
    advanceWizard();
}

//View that's displayed when selecting the recommended option
void InitialConfig::presetWizard()
{
    this->setCursor(Qt::BusyCursor);
    ui->configureBtn->setVisible(false);
    ui->recommendedBtn->setText(tr("Chargement..."));
    m_willRestart = true;
    presetConfig();
    ui->recommendedBtn->setVisible(false);
    finalPage();
}

//Do the work of configuring the player's data when choosing recommended option
void InitialConfig::presetConfig()
{
    m_settings.beginGroup("Additional_Features");
    m_settings.setValue("libraryAtStartup", true);
    m_settings.setValue("refreshWhenNeeded", true);
    m_settings.setValue("saveTrackIndex", true);
    m_settings.setValue("framelessWindow", true);
    m_settings.endGroup();

    m_settings.setValue("playlistAtStartup", false);
    m_settings.setValue("opacity", 1.0);
    m_settings.setValue("volume", 50);
    m_settings.setValue("mediapath",
                        QStandardPaths::standardLocations(QStandardPaths::MusicLocation));
    QFileInfo info (m_settings.value("mediapath").toString());
    m_settings.setValue("libModified", info.lastModified().toMSecsSinceEpoch());
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
    if(m_page == 2)
    {
        ui->description->setText(tr("Playlist"));
        ui->playlistAtStartupCheck->setVisible(true);
        ui->nextBtn->setVisible(true);

        m_animManager.setDefaultDuration(300);
        m_animManager.clearGroup(FadeManager::Parallel);
        m_animManager.addTarget(ui->description);
        m_animManager.addTarget(ui->playlistAtStartupCheck);
        m_animManager.addTarget(ui->nextBtn);
        m_animManager.startGroup(FadeManager::Parallel, false);
    }
    if(m_page == 3)
    {
        m_settings.setValue("playlistAtStartup",
                            ui->playlistAtStartupCheck->isChecked());
        ui->playlistAtStartupCheck->setVisible(false);
        ui->description->setText(tr("Opacité"));
        ui->label->setText(tr("Laissez à 100% si vous ne souhaitez\n"
                                "pas de transparence"));
        ui->label->setVisible(true);
        ui->opacityLabel->setVisible(true);
        ui->opacitySlide->setVisible(true);
        updateSlideValue(100);

        m_animManager.clearGroup(FadeManager::Parallel);
        m_animManager.addTarget(ui->description);
        m_animManager.addTarget(ui->label);
        m_animManager.addTarget(ui->opacityLabel);
        m_animManager.addTarget(ui->opacitySlide);
        m_animManager.startGroup(FadeManager::Parallel, false);

        connect(ui->opacitySlide, SIGNAL(valueChanged(int)),
                this, SLOT(updateSlideValue(int)));
    }

    if(m_page == 4)
    {
        m_settings.setValue("opacity", m_slideValue);
        ui->opacityLabel->setVisible(false);
        ui->opacitySlide->setVisible(false);
        ui->libraryAtStartupCheck->setVisible(true);
        ui->libraryAtStartupCheck_2->setVisible(true);
        ui->playlistAtStartupCheck->setVisible(false);
        ui->label->setVisible(false);
        ui->description->setText(tr("Démarrage avec librairie"));

        m_animManager.clearGroup(FadeManager::Parallel);
        m_animManager.addTarget(ui->libraryAtStartupCheck);
        m_animManager.addTarget(ui->libraryAtStartupCheck_2);
        m_animManager.addTarget(ui->description);
        m_animManager.startGroup(FadeManager::Parallel, false);

        connect(ui->libraryAtStartupCheck, SIGNAL(clicked(bool)),
                this, SLOT(setLibrary(bool)));
        connect(ui->libraryAtStartupCheck_2, SIGNAL(clicked(bool)),
                this, SLOT(setDefault(bool)));
    }
    if(m_page == 5)
    {
        this->setCursor(Qt::BusyCursor); //Showing to the user processing is being done
        m_settings.setValue("Additional_Features/libraryAtStartup",
                            ui->libraryAtStartupCheck->isChecked());
        m_settings.setValue("Additional_Features/refreshWhenNeeded",
                            ui->libraryAtStartupCheck->isChecked());
        m_settings.setValue("saveTrackIndex", true);
        QFileInfo info (m_settings.value("mediapath").toString());
        m_settings.setValue("libModified", info.lastModified().toMSecsSinceEpoch());
        ui->libraryAtStartupCheck->setVisible(false);
        ui->libraryAtStartupCheck_2->setVisible(false);
        neuPlayer = new Player(); //The player will update itself, and write that the config has been done after that
        neuPlayer->hide(); //The update will force-show, but I don't want to show the player to the user yet ;)
        finalPage();
    }
    m_page++;
}

void InitialConfig::updateSlideValue(int value)
{
    m_slideValue = value /100.0;
    ui->opacityLabel->setText(QString::number(value) + "%");
    this->setWindowOpacity(m_slideValue);
}


void InitialConfig::setDefault(bool isActivated)
{
    if(isActivated)
    {
        m_settings.setValue("mediapath", QStandardPaths::standardLocations(QStandardPaths::MusicLocation));
        ui->libraryAtStartupCheck->setChecked(true);
    }
}

//Handle les states de la checkbox library
void InitialConfig::setLibrary(bool isActivated)
{
    if(isActivated)
    {
        QString tempPath = QFileDialog::getExistingDirectory(this,
                                                             tr("Selectionnez votre répertoire de musique"),
                                                             "",
                                                             QFileDialog::DontResolveSymlinks);
        if(tempPath.isEmpty())
        {
            ui->libraryAtStartupCheck_2->setChecked(true);
            m_settings.setValue("mediapath", QStandardPaths::standardLocations(QStandardPaths::MusicLocation));
            QMessageBox::information(this, tr("Information"),
                                     "default folder : " +
                                     m_settings.value("mediapath").toString()); //On get depuis le QSettings pour avoir un string normal affichable
        }
        else //Sinon on a de quoi remplir la var
        {
            ui->libraryAtStartupCheck_2->setChecked(false);
            m_mediaPath = tempPath;
            m_settings.setValue("mediapath", m_mediaPath);
        }
    }
}

void InitialConfig::finalPage()
{
    m_page = 5;
    ui->description->setText(tr("Vous êtes prêt à utiliser le lecteur !"));
    ui->welcome->setText(tr("Terminé !"));
    ui->label->setVisible(true);
    ui->label->setText(tr("Si le lecteur n'affiche pas vos musiques\n"
                            "configurez-le manuellement dans les paramètres"));
    ui->finishButton->setVisible(true);
    ui->finishButton->setFocus(); //So the user can hit enter and finish right away
    m_animManager.clearGroup(FadeManager::Parallel);
    m_animManager.addTarget(ui->welcome);
    m_animManager.addTarget(ui->label);
    m_animManager.addTarget(ui->description);
    m_animManager.addTarget(ui->finishButton);
    m_animManager.startGroup(FadeManager::Parallel); //Will delete the animation. We're done with it !
    this->setCursor(Qt::ArrowCursor);
}

void InitialConfig::finishWizard()
{
    m_page = 10;
    if(m_settings.value("playlistAtStartup").toBool() == true)
        neuPlayer = new Player();
    if(!m_willRestart)
        neuPlayer->show();
    neuPlayer->setIndexOfThePlayer(0, false);
    close(); //Will call closeEvent
}

void InitialConfig::delayedClose()
{
    m_canClose = true;
    close();
}

void InitialConfig::closeEvent(QCloseEvent *event = 0)
{
    if(m_canClose)
    {
        QFile fileHandler(".configdone");
        fileHandler.open(QFile::ReadWrite); //Create it and I'm done
        if(m_page <= 5) //Create it if it hasn't been configured yet
        {
            neuPlayer = new Player();
        }
        if(m_settings.value("Additional_Features/framelessWindow").toBool() == true)
            //Didn't find any solution, so we'll go with that. The main will always setup the frameless window nicely
            neuPlayer->restart();

        //else, simply use the object created
        else
            neuPlayer->show();
        event->accept();
    }
    else
    {
        event->ignore();
        QPointer <FadeWindow> fadeOut = new FadeWindow(this, 450, FadeWindow::FadeOut, this);
        fadeOut->start();
        m_timer.start(650);
        connect(&m_timer, SIGNAL(timeout()), this, SLOT(delayedClose()));
    }
}

InitialConfig::~InitialConfig()
{
    delete ui;
}
