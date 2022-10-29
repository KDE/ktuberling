FILE(GLOB datafiles *.wav )
install( FILES ${datafiles} DESTINATION  ${KDE_INSTALL_DATADIR}/ktuberling/sounds/fr )

install( FILES fr.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ )
