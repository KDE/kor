cmake_minimum_required( VERSION 2.6 )

project( kor )

find_package( KDE4 REQUIRED )
include( MacroLibrary )
include( KDE4Defaults )
add_definitions( ${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DHAVE_CONFIG_H=1 )
include_directories( ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES} )

########### next target ###############

set( kor_SRCS
     main.cpp
     application.cpp
     minicli.cpp
     miniclidialog.cpp
     )

kde4_add_ui_files( kor_SRCS miniclidialog.ui )

kde4_add_kdeinit_executable( kor ${kor_SRCS} )

target_link_libraries( kdeinit_kor ${KDE4_KDEUI_LIBS} )

install( TARGETS kor ${INSTALL_TARGETS_DEFAULT_ARGS})
install( TARGETS kdeinit_kor ${INSTALL_TARGETS_DEFAULT_ARGS})
