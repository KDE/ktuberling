FILE(GLOB datafiles *.ogg)
install( FILES ${datafiles} DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/de )
install( FILES de.soundtheme DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ )
