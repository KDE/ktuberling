kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/ktuberling/pics FILES penguin-game.png penguin-mask.png              potato-game.png potato-mask.png              aquarium-game.png aquarium-mask.png              layout.xml )

kde4_footer()



#original Makefile.am contents follow:

#
## add here all files
#pics_DATA = penguin-game.png penguin-mask.png \
#            potato-game.png potato-mask.png \
#            aquarium-game.png aquarium-mask.png \
#            layout.xml
#
#picsdir = $(kde_datadir)/ktuberling/pics
