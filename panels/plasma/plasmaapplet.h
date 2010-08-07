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

#ifndef KOR_PLASMAAPPLET_H
#define KOR_PLASMAAPPLET_H

#include <plasma/applet.h>
#include <plasma/containment.h>
#include <plasma/corona.h>
#include <qgraphicsview.h>

#include "applet.h"

#include <X11/Xlib.h>
#include <X11/extensions/Xrender.h>
#include <fixx11h.h>

namespace Kor
{

class Panel;

// for necessary overrides
class Corona
    : public Plasma::Corona
    {
    Q_OBJECT
    public:
        virtual QRect screenGeometry( int id ) const;
    };

// based on plasma's plasmoidviewer
class PlasmaApplet
    : public QGraphicsView, public Kor::Applet
    {
    Q_OBJECT
    public:
        PlasmaApplet( Kor::Panel* panel );
        virtual ~PlasmaApplet();
        virtual void load( const QString& id );
        virtual QSize sizeHint() const;
        virtual QSize minimumSizeHint() const;
    protected:
        virtual void resizeEvent( QResizeEvent* event );
        virtual void paintEvent( QPaintEvent* event );
    private slots:
        void appletRemoved();
        void sceneRectChanged( const QRectF &rect );
        void sizeHintChanged( Qt::SizeHint which );
        void appletGeometryChanged();
        void trayHackPaint();
    private:
        QSize constrainSize( QSize s ) const;
        void checkHacks();
    private:
        Corona corona;
        Plasma::Containment* containment;
        Plasma::Applet* applet;
        QString name;
        int sizeLimit; // 0 - none, otherwise max length of the applet
        bool trayHackBlockRecursion;
        Picture trayHackPicture;
    };

} // namespace

#endif
