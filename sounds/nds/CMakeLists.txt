FILE(GLOB datafiles *.wav)
install( FILES ${datafiles} DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/nds )
install( FILES nds.soundtheme DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ )
