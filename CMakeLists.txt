project(ktuberling)

add_subdirectory( sounds ) 
add_subdirectory( pics ) 

########### next target ###############

set(ktuberling_SRCS 
   action.cpp 
   main.cpp 
   toplevel.cpp 
   playground.cpp 
   todraw.cpp 
   soundfactory.cpp 
   playgrounddelegate.cpp )

kde4_add_app_icon(ktuberling_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/hi*-app-ktuberling.png")
kde4_add_executable(ktuberling ${ktuberling_SRCS})

target_link_libraries(ktuberling  ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} kdegames ${KDE4_PHONON_LIBS} )

install(TARGETS ktuberling  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS ktuberling.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES ktuberlingui.rc  DESTINATION  ${DATA_INSTALL_DIR}/ktuberling )

kde4_install_icons( ${ICON_INSTALL_DIR}   )





