include( ../../../cmake_modules/srDataMacros.cmake )

# Sounds reside only in sr, because LL.soundtheme.in points there in both cases.
file( GLOB SOUNDFILES *.ogg )
install( FILES ${SOUNDFILES}
         DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/sr )

 #install_data_sr( FILES LL.soundtheme.in
 #                DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ )
