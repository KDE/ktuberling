kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/ktuberling/museum FILES businessman.tuberling cool.tuberling cyclop.tuberling dali.tuberling              einstein.tuberling fly.tuberling grandpa.tuberling happy.tuberling              idiot.tuberling miss.tuberling mouse.tuberling picasso.tuberling              serious.tuberling              crazy.tuberling hippie.tuberling              sea.tuberling )

kde4_footer()



#original Makefile.am contents follow:

#
## add here all files
#ktuberling_DATA = businessman.tuberling cool.tuberling cyclop.tuberling dali.tuberling \
#            einstein.tuberling fly.tuberling grandpa.tuberling happy.tuberling \
#            idiot.tuberling miss.tuberling mouse.tuberling picasso.tuberling \
#            serious.tuberling \
#            crazy.tuberling hippie.tuberling \
#            sea.tuberling
#
#ktuberlingdir = $(kde_datadir)/ktuberling/museum
