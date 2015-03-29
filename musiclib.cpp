/*
Copyright 2015 Armin Zirkel

This file is part of lalamachine.

Lalamachine is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Lalamachine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with lalamachine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "musiclib.h"

#include <QMutexLocker>
#include <QDir>
#include <QDirIterator>
#include <QJsonObject>
#include <QUrl>
#include <QSqlQuery>
#include <QSqlError>
#include <QElapsedTimer>

#include "metadataprovider.h"
#include "config.h"

MusicLibScanner::MusicLibScanner(QObject *parent) :
    QObject(parent)
{
}

void MusicLibScanner::setDb(QSqlDatabase *db)
{
    scanDb_ = db;
}

void MusicLibScanner::scanLib(const QString &path)
{
    QElapsedTimer timer;
    timer.start();

    if (path == "" || !QDir(path).exists()) {
        qCritical("I can't scan a non-existing folder.");
        return;
    }

    qDebug() << "Start scan";
    emit scanStarted();
    QVector<QString> lib(11);
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
                lib = meta.metaData(QUrl(line));
                addTrackToDB(lib.at(0),
                             lib.at(1),
                             lib.at(2),
                             lib.at(3),
                             lib.at(4),
                             lib.at(5),
                             lib.at(6),
                             lib.at(7),
                             lib.at(8),
                             lib.at(9),
                             lib.at(10));
            }
        }
    }

    qDebug() << "End scan" << timer.elapsed();
    emit scanComplete();
}

void MusicLibScanner::addTrackToDB(QString album,
                                   QString artist,
                                   QString comment,
                                   QString genre,
                                   QString length,
                                   QString lengthString,
                                   QString mrl,
                                   QString path,
                                   QString title,
                                   QString track,
                                   QString year)
{
    QString query("INSERT into `musiclib` ");
    QString str1;
    query.append("(`album`, `artist`, `comment`, `genre`, `length`, ");
    query.append("`lengthString`, `mrl`, `path`, `title`, `track`, `year`) ");
    query.append("VALUES (");
    query.append(QString("'%1', '%2', '%3', ").arg(
                     MusicLib::escapeString(album),
                     MusicLib::escapeString(artist),
                     MusicLib::escapeString(comment)));
    query.append(QString("'%1', %2, '%3', '%4', '%5', '%6', %7, %8)").arg(
                     MusicLib::escapeString(genre),
                     length,
                     lengthString,
                     MusicLib::escapeString(mrl),
                     MusicLib::escapeString(path),
                     MusicLib::escapeString(title),
                     track,
                     year));

    //qDebug() << scanDb_->exec(query).lastError();
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

    db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_.setDatabaseName(Config::MUSICLIBDB);
    db_.open();
    ensureAllTables();
    scanner_->setDb(&db_);

    connect(&scannerThread_, &QThread::finished,
            scanner_, &QObject::deleteLater);

    connect(this, &MusicLib::startScan,
            scanner_, &MusicLibScanner::scanLib);

    connect(scanner_, &MusicLibScanner::scanComplete,
            this, &MusicLib::scanFinished);

    connect(scanner_, &MusicLibScanner::scanStarted,
            this, &MusicLib::scanStarted);

    connect(this, &MusicLib::musicLibChanged,
            this, &MusicLib::setDisplayLib);

    connect(this, &MusicLib::genreFilterChanged,
            this, &MusicLib::setDisplayLib);

    connect(this, &MusicLib::artistFilterChanged,
            this, &MusicLib::setDisplayLib);

    connect(this, &MusicLib::albumFilterChanged,
            this, &MusicLib::setDisplayLib);

    connect(this, &MusicLib::musicLibChanged,
            this, &MusicLib::setGenreList);

    connect(this, &MusicLib::musicLibChanged,
            this, &MusicLib::setArtistList);

    connect(this, &MusicLib::musicLibChanged,
            this, &MusicLib::setAlbumList);

    connect(this, &MusicLib::genreFilterChanged,
            this, &MusicLib::setArtistList);

    connect(this, &MusicLib::genreFilterChanged,
            this, &MusicLib::setAlbumList);

    connect(this, &MusicLib::artistFilterChanged,
            this, &MusicLib::setAlbumList);

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
    db_.close();
}

const QString MusicLib::ALL_FILTER {QString("--all--")};

QJsonObject MusicLib::musicLib() const
{
    return lib_;
}

bool MusicLib::scanning() const
{
    return scanning_;
}

void MusicLib::setScanning(bool val)
{
    scanning_ = val;
    emit scanningChanged();
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
    rescan();
}

QString MusicLib::genreFilter() const
{
    return genreFilter_;
}

void MusicLib::setGenreFilter(const QString &val)
{
    qDebug() << "MusicLib::setGenreFilter(" << val << ")";

    if (val == ALL_FILTER) {
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
    if (val == ALL_FILTER) {
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

    if (val == ALL_FILTER) {
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

void MusicLib::rescan()
{
    qDebug() << "emitting rescan" << libPath();

    if (!scannerThread_.isRunning()) {
        scannerThread_.start();
    }

    emit startScan(libPath());
}

QString MusicLib::escapeString(QString str)
{
    return str.replace("\'", "\'\'").replace(",", "\'+\',\'+\'");
}

QStringList MusicLib::getList(const QString &what) const
{
    qDebug() << "QStringList MusicLib::getList(" << what << ")";
    Q_ASSERT(what == "genre" || what == "artist" || what == "album");
    QStringList retVal;
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

void MusicLib::ensureAllTables()
{
    auto tables = db_.tables();

    if (!tables.contains("musiclib")) {
        QString qs("CREATE TABLE `musiclib` ");
        qs.append("(\n");
        //qs.append("`ID` INTEGER NOT NULL AUTOINCREMENT,\n");
        qs.append("`album` TEXT,\n");
        qs.append("`artist` TEXT,\n");
        qs.append("`comment` TEXT,\n");
        qs.append("`genre` TEXT,\n");
        qs.append("`length` int NOT NULL,\n");
        qs.append("`lengthString` TEXT NOT NULL,\n");
        qs.append("`mrl` TEXT NOT NULL PRIMARY KEY,\n");
        qs.append("`path` TEXT NOT NULL,\n");
        qs.append("`title` TEXT NOT NULL,\n");
        qs.append("`track` int,\n");
        qs.append("`year` int\n");
        qs.append(")");
        qDebug() << db_.exec(qs).lastError();
    }
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
    genreList_.sort();
    genreList_.prepend(ALL_FILTER);
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
    artistList_.sort();
    artistList_.prepend(ALL_FILTER);
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
    albumList_.sort();
    albumList_.prepend(ALL_FILTER);
    emit albumListChanged();
}

void MusicLib::scanStarted()
{
    setScanning(true);
}

void MusicLib::scanFinished()
{
    //lib_ = lib;
    emit musicLibChanged();
    writeLibFile();
    setScanning(false);
}

bool MusicLib::checkVal(const QString &check, const QString &val) const
{
    return check == "" || check == val;
}
