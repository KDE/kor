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

#include "applet.h"

#include <qapplication.h>
#include <qdesktopwidget.h>

#include "panel.h"

#include "clock/clock.h"
#include "hello/hello.h"
#include "plasma/plasmaapplet.h"
#include "spacer.h"

namespace Kor
{

Applet* Applet::create( const QString& type, Panel* panel )
    {
    if( type == "Plasma" )
        return new PlasmaApplet( panel );
    if( type == "Spacer" )
        return new Spacer( panel );
    if( type == "Hello" )
        return new HelloApplet( panel );
    if( type == "Clock" )
        return new ClockApplet( panel );
    return NULL;
    }

QPoint Applet::popupPosition( QWidget* popup, const QWidget* source, const QPoint& offset )
    {
    popup->adjustSize(); // make sure it has already a proper size
    Panel::MainEdge edge = panel->mainEdge();
    QRect r;
    if( source->isTopLevel())
        r = source->geometry();
    else
        {
        r = QRect( source->mapToGlobal( QPoint( 0, 0 )), QSize( source->width(), source->height()));
        switch( edge )
            {
            case Panel::MainEdgeLeft:
            case Panel::MainEdgeRight:
                r.setLeft( source->topLevelWidget()->x());
                r.setWidth( source->topLevelWidget()->width());
                break;
            case Panel::MainEdgeTop:
            case Panel::MainEdgeBottom:
                r.setTop( source->topLevelWidget()->y());
                r.setHeight( source->topLevelWidget()->height());
                break;
            }
        }
    QDesktopWidget* desktop = QApplication::desktop();
    QRect screen = desktop->screenGeometry( desktop->screenNumber( source ));
    switch( edge )
        {
        case Panel::MainEdgeLeft:
        case Panel::MainEdgeRight:
            {
            int x = ( edge == Panel::MainEdgeRight ) ? r.left() - popup->width() : r.right() + 1;
            int y = qMin( r.top() + offset.y(), screen.bottom() - popup->height() + 1 );
            return QPoint( x, y );
            }
        case Panel::MainEdgeTop:
        case Panel::MainEdgeBottom:
            {
            int x = 0;
            int y = ( edge == Panel::MainEdgeBottom ) ? r.top() - popup->height() : r.bottom() + 1;
            if( QApplication::isRightToLeft())
                {
                x = r.right() - popup->width() + 1;
                if( offset.x() > 0)
                    x -= r.width() - offset.x();
                // try to keep this on the screen
                if( x - popup->width() < screen.left())
                    x = screen.left();
                }
            else
                x = qMin( r.left() + offset.x(), screen.right() - popup->width() + 1 );
            return QPoint( x, y );
            }
        }
    }


} // namespace
