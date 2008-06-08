########### install files ###############

FILE( GLOB oggfiles *.ogg )

INSTALL( FILES ${oggfiles} DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/en )

INSTALL( FILES en.soundtheme DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ )
