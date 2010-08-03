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

#include "hello.h"

#include <kconfiggroup.h>
#include <kglobal.h>
#include <ksharedconfig.h>

#include "panel.h"

namespace Kor
{

HelloApplet::HelloApplet( Kor::Panel* panel )
    : QLabel( panel->window())
    , Applet( panel )
    {
    setAutoFillBackground( true );
    }

void HelloApplet::load( const QString& id )
    {
    KConfigGroup cfg( KGlobal::config(), id );
    setText( cfg.readEntry( "Text", "Hello" ));
    QPalette p = palette();
    p.setColor( QPalette::Background, cfg.readEntry( "Background", QColor( Qt::cyan )));
    p.setColor( QPalette::WindowText, cfg.readEntry( "Color", QColor( Qt::black )));
    setPalette( p );
    }

} // namespace

#include "hello.moc"
