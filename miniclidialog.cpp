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

#include "miniclidialog.h"

#include <klocale.h>
#include <kwindowsystem.h>

namespace Kor
{

MinicliDialog::MinicliDialog()
    {
    setCaption( i18n( "Run Command" ));
    setButtons( KDialog::Ok | KDialog::Cancel );
    widget = new QWidget( this );
    setMainWidget( widget );
    ui.setupUi( widget );
    showButtonSeparator( true );
    connect( this, SIGNAL( okClicked()), SLOT( handleOk()));
    connect( this, SIGNAL( cancelClicked()), SLOT( hide()));
    connect( ui.lineedit, SIGNAL( textChanged( const QString& )), this, SLOT( textChanged( const QString& )));
    }

void MinicliDialog::activate()
    {
    reset();
    ui.lineedit->setFocus();
    if( isVisible())
        { // TODO do this properly
        KWindowSystem::activateWindow( winId());
        return;
        }
    show();
    }

// KDialog::restoreDialogSize()

void MinicliDialog::reset()
    { // TODO compute icon size from lineedit's sizehint->height ?
    const int ICON_SIZE = 32;
    ui.label->setFixedSize( ICON_SIZE, ICON_SIZE );
    ui.label->setPixmap( KIconLoader::global()->loadIcon( "kde", KIconLoader::NoGroup, ICON_SIZE ));
    ui.lineedit->clear();
    enableButtonOk( false );
    }

void MinicliDialog::textChanged( const QString& command )
    {
    enableButtonOk( !command.isEmpty());
    emit commandChanged( command );
    }

void MinicliDialog::handleOk()
    {
    emit runCommand( ui.lineedit->text());
    }

} // namespace

#include "miniclidialog.moc"
