FILE(GLOB datafiles *.wav)
install( FILES ${datafiles} DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ro)


install( FILES ro.soundtheme DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ )
