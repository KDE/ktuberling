FILE(GLOB datafiles *.ogg)
install( FILES ${datafiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ca )
install( FILES ca.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ )

