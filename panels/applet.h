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

#ifndef KOR_APPLET_H
#define KOR_APPLET_H

#include <qpoint.h>
#include <qstring.h>

class QWidget;

namespace Kor
{

class Panel;

class Applet
    {
    public:
        static Applet* create( const QString& type, Panel* panel );
        virtual void load( const QString& id ) = 0;
        virtual ~Applet();
    protected:
        Applet( Panel* panel );
        /**
         * Calculate the appropriate position for a popup menu based on the size of the menu,
         * the widget geometry, and a optional point in the local coordinates of the widget.
         */
        QPoint popupPosition( QWidget* popup, const QWidget* source, const QPoint& offset = QPoint( 0, 0 ));
    protected:
        Panel* const panel;
    };

inline
Applet::Applet( Panel* panel )
    : panel( panel )
    {
    }

inline
Applet::~Applet()
    {
    }

} // namespace

#endif
