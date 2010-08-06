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

#ifndef KOR_STARTUPSUSPENDHANDLER_H
#define KOR_STARTUPSUSPENDHANDLER_H

#include <qhash.h>
#include <qtimer.h>

namespace Kor
{

// this class is used for suspending startup in ksmserver until Kor is ready
// (after which startup can continue and hide the splash)
class StartupSuspendHandler
    : public QObject
    {
    Q_OBJECT
    public:
        static StartupSuspendHandler* self();
        void suspend( QObject* guard );
        void resume( QObject* guard );
#ifndef NDEBUG
        void startCheck();
#endif
    public slots:
        // like suspend( QObject* ), but the object will be sender()
        void suspendSlot();
        void resumeSlot();
    private slots:
        void finish();
        void check();
    private:
        StartupSuspendHandler();
        QHash< void*, int > suspendCount;
        QTimer finalTimer;
#ifndef NDEBUG
        QTimer checkTimer;
#endif
    };

} // namespace

#endif
