FILE(GLOB datafiles *.opus)
install( FILES ${datafiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ru )
install( FILES ru.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ )
