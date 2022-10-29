FILE(GLOB datafiles *.ogg )
install( FILES ${datafiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/gl )
install( FILES gl.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ ) 
