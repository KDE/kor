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
#include <kephal/screens.h> // TODO is ABI stable?
#include <kglobal.h>
#include <ksharedconfig.h>
#include <kwindowsystem.h>
#include <qwidget.h>

namespace Kor
{

Desktop::Desktop( const QString& id, QObject* parent )
    : QObject( parent )
    , id( id )
    , window( new QWidget )
    {
    KWindowSystem::setType( window->winId(), NET::Desktop );
    KWindowSystem::setOnAllDesktops( window->winId(), true );
    loadConfig();
    window->show();
    }

void Desktop::loadConfig()
    {
    KConfigGroup cfg( KGlobal::config(), id );
    configuredScreen = cfg.readEntry( "Screen", Kephal::ScreenUtils::primaryScreenId());
    updatePosition();
    }

void Desktop::updatePosition()
    {
    int screen = qBound( 0, configuredScreen, Kephal::ScreenUtils::numScreens() - 1 );
    QRect screenGeom = Kephal::ScreenUtils::screenGeometry( screen );
    window->setGeometry( screenGeom );
    }

} // namespace

#include "desktop.moc"
