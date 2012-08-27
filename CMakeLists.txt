project(ktuberling)

if(NOT COMPILING_KDEGAMES)
	find_package(KDE4 REQUIRED)
	include(KDE4Defaults)
	include(MacroLibrary)
	find_package(KDEGames REQUIRED)

	add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
	add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
	include_directories(${KDEGAMES_INCLUDE_DIRS} ${KDE4_INCLUDES} ${QDBUS_INCLUDE_DIRS})
endif(NOT COMPILING_KDEGAMES)

add_subdirectory( sounds ) 
add_subdirectory( pics ) 
add_subdirectory( doc )

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





