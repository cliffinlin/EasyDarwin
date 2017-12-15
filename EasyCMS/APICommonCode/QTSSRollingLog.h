/*
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * Copyright (c) 1999-2008 Apple Inc.  All Rights Reserved.
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 *
 */
 /*
	 Copyleft (c) 2013-2015 EasyDarwin.ORG.  All rights reserved.
	 Github: https://github.com/EasyDarwin
	 WEChat: EasyDarwin
	 Website: http://www.easydarwin.org
 */
 /*
	 File:       QTSSRollingLog.h

	 Contains:   A log toolkit, log can roll either by time or by size, clients
				 must derive off of this object ot provide configuration information.



 */

#ifndef __QTSS_ROLLINGLOG_H__
#define __QTSS_ROLLINGLOG_H__

#include <stdio.h>
#include <time.h>
#ifndef __Win32__
#include <sys/time.h>
#endif
#include "OSHeaders.h"
#include "OSMutex.h"
#include "Task.h"

const bool kAllowLogToRoll = true;

class QTSSRollingLog : public Task
{
public:

	//pass in whether you'd like the log roller to log errors.
	QTSSRollingLog();

	//
	// Call this to delete. Closes the log and sends a kill event
	void    Delete()
	{
		CloseLog(false); this->Signal(Task::kKillEvent);
	}

	//
	// Write a log message
	void    WriteToLog(char* inLogData, bool allowLogToRoll);

	//log rolls automatically based on the configuration criteria,
	//but you may roll the log manually by calling this function.
	//Returns true if no error, false otherwise
	bool  RollLog();

	//
	// Call this to open the log file and begin logging     
	void EnableLog(bool appendDotLog = true);

	//
// Call this to close the log
// (pass leaveEnabled as true when we are temporarily closing.)
	void CloseLog(bool leaveEnabled = false);

	//
	//mainly to check and see if errors occurred
	bool  IsLogEnabled();

	//master switch
	bool  IsLogging() { return fLogging; }
	void  SetLoggingEnabled(bool logState) { fLogging = logState; }

	//General purpose utility function
	//returns false if some error has occurred
	static bool   FormatDate(char *ioDateBuffer, bool logTimeInGMT);

	// Check the log to see if it needs to roll
	// (rolls the log if necessary)
	bool          CheckRollLog();

	// Set this to true to get the log to close the file between writes.
	static void		SetCloseOnWrite(bool closeOnWrite);

	enum
	{
		kMaxDateBufferSizeInBytes = 30, //UInt32
		kMaxFilenameLengthInBytes = 31  //UInt32
	};

protected:

	//
	// Task object. Do not delete directly
	virtual ~QTSSRollingLog();

	//Derived class must provide a way to get the log & rolled log name
	virtual char* getLogName() = 0;
	virtual char* getLogDir() = 0;
	virtual UInt32 getRollIntervalInDays() = 0;//0 means no interval
	virtual UInt32 getMaxLogBytes() = 0;//0 means unlimited

	//to record the time the file was created (for time based rolling)
	virtual time_t  writeLogHeader(FILE* inFile);
	time_t          readLogHeader(FILE* inFile);

private:

	//
	// Run function to roll log right at midnight   
	virtual SInt64      Run();

	bool          renameLogFile(const char* inFileName);
	bool          doesFileExist(const char* inPath);
	static void     resetToMidnight(time_t* inTimePtr, time_t* outTimePtr);
	char*           getLogPath(char* extension);

	FILE*           fLog;
	time_t          fLogCreateTime;
	char*           fLogFullPath;
	bool          fAppendDotLog;
	bool          fLogging;
	

	// To make sure what happens in Run doesn't also happen at the same time
	// in the public functions.
	OSMutex         fMutex;
};

#endif // __QTSS_ROLLINGLOG_H__
