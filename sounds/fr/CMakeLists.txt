FILE(GLOB datafiles *.wav )
install( FILES ${datafiles} DESTINATION  ${DATA_INSTALL_DIR}/ktuberling/sounds/fr )

install( FILES fr.soundtheme DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ )
