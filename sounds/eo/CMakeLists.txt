FILE(GLOB datafiles *.ogg)
install( FILES ${datafiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/eo )
install( FILES eo.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ )
