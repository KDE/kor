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

#include "clock.h"

#include <kconfiggroup.h>
#include <kglobal.h>
#include <klocale.h>
#include <ksharedconfig.h>

#include "clockconfig.h"
#include "panel.h"

namespace Kor
{

ClockApplet::ClockApplet( Kor::Panel* panel )
    : QLabel( panel->window())
    , Applet( panel )
    {
    connect( &timer, SIGNAL( timeout()), this, SLOT( updateClock()));
    }

void ClockApplet::load( const QString& id )
    {
    ClockAppletConfig config( id );
    showSeconds = config.showSeconds();
    timer.start( 1000 ); // TODO longer interval if not showing seconds
    updateClock();
    }

void ClockApplet::updateClock()
    {
    setText( KGlobal::locale()->formatTime( QTime::currentTime(), showSeconds ));
    update();
    }

} // namespace

#include "clock.moc"
