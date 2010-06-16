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

#include "panel.h"

#include <kconfiggroup.h>
#include <kdebug.h>
#include <kephal/screens.h> // TODO is ABI stable?
#include <kglobal.h>
#include <ksharedconfig.h>
#include <kwindowsystem.h>

#include "applet.h"
#include "panelwindow.h"

namespace Kor
{

Panel::Panel( const QString& id, QObject* parent )
    : QObject( parent )
    , id( id )
    , window( new PanelWindow( this ))
    {
    KWindowSystem::setType( window->winId(), NET::Dock );
    KWindowSystem::setOnAllDesktops( window->winId(), true );
    loadConfig();
    loadApplets();
    window->show();
    }

void Panel::loadConfig()
    {
    KConfigGroup cfg( KGlobal::config(), id );
    position = Position( cfg.readEntry( "Position", int( PositionTop ))); // TODO check
    horizontal = cfg.readEntry( "Horizontal", bool( position & ( PositionTop | PositionBottom )));
    configuredWidth = cfg.readEntry( "Width", 24 ); // TODO
    configuredLength = cfg.readEntry( "Length", int( FullLength ));
    configuredScreen = cfg.readEntry( "Screen", Kephal::ScreenUtils::primaryScreenId());
    updatePosition();
    }

void Panel::loadApplets()
    {
    KConfigGroup cfg( KGlobal::config(), id );
    foreach( const QString& appletid, cfg.readEntry( "Applets", QStringList()))
        {
        KConfigGroup appletcfg( KGlobal::config(), appletid );
        QString type = appletcfg.readEntry( "Type" );
        Applet* applet = Applet::create( type, this, window.data());
        if( applet == NULL )
            continue;
        applet->setGeometry( 0, 0, window->width(), window->height());
        applet->load( appletid );
        applets.append( applet );
        }
    }

void Panel::updatePosition()
    {
    int screen = qBound( 0, configuredScreen, Kephal::ScreenUtils::numScreens() - 1 );
    QRect screenGeom = Kephal::ScreenUtils::screenGeometry( screen );
    int width; // configuredWidth is thickness, but this is widthxheight
    int height;
    if( horizontal )
        {
        width = qMin( configuredLength, screenGeom.width());
        height = configuredWidth;
        }
    else
        {
        height = qMin( configuredLength, screenGeom.height());
        width = configuredWidth;
        }
    QPoint pos;
    switch( position )
        {
        case PositionTop:
            pos = QPoint( screenGeom.center().x() - width / 2, screenGeom.top());
            break;
        case PositionBottom:
            pos = QPoint( screenGeom.center().x() - width / 2, screenGeom.bottom() - height );
            break;
        case PositionLeft:
            pos = QPoint( screenGeom.left(), screenGeom.center().y() - height / 2 );
            break;
        case PositionRight:
            pos = QPoint( screenGeom.right() - width, screenGeom.center().y() - height / 2 );
            break;
        case PositionTopLeft:
            pos = screenGeom.topLeft();
            break;
        case PositionTopRight:
            pos = QPoint( screenGeom.right() - width, screenGeom.top());
            break;
        case PositionBottomLeft:
            pos = QPoint( screenGeom.left(), screenGeom.bottom() - height );
            break;
        case PositionBottomRight:
            pos = QPoint( screenGeom.right() - width, screenGeom.bottom() - height );
            break;
        }
    window->setGeometry( pos.x(), pos.y(), width, height );
    if( horizontal && ( position & PositionTop ))
        KWindowSystem::setExtendedStrut( window->winId(), 0, 0, 0, 0, 0, 0,
            height, pos.x(), pos.x() + width, 0, 0, 0 );
    else if( horizontal && ( position & PositionBottom ))
        KWindowSystem::setExtendedStrut( window->winId(), 0, 0, 0, 0, 0, 0,
            0, 0, 0, height, pos.x(), pos.x() + width );
    else if( !horizontal && ( position & PositionLeft ))
        KWindowSystem::setExtendedStrut( window->winId(), width, pos.y(), pos.y() + height, 0, 0, 0,
            0, 0, 0, 0, 0, 0 );
    else if( !horizontal && ( position & PositionRight ))
        KWindowSystem::setExtendedStrut( window->winId(), 0, 0, 0, width, pos.y(), pos.y() + height,
            0, 0, 0, 0, 0, 0 );
    else
        abort();
    }

} // namespace

#include "panel.moc"
