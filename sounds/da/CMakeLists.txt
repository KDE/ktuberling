FILE(GLOB datafiles *.ogg)
install( FILES ${datafiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/da )
install( FILES da.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ )
