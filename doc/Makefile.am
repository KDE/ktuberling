
# add here all files
docs_DATA = index.html index-1.html index-2.html index-3.html index-4.html \
		index-5.html logotp3.png

# this is the directory, where all datas are installed
# you have given the datas in data_DATA
docsdir = $(kde_htmldir)/en/ktuberling

EXTRA_DIST = $(docs_DATA) index.sgml index.nif

doc: index.sgml index.nif
	ksgml2html index en
