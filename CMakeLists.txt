cmake_minimum_required( VERSION 2.6 )

project( kor )

find_package( KDE4 REQUIRED )
# older KDE versions do not have this cmake module
# find_package( KDE4Workspace REQUIRED )
find_path( KDE4WORKSPACE_INCLUDE_DIR NAMES kworkspace/kworkspace.h PATHS ${KDE4_INCLUDES} )
find_library( KDE4WORKSPACE_KWORKSPACE_LIBS NAMES kworkspace PATHS ${KDE4_LIB_DIR} )
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
     startupsuspendhandler.cpp
     desktop/desktop.cpp
     desktop/plasmawallpaper.cpp
     desktop/wallpaper.cpp
     minicli/minicli.cpp
     minicli/miniclidialog.cpp
     minicli/miniclihandler.cpp
     panels/applet.cpp
     panels/panel.cpp
     panels/spacer.cpp
     panels/clock/clock.cpp
     panels/hello/hello.cpp
     panels/plasma/plasmaapplet.cpp
     )

kde4_add_ui_files( kor_SRCS
    minicli/miniclidialog.ui
    )

set( kor_KCFGC_FILES
     minicli/minicliconfig.kcfgc
     minicli/miniclidialogconfig.kcfgc
     panels/clock/clockconfig.kcfgc
     panels/hello/helloconfig.kcfgc
     panels/plasma/plasmaappletconfig.kcfgc
     )

kde4_add_kcfg_files( kor_SRCS ${kor_KCFGC_FILES} )
foreach( kcfgc ${kor_KCFGC_FILES} )
    get_filename_component( kcfgc_path ${kcfgc} PATH )
    get_filename_component( kcfgc_name ${kcfgc} NAME_WE )
    set( kor_KCFG_FILES ${kor_KCFG_FILES} ${kcfgc_path}/${kcfgc_name}.kcfg )
endforeach( kcfgc ${kor_KCFGC_FILES} )

kde4_add_kdeinit_executable( kor ${kor_SRCS} )
target_link_libraries( kdeinit_kor
    ${KDE4_KIO_LIBS}
    ${KDE4WORKSPACE_KWORKSPACE_LIBS}
    ${X11_LIBRARIES})
if( KDE4_PLASMA_LIBS )
    target_link_libraries( kdeinit_kor ${KDE4_PLASMA_LIBS} )
else( KDE4_PLASMA_LIBS )
    target_link_libraries( kdeinit_kor plasma )
endif( KDE4_PLASMA_LIBS )

# for plasma systray workaround
target_link_libraries( kdeinit_kor ${X11_Xrender_LIB} )

install( TARGETS kor ${INSTALL_TARGETS_DEFAULT_ARGS})
install( TARGETS kdeinit_kor ${INSTALL_TARGETS_DEFAULT_ARGS})
install( FILES kor.desktop  DESTINATION  ${AUTOSTART_INSTALL_DIR})
install( FILES ${kor_KCFG_FILES} DESTINATION ${KCFG_INSTALL_DIR}/kor )
install( FILES korrc DESTINATION ${CONFIG_INSTALL_DIR} )
