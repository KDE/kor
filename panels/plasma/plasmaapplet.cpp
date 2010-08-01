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

#include <qaction.h>
#include <qapplication.h>
#include <qdesktopwidget.h>

#include "panel.h"

//#define DEBUG_LAYOUT

namespace Kor
{

PlasmaApplet::PlasmaApplet( Kor::Panel* panel, QWidget* parent )
    : QGraphicsView( parent )
    , Applet( panel )
    , applet( NULL )
    , sizeLimit( 0 )
    {
    setFrameStyle( NoFrame );
    setScene( &corona );
    connect( &corona, SIGNAL( sceneRectChanged( QRectF )), this, SLOT( sceneRectChanged( QRectF )));
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setAlignment( Qt::AlignCenter );
    }

void PlasmaApplet::load( const QString& id )
    {
    KConfigGroup cfg( KGlobal::config(), id );
    containment = corona.addContainment( "null" );
    containment->setFormFactor( panel->horizontal() ? Plasma::Horizontal : Plasma::Vertical );
    if( panel->horizontal() && ( panel->position() & Panel::PositionTop ))
        containment->setLocation( Plasma::TopEdge );
    else if( panel->horizontal() && ( panel->position() & Panel::PositionBottom ))
        containment->setLocation( Plasma::BottomEdge );
    else if( !panel->horizontal() && ( panel->position() & Panel::PositionLeft ))
        containment->setLocation( Plasma::LeftEdge );
    else if( !panel->horizontal() && ( panel->position() & Panel::PositionRight ))
        containment->setLocation( Plasma::RightEdge );
    else
        abort();
    sizeLimit = cfg.readEntry( "SizeLimit", 0 );
    containment->resize( constrainSize( size()));
    setScene( containment->scene());
    setSceneRect( containment->geometry());
    name = cfg.readEntry( "PlasmaName" );
    applet = Plasma::Applet::load( name, cfg.readEntry( "PlasmaAppletId", 0 ));
    if( applet != NULL )
        {
        applet->setFlag( QGraphicsItem::ItemIsMovable, false );
        // Here addApplet is intentionally called with the default dontInit = true, as that prevents
        // the annoying zoom-in animation. The two following calls are required to do the init.
        containment->addApplet( applet );
        applet->init();
        applet->flushPendingConstraintsEvents();
        connect( applet, SIGNAL( sizeHintChanged( Qt::SizeHint )), this, SLOT( sizeHintChanged( Qt::SizeHint )));
        connect( applet, SIGNAL( geometryChanged()), this, SLOT( appletGeometryChanged()));
        appletGeometryChanged();
        }
    connect( containment, SIGNAL( appletRemoved( Plasma::Applet* )), this, SLOT( appletRemoved()));
    delete applet->action( "remove" ); // remove default popup menu actions that do not fit
    }

void PlasmaApplet::appletRemoved()
    { // TODO ?
    applet = NULL;
    }

void PlasmaApplet::appletGeometryChanged()
    {
#ifdef DEBUG_LAYOUT
        qDebug() << "XXX geometry changed:" << applet->sceneBoundingRect() << applet->size() << name;
#endif
    setSceneRect( applet->sceneBoundingRect());
    resize( constrainSize( applet->size().toSize()));
    setSizePolicy( applet->sizePolicy());
    updateGeometry();
    }

void PlasmaApplet::resizeEvent( QResizeEvent *event )
    {
    QGraphicsView::resizeEvent( event );
    containment->setMaximumSize( QWIDGETSIZE_MAX, QWIDGETSIZE_MAX );
    containment->setMinimumSize( size());
    containment->setMaximumSize( size());
    containment->resize( size());
    if( applet != NULL )
        {
#ifdef DEBUG_LAYOUT
        qDebug() << "XXX resize event:" << size() << name;
#endif
        if( sizeLimit > 0 )
            applet->setMaximumSize( size());
        applet->resize( size());
        }
    }

void PlasmaApplet::sceneRectChanged( const QRectF& )
    {
    if( applet != NULL )
        {
#ifdef DEBUG_LAYOUT
        qDebug() << "XXX scene rect changed:" << applet->geometry() << name;
#endif
        setSceneRect( applet->geometry());
        }
    }

QSize PlasmaApplet::sizeHint() const
    {
    if( applet != NULL )
        {
#ifdef DEBUG_LAYOUT
        qDebug() << "XXX size hint:" << applet->effectiveSizeHint( Qt::PreferredSize ) << name;
#endif
        return constrainSize( applet->effectiveSizeHint( Qt::PreferredSize ).toSize());
        }
    return QSize();
    }

QSize PlasmaApplet::minimumSizeHint() const
    {
    if( applet != NULL )
        {
#ifdef DEBUG_LAYOUT
        qDebug() << "XXX minimum size hint:" << applet->effectiveSizeHint( Qt::MinimumSize ) << name;
#endif
        return constrainSize( applet->effectiveSizeHint( Qt::MinimumSize ).toSize());
        }
    return QSize();
    }

void PlasmaApplet::sizeHintChanged( Qt::SizeHint which )
    {
#ifdef DEBUG_LAYOUT
    qDebug() << "XXX size hint changed:" << applet->effectiveSizeHint( which ) << name;
#else
    Q_UNUSED( which );
#endif
    updateGeometry();
    }

QSize PlasmaApplet::constrainSize( QSize s ) const
    {
    if( sizeLimit > 0 )
        {
        if( panel->horizontal())
            s.setWidth( qMin( s.width(), sizeLimit ));
        else
            s.setHeight( qMin( s.height(), sizeLimit ));
        }
    return s;
    }

// this needs to be overriden for some reason
QRect Corona::screenGeometry( int id ) const
    {
    return qApp->desktop()->screenGeometry( id );
    }

} // namespace

#include "plasmaapplet.moc"
