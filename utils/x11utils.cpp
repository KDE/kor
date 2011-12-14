/****************************************************************************

 Copyright (C) 2011 Lubos Lunak        <l.lunak@kde.org>

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

****************************************************************************/

#include "x11utils.h"

#include <qpainter.h>
#include <qx11info_x11.h>

#include <qdebug.h>
#include <X11/Xlib.h>

namespace Kor
{

// Qt has qt_toX11Pixmap(), but that's sadly not public API. So the only
// option seems to be to create X11-based QPixmap using QPixmap::fromX11Pixmap()
// and draw the non-native pixmap to it.
QPixmap toX11Pixmap( const QPixmap& pix )
{
#if QT_VERSION >= 0x040500
    if( pix.handle() != 0 ) // X11 pixmap
        return pix;
    Pixmap xpix = XCreatePixmap( pix.x11Info().display(), RootWindow( pix.x11Info().display(), pix.x11Info().screen()),
        pix.width(), pix.height(), QX11Info::appDepth());
    QPixmap wrk = QPixmap::fromX11Pixmap( xpix );
    QPainter paint( &wrk );
    paint.drawPixmap( 0, 0, pix );
    paint.end();
    QPixmap ret = wrk.copy();
    wrk = QPixmap(); // reset, so that xpix can be freed (QPixmap does not own it)
    XFreePixmap( pix.x11Info().display(), xpix );
    return ret;
#else
    return pix;
#endif
}


} // namespace
