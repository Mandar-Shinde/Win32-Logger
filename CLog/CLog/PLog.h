#if !defined(_PLOG_H)
#define _PLOG_H
#include <Windows.h>


#include <tchar.h>

#include <fstream>




#define PLOG_ERROR_LEVEL	4
#define PLOG_MSG_LEVEL		2
#define PLOG_INFO_LEVEL		1
#define PLOG_DEBUG_LEVEL	0



#define PLDEBUG PLOG_DEBUG_LEVEL
#define PLERROR PLOG_ERROR_LEVEL
#define PLINFO PLOG_INFO_LEVEL
#define PLMSG PLOG_MSG_LEVEL


using namespace std;

#ifdef _UNICODE
#define _tstring wstring
#else
#define _tstring string
#endif

class PLog
{
private: 
	_tstring logFileLocation;
	_tstring logFileName;
	_tstring logFileFullPath;

	_tstring eventlogFileLocation;
	_tstring eventlogFileName;
	_tstring eventlogFileFullPath;

	int mLogLevel;

	CRITICAL_SECTION logfileCriticalSection ;
	//CRITICAL_SECTION logeventfileCriticalSection ;
	DWORD maxLogFileSize;

public:
	PLog();
	~PLog();

	void  GetRecentLogEvents(_tstring&);
	void GetRecentLogEventsFilePath(_tstring & rdata);
	void GetRecentLogEventsFromFile(_tstring & filePath, _tstring & rdata);
	
	void InitLogFile(_tstring strLoc, _tstring filename, DWORD maxSize);

	_tstring GetLocation()
	{ return logFileLocation;
	}

	void RotateFile(std::_tstring filepath, size_t filesize);
	void RotateEventFile(std::_tstring filepath, size_t filesize);
	void PrintBanner(TCHAR * banner);
	void Log(int loglevel,const TCHAR * lpszFormat,  ...);
#ifdef _UNICODE
	void Log(int loglevel,const CHAR * lpszFormat,  ...);
#endif

#ifdef UNICODE
	void LogError(int errorValue,TCHAR *lpszFormat,  ...);
#endif

	void LogError(int errorValue,CHAR *lpszFormat,  ...);

	void SetLogFileName(_tstring strFileName);
	bool IsTruncateFile(ofstream & logFile, _tstring & path);
	int GetLogLevelFromRegistry();

	void Text(_tstring strText);
	void Text(_tstring strText,_tstring strTexttwo);
	void Text(_tstring strText,int data);
	void Text(int level,_tstring strText);
#ifdef _UNICODE

	void Text(char * strText);
	void Text(char * strText,char * strTexttwo);
	void Text(char * strText,string strTexttwo);
	void Text(char *  strText,int data);
	void Text(int level,char * strText);
#endif

	//vijender: to migrate DCSA to new log
#ifdef _UNICODE
	void Log_file(_tstring & strText){
		Log(PLDEBUG,(TCHAR *)strText.c_str());
	}
	void Log_file(const TCHAR * strText){
		Log(PLDEBUG,strText);
	}
#endif
	void Log_file(string & strText){
		Log(PLDEBUG,(char *)strText.c_str());
	}
	void Log_file(const char * strText){
		Log(PLDEBUG,strText);
	}



};


extern PLog gplog;


#endif  //_LOG_H
