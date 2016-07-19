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

#include "cdwriter.h"
#include "systeminteractor.h"

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>

CdWriter::CdWriter() : sys_(new SystemInteractor(this)) {}

CdWriter::~CdWriter() {}

bool CdWriter::hasK3b() const
{
    QString result = sys_->exec("which k3b");
    return !result.contains("no k3b");
}

bool CdWriter::burnList(const QStringList &pathList) const
{
    if (!hasK3b()) {
        qCritical() << "K3b could not be found.";
        return false;
    }

    QString program("k3b");
    QStringList arguments;
    arguments << "--audiocd";
    arguments << pathList;

    return sys_->startDetached(program, arguments);
}
