#include "neuplaylist.h"

neuPlaylist::neuPlaylist(QObject *parent) :
    QMediaPlaylist(parent)
{
    m_listFilter << "*.wav";
    m_listFilter << "*.mp3";
    m_listFilter << "*.mp4";
    m_listFilter << "*.m4a";
}

void neuPlaylist::save(const QString &toFileName) //Default
{
    this->clear();
    QSettings settings("neuPlayer.ini", QSettings::IniFormat, this);
    QDirIterator dirIterator(settings.value("mediapath").toString(),
                             m_listFilter, QDir::Files | QDir::NoSymLinks,
                             QDirIterator::Subdirectories);

    QFile fileHandler(toFileName);

    if(!fileHandler.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QMessageBox::critical(nullptr, tr("Erreur ! "),
                              tr("Le fichier de playlist n'a pas pu être ouvert"));
        return;
    }
    //We're going to write in this streamer which will serve as a buffer
    QTextStream out(&fileHandler);
    out.setCodec("UTF-8");
    while(dirIterator.hasNext())
    {
        //adding the common structure of a m3u file to the URL
        out << dirIterator.next().prepend("file:///").append("\n").toUtf8();
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    fileHandler.close();
}



void neuPlaylist::save(const QString &toFileName, const QUrl &pathSaved)
{
    this->clear();
    QDirIterator dirIterator(pathSaved.toString(), m_listFilter ,QDir::Files |
                             QDir::NoSymLinks, QDirIterator::Subdirectories);

    QFile fileHandler(toFileName);
    if(!fileHandler.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QMessageBox::critical(nullptr, tr("Erreur ! "),
                              tr("Le fichier de playlist n'a pas pu être ouvert"));
        return;
    }
    QTextStream out(&fileHandler);
    out.setCodec("UTF-8");
    while(dirIterator.hasNext())
    {
        out << dirIterator.next().prepend("file:///").append("\n").toUtf8();
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    fileHandler.close();
}

void neuPlaylist::saveFromPlaylist(const QString &playlistName)
{
    QFile fileHandler(playlistName);
    if(!fileHandler.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QMessageBox::critical(nullptr, tr("Erreur ! "),
                              tr("Le fichier de playlist n'a pas pu être ouvert"));
        return;
    }
    QTextStream out(&fileHandler);
    out.setCodec("UTF-8");
    int const mediaCount = this->mediaCount();
    for(int i (0); i < mediaCount; i++ )
    {
        out << this->media(i).canonicalUrl().path().append("\n").toUtf8();
    }
    fileHandler.close();
}

//Called from Settings
QList <QUrl> neuPlaylist::setLibrary(const QString &path)
{
    QDirIterator dirIterator(path, m_listFilter ,QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    QFile fileHandler("neuLibrary.m3u8");

    if(!fileHandler.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QMessageBox::critical(nullptr, tr("Erreur ! "),
                              tr("Le fichier de playlist n'a pas pu être ouvert"));
    }
    else
    {
        //We're going to write in this streamer which will serve as a buffer
        QTextStream out(&fileHandler);
        out.setCodec("UTF-8");
        QList <QUrl> *medias;
        medias = new QList <QUrl>();
        while(dirIterator.hasNext())
        {
            out << dirIterator.next().prepend("file:///").append("\n").toUtf8();
            medias->append(QUrl(dirIterator.filePath().prepend("file:///").toUtf8()));
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        }
        fileHandler.close();
        return *medias;
    }
}

//Returns the updated list
QList <QUrl> neuPlaylist::update(const QString &path)
{
    QDirIterator dirIterator(path, m_listFilter ,QDir::Files |
                             QDir::NoSymLinks, QDirIterator::Subdirectories);

    // Variable qui contiendra tous les fichiers correspondant à notre recherche
    QList <QUrl> *urlList;
    urlList = new QList <QUrl>();
    // Tant qu'on n'est pas arrivé à la fin de l'arborescence...
    while(dirIterator.hasNext())
    {
        urlList->append(QUrl::fromLocalFile(dirIterator.next().toUtf8()));
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    return *urlList;
}

neuPlaylist::~neuPlaylist(){}
