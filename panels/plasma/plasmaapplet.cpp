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
#include <qtimer.h>
#include <qx11embed_x11.h>
#include <qx11info_x11.h>

#include "panel.h"
#include "plasmaappletconfig.h"

//#define DEBUG_LAYOUT

namespace Kor
{

PlasmaApplet::PlasmaApplet( Kor::Panel* panel )
    : QGraphicsView( panel->window())
    , Applet( panel )
    , applet( NULL )
    , sizeLimit( 0 )
    , trayHackBlockRecursion( false )
    , trayHackPicture( None )
    {
    setFrameStyle( NoFrame );
    setScene( &corona );
    connect( &corona, SIGNAL( sceneRectChanged( QRectF )), this, SLOT( sceneRectChanged( QRectF )));
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setAlignment( Qt::AlignCenter );
    setBackgroundRole( QPalette::Window ); // QGraphicsView sets this to Base, undo that
    }

PlasmaApplet::~PlasmaApplet()
    {
    if( trayHackPicture != None )
        XRenderFreePicture( x11Info().display(), trayHackPicture );
    }

void PlasmaApplet::load( const QString& id )
    {
    PlasmaAppletConfig cfg( id );
    // We would prefer if the containment didn't write any configuration, as it is always manually set up
    // by us, but Plasma code makes it write something even if save() is made empty, so make sure it
    // keeps its Plasma id, otherwise the config will grow uselessly. Additionally Plasma applets use
    // also their containment's id for saving config, so remembering the id is needed for them
    // to reliably remember their config. Moreover Plasma seems to be built with the idea that user
    // code creates things manually, gets ids assigned by Plasma code and since then this setup is
    // written and restore by Plasma code, but we want manual setup always, and Plasma API doesn't allow
    // setting the containment id this way. So use a hack (see fixId() in our Containment ctor).
    // Additionally CoronaPrivate::addContainment() deletes "old stale" config data if it thinks
    // the containment is new (which it thinks here, because of the inability to pass in the id),
    // so temporarily save the proper config group to a temporary KConfig object and restore,
    // otherwise the whole configuration (including the applet config) is lost.
    int containmentid = checkPlasmaId( cfg.plasmaContainmentId(), cfg );
    KConfigGroup coronaconf( corona.config(), "Containments" );
    KConfigGroup keepconf( &coronaconf, QString::number( containmentid ));
    KConfig tmpconf( QString(), KConfig::SimpleConfig );
    KConfigGroup saveconf( &tmpconf, "tmp" );
    keepconf.copyTo( &saveconf );
    containment = corona.addContainment( "korinternal", QVariantList() << containmentid );
    saveconf.copyTo( &keepconf );

    if( containment->id() != cfg.plasmaContainmentId())
        {
        cfg.setPlasmaContainmentId( containment->id());
        cfg.setTakenIds( cfg.takenIds() += containment->id());
        }
    containment->setFormFactor( panel->horizontal() ? Plasma::Horizontal : Plasma::Vertical );
    switch( panel->mainEdge())
        {
        case Panel::MainEdgeTop:
            containment->setLocation( Plasma::TopEdge );
            break;
        case Panel::MainEdgeBottom:
            containment->setLocation( Plasma::BottomEdge );
            break;
        case Panel::MainEdgeLeft:
            containment->setLocation( Plasma::LeftEdge );
            break;
        case Panel::MainEdgeRight:
            containment->setLocation( Plasma::RightEdge );
            break;
        }
    sizeLimit = cfg.sizeLimit();
    containment->resize( constrainSize( size()));
    setScene( containment->scene());
    setSceneRect( containment->geometry());
    name = cfg.plasmaName();
    applet = Plasma::Applet::load( name, checkPlasmaId( cfg.plasmaAppletId(), cfg ));
    if( applet != NULL )
        {
        if( applet->id() != cfg.plasmaAppletId()) // save the plasma applet id for the next time, otherwise plasma
            {
            cfg.setPlasmaAppletId( applet->id()); // will often assign different ids and lose applet config
            cfg.setTakenIds( cfg.takenIds() += applet->id());
            }
        applet->setFlag( QGraphicsItem::ItemIsMovable, false );
        // Here addApplet is intentionally called with the default dontInit = true, as that prevents
        // the annoying zoom-in animation. The two following calls are required to do the init.
        containment->addApplet( applet );
        applet->init();
        applet->flushPendingConstraintsEvents();
        connect( applet, SIGNAL( sizeHintChanged( Qt::SizeHint )), this, SLOT( sizeHintChanged( Qt::SizeHint )));
        connect( applet, SIGNAL( geometryChanged()), this, SLOT( appletGeometryChanged()));
        appletGeometryChanged();
        delete applet->action( "remove" ); // remove default popup menu actions that do not fit
        checkHacks();
        }
    connect( containment, SIGNAL( appletRemoved( Plasma::Applet* )), this, SLOT( appletRemoved()));
    cfg.writeConfig(); // TODO lame, kconfigxt needs this explicitly
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
    QSizePolicy sizePolicy( applet->sizePolicy());
    // Reset height-for-width, it does not seem to work right with systemtray, which has this set,
    // but in vertical mode the width of size hints is sometimes 0, resulting in height 0 as well.
    // Instead let the applet sort out its size and then use that.
    sizePolicy.setHeightForWidth( false );
#if QT_VERSION < 0x040800
    sizePolicy.setWidthForHeight( false );
#endif
    setSizePolicy( sizePolicy );
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

void PlasmaApplet::checkHacks()
    {
    // Systray implementation puts a widgets with a foreign visual inside the toplevel and Qt's
    // painting code doesn't expect that, so the embedded systray icon may not be visible
    // (it works with Plasma-desktop though since that one is always forced for 32bpp visual).
    // So just render into a pixmap instead and then blit directly to screen.
    if( name == "systemtray" )
        {
        XRenderPictureAttributes attrs;
        attrs.subwindow_mode = IncludeInferiors;
        trayHackPicture = XRenderCreatePicture( x11Info().display(), winId(),
            XRenderFindStandardFormat( x11Info().display(), PictStandardRGB24 ), CPSubwindowMode, &attrs );
        }
    }

void PlasmaApplet::paintEvent( QPaintEvent* event )
    {
    if( trayHackPicture != None && !trayHackBlockRecursion )
        QTimer::singleShot( 0, this, SLOT( trayHackPaint()));
    QGraphicsView::paintEvent( event );
    }

void PlasmaApplet::trayHackPaint()
    {
    // Actually use the hack only for the systray icons which need it. Just using a full render()
    // would trigger code for non-alpha xembed-based icons that'd after a delay
    // call repaint() again, triggering this code again, and so on.
    QRegion reg;
    foreach( QWidget* w, findChildren< QX11EmbedContainer* >())
        if( w->x11Info().visual() != x11Info().visual())
            reg += QRect( w->mapTo( this, QPoint( 0, 0 )), w->size());
    if( reg.isEmpty())
        return;
    QPixmap pix(size());
    trayHackBlockRecursion = true;
    QWidget::render( &pix, reg.boundingRect().topLeft(), reg );
    trayHackBlockRecursion = false;
    XRenderSetPictureClipRegion( x11Info().display(), trayHackPicture, reg.handle());
    XRenderComposite( x11Info().display(), PictOpSrc, pix.x11PictureHandle(), None, trayHackPicture,
        0, 0, 0, 0, 0, 0, width(), height());
    }

int PlasmaApplet::checkPlasmaId( int id, const PlasmaAppletConfig& cfg )
    {
    if( id != 0 )
        return id; // assume this is wanted, even if there may be a duplicate
    // make sure a newly created id wouldn't conflict with some other id we use
    id = 1;
    foreach( int used, cfg.takenIds())
        id = qMax( id, used + 1);
    return id;
    }

// this needs to be overriden for some reason
QRect Corona::screenGeometry( int id ) const
    {
    return qApp->desktop()->screenGeometry( id );
    }

} // namespace

#include "plasmaapplet.moc"
