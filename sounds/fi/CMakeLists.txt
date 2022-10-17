FILE(GLOB datafiles *.wav)
install( FILES ${datafiles} DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/fi )
install( FILES fi.soundtheme DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ )
