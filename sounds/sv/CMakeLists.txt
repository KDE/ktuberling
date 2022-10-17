FILE(GLOB datafiles *.wav)
install( FILES ${datafiles} DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds//sv )

install( FILES sv.soundtheme DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ )
