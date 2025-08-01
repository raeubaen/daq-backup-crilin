SUBDIRS=src

dist_noinst_HEADERS = src/X742CorrectionRoutines.h \
	src/synctest.h \
	src/paramparser.h
	
EXTRA_DIST = bin PLL README.md ReleaseNotes

#datadir=/etc/synctest
datadir=$(sysconfdir)/synctest/
dist_data_DATA= bin/Synctest_Config.txt 

#chmod 666 /etc/synctest/Synctest_Config.txt
install-data-hook:
	chmod 666 $(sysconfdir)/synctest/Synctest_Config.txt	
