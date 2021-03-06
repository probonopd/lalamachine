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

#pragma once

#include <QJsonObject>
#include <QObject>
#include <QUrl>

#include "tags.h"

/*!
 * \brief The MetaDataProvider class is basically just a wrapper around the
 * metaData() function. Because it is used from QML it also contains the
 * function metaDataAsJson().
 */
class MetaDataProvider : public QObject {
    Q_OBJECT
public:
    explicit MetaDataProvider(QObject* parent = 0);
    static Tags metaData(const QUrl& path);
    static uint discNumberOfMp3(const QString& path);

    /*!
     * \brief metaDataAsJson is a convenience function to be used from QML.
     * \param path The path to the media file.
     * \return The result of Tags::toJson()
     */
    Q_INVOKABLE QJsonObject metaDataAsJson(const QUrl& path) const;
};
