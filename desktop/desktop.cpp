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

#include "desktop.h"

#include <kconfiggroup.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kicon.h>
#include <ksharedconfig.h>
#include <kwindowsystem.h>
#include <qwidget.h>
#include <qx11info_x11.h>

#include <kdeversion.h>
#include <qapplication.h>
#include <qdesktopwidget.h>

#include <X11/Xlib.h>

#include "startupsuspendhandler.h"
#include "x11utils.h"
#include "wallpaper.h"

namespace Kor
{

Desktop::Desktop( const QString& id, QObject* parent )
    : QObject( parent )
    , id( id )
    , window( new QWidget )
    {
    StartupSuspendHandler::self()->suspend( this );
    window->setWindowRole( id.toLower());
    window->setWindowIcon( KIcon( "user-desktop" ));
    KWindowSystem::setType( window->winId(), NET::Desktop );
    KWindowSystem::setOnAllDesktops( window->winId(), true );
    // see wallpaperLoaded()
    window->setAttribute( Qt::WA_PaintOnScreen, true );
    window->setAttribute( Qt::WA_OpaquePaintEvent, true );
    loadConfig();
    window->show();
    }

void Desktop::loadConfig()
    {
    KConfigGroup cfg( KGlobal::config(), id );
    configuredScreen = cfg.readEntry( "Screen", qApp->desktop()->primaryScreen());
    updatePosition();
    loadWallpaperConfig( cfg.readEntry( "Wallpaper" ));
    }

void Desktop::loadWallpaperConfig( const QString& id )
    {
    KConfigGroup cfg( KGlobal::config(), id );
    wallpaper.reset( Wallpaper::create( cfg.readEntry( "WallpaperType" )));
    if( wallpaper.data() != NULL )
        {
        connect( wallpaper.data(), SIGNAL( loaded( QPixmap )), this, SLOT( wallpaperLoaded( QPixmap )));
        wallpaper->setSize( window->size());
        wallpaper->load( id );
        }
    }

void Desktop::updatePosition()
    {
    int screen = qBound( 0, configuredScreen, qApp->desktop()->numScreens() - 1 );
    QRect screenGeom = qApp->desktop()->screenGeometry( screen );
    window->setGeometry( screenGeom );
    }

void Desktop::wallpaperLoaded( QPixmap pixmap )
    {
    // Simply set the window as the background pixmap of the X window. That will avoid the requirement
    // to keep the pixmap here and avoid flicker or a need for manual repaints.
    XSetWindowBackgroundPixmap( QX11Info::display(), window->winId(), toX11Pixmap( pixmap ).handle());
    XClearWindow( QX11Info::display(), window->winId());
    StartupSuspendHandler::self()->resume( this );
    }

} // namespace

#include "desktop.moc"
