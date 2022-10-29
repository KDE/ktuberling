FILE(GLOB datafiles *.wav)
install( FILES ${datafiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/nl)
install( FILES nl.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ )
