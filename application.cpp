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

#include "application.h"

#include <kauthorized.h>

#include "minicli.h"
#include "panel.h"

namespace Kor
{

Application::Application()
    {
    KConfigGroup cfg( KGlobal::config(), "Layout" );
    if( cfg.readEntry( "Minicli", true ) && KAuthorized::authorizeKAction( "run_command" ))
        ( void ) new Minicli( this );
    foreach( const QString& panelid, cfg.readEntry( "Panels", QStringList()))
        ( void ) new Panel( panelid, this ); // TODO
    setQuitOnLastWindowClosed( false );
    }

} // namespace

#include "application.moc"
