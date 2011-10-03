/********************************************************************
 Copyright 2011 Lubos Lunak <l.lunak@kde.org>

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

#include "desktopsonscreens.h"

#include <kapplication.h>

#include "panel.h"

namespace Kor
{

static const unsigned long props[] = { 0, NET::WM2CurrentDesktopForScreens };

DesktopsOnScreensApplet::DesktopsOnScreensApplet( Kor::Panel* panel )
    : QLabel( panel->window())
    , Applet( panel )
    , rootInfo( QX11Info::display(), props, 2 )
    {
    setAutoFillBackground( true );
    kapp->installX11EventFilter( this );
    }

void DesktopsOnScreensApplet::load( const QString& )
    {
    // no config
    updateStatus();
    }

bool DesktopsOnScreensApplet::x11Event( XEvent* e )
    {
    if( e->xany.window == QX11Info::appRootWindow())
        {
        unsigned long m[ 5 ];
	rootInfo.event( e, m, 5 );
	if( m[ NETRootInfo::PROTOCOLS2 ] & NET::WM2CurrentDesktopForScreens )
	    updateStatus();
        }
    return QWidget::x11Event( e );
    }

void DesktopsOnScreensApplet::updateStatus()
    {
    if( !qApp->desktop()->isVirtualDesktop())
        {
        setText( "???" );
        return;
        }
    QString txt;
    int numScreens = qApp->desktop()->screenCount();
    for( int scr = 0;
         scr < numScreens;
         ++scr )
        {
        if( !txt.isEmpty())
            txt += " | ";
        txt += QString::number( rootInfo.currentDesktopForScreen( scr ));
        }
    setText( txt );
    }

} // namespace

#include "desktopsonscreens.moc"
