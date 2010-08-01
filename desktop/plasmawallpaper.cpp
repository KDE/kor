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

#include "plasmawallpaper.h"

#include <assert.h>
#include <kconfiggroup.h>
#include <qpainter.h>

namespace Kor
{

// based on plasma's wallpaperviewer

PlasmaWallpaper::PlasmaWallpaper()
    : plasma( NULL )
    {
    }

PlasmaWallpaper::~PlasmaWallpaper()
    {
    delete plasma;
    }

void PlasmaWallpaper::load( const QString& id )
    {
    configId = id;
    KConfigGroup cfg( KGlobal::config(), id );
    plasma = Plasma::Wallpaper::load( cfg.readEntry( "PluginName" ));
    if( plasma != NULL )
        {
#if KDE_IS_VERSION( 4, 2, 0 )
        QString mode = cfg.readEntry( "Mode" );
        if( !mode.isEmpty())
            plasma->setRenderingMode( mode );
#endif
        connect( plasma, SIGNAL( update( QRectF )), this, SLOT( update( QRectF )));
        plasma->setBoundingRect( QRect( 0, 0, size.width(), size.height()));
        plasma->restore( plasmaConfigGroup());
// TODO        connect(m_wallpaper, SIGNAL(configNeedsSaving()), this, SLOT(syncConfig()));
        pixmap = QPixmap( size );
        }
    else
        pixmap = QPixmap();
    }

void PlasmaWallpaper::setSize( const QSize& s )
    {
    Wallpaper::setSize( s );
    if( plasma != NULL )
        {
        plasma->setBoundingRect( QRect( 0, 0, size.width(), size.height()));
        pixmap = QPixmap( size );
        update();
        }
    }

void PlasmaWallpaper::update()
    {
    update( QRectF( 0, 0, size.width(), size.height()));
    }

void PlasmaWallpaper::update( QRectF r )
    {
    if( plasma != NULL )
        {
        QPainter p( &pixmap );
        plasma->paint( &p, r );
        p.end();
        emit loaded( pixmap );
        }
    }

KConfigGroup PlasmaWallpaper::plasmaConfigGroup()
    {
    assert( plasma != NULL );
    KConfigGroup wallpaperConfig( KGlobal::config(), configId );
    return KConfigGroup( &wallpaperConfig, plasma->pluginName());
    }

} // namespace

#include "plasmawallpaper.moc"
