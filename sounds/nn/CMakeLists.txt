FILE(GLOB datafiles *.opus)
install( FILES ${datafiles} DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/nn )
install( FILES nn.soundtheme DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ )
