FILE(GLOB datafiles *.ogg)
install( FILES ${datafiles} DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/uk )
install( FILES uk.soundtheme DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ )
