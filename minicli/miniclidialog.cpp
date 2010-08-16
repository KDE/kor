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

#include <kcompletionbox.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kwindowsystem.h>
#include <qlineedit.h>

#include "minicli.h"
#include "miniclidialogconfig.h"

namespace Kor
{

// The actual minicli dialog (the GUI). This separation allows a different UI implementation
// if needed for whatever reason.
MinicliDialog::MinicliDialog( Minicli* minicli )
    : minicli( minicli )
    {
    setPlainCaption( i18n( "Run Command" ));
    setWindowIcon( KIcon( "system-run" ));
    setWindowRole( "minicli" );
    setButtons( KDialog::Ok | KDialog::Cancel );
    widget = new QWidget( this );
    setMainWidget( widget );
    ui.setupUi( widget );
    ui.command->setDuplicatesEnabled( false );
    showButtonSeparator( true );
    connect( ui.command, SIGNAL( editTextChanged( const QString& )), this, SLOT( textChanged( const QString& )));
    // Popup completion generally should not also accept the dialog, but for minicli it's very convenient
    // this way. However prevent the normal dialog handling on Enter to avoid double accepting (e.g. two
    // error dialogs on something incorrect).
    ui.command->setTrapReturnKey( true );
    connect( ui.command, SIGNAL( returnPressed()), this, SLOT( accept()));
    adjustSize();
    readConfig();
    }

MinicliDialog::~MinicliDialog()
    {
    writeConfig();
    }

void MinicliDialog::activate()
    {
    reset();
    ui.command->setFocus();
    if( isVisible())
        { // TODO do this properly
        KWindowSystem::activateWindow( winId());
        return;
        }
    // TODO positioning for other WMs? KWin can place it properly itself
    KWindowSystem::setState( winId(), NET::KeepAbove ); // TODO
    show();
    }

// KDialog::restoreDialogSize()

void MinicliDialog::reset()
    { // TODO compute icon size from command's sizehint->height ?
    ui.label->setFixedSize( ICON_SIZE, ICON_SIZE );
    updateIcon( QString());
    ui.command->clearEditText();
    enableButtonOk( false );
    }

void MinicliDialog::textChanged( const QString& text )
    {
    enableButtonOk( !text.isEmpty());
    minicli->commandChanged( text );
    }

void MinicliDialog::updateIcon( const QString& iconName )
    {
    QPixmap pixmap = KIconLoader::global()->loadIcon( iconName, KIconLoader::NoGroup, ICON_SIZE );
    if( !pixmap.isNull())
        ui.label->setPixmap( pixmap );
    else
        ui.label->setPixmap( KIconLoader::global()->loadIcon( "kde", KIconLoader::NoGroup, ICON_SIZE ));
    }

void MinicliDialog::accept()
    {
    QString result;
    if( !minicli->runCommand( ui.command->currentText(), &result ))
        { // TODO not modal
        if( !result.isEmpty())
            KMessageBox::sorry( this, result );
        return;
        }
    if( !result.isEmpty())
        { // set to new text, don't close dialog, don't add to history
        ui.command->lineEdit()->setText( result );
        return;
        }
    ui.command->addToHistory( ui.command->currentText());
    KDialog::accept();
    writeConfig();
    }

void MinicliDialog::readConfig()
    {
    MinicliDialogConfig config;
    ui.command->setMaxCount( config.historyLength());
    ui.command->setHistoryItems( config.history());
    // KHistoryComboBox docs suggests to do this, for whatever reason
    ui.command->completionObject()->setItems(
        config.completionItems().isEmpty() ? config.history() : config.completionItems());
    ui.command->completionObject()->setCompletionMode(
        static_cast< KGlobalSettings::Completion >( config.completionMode()));
    }

void MinicliDialog::writeConfig()
    {
    MinicliDialogConfig config;
    config.setHistory( ui.command->historyItems());
    // KHistoryComboBox docs suggests to do this, for whatever reason
    config.setCompletionItems( ui.command->completionObject()->items());
    config.setCompletionMode( ui.command->completionObject()->completionMode());
    config.writeConfig();
    }

} // namespace

#include "miniclidialog.moc"
