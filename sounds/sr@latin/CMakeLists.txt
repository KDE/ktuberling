file( GLOB datafiles *.ogg )
install( FILES ${datafiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/sr@latin )
install( FILES sr@latin.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds )
