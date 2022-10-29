FILE(GLOB datafiles *.ogg)

install( FILES ${datafiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/sl )
install( FILES sl.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ )