FILE(GLOB datafiles *.ogg)
install( FILES ${datafiles} DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/el )
install( FILES el.soundtheme DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ )
