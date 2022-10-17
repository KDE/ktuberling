FILE(GLOB datafiles *.ogg)
install( FILES ${datafiles} DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/da )
install( FILES da.soundtheme DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ )
