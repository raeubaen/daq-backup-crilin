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

#include <CAENDigitizer.h>
#include "synctest.h"

int brd = -1, ch = -1;
int ValidParameterName = 0;

// --------------------------------------------------------------------------------------------------------- 
// Description: compare two strings
// Inputs:		str1, str2: strings to compare
// Outputs:		-
// Return:		1=equal, 0=not equal
// --------------------------------------------------------------------------------------------------------- 
int streq(char* str1, char* str2)
{
	if (strcmp(str1, str2) == 0) {
		ValidParameterName = 1;
		return 1;
	}
	else {
		return 0;
	}
}

// --------------------------------------------------------------------------------------------------------- 
// Description: Read an integer (decimal) from the conig file
// Inputs:		f_ini: config file
// Outputs:		-
// Return:		integer value read from the file
// --------------------------------------------------------------------------------------------------------- 
int GetInt(FILE* f_ini)
{
	int ret;
	fscanf(f_ini, "%d", &ret);
	return ret;
}

// --------------------------------------------------------------------------------------------------------- 
// Description: Read a value from the conig file followed by an optional time unit (ps, ns, us, ms, s)
//              and convert it in a time expressed in ns as a float 
// Inputs:		f_ini: config file
//				tu: time unit of the returned time value
// Outputs:		-
// Return:		time value (in ns) read from the file
// --------------------------------------------------------------------------------------------------------- 
float GetTime(FILE* f_ini, char* tu)
{
	double timev, ns;
	long fp;
	char str[100];

	fscanf(f_ini, "%lf", &timev);
	// try to read the unit from the config file (string)
	fp = ftell(f_ini);  // save current pointer before "str"
	fscanf(f_ini, "%s", str);  // read string "str"
	if (streq(str, "ps"))		ns = timev * 1e-3;
	else if (streq(str, "ns"))	ns = timev;
	else if (streq(str, "us"))	ns = timev * 1e3;
	else if (streq(str, "ms"))	ns = timev * 1e6;
	else if (streq(str, "s"))	ns = timev * 1e9;
	else if (streq(str, "m"))	ns = timev * 60e9;
	else if (streq(str, "h"))	ns = timev * 3600e9;
	else if (streq(str, "d"))	ns = timev * 24 * (3600e9);
	else {
		fseek(f_ini, fp, SEEK_SET); // move pointer back to beginning of "str" and use it again for next parsing
		return (float)timev;  // no time unit specified in the config file; assuming equal to the requested one
	}

	if (streq(tu, "ps"))		return (float)(ns * 1e3);
	else if (streq(tu, "ns"))	return (float)(ns);
	else if (streq(tu, "us"))	return (float)(ns / 1e3);
	else if (streq(tu, "ms"))	return (float)(ns / 1e6);
	else if (streq(tu, "s"))	return (float)(ns / 1e9);
	else if (streq(tu, "m"))	return (float)(ns / 60e9);
	else if (streq(tu, "h"))	return (float)(ns / 3600e9);
	else if (streq(tu, "d"))	return (float)(ns / (24 * 3600e9));
	else return (float)timev;
}

// --------------------------------------------------------------------------------------------------------- 
// Description: Read an integer (hexadecimal) from the conig file
// Inputs:		f_ini: config file
// Outputs:		-
// Return:		integer value read from the file
// --------------------------------------------------------------------------------------------------------- 
int GetHex(FILE* f_ini)
{
	int ret;
	char str[100];
	fscanf(f_ini, "%s", str);
	if ((str[1] == 'x') || (str[1] == 'X'))
		sscanf(str + 2, "%x", &ret);
	else
		sscanf(str, "%x", &ret);
	return ret;
}

// --------------------------------------------------------------------------------------------------------- 
// Description: Read a float from the conig file
// Inputs:		f_ini: config file
// Outputs:		-
// Return:		float value read from the file
// --------------------------------------------------------------------------------------------------------- 
float GetFloat(FILE* f_ini)
{
	float ret;
	char str[1000];
	int i;
	fgets(str, 1000, f_ini);
	// replaces ',' with '.' (decimal separator)
	for (i = 0; i < (int)strlen(str); i++)
		if (str[i] == ',') str[i] = '.';
	sscanf(str, "%f", &ret);
	return ret;
}

int ParseConfigFile(FILE* f_ini, UserParams_t* Pcfg)
{
	char str[1000], str1[1000];
	int b, val, tr = -1;
	int brd1;  // target board defined as ParamName[b]
	int brd2 = -1;  // target board defined as Section ([BOARD b])

	/* Default settings */
	Pcfg->NumBrd = MAX_NBRD;	// At the moment only 2 boards supported
	Pcfg->Debug = 0;
	Pcfg->RecordLength = 1024;
	Pcfg->TriggerEdge = CAEN_DGTZ_TriggerOnFallingEdge;
	Pcfg->MatchingWindow = 0;
	Pcfg->TestPattern = 0;
	Pcfg->IOlevel = CAEN_DGTZ_IOLevel_NIM;
	Pcfg->SyncMode = 1;
	Pcfg->StartMode = START_SW_CONTROLLED;
	Pcfg->EnableLog = 1;
	Pcfg->EnableWaves = 1;
	Pcfg->DRS4Frequency = CAEN_DGTZ_DRS4_5GHz;
	Pcfg->HistoNbins = 100;
	Pcfg->HistoOffset = 0;
	Pcfg->HistoBinSize = 0.5;

	for (b = 0; b < 2; b++) {
		Pcfg->ConnectionType[b] = CAEN_DGTZ_OpticalLink;
		Pcfg->LinkNum[b] = 0;
		Pcfg->ConetNode[b] = 0;
		Pcfg->BaseAddress[b] = 0x1000;

		Pcfg->RefChannel[b] = 0;
		Pcfg->FastTriggerThreshold[b] = 10000;
		Pcfg->FastTriggerOffset[b] = 10000;
		Pcfg->DCoffset[b] = 0x100;
		Pcfg->ChannelThreshold[b] = 0;
		Pcfg->TRThreshold[b] = 1000;
		Pcfg->ChannelPulseEdge[b] = CAEN_DGTZ_TriggerOnFallingEdge;
		Pcfg->PostTrigger[b] = 0;
	}

	/* read config file and assign parameters */
	while (!feof(f_ini)) {
		int read;
		char* cb;

		brd1 = -1;
		ValidParameterName = 0;
		// read a word from the file
		read = fscanf(f_ini, "%s", str);
		if (!read || (read == EOF) || !strlen(str))
			continue;

		// skip comments
		if (str[0] == '#') {
			fgets(str, 1000, f_ini);
			continue;
		}

		// Section (COMMON or individual channel)
		if (str[0] == '[') {
			ValidParameterName = 1;
			if (strstr(str, "COMMON") != NULL) {
				brd2 = -1;
			}
			else if (strstr(str, "BOARD") != NULL) {
				fscanf(f_ini, "%s", str1);
				sscanf(str1, "%d", &val);
				if (val < 0 || val >= 2) printf("%s: Invalid board number\n", str);
				else brd2 = val;
			}
			else if (strstr(str, "CHANNEL") != NULL) {
				fscanf(f_ini, "%s", str1);
				sscanf(str1, "%d", &val);
				if ((brd2 == -1) && (Pcfg->NumBrd == 1)) brd2 = 0;
			}
		}
		else if ((cb = strstr(str, "[")) != NULL) {
			char* ei;
			sscanf(cb + 1, "%d", &brd1);
			if ((ei = strstr(cb, "]")) == NULL) {
				printf("%s: Invalid board index\n", str);
				fgets(str1, 1000, f_ini);
			}
			*cb = 0;
		}
		if ((brd1 >= 0) && (brd1 < 2)) {
			brd = brd1;
		}
		else {
			brd = brd2;
		}
		if (streq(str, "Open")) {
			if (brd == -1) {
				printf("%s: cannot be a common setting (must be in a [BOARD] section)\n", str);
				fgets(str1, 1000, f_ini);
			}
			else {
				int ip = 0;
				read = fscanf(f_ini, "%s", str1);
				if (strcmp(str1, "USB") == 0)
					Pcfg->ConnectionType[brd] = CAEN_DGTZ_USB;
				else
					if (strcmp(str1, "PCI") == 0)
						Pcfg->ConnectionType[brd] = CAEN_DGTZ_OpticalLink;
					else if (strcmp(str1, "USB_A4818") == 0)
						Pcfg->ConnectionType[brd] = CAEN_DGTZ_USB_A4818;
					else if (strcmp(str1, "USB_A4818_V2718") == 0)
						Pcfg->ConnectionType[brd] = CAEN_DGTZ_USB_A4818_V2718;
					else if (strcmp(str1, "USB_A4818_V3718") == 0)
						Pcfg->ConnectionType[brd] = CAEN_DGTZ_USB_A4818_V3718;
					else if (strcmp(str1, "USB_A4818_V4718") == 0)
						Pcfg->ConnectionType[brd] = CAEN_DGTZ_USB_A4818_V4718;
					else if (strcmp(str1, "USB_V4718") == 0)
						Pcfg->ConnectionType[brd] = CAEN_DGTZ_USB_V4718;
					else if (strcmp(str1, "ETH_V4718") == 0) {
						Pcfg->ConnectionType[brd] = CAEN_DGTZ_ETH_V4718;
						ip = 1;
					}
					else {
						printf("%s %s: Invalid connection type\n", str, str1);
						return -1;
					}
				if (ip == 1) {
					read = fscanf(f_ini, "%s", &Pcfg->ipAddress[brd]);
				}
				else
					read = fscanf(f_ini, "%d", &Pcfg->LinkNum[brd]);
				if (Pcfg->ConnectionType[brd] == CAEN_DGTZ_USB)
					Pcfg->ConetNode[brd] = 0;
				else
					read = fscanf(f_ini, "%d", &Pcfg->ConetNode[brd]);
				read = fscanf(f_ini, "%x", &Pcfg->BaseAddress[brd]);

                                printf("board_idx,conntype, linknum, conetnode, baseaddress: %i %s %i %i %i", brd, str1, Pcfg->LinkNum[brd], Pcfg->ConetNode[brd], Pcfg->BaseAddress[brd]);

				continue;
				//fscanf(f_ini, "%s", str1);
				//if (streq(str1, "USB"))				Pcfg->ConnectionType[brd] = CAEN_DGTZ_USB;
				//else if (streq(str1, "OPTLINK"))	Pcfg->ConnectionType[brd] = CAEN_DGTZ_OpticalLink;
				//else 	printf("%s: invalid setting for %s\n", str1, str);
				////++Pcfg->NumBrd;
			}
		}
		if (streq(str, "ChannelPulseEdge")) {
			if (brd == -1) {
				printf("%s: cannot be a common setting (must be in a [BOARD] section)\n", str);
				fgets(str1, 1000, f_ini);
			}
			else {
				fscanf(f_ini, "%s", str1);
				if (streq(str1, "FALLINGEDGE"))		Pcfg->ChannelPulseEdge[brd] = CAEN_DGTZ_TriggerOnFallingEdge;
				else if (streq(str1, "RISINGEDGE"))			Pcfg->ChannelPulseEdge[brd] = CAEN_DGTZ_TriggerOnRisingEdge;
				else 	printf("%s: invalid setting for %s\n", str1, str);
			}
		}
		if (streq(str, "LinkNum")) {
			if (brd == -1) {
				printf("%s: cannot be a common setting (must be in a [BOARD] section)\n", str);
				fgets(str1, 1000, f_ini);
			}
			else Pcfg->LinkNum[brd] = GetInt(f_ini);
		}
		if (streq(str, "ConetNode")) {
			if (brd == -1) {
				printf("%s: cannot be a common setting (must be in a [BOARD] section)\n", str);
				fgets(str1, 1000, f_ini);
			}
			else  Pcfg->ConetNode[brd] = GetInt(f_ini);
		}
		if (streq(str, "BaseAddress")) {
			if (brd == -1) {
				printf("%s: cannot be a common setting (must be in a [BOARD] section)\n", str);
				fgets(str1, 1000, f_ini);
			}
			else Pcfg->BaseAddress[brd] = GetHex(f_ini);
		}
		if (streq(str, "RefChannel")) {
			if (brd == -1) {
				printf("%s: cannot be a common setting (must be in a [BOARD] section)\n", str);
				fgets(str1, 1000, f_ini);
			}
			else Pcfg->RefChannel[brd] = GetInt(f_ini);
		}
		if (streq(str, "FastTriggerThreshold")) {
			if (brd == -1) {
				printf("%s: cannot be a common setting (must be in a [BOARD] section)\n", str);
				fgets(str1, 1000, f_ini);
			}
			else Pcfg->FastTriggerThreshold[brd] = GetInt(f_ini);
		}
		if (streq(str, "FastTriggerOffset")) {
			if (brd == -1) {
				printf("%s: cannot be a common setting (must be in a [BOARD] section)\n", str);
				fgets(str1, 1000, f_ini);
			}
			else Pcfg->FastTriggerOffset[brd] = GetInt(f_ini);
		}
		if (streq(str, "DCoffset")) {
			if (brd == -1) {
				printf("%s: cannot be a common setting (must be in a [BOARD] section)\n", str);
				fgets(str1, 1000, f_ini);
			}
			else Pcfg->DCoffset[brd] = GetHex(f_ini);
		}
		if (streq(str, "PostTrigger")) {
			if (brd == -1) {
				printf("%s: cannot be a common setting (must be in a [BOARD] section)\n", str);
				fgets(str1, 1000, f_ini);
			}
			else Pcfg->PostTrigger[brd] = GetInt(f_ini);
		}
		if (streq(str, "ChannelThreshold")) {
			if (brd == -1) {
				printf("%s: cannot be a common setting (must be in a [BOARD] section)\n", str);
				fgets(str1, 1000, f_ini);
			}
			else Pcfg->ChannelThreshold[brd] = GetInt(f_ini);
		}
		if (streq(str, "TRThreshold")) {
			if (brd == -1) {
				printf("%s: cannot be a common setting (must be in a [BOARD] section)\n", str);
				fgets(str1, 1000, f_ini);
			}
			else Pcfg->TRThreshold[brd] = GetInt(f_ini);
		}

		if (streq(str, "TriggerEdge")) {
			fscanf(f_ini, "%s", str1);
			if (streq(str1, "FALLINGEDGE"))	Pcfg->TriggerEdge = CAEN_DGTZ_TriggerOnFallingEdge;
			else if (streq(str1, "RISINGEDGE"))		Pcfg->TriggerEdge = CAEN_DGTZ_TriggerOnRisingEdge;
			else 	printf("%s: invalid setting for %s\n", str1, str);
		}
		if (streq(str, "IOlevel")) {
			fscanf(f_ini, "%s", str1);
			if (streq(str1, "NIM"))		Pcfg->IOlevel = CAEN_DGTZ_IOLevel_NIM;
			else if (streq(str1, "TTL"))		Pcfg->IOlevel = CAEN_DGTZ_IOLevel_TTL;
			else 	printf("%s: invalid setting for %s\n", str1, str);
		}
		if (streq(str, "DRS4Frequency")) {
			fscanf(f_ini, "%s", str1);
			if (streq(str1, "DRS4_5GHz"))		Pcfg->DRS4Frequency = CAEN_DGTZ_DRS4_5GHz;
			else if (streq(str1, "DRS4_2_5GHz"))	Pcfg->DRS4Frequency = CAEN_DGTZ_DRS4_2_5GHz;
			else if (streq(str1, "DRS4_1GHz"))		Pcfg->DRS4Frequency = CAEN_DGTZ_DRS4_1GHz;
			else	printf("%s: invalid setting for %s\n", str1, str);
		}
		if (streq(str, "StartMode")) {
			fscanf(f_ini, "%s", str1);
			if (streq(str1, "SOFTWARE"))	Pcfg->StartMode = START_SW_CONTROLLED;
			else if (streq(str1, "HARDWARE"))	Pcfg->StartMode = START_HW_CONTROLLED;
			else	printf("%s: invalid setting for %s\n", str1, str);
		}
		//if (streq(str, "SlaveStart")) {
		//	fscanf(f_ini, "%s", str1);
		//	if		(streq(str1, "LVDS"))		Pcfg->StartSlave = START_LVDS;
		//	else if (streq(str1, "S-IN"))		Pcfg->StartSlave = START_SIN;
		//	else	printf("%s: invalid setting for %s\n", str1, str);
		//}
		if (streq(str, "SyncMode")) {
			fscanf(f_ini, "%s", str1);
			if (streq(str1, "TRGEXT"))				Pcfg->SyncMode = TRIGGER_VETED_EXT;
			else if (streq(str1, "BUSYDCHAIN"))		Pcfg->SyncMode = BUSY_IN_DAISYCHAIN;
			else	printf("%s: invalid setting for %s\n", str1, str);
		}
		if (streq(str, "RecordLength"))			Pcfg->RecordLength		= GetInt(f_ini);
		if (streq(str, "MatchingWindow"))		Pcfg->MatchingWindow	= GetInt(f_ini);
		if (streq(str, "TestPattern"))			Pcfg->TestPattern		= GetInt(f_ini);
		if (streq(str, "EnableLog"))			Pcfg->EnableLog			= GetInt(f_ini);
		if (streq(str, "EnableWaveFiles"))		Pcfg->EnableWaves		= GetInt(f_ini);
		if (streq(str, "HistoNbins"))			Pcfg->HistoNbins		= GetInt(f_ini);
		if (streq(str, "HistoBinSize"))			Pcfg->HistoBinSize		= GetFloat(f_ini);
		if (streq(str, "HistoOffset"))			Pcfg->HistoOffset		= GetFloat(f_ini);
		if (streq(str, "Debug"))				Pcfg->Debug				= GetHex(f_ini);


		if (!ValidParameterName) {
			printf("WARNING: %s: unkwown parameter\n", str);
			fgets(str, (int)strlen(str) - 1, f_ini);
		}
	}

	// Disable not existing channels 
	//for (b = WDcfg->NumBrd; b < MAX_NBRD; b++) {
	//	for (ch = 0; ch < MAX_NCH; ch++) {
	//		WDcfg->EnableInput[b][ch] = 0;
	//	}
	//}

	// Get Run Number
	//if (WDcfg->AutoRunNumber) { // Auto
	//	int rn;
	//	FILE* rnf = fopen("RunNumber.txt", "r");
	//	if (rnf != NULL) fscanf(rnf, "%d", &rn);
	//	if (rn >= 0) WDcfg->RunNumber = rn;
	//	else WDcfg->RunNumber = 0;
	//}

	return 0;
}

