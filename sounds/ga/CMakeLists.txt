FILE(GLOB datafiles *.opus)
install( FILES ${datafiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ga )

install( FILES ga.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ )
