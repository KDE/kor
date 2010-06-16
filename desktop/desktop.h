/********************************************************************
 Copyright 2010 Lubos Lunak <l.lunak@kde.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/

#ifndef KOR_DESKTOP_H
#define KOR_DESKTOP_H

#include <qobject.h>
#include <qscopedpointer.h>

namespace Kor
{

class Desktop
    : public QObject
    {
    Q_OBJECT
    public:
        Desktop( const QString& id, QObject* parent = NULL );
    private:
        void loadConfig();
        void updatePosition();
    private:
        QString id;
        QScopedPointer< QWidget > window;
        int configuredScreen;
    };

} // namespace

#endif
