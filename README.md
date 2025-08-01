-----------------------------------------------------------------------------

                   --- CAEN SpA - Computing Division ---

                                www.caen.it

  -----------------------------------------------------------------------------

  SyncTest version 3.0.0 README
  
  February 2023

  -----------------------------------------------------------------------------


  Content
  -----------------------------------------------------------------------------
  README        :  This file.
  ReleaseNotes  :  Revision History and notes.
  src           :  Source files.
  bin	        :  Configuration file.
  PLL		:  PLL configuration files.


  System Requirements
  -----------------------------------------------------------------------------
  - CAENVME library Ver 3.2.0 or above
  - CAENComm library Ver 1.4.0 or above
  - CAEN Digitizer Library Ver 2.16.2 or above
  - GNUplot >=4.2 (www.gnuplot.org)


  INSTALLATION
  -----------------------------------------------------------------------------
  From the main folder run:
  ./configure
  make
  sudo make install
  
  
  SYNTAX
  -----------------------------------------------------------------------------
  synctest [ConfigFile]
  Default config file is "/usr/local/etc/synctest/Synctest_Config.txt" or in the 
  bin project folder.


  SUPPORT
  -----------------------------------------------------------------------------   
  For technical support, go to https://www.caen.it/mycaen/support/ (login and MyCAEN+ account required).

# daq-backup-crilin --> based on CAEN synctest
