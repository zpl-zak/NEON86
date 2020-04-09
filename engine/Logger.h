// Logger.h: interface for the CLogger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGGER_H__0A31CA8E_C81B_4921_88BE_27EB3C0A71F7__INCLUDED_)
#define AFX_LOGGER_H__0A31CA8E_C81B_4921_88BE_27EB3C0A71F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "system.h"

enum LogLevel
{
	Trace,
	Debug,
	Info,
	Warn,
	Error
};

class ENGINE_API CLogger  
{
public:
	CLogger();
	virtual ~CLogger();

	/// Methods

	/// Accessors
	LogLevel GetLogLevel() const { return mLogLevel; }
	void SetLogLevel(LogLevel level) { mLogLevel = level; }

protected:
	LogLevel mLogLevel;
};

#endif // !defined(AFX_LOGGER_H__0A31CA8E_C81B_4921_88BE_27EB3C0A71F7__INCLUDED_)
