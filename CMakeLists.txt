cmake_minimum_required( VERSION 2.6 )

project( kor )

find_package( KDE4 REQUIRED )
# older KDE versions do not have this cmake module
# find_package( KDE4Workspace REQUIRED )
find_path( KDE4WORKSPACE_INCLUDE_DIR NAMES kworkspace/kworkspace.h PATHS ${KDE4_INCLUDES} )
find_library( KDE4WORKSPACE_KWORKSPACE_LIBS names kworkspace PATHS ${KDE4_LIB_DIR} )
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( KDE4Workspace DEFAULT_MSG KDE4WORKSPACE_KWORKSPACE_LIBS KDE4WORKSPACE_INCLUDE_DIR )
if( NOT KDE4WORKSPACE_FOUND )
    message( FATAL_ERROR "KDE Workspace development files not found." )
endif( NOT KDE4WORKSPACE_FOUND )
include( MacroLibrary )
include( KDE4Defaults )
add_definitions( ${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DHAVE_CONFIG_H=1 )
include_directories( ${KDE4_INCLUDES} ${KDE4WORKSPACE_INCLUDE_DIR} )

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/desktop
    ${CMAKE_SOURCE_DIR}/minicli
    ${CMAKE_SOURCE_DIR}/panels
    ${CMAKE_SOURCE_DIR}/utils
    ${CMAKE_BINARY_DIR}
    )

########### next target ###############

set( kor_SRCS
     main.cpp
     application.cpp
     desktop/desktop.cpp
     desktop/wallpaper.cpp
     minicli/minicli.cpp
     minicli/miniclidialog.cpp
     minicli/miniclihandler.cpp
     panels/applet.cpp
     panels/panel.cpp
     panels/panelwindow.cpp
     panels/spacer.cpp
     panels/hello/hello.cpp
     panels/plasma/plasmaapplet.cpp
     )

kde4_add_ui_files( kor_SRCS minicli/miniclidialog.ui )
kde4_add_kcfg_files( kor_SRCS minicli/miniclidialogconfig.kcfgc )

kde4_add_kdeinit_executable( kor ${kor_SRCS} )
target_link_libraries( kdeinit_kor
    ${KDE4_KIO_LIBS}
    ${KDE4WORKSPACE_KWORKSPACE_LIBS}
    ${KDE4_PLASMA_LIBS}
    kephal
    ${X11_LIBRARIES})

install( TARGETS kor ${INSTALL_TARGETS_DEFAULT_ARGS})
install( TARGETS kdeinit_kor ${INSTALL_TARGETS_DEFAULT_ARGS})
install( FILES kor.desktop  DESTINATION  ${AUTOSTART_INSTALL_DIR})
install( FILES minicli/miniclidialogconfig.kcfg DESTINATION ${KCFG_INSTALL_DIR} RENAME korminiclidialog.kcfg )
install( FILES korrc DESTINATION ${CONFIG_INSTALL_DIR} )
