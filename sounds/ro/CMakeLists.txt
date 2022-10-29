FILE(GLOB datafiles *.wav)
install( FILES ${datafiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ro)


install( FILES ro.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ )
