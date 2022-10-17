FILE(GLOB datafiles *.wav)
install( FILES ${datafiles} DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ga )

install( FILES ga.soundtheme DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ )
