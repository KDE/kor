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

#ifndef KOR_PANELWINDOW_H
#define KOR_PANELWINDOW_H

#include <qwidget.h>

namespace Kor
{

class Panel;

class PanelWindow
    : public QWidget
    {
    Q_OBJECT
    public:
        PanelWindow( Panel* panel );
    private:
        Panel* panel;
    };

} // namespace

#endif
