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

#ifndef KOR_MINICLIDIALOG_H
#define KOR_MINICLIDIALOG_H

#include <kdialog.h>

#include "ui_miniclidialog.h"

namespace Kor
{

class Minicli;

// Implementation of the actual UI of minicli, without the logic.
// This allows a different UI if needed for whatever reason.
class MinicliDialog
    : public KDialog
    {
    Q_OBJECT
    public:
        MinicliDialog( Minicli* minicli );
        virtual ~MinicliDialog();
        void activate();
    protected:
        virtual void accept();
    private slots:
        void textChanged( const QString& text );
    private:
        void reset();
        void readConfig();
        void writeConfig();
        QWidget* widget;
        Ui::MinicliDialog ui;
        Minicli* minicli;
    };

} // namespace

#endif
