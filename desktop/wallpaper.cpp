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

#include "wallpaper.h"

#include <kconfiggroup.h>
#include <kglobal.h>
#include <ksharedconfig.h>

#include <qdebug.h>

namespace Kor
{

Wallpaper* Wallpaper::create( const QString& type )
    {
    if( type == "Color" )
        return new WallpaperColor;
    if( type == "Image" )
        return new WallpaperImage;
    return NULL;
    }

void Wallpaper::setSize( const QSize& s )
    {
    size = s;
    }

void WallpaperColor::load( const QString& id )
    {
    KConfigGroup cfg( KGlobal::config(), id );
    // use QColor ctor rather than readEntry( QColor ) because KConfig can't handle e.g. 'green'
    color = QColor( cfg.readEntry( "Color" )); 
    QImage image( 1, 1, QImage::Format_RGB32 ); // 1,1 size is enough, it will be tilled
    image.fill( color.rgb());
    emit loaded( image );
    }

void WallpaperImage::load( const QString& id )
    {
    KConfigGroup cfg( KGlobal::config(), id );
    QImage image( cfg.readEntry( "File" ));
    if( !image.isNull())
        {
        if( image.size() != size )
            image = image.scaled( size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
        emit loaded( image );
        }
    }

} // namespace

#include "wallpaper.moc"
