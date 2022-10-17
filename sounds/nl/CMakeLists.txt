FILE(GLOB datafiles *.wav)
install( FILES ${datafiles} DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/nl)
install( FILES nl.soundtheme DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ )
