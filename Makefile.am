# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.desktop) should go to. 
# kde_icondir       Where your icon should go to.
# kde_sounddir      Where system sounds should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

INCLUDES= -I$(top_srcdir)/libkdegames $(all_includes)
SUBDIRS = . museum sounds pics

bin_PROGRAMS = ktuberling

ktuberling_SOURCES = action.cpp main.cpp toplevel.cpp playground.cpp todraw.cpp soundfactory.cpp

ktuberling_METASOURCES = AUTO
ktuberling_LDFLAGS =  $(all_libraries) $(KDE_RPATH)
ktuberling_LDADD = $(LIB_KDEGAMES) $(LIB_KFILE) -lkdeprint 
ktuberling_DEPENDENCIES = $(LIB_KDEGAMES_DEP)

KDE_ICON = ktuberling

xdg_apps_DATA = ktuberling.desktop

mimetypeapplicationdata_DATA = x-tuberling.desktop
mimetypeapplicationdatadir = $(kde_mimedir)/application

appsrc_DATA = ktuberlingui.rc
appsrcdir = $(kde_datadir)/ktuberling

messages: rc.cpp
	$(XGETTEXT) rc.cpp $(ktuberling_SOURCES) pics/layout.i18n -o $(podir)/ktuberling.pot

