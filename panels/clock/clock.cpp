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
#include <kdatepicker.h>
#include <kglobal.h>
#include <klocale.h>
#include <ksharedconfig.h>
#include <kwindowsystem.h>
#include <qevent.h>

#include "clockconfig.h"
#include "panel.h"

namespace Kor
{

ClockApplet::ClockApplet( Kor::Panel* panel )
    : QLabel( panel->window())
    , Applet( panel )
    , datePicker( NULL )
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

void ClockApplet::mousePressEvent( QMouseEvent* event )
    {
    QLabel::mousePressEvent( event );
    if( event->button() != Qt::LeftButton )
        return;
    if( datePicker == NULL )
        {
        // TODO
        // handle things from kicker (close on escape, etc.)
        // position properly (global function?)

        datePicker = new KDatePicker;
        datePicker->setParent( this, Qt::Tool ); // KDatePicker doesn't take Qt::WindowFlags in its ctor
        KWindowSystem::setMainWindow( datePicker, window()->winId());
        // It is convenient to have the date picker kept on top by default, as that makes
        // it easy to focus something else and have the (temporary, anyway) date picker still visible.
        KWindowSystem::setState( datePicker->winId(), NET::KeepAbove );
        datePicker->setAttribute( Qt::WA_DeleteOnClose );
        connect( datePicker, SIGNAL( destroyed()), this, SLOT( datePickerDeleted()));
        datePicker->show();
        // Activate the window. KWin will not activate the window automatically, because the panel is not active.
        // This call activates it normally (not forced), because the user activity is in the panel (user timestamp).
        // Other calls such as QWidget::activateWindow() are kind of broken for this.
        KWindowSystem::activateWindow( datePicker->winId());
        }
    else
        {
        delete datePicker;
        datePicker = NULL;
        }
    }

void ClockApplet::datePickerDeleted()
    {
    datePicker = NULL;
    }

} // namespace

#include "clock.moc"
