FILE(GLOB datafiles *.wav)
install( FILES ${datafiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/sv )

install( FILES sv.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ )
