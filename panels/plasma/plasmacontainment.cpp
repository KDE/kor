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

#include "plasmacontainment.h"

namespace Kor
{

PlasmaContainment::PlasmaContainment( QObject* parent, const QVariantList& args )
    : Plasma::Containment( parent, args )
    {
    setDrawWallpaper( false );
    }

} // namespace

// TODO it would be simpler to use Plasma::PluginLoader() and create the internal containment
// directly, but the class is only since 4.6, so somewhen later
K_EXPORT_PLASMA_APPLET( korinternal, Kor::PlasmaContainment )

#include "plasmacontainment.moc"
