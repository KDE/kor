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

#include <kwindowsystem.h>

#include "panelwindow.h"

namespace Kor
{

Panel::Panel( QObject* parent )
    : QObject( parent )
    , window( new PanelWindow( this ))
    {
    KWindowSystem::setType( window->winId(), NET::Dock );
    KWindowSystem::setOnAllDesktops( window->winId(), true );
    window->setGeometry( 400, 0, 200, 100 );
    window->loadApplets();
    window->show();
    }

} // namespace

#include "panel.moc"
