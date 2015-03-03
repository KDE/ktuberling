########### install files ###############

FILE( GLOB oggfiles *.ogg )

INSTALL( FILES ${oggfiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/en )

INSTALL( FILES en.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ )
