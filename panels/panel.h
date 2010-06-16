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

#ifndef KOR_PANEL_H
#define KOR_PANEL_H

#include <qobject.h>
#include <qscopedpointer.h>

namespace Kor
{

class Applet;
class PanelWindow;

class Panel
    : public QObject
    {
    Q_OBJECT
    public:
        Panel( const QString& id, QObject* parent = NULL );
    private:
        void loadConfig();
        void loadApplets();
        void updatePosition();
    private:
        QString id;
        QScopedPointer< PanelWindow > window;
        enum Position
            {
            PositionTop    = 0x01,
            PositionBottom = 0x02,
            PositionLeft   = 0x10,
            PositionRight  = 0x20,
            PositionTopLeft = PositionTop | PositionLeft,
            PositionTopRight = PositionTop | PositionRight,
            PositionBottomLeft = PositionBottom | PositionLeft,
            PositionBottomRight = PositionBottom | PositionRight,
            };
        Position position;
        bool horizontal;
        int configuredWidth;
        enum { FullLength = 1000000 };
        int configuredLength;
        int configuredScreen;
        QList< Applet* > applets;
    };

} // namespace

#endif