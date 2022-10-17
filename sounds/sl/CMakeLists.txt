FILE(GLOB datafiles *.ogg)

install( FILES ${datafiles} DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/sl )
install( FILES sl.soundtheme DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ )