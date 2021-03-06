/*********************************************************************
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

#include "miniclihandler.h"

#include <kauthorized.h>
#include <kprocess.h>
#include <krun.h>
#include <kservice.h>
#include <kshell.h>
#include <kstandarddirs.h>
#include <kworkspace/kworkspace.h>
#include <qfile.h>
#include <qtextdocument.h>
#include <qtextstream.h>
#include <stdio.h>

#include "minicli.h"
#include "minicliconfig.h"

namespace Kor
{

MinicliHandlerCommandUrl::MinicliHandlerCommandUrl()
    {
    ( void ) KUriFilter::self(); // trigger creation (so that there is not a race condition later if used from threads)
    }

MinicliHandler::HandledState MinicliHandlerCommandUrl::run( const QString& command, QWidget* widget, QString* error )
    {
    KUriFilterData data;
    data.setData( command );
    KUriFilter::self()->filterUri( data, Minicli::self()->finalURIFilters());
    QString cmd;
    KUrl uri = data.uri();
    if( uri.isLocalFile() && !uri.hasRef() && uri.query().isEmpty())
        cmd = uri.path();
    else
        cmd = uri.url();
    QString exec;
    switch( data.uriType())
        {
        case KUriFilterData::LocalFile:
        case KUriFilterData::LocalDir:
        case KUriFilterData::NetProtocol:
        case KUriFilterData::Help:
            {
            // No need for kfmclient, KRun does it all (David)
            ( void ) new KRun( data.uri(), widget );
            return HandledOk;
            }
        case KUriFilterData::Executable:
            {
            if( !data.hasArgsAndOptions())
                {
                // Look for desktop file
                KService::Ptr service = KService::serviceByDesktopName( cmd );
                if( service && service->isValid() && service->isApplication())
                    {
                    KRun::run( *service, KUrl::List(), widget );
                    return HandledOk;
                    }
                }
            }
            // fall-through to shell case
        case KUriFilterData::Shell:
            {
            if( KAuthorized::authorizeKAction( "shell_access" ))
                {
                exec = cmd;
                if( data.hasArgsAndOptions())
                    cmd += data.argsAndOptions();
                break;
                }
            else
                {
                *error = i18n( "<center><b>%1</b></center>\nYou do not have permission to execute this command.",
                    Qt::convertFromPlainText( cmd ));
                return HandledFailed;
                }
            }
        case KUriFilterData::Error:
        case KUriFilterData::Blocked:
            {
            *error = data.errorMsg();
            return HandledFailed;
            }
        case KUriFilterData::Unknown:
            {
            // Look for desktop file
            if( KService::Ptr service = findService( cmd ))
                {
                KRun::run( *service, KUrl::List(), widget );
                return HandledOk;
                }
            return NotHandled;
            }
        }
    if( KRun::runCommand( cmd, exec, data.iconName(), widget ))
        return HandledOk;
    *error = i18n( "<center><b>%1</b></center>\nThe specified command does not exist.",
        Qt::convertFromPlainText( cmd ));
    return HandledFailed;
    }

MinicliHandler::HandledState MinicliHandlerCommandUrl::update( const QString& command, QString* iconName )
    {
    KUriFilterData data;
    data.setData( command );
    KUriFilter::self()->filterUri( data, Minicli::self()->progressURIFilters());
    switch( data.uriType())
        {
        case KUriFilterData::LocalFile:
        case KUriFilterData::LocalDir:
        case KUriFilterData::NetProtocol:
#if KDE_IS_VERSION( 4, 6, 0 )
        case KUriFilterData::Executable:
#endif
        case KUriFilterData::Help:
        case KUriFilterData::Shell:
            *iconName = data.iconName();
            return HandledOk;
#if ! KDE_IS_VERSION( 4, 6, 0 )
// workarounds for KUriFilter bugs
        case KUriFilterData::Executable:
            {
            QString exeName = data.uri().path();
            exeName = exeName.mid( exeName.lastIndexOf( '/' ) + 1 ); // strip path if given
            KService::Ptr service = KService::serviceByDesktopName( exeName );
            if (service && service->icon() != QLatin1String( "unknown" ))
                *iconName = service->icon();
            else if ( !KIconLoader::global()->iconPath( exeName, KIconLoader::NoGroup, true ).isNull() )
                *iconName = exeName;
            else
                // not found, use default
                *iconName = QLatin1String("system-run");
            return HandledOk;
            }
#endif
        case KUriFilterData::Error:
            *iconName = "dialog-error";
            return HandledFailed;
        case KUriFilterData::Blocked:
            *iconName = "object-locked";
            return HandledFailed;
        case KUriFilterData::Unknown:
            {
            KUrl uri = data.uri();
            if( uri.isLocalFile() && !uri.hasRef() && uri.query().isEmpty())
                {
                if( KService::Ptr service = findService( uri.path()))
                    {
                    *iconName = service->icon();
                    return HandledOk;
                    }
                }
            return NotHandled;
            }
        }
    abort();
    }

KService::Ptr MinicliHandlerCommandUrl::findService( const QString& cmd )
    {
    KService::Ptr service = KService::serviceByDesktopName( cmd );
    if( service && service->isValid() && service->isApplication())
        return service;
    service = KService::serviceByName( cmd );
    if( service && service->isValid() && service->isApplication())
        return service;
    return KService::Ptr();
    }

MinicliHandler::HandledState MinicliHandlerSpecials::run( const QString& command, QWidget*, QString* error )
    {
    if( command == "logout" )
        {
        KWorkSpace::propagateSessionManager();
        KWorkSpace::requestShutDown();
#if 0 // TODO requestShutDown() no longer returns bool
            {
            *error = i18n( "Failed to contact the session manager" );
            return HandledFailed;
            }
#endif
        return HandledOk;
        }
    return NotHandled;
    }

MinicliHandler::HandledState MinicliHandlerSpecials::update( const QString& command, QString* iconName )
    {
    if( command == "logout" )
        {
        *iconName = "system-log-out";
        return HandledOk;
        }
    return NotHandled;
    }

MinicliHandler::HandledState MinicliHandlerCalculator::run( const QString& command, QWidget*, QString* error )
    {
    if( !command.startsWith( '=' ))
        return NotHandled;
    QString exp = command.mid( 1 );
    QString cmd;
    const QString bc = KStandardDirs::findExe( "bc" );
    if( !bc.isEmpty())
        cmd = QString( "echo %1 | %2" ).arg( KShell::quoteArg( QString("scale=8; ") + exp ), KShell::quoteArg( bc ));
    else
        cmd = QString( "echo $((%1)) ").arg( exp );
    if( FILE* fs = popen( QFile::encodeName( cmd ), "r" ))
        {
        QString result = QTextStream( fs ).readAll().trimmed();
        pclose(fs);
        *error = '=' + result;
        return HandledOk;
        }
    return HandledFailed;
    }

MinicliHandler::HandledState MinicliHandlerCalculator::update( const QString& command, QString* iconName )
    {
    if( !command.startsWith( '=' ))
        return NotHandled;
    *iconName = "accessories-calculator";
    return HandledOk;
    }

} // namespace
