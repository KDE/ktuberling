FILE(GLOB datafiles *.ogg)
install( FILES ${datafiles} DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/wa )
install( FILES wa.soundtheme DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ ) 
