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

#include "minicliconfig.h"

namespace Kor
{

MinicliHandlerCommandUrl::MinicliHandlerCommandUrl()
    {
    MinicliConfig config;
    if( config.removeFinalURIFilters().count() == 1 && config.removeFinalURIFilters().first() == "all" )
        finalFilters.clear();
    else
        {
        finalFilters = KUriFilter::self()->pluginNames();
        // remove everything not wanted at all
        foreach( QString filter, config.removeFinalURIFilters())
            finalFilters.removeAll( filter );
        }
    if( config.removeProgressURIFilters().count() == 1 && config.removeProgressURIFilters().first() == "all" )
        progressFilters.clear();
    else
        {
        progressFilters = KUriFilter::self()->pluginNames();
        // remove everything not wanted when parsing while typing
        foreach( QString filter, config.removeProgressURIFilters())
            progressFilters.removeAll( filter );
        }
    }

MinicliHandler::HandledState MinicliHandlerCommandUrl::run( const QString& command, QWidget* widget, QString* error )
    {
    KURIFilterData data;
    data.setData( command );
    KUriFilter::self()->filterUri( data, finalFilters );
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
        case KUriFilterData::Unknown:
        case KUriFilterData::Error: // TODO or rather do not handle at all here?
        default:
            {
            // Look for desktop file
            KService::Ptr service = KService::serviceByDesktopName( cmd );
            if( service && service->isValid() && service->isApplication())
                {
                KRun::run( *service, KUrl::List(), widget );
                return HandledOk;
                }
            service = KService::serviceByName( cmd );
            if( service && service->isValid() && service->isApplication())
                {
                KRun::run( *service, KUrl::List(), widget );
                return HandledOk;
                }
            *error = i18n( "<center><b>%1</b></center>\nCould not run the specified command.",
                Qt::convertFromPlainText( cmd ));
            return HandledFailed;
            }
        }
    if( KRun::runCommand( cmd, exec, data.iconName(), widget ))
        return HandledOk;
    *error = i18n( "<center><b>%1</b></center>\nThe specified command does not exist.",
        Qt::convertFromPlainText( cmd ));
    return HandledFailed;
    }

MinicliHandler::HandledState MinicliHandlerSpecials::run( const QString& command, QWidget*, QString* error )
    {
    if( command == "logout" )
        {
        KWorkSpace::propagateSessionManager();
        if( !KWorkSpace::requestShutDown())
            {
            *error = i18n( "Failed to contact the session manager" );
            return HandledFailed;
            }
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

} // namespace
