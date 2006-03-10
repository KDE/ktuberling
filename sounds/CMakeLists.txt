kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/ktuberling/sounds/en FILES badge.wav bow.wav cigar.wav ear.wav earring.wav eye.wav eyebrow.wav 	    hat.wav moustache.wav mouth.wav nose.wav spectacles.wav  	    sunglasses.wav tuberling.wav watch.wav )

kde4_footer()



#original Makefile.am contents follow:

#
## add here all files
#sound_DATA = badge.wav bow.wav cigar.wav ear.wav earring.wav eye.wav eyebrow.wav\
#	    hat.wav moustache.wav mouth.wav nose.wav spectacles.wav \
#	    sunglasses.wav tuberling.wav watch.wav
#
#sounddir = $(kde_datadir)/ktuberling/sounds/en
#
## DONT ADD TRANSLATIONS HERE
