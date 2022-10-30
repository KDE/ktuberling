file( GLOB datafiles *.ogg )
install( FILES ${datafiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/sr )
install( FILES sr.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds )
