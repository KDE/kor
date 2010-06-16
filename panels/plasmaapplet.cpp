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

#include "plasmaapplet.h"

namespace Kor
{

PlasmaApplet::PlasmaApplet( Kor::Panel*, QWidget* parent )
    : QGraphicsView( parent )
    {
    setScene( &corona );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setAlignment( Qt::AlignCenter );
    }

void PlasmaApplet::load( const QString& id ) // TODO
    {
    KConfigGroup cfg( KGlobal::config(), id );
    containment = corona.addContainment( "null" );
    containment->setFormFactor( Plasma::Horizontal ); // TODO
    containment->setLocation( Plasma::TopEdge ); // TODO
    containment->resize( size());
    setScene( containment->scene());
    setSceneRect( containment->geometry());
    applet = containment->addApplet( cfg.readEntry( "Name" ));
    if( applet != NULL )
        {
        applet->setFlag( QGraphicsItem::ItemIsMovable, false );
        applet->resize( size());
        }
    connect( containment, SIGNAL( appletRemoved( Plasma::Applet* )), this, SLOT( appletRemoved()));
    }

void PlasmaApplet::setGeometry( int x, int y, int width, int height )
    {
    QGraphicsView::setGeometry( x, y, width, height );
    }

void PlasmaApplet::appletRemoved()
    { // TODO ?
    applet = NULL;
    }

void PlasmaApplet::resizeEvent( QResizeEvent *event )
    {
    QGraphicsView::resizeEvent(event);
    containment->setMaximumSize( QWIDGETSIZE_MAX, QWIDGETSIZE_MAX );
    containment->setMinimumSize( size());
    containment->setMaximumSize( size());
    containment->resize( size());
    if( applet != NULL )
        applet->resize( size());
    }

} // namespace

#include "plasmaapplet.moc"
