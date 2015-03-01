#include "musiclib.h"

#include <QMutexLocker>
#include <QDir>
#include <QDirIterator>
#include <QJsonObject>
#include <QUrl>

#include "metadataprovider.h"
#include "config.h"

MusicLibScanner::MusicLibScanner(QObject *parent) :
    QObject(parent)
{
}

void MusicLibScanner::scanLib(const QString &path)
{
    QJsonObject lib;
    MetaDataProvider meta;
    QDir rootDir(path);
    QMutexLocker locker(&mutex_);

    if (!rootDir.exists()) {
        qDebug() << "Dir not found.";
    }
    else {
        QDirIterator it(rootDir, QDirIterator::Subdirectories);

        while (it.hasNext()) {
            QString line = it.next();

            if (suffixCheck(line)) {
                lib.insert(line, meta.metaData(QUrl(line)));
            }
        }
    }

    emit scanComplete(lib);
}

bool MusicLibScanner::suffixCheck(const QString &val) const
{
    if (val.endsWith(".mp3")) {
        return true;
    }

    if (val.endsWith(".m4a")) {
        return true;
    }

    if (val.endsWith(".ogg")) {
        return true;
    }

    return false;
}


MusicLib::MusicLib(QQuickItem *parent)
    : QQuickItem(parent)
{
    readLibFile();
    // This moveToThread is making the thread the parent of the scanner_.
    // Therefor it is vital that the scanner_ is a raw pointer, or double free
    // happens.
    scanner_->moveToThread(&scannerThread_);
    connect(&scannerThread_, &QThread::finished, scanner_, &QObject::deleteLater);
    connect(this, &MusicLib::startScan, scanner_, &MusicLibScanner::scanLib);
    connect(scanner_, &MusicLibScanner::scanComplete, this,
            &MusicLib::scanFinished);
    connect(this, &MusicLib::musicLibChanged, this, &MusicLib::setDisplayLib);
    connect(this, &MusicLib::genreFilterChanged, this, &MusicLib::setDisplayLib);
    connect(this, &MusicLib::artistFilterChanged, this, &MusicLib::setDisplayLib);
    connect(this, &MusicLib::albumFilterChanged, this, &MusicLib::setDisplayLib);
    connect(this, &MusicLib::musicLibChanged, this, &MusicLib::setGenreList);
    connect(this, &MusicLib::genreFilterChanged, this, &MusicLib::setArtistList);
    connect(this, &MusicLib::genreFilterChanged, this, &MusicLib::setAlbumList);
    connect(this, &MusicLib::artistFilterChanged, this, &MusicLib::setAlbumList);
    readLibFile();

    if (lib_.isEmpty()) {
        scannerThread_.start();
    }

    setGenreList();
}

MusicLib::~MusicLib()
{
    scannerThread_.quit();
    scannerThread_.wait(5000);
}

QJsonObject MusicLib::musicLib() const
{
    return lib_;
}

QJsonObject MusicLib::displayLib() const
{
    return displayLib_;
}

void MusicLib::readLibFile()
{
    lib_ = Config::loadJsonFile(Config::MUSICLIBPATH);
    emit musicLibChanged();
}

void MusicLib::writeLibFile()
{
    Config::saveJsonFile(Config::MUSICLIBPATH, lib_);
}

void MusicLib::debugSignal()
{
    qDebug() << "DEBUGGING SIGNAL";
}

void MusicLib::setDisplayLib()
{
    QJsonObject retVal;
    QJsonObject::const_iterator itr;

    for (itr = lib_.constBegin(); itr != lib_.constEnd(); ++itr) {
        if (itr.value().isObject()) {
            QJsonObject tmp = itr.value().toObject();

            if (checkVal(genreFilter(), tmp.value("genre").toString()) &&
                checkVal(artistFilter(), tmp.value("artist").toString()) &&
                checkVal(albumFilter(), tmp.value("album").toString())) {
                retVal.insert(itr.key(), tmp);
            }
        }
    }

    displayLib_ = retVal;
    emit displayLibChanged();
}

QString MusicLib::libPath() const
{
    return libPath_;
}

void MusicLib::setLibPath(const QString &path)
{
    libPath_ = path;
    emit libPathChanged();
    emit startScan(libPath_);
}

QString MusicLib::genreFilter() const
{
    return genreFilter_;
}

void MusicLib::setGenreFilter(const QString &val)
{
    qDebug() << "MusicLib::setGenreFilter(" << val << ")";

    if (val == "all") {
        genreFilter_ = "";
    }
    else {
        genreFilter_ = val;
    }

    setArtistFilter("");
    emit genreFilterChanged();
}

QString MusicLib::artistFilter() const
{
    return artistFilter_;
}

void MusicLib::setArtistFilter(const QString &val)
{
    if (val == "all") {
        artistFilter_ = "";
    }
    else {
        artistFilter_ = val;
    }

    setAlbumFilter("");
    emit artistFilterChanged();
}

QString MusicLib::albumFilter() const
{
    return albumFilter_;
}

void MusicLib::setAlbumFilter(const QString &val)
{
    qDebug() << "setAlbumFilter(" << val << ")";

    if (val == "all") {
        albumFilter_ = "";
    }
    else {
        albumFilter_ = val;
    }

    emit albumFilterChanged();
}

QStringList MusicLib::genreList() const
{
    return genreList_;
}

QStringList MusicLib::artistList() const
{
    return artistList_;
}

QStringList MusicLib::albumList() const
{
    return albumList_;
}

QStringList MusicLib::getList(const QString &what) const
{
    qDebug() << "QStringList MusicLib::getList(" << what << ")";
    Q_ASSERT(what == "genre" || what == "artist" || what == "album");
    QStringList retVal;
    // Add the field to display all.
    retVal << "all";
    QJsonObject tmplib = displayLib();
    QJsonObject::const_iterator itr;

    for (itr = tmplib.constBegin(); itr != tmplib.constEnd(); ++itr) {
        if (itr.value().isObject()) {
            QJsonObject tmpo = itr.value().toObject();
            QString tmps = tmpo.value(what).toString();

            if (!retVal.contains(tmps) && tmps.simplified() != "") {
                retVal << tmps;
            }
        }
    }

    return retVal;
}

void MusicLib::setGenreList()
{
    QStringList tmp;

    if (genreFilter() != "") {
        tmp << genreFilter();
    }
    else {
        tmp = getList(QString("genre"));
    }

    qDebug() << "MusicLib::genreList()";
    genreList_ = tmp;
    emit genreListChanged();
}

void MusicLib::setArtistList()
{
    QStringList tmp;

    if (artistFilter() != "") {
        tmp << artistFilter();
    }
    else {
        tmp = getList(QString("artist"));
    }

    artistList_ = tmp;
    emit artistListChanged();
}

void MusicLib::setAlbumList()
{
    QStringList tmp;
    qDebug() << "MusicLib::albumList()";

    if (albumFilter() != "") {
        tmp << albumFilter();
    }
    else {
        tmp = getList(QString("album"));
    }

    albumList_ = tmp;
    emit albumListChanged();
}

void MusicLib::scanFinished(const QJsonObject &lib)
{
    lib_ = lib;
    emit musicLibChanged();
    writeLibFile();
}

bool MusicLib::checkVal(const QString &check, const QString &val) const
{
    return check == "" || check == val;
}