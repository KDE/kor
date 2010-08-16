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

#ifndef KOR_MINICLIHANDLER_H
#define KOR_MINICLIHANDLER_H

#include <kservice.h>
#include <kurifilter.h>

namespace Kor
{

class MinicliHandler
    {
    public:
        virtual ~MinicliHandler() {};
        enum HandledState { HandledOk, HandledFailed, NotHandled };
        /** Run the command, if possible. If the command cannot be handled at all
            (for example, the handler is a calculator but the command doesn't start with '='),
            return NotHandled. Otherwise try to run it and return whether the running was succesful
            or not. If there was a problem, set result to error message. If result is set when
            the command was handled successfully, this result will be presented to the user.
            @arg widget window for associating commands with
        **/
        virtual HandledState run( const QString& command, QWidget* widget, QString* error ) = 0;
        /** Similar to run(), called when the user has typed something but has not confirmed it yet,
            so it can be used to update the icon showed for the command.
        **/
        virtual HandledState update( const QString& command, QString* iconName ) = 0;
    };

class MinicliHandlerCommandUrl
    : public MinicliHandler
    {
    public:
        MinicliHandlerCommandUrl();
        virtual HandledState run( const QString& command, QWidget* widget, QString* error );
        virtual HandledState update( const QString& command, QString* iconName );
    private:
        KService::Ptr findService( const QString& cmd );
        QStringList progressFilters; // used to update minicli status (e.g. icon) before Enter is pressed
        QStringList finalFilters; // used when Enter is pressed
    };

class MinicliHandlerSpecials
    : public MinicliHandler
    {
    public:
        virtual HandledState run( const QString& command, QWidget* widget, QString* error );
        virtual HandledState update( const QString& command, QString* iconName );
    };

class MinicliHandlerCalculator
    : public MinicliHandler
    {
    public:
        virtual HandledState run( const QString& command, QWidget* widget, QString* error );
        virtual HandledState update( const QString& command, QString* iconName );
    };

} // namespace

#endif
