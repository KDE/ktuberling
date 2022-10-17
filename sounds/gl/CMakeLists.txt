FILE(GLOB datafiles *.ogg )
install( FILES ${datafiles} DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/gl )
install( FILES gl.soundtheme DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ ) 
