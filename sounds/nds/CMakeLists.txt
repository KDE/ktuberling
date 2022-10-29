FILE(GLOB datafiles *.wav)
install( FILES ${datafiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/nds )
install( FILES nds.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ )
