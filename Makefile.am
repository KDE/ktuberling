# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_sounddir      Where system sounds should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

APPSDIR = $(kde_appsdir)/Utilities
INCLUDES= $(all_includes)
SUBDIRS = doc museum sounds pics

bin_PROGRAMS = ktuberling

ktuberling_SOURCES = main.cpp toplevel.cpp todraw.cpp

ktuberling_METASOURCES = USE_AUTOMOC
ktuberling_LDFLAGS = $(all_libraries) $(KDE_RPATH)

ktuberling_LDADD   = $(LIB_KFILE) -lmediatool $(LIB_KIMGIO)

kde_icon_DATA = ktuberling.xpm

DISTCLEANFILES = $()

install-data-local: 
	$(mkinstalldirs) $(APPSDIR)
	$(INSTALL_DATA) ktuberling.kdelnk $(APPSDIR)
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) mini-ktuberling.xpm $(kde_minidir)/ktuberling.xpm

uninstall-local:
	-rm -f $(APPSDIR)/ktuberling.kdelnk
	-rm -f $(kde_minidir)/ktuberling.xpm

