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

#ifndef AUTOPLAYLISTMANAGER_H
#define AUTOPLAYLISTMANAGER_H

#include <QObject>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileSystemWatcher>
#include <QMap>
#include "config.h"
#include "autoplaylistobject.h"
#include "autoplaylist.h"

class AutoPlaylistManager : public QObject
{
    Q_OBJECT

    // clang-format off
    Q_PROPERTY(QString currentList
               READ currentList
               WRITE setCurrentList
               NOTIFY currentListChanged)
    Q_PROPERTY(QStringList autoPlaylistNames
               READ autoPlaylistNames
               NOTIFY autoPlaylistNamesChanged)
    // clang-format on
public:
    explicit AutoPlaylistManager(QObject *parent = 0);
    ~AutoPlaylistManager();

    QString currentList() const;
    void setCurrentList(const QString &val);

    QStringList autoPlaylistNames();
    void setAutoPlaylistNames(const QStringList &names);

    Q_INVOKABLE QJsonArray getTracks(const QString name) const;
    Q_INVOKABLE QJsonArray getAutoPlaylist(const QString name) const;
    Q_INVOKABLE void saveAutoPlaylist(const QString &name,
                                      const QJsonArray &args);
    Q_INVOKABLE void deleteAutoPlaylist(const QString &name);

    static QList<AutoPlaylistObject> jsonToApo(const QJsonArray &args);

signals:
    void currentListChanged();
    void autoPlaylistNamesChanged();

private:
    QString currentList_{""};
    QMap<QString, AutoPlaylist *> playlists_;
    void saveAutoPlaylist(const QString &name,
                          const QList<AutoPlaylistObject> &args) const;

    QString getPath(const QString &name) const;
    QStringList getPlaylistNames() const;
    void loadPlaylists();
};

#endif  // AUTOPLAYLISTMANAGER_H
