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
#include <krun.h>
#include <kservice.h>
#include <qtextdocument.h>

namespace Kor
{

MinicliHandler::HandledState MinicliHandlerCommandUrl::run( const QString& command, QWidget* widget, QString* error )
    {
    data.setData( command );
    KUriFilter::self()->filterUri( data );
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

} // namespace
