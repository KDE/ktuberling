FILE(GLOB datafiles *.ogg)
install( FILES ${datafiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/el )
install( FILES el.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ )
