FILE(GLOB datafiles *.opus)
install( FILES ${datafiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/nn )
install( FILES nn.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ )
