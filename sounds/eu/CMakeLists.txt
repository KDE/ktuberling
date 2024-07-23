FILE(GLOB datafiles *.ogg)
install( FILES ${datafiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/eu )
install( FILES eu.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ )

