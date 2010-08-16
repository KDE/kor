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

#ifndef KOR_MINICLI_H
#define KOR_MINICLI_H

#include <kdialog.h>
#include <qthread.h>
#include <qtimer.h>

#include "minicliconfig.h"

namespace Kor
{

class MinicliDialog;
class MinicliHandler;

class Minicli
    : public QObject
    {
    Q_OBJECT
    public:
        Minicli( QObject* parent = NULL );
        virtual ~Minicli();
        static Minicli* self();
        void commandChanged( const QString& command );
        bool runCommand( const QString& command, QString* result );
        QStringList finalURIFilters() const;
        QStringList progressURIFilters() const;
    private slots:
        void showDialog();
        void doUpdate();
        void updateThreadFinished();
    private:
        static QStringList makeURIFilters( const QStringList& remove );
        static void doUpdateInternal( const QString& text, QList< MinicliHandler* > handlers, QString* iconName );
        void cancelUpdateThread();
        MinicliDialog* dialog;
        QList< MinicliHandler* > handlers;
        MinicliConfig config;
        QTimer updateTimer;
        QString updateText;
        class UpdateThread;
        UpdateThread* updateThread;
    };

class Minicli::UpdateThread
    : public QThread
    {
    Q_OBJECT
    public:
        QString text;
        QString iconName;
        QList< MinicliHandler* > handlers;
    protected:
        virtual void run();
    };

} // namespace

#endif
