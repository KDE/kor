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

#include "applet.h"

#include "hello/hello.h"
#include "panelwindow.h"
#include "plasma/plasmaapplet.h"
#include "spacer.h"

namespace Kor
{

Applet* Applet::create( const QString& type, Panel* panel, PanelWindow* window )
    {
    if( type == "Plasma" )
        return new PlasmaApplet( panel, window );
    if( type == "Spacer" )
        return new Spacer( panel, window );
    if( type == "Hello" )
        return new HelloApplet( panel, window );
    return NULL;
    }

} // namespace
