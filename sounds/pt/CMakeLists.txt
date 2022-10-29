FILE(GLOB datafiles *.wav)
install( FILES ${datafiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/pt )

install( FILES pt.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ )
