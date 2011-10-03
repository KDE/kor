include(CheckCXXSourceCompiles)

find_package( KDE4 REQUIRED )
include( MacroLibrary )
include( KDE4Defaults )

macro_push_required_vars()
set(CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES};${KDE4_INCLUDES}")
set(CMAKE_REQUIRED_LIBRARIES "${CMAKE_REQUIRED_LIBRARIES};${KDE4_KDEUI_LIBS}")
check_cxx_source_compiles("
#include <netwm.h>

int main()
    {
    NETRootInfo2( NULL, 0 ).currentDesktopForScreen( 0 );
    return 0;
    }
" HAVE_NETROOTINFO2_CURRENTDESKTOPFORSCREEN)
macro_pop_required_vars()

set(HAVE_NETROOTINFO2_CURRENTDESKTOPFORSCREEN ${HAVE_NETROOTINFO2_CURRENTDESKTOPFORSCREEN} CACHE BOOL "NETWM support has desktopPerScreen feature")

mark_as_advanced(HAVE_NETROOTINFO2_CURRENTDESKTOPFORSCREEN)
