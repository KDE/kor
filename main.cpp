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

#include "main.h"

#include "application.h"

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <stdio.h>

extern "C" KDE_EXPORT
int kdemain( int argc, char* argv[] )
    {
    KAboutData about( "kor", NULL, ki18n( "Kor Testudo Shell" ), KOR_VERSION );
    about.setLicense( KAboutData::License_GPL );
    about.addAuthor( ki18n( "Luboš Luňák" ), KLocalizedString(), "l.lunak@kde.org" );
    KCmdLineArgs::init( argc, argv, &about );
    if( !KUniqueApplication::start())
        {
        fprintf( stderr, "Kor is already running." );
        return 0;
        }
    Kor::Application app;
    app.disableSessionManagement(); // Do SM, but don't restart.
    return app.exec();
    }
