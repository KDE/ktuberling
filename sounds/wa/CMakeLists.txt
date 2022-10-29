FILE(GLOB datafiles *.ogg)
install( FILES ${datafiles} DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/wa )
install( FILES wa.soundtheme DESTINATION ${KDE_INSTALL_DATADIR}/ktuberling/sounds/ ) 
