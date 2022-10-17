FILE(GLOB datafiles *.wav)
install( FILES ${datafiles} DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/es )
install( FILES es.soundtheme DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ )
