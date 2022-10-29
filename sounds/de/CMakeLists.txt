FILE(GLOB datafiles *.ogg)
install( FILES ${datafiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/de )
install( FILES de.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ )
