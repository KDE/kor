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

#include "startupsuspendhandler.h"

#include <assert.h>
#include <kdebug.h>
#include <qdbusinterface.h>

namespace Kor
{

StartupSuspendHandler* StartupSuspendHandler::self()
    {
    static StartupSuspendHandler handler;
    return &handler;
    }

StartupSuspendHandler::StartupSuspendHandler()
    : finished( false )
    {
    connect( &finalTimer, SIGNAL( timeout()), this, SLOT( finish()));
    finalTimer.setSingleShot( true );
    QDBusInterface ksmserver( "org.kde.ksmserver", "/KSMServer" );
    ksmserver.call( "suspendStartup", "kor" );
    }

#ifndef NDEBUG
void StartupSuspendHandler::startCheck()
    {
    connect( &checkTimer, SIGNAL( timeout()), this, SLOT( check()));
    checkTimer.setSingleShot( true );
    checkTimer.start( 30000 ); // for testing purposes, check if we actually resume
    }
#endif

void StartupSuspendHandler::suspend( QObject* guard )
    {
    if( finished )
        return;
    if( suspendCount.contains( guard ))
        ++suspendCount[ guard ];
    else
        suspendCount[ guard ] = 1;
    finalTimer.stop();
    }

void StartupSuspendHandler::resume( QObject* guard )
    {
    if( finished )
        return;
    if( !suspendCount.contains( guard ))
        return;
    if( --suspendCount[ guard ] == 0 )
        suspendCount.remove( guard );
    // use additional timer, since some applets have delayed init
    if( suspendCount.isEmpty())
        finalTimer.start( 100 );
    }

void StartupSuspendHandler::suspendSlot()
    {
    suspend( sender());
    }

void StartupSuspendHandler::resumeSlot()
    {
    resume( sender());
    }

void StartupSuspendHandler::finish()
    {
    finished = true;
    kDebug() << "Resuming workspace startup";
    QDBusInterface ksmserver( "org.kde.ksmserver", "/KSMServer" );
    ksmserver.call( "resumeStartup", "kor" );
    }

void StartupSuspendHandler::check()
    {
    kWarning() << "Suspended startup not resumed:";
    kWarning() << suspendCount;
    }

} // namespace

#include "startupsuspendhandler.moc"
