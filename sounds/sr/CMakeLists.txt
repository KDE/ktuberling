include( ../../../cmake_modules/srDataMacros.cmake )

# Sounds reside only in sr, because LL.soundtheme.in points there in both cases.
file( GLOB SOUNDFILES *.ogg )
install( FILES ${SOUNDFILES}
         DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/sr )

 #install_data_sr( FILES LL.soundtheme.in
 #                DESTINATION ${DATA_INSTALL_DIR}/ktuberling/sounds/ )
