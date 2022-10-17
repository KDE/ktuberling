FILE(GLOB datafiles *.ogg)
install( FILES ${datafiles} DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/lt )
install( FILES lt.soundtheme DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ )

