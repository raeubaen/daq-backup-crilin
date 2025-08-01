/******************************************************************************
*
* CAEN SpA - Software Division
* Via Vetraia, 11 - 55049 - Viareggio ITALY
* +390594388398 - www.caen.it
*
***************************************************************************//**
* \note TERMS OF USE:
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation. This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. The user relies on the
* software, documentation and results solely at his own risk.
******************************************************************************/


#ifndef _synctest_H
#define _synctest_H

#define SyncTest_Release        "3.0.0"
#define RELEASEDATE				"20230216"
#define MAX_NBRD				2
#define MAX_NCH					32
#define MAX_NGR					4

#include <CAENDigitizer.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <float.h>
#include <inttypes.h>

#ifdef WIN32

#define _WINSOCKAPI_    // stops windows.h including winsock.h
#include <Windows.h>
#include <WinSock2.h>
#include <conio.h>
#include <time.h>
#include <sys/timeb.h>
#include <process.h>


//#define getch _getch     /*! < redefine POSIX 'deprecated' */
//#define kbhit _kbhit     /*! < redefine POSIX 'deprecated' */
#define	_PACKED_
#define	_INLINE_		
#define popen  _popen    /* redefine POSIX 'deprecated' popen as _popen */
#define pclose  _pclose  /* redefine POSIX 'deprecated' pclose as _pclose */

#define SLEEP(x) Sleep(x)

#else
#include <time.h>
#include <sys/time.h> /* struct timeval, select() */
#include <sys/stat.h>
#include <termios.h> /* tcgetattr(), tcsetattr() */
#include <stdlib.h> /* atexit(), exit(), C99 compliant compilers: uint64_t */
#include <unistd.h> /* read() */
#include <stdio.h> /* printf() */
#include <string.h> /* memcpy() */
#include <ctype.h>    /* toupper() */

#define UINT64_T uint64_t
#define UINT32_T uint32_t

#define		_PACKED_		__attribute__ ((packed, aligned(1)))
#define		_INLINE_		__inline__ 

#define SLEEP(x) usleep(x*1000)

static struct termios g_old_kbd_mode;

/*****************************************************************************/
static void cooked(void)
{
	tcsetattr(0, TCSANOW, &g_old_kbd_mode);
}

static void raw(void)
{
	static char init;
	struct termios new_kbd_mode;

	if (init) return;
	/* put keyboard (stdin, actually) in raw, unbuffered mode */
	tcgetattr(0, &g_old_kbd_mode);
	memcpy(&new_kbd_mode, &g_old_kbd_mode, sizeof(struct termios));
	new_kbd_mode.c_lflag &= ~(ICANON | ECHO);
	new_kbd_mode.c_cc[VTIME] = 0;
	new_kbd_mode.c_cc[VMIN] = 1;
	tcsetattr(0, TCSANOW, &new_kbd_mode);
	/* when we exit, go back to normal, "cooked" mode */
	atexit(cooked);
	init = 1;
}



#endif

/*****************************************************************************/
/*  GETCH  */
/*****************************************************************************/
int st_getch(void);

/*****************************************************************************/
/*  KBHIT  */
/*****************************************************************************/
int st_kbhit();
/*****************************************************************************/
/*  SCANF  */
/*****************************************************************************/
#ifdef WIN32
#define st_scanf	scanf
#else
#define st_scanf	_scanf
#endif
//****************************************************************************
// Some register addresses
//****************************************************************************

#define ADDR_BOARD_CONFIG		 0x8000
#define ADDR_GLOBAL_TRG_MASK     0x810C
#define ADDR_TRG_OUT_MASK        0x8110
#define ADDR_FRONT_PANEL_IO_SET  0x811C
#define ADDR_ACQUISITION_MODE    0x8100
#define ADDR_EXT_TRG_INHIBIT     0x817C
#define ADDR_RUN_DELAY           0x8170
#define ADDR_FORCE_SYNC			 0x813C
#define ADDR_LVDS_NEW_FEATURES	 0x81A0
#define ADDR_RELOAD_PLL			 0xEF34


//****************************************************************************
// Run Modes
//****************************************************************************
// start on software command 
#define RUN_START_ON_SOFTWARE_COMMAND     0xC 
// start on S-IN level (logical high = run; logical low = stop)
#define RUN_START_ON_SIN_LEVEL            0xD
// start on first TRG-IN or Software Trigger 
#define RUN_START_ON_TRGIN_RISING_EDGE    0xE
// start on LVDS I/O level
#define RUN_START_ON_LVDS_IO              0xF


//****************************************************************************
// Sync Modes
//****************************************************************************
// NOT COHERENCE WITH THE CFG FILE
//#define COMMON_EXTERNAL_TRIGGER_TRGIN_TRGOUT	0
//#define INDIVIDUAL_TRIGGER_SIN_TRGOUT			1
//#define TRIGGER_ONE2ALL_EXTOR					2
#define TRIGGER_VETED_EXT		0
#define BUSY_IN_DAISYCHAIN		1

//****************************************************************************
// Start Modes
//****************************************************************************
#define START_SW_CONTROLLED   0
#define START_HW_CONTROLLED   1

//****************************************************************************
// Start Slaves Modes
//****************************************************************************
#define START_LVDS			0
#define START_SIN			1

//****************************************************************************
// Max number of event to read in a Block Transfer
//****************************************************************************
#define MAX_EVENTS_XFER   255

//****************************************************************************
// Max number of event to read in a Block Transfer
//****************************************************************************
#define PLOT_HISTOGRAM		0
#define PLOT_WAVEFORM		1

//****************************************************************************
// Max number of event to read in a Block Transfer
//****************************************************************************
#define DEBUG_WAVEFORM_FILE		1
#define DEBUG_TRIGGER_FILE		2

//****************************************************************************
// Variables for the user parameters
//****************************************************************************
typedef struct {
	int NumBrd;

	int Debug;
	CAEN_DGTZ_ConnectionType ConnectionType[MAX_NBRD];
	int LinkNum[MAX_NBRD];
	int ConetNode[MAX_NBRD];
	uint32_t BaseAddress[MAX_NBRD];
	char ipAddress[MAX_NBRD][25];

	int RefChannel[MAX_NBRD];
	uint16_t FastTriggerThreshold[MAX_NBRD];
	uint16_t FastTriggerOffset[MAX_NBRD];
	uint16_t DCoffset[MAX_NBRD];
	uint16_t ChannelThreshold[MAX_NBRD];
	uint16_t TRThreshold[MAX_NBRD];
	CAEN_DGTZ_TriggerPolarity_t ChannelPulseEdge[MAX_NBRD];
	uint32_t PostTrigger[MAX_NBRD];

	int RecordLength;
	CAEN_DGTZ_TriggerPolarity_t TriggerEdge;
	int MatchingWindow;
	int TestPattern;
	CAEN_DGTZ_IOLevel_t IOlevel;
	int SyncMode;
	int StartMode;
	int EnableLog;
	int EnableWaves;
	CAEN_DGTZ_DRS4Frequency_t DRS4Frequency;

	int HistoNbins;
	double HistoOffset;
	double HistoBinSize;
} UserParams_t;

//****************************************************************************
// Statistics
//****************************************************************************
typedef struct {
	double TTT[MAX_NBRD];
	double PrevTTT[MAX_NBRD]; // = { 0 };
	double DeltaT;
	double DeltaTTT;
	double PulseEdgeTime[MAX_NBRD];
	double TrEdgeTime[MAX_NBRD];
	double MeanT; // = 0.0;
	double MeanTTT; // = 0.0;
	double SigmaT; // = 0.0;
	double SigmaTTT; // = 0.0;
	uint64_t NsT;
	uint64_t NsTTT;
} Stats_t;

//****************************************************************************
// Functions
//****************************************************************************
int ParseConfigFile(FILE* f_ini, UserParams_t* Pcfg);

#endif
