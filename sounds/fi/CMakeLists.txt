FILE(GLOB datafiles *.wav)
install( FILES ${datafiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/fi )
install( FILES fi.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ )
