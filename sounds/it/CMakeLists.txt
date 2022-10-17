FILE(GLOB datafiles *.wav)
install( FILES ${datafiles} DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/it )


install( FILES it.soundtheme DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ )
