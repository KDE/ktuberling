project(ktuberling)

message(STATUS "TODO: Port KTuberling museum to new version")
# add_subdirectory( museum ) 
add_subdirectory( sounds ) 
add_subdirectory( pics ) 

########### next target ###############

set(ktuberling_SRCS 
   action.cpp 
   main.cpp 
   toplevel.cpp 
   playground.cpp 
   todraw.cpp 
   soundfactory.cpp )

set(ktuberling_icons
	hi16-app-ktuberling.png
	hi32-app-ktuberling.png
	hi48-app-ktuberling.png
	hi64-app-ktuberling.png
	hi128-app-ktuberling.png
)
kde4_add_win32_app_icon(ktuberling_SRCS ${ktuberling_icons})
kde4_add_executable(ktuberling ${ktuberling_SRCS})

target_link_libraries(ktuberling  ${KDE4_KDEUI_LIBS} kdegames ${KDE4_PHONON_LIBS} )

install(TARGETS ktuberling  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES ktuberling.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES ktuberlingui.rc  DESTINATION  ${DATA_INSTALL_DIR}/ktuberling )

kde4_install_icons( ${ICON_INSTALL_DIR}   )




