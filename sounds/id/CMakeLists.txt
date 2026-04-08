FILE(GLOB datafiles *.opus )
install( FILES ${datafiles} DESTINATION  ${KDE_INSTALL_DATADIR}/ktuberling/sounds/id )

install( FILES id.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ )
