project(ktuberling)

add_subdirectory( museum ) 
add_subdirectory( sounds ) 
add_subdirectory( pics ) 

include_directories( ${CMAKE_SOURCE_DIR}/libkdegames  )


########### next target ###############

set(ktuberling_SRCS 
   action.cpp 
   main.cpp 
   toplevel.cpp 
   playground.cpp 
   todraw.cpp 
   soundfactory.cpp )

kde4_automoc(${ktuberling_SRCS})

kde4_add_executable(ktuberling ${ktuberling_SRCS})

target_link_libraries(ktuberling  ${KDE4_KDEUI_LIBS} kdeprint kdegames ${KDE4_PHONONCORE_LIBRARIES} )

install_targets(/bin ktuberling )


########### install files ###############

install_files( ${XDG_APPS_DIR} FILES ktuberling.desktop )
install_files( ${MIME_INSTALL_DIR}/application FILES x-tuberling.desktop )
install_files( ${DATA_INSTALL_DIR}/ktuberling FILES ktuberlingui.rc )

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )




#original Makefile.am contents follow:

## this 10 paths are KDE specific. Use them:
## kde_htmldir       Where your docs should go to. (contains lang subdirs)
## kde_appsdir       Where your application file (.desktop) should go to. 
## kde_icondir       Where your icon should go to.
## kde_sounddir      Where system sounds should go to.
## kde_datadir       Where you install application data. (Use a subdir)
## kde_locale        Where translation files should go to.(contains lang subdirs)
## kde_cgidir        Where cgi-bin executables should go to.
## kde_confdir       Where config files should go to.
## kde_mimedir       Where mimetypes should go to.
## kde_toolbardir    Where general toolbar icons should go to.
## kde_wallpaperdir  Where general wallpapers should go to.
#
#INCLUDES= -I$(top_srcdir)/libkdegames $(all_includes)
#SUBDIRS = . museum sounds pics
#
#bin_PROGRAMS = ktuberling
#
#ktuberling_SOURCES = action.cpp main.cpp toplevel.cpp playground.cpp todraw.cpp soundfactory.cpp
#
#ktuberling_METASOURCES = AUTO
#ktuberling_LDFLAGS =  $(all_libraries) $(KDE_RPATH)
#ktuberling_LDADD = $(LIB_KDEGAMES) $(LIB_KFILE) -lkdeprint 
#ktuberling_DEPENDENCIES = $(LIB_KDEGAMES_DEP)
#
#KDE_ICON = ktuberling
#
#xdg_apps_DATA = ktuberling.desktop
#
#mimetypeapplicationdata_DATA = x-tuberling.desktop
#mimetypeapplicationdatadir = $(kde_mimedir)/application
#
#appsrc_DATA = ktuberlingui.rc
#appsrcdir = $(kde_datadir)/ktuberling
#
#messages: rc.cpp
#	$(XGETTEXT) rc.cpp $(ktuberling_SOURCES) pics/layout.i18n -o $(podir)/ktuberling.pot
#
