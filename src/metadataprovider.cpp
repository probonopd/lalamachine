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

#include "metadataprovider.h"

#include <QDebug>
#include <QString>
#include <QVariantMap>
#include <QJsonObject>
#include <QTime>

#include <tag.h>
#include <fileref.h>

#include "timeconverter.h"
#include "tags.h"

MetaDataProvider::MetaDataProvider(QObject *parent) : QObject(parent) {}

Tags MetaDataProvider::metaData(const QString &path)
{
    QString tmp = path;
    tmp.remove("file://");
    TagLib::FileRef f(tmp.toUtf8().data(),
                      true,
                      TagLib::AudioProperties::Accurate);
    TimeConverter tc;

    if (!f.isNull() && f.tag()) {
        TagLib::Tag *tag = f.tag();
        // Clearing the timeconverter and get the time as displayable string.
        tc.clear();
        tc.setSeconds(f.audioProperties()->length());
        return Tags(tag,
                    path,
                    path,
                    f.audioProperties()->length(),
                    tc.toString());
    }

    return Tags();
}

QJsonObject MetaDataProvider::metaDataAsJson(const QString &path) const
{
    return metaData(path).toJson();
}