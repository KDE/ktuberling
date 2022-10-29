FILE(GLOB datafiles *.wav)
install( FILES ${datafiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/es )
install( FILES es.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ )
