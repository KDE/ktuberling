FILE(GLOB datafiles *.ogg)
install( FILES ${datafiles} DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ca )
install( FILES ca.soundtheme DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ )

