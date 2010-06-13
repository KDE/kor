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

#include "minicli.h"

#include <kaction.h>
#include <kactioncollection.h>
#include <klocale.h>

#include <kdebug.h>

#include "miniclidialog.h"

namespace Kor
{

Minicli::Minicli( QObject* parent )
    : QObject( parent )
    , dialog( new MinicliDialog( this ))
    {
    KActionCollection* actions = new KActionCollection( this );
    KAction* action = actions->addAction( "showruncommand" );
    action->setText( i18n( "Show run command dialog" ));
    // TODO change to F2
    action->setGlobalShortcut( KShortcut( Qt::ALT + Qt::Key_F5 ));
    connect( action, SIGNAL( triggered( bool )), this, SLOT( showDialog()));
    }

void Minicli::showDialog()
    {
    dialog->activate();
    }

bool Minicli::runCommand( const QString& command, QString* result )
    {
    kDebug() << "Command:" << command;
    if( true ) // TODO
        {
        *result = command.trimmed();
        return true;
        }
    if( false )
        {
        *result = i18n( "Could not run the specified command" );
        return false;
        }
    }

void Minicli::commandChanged( const QString& command )
    {
    }

} // namespace

#include "minicli.moc"
