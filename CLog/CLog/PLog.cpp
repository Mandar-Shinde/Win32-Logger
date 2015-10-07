#include "PLog.h"
//#include "stdio.h"
#include <strsafe.h>
//#include "Registry.h"
//#include "common.h"
#include <ShlObj.h>

//char enter = 13;
/*
#ifdef UNICODE
#define _UNICODE
#endif

#ifdef _UNICODE
#define UNICODE
#endif
*/

#ifdef _UNICODE
#define _tfpintf fwprintf
#else
#define _tfpintf fprintf
#endif


#define LOG_FILE_ENTRY_LEN 2*1024
#define DEFAULT_LOG_FILE_SIZE_THRESHOLD	(10*1024*1024)



static TCHAR szBuffer[2*LOG_FILE_ENTRY_LEN+1]={0};


#ifdef _UNICODE

void PLog::Log(int loglevel,const CHAR * lpszFormat,  ...)
{
	try{
	CHAR localBuff[2*LOG_FILE_ENTRY_LEN+1]={0};
	wchar_t widemes[2*LOG_FILE_ENTRY_LEN+1]={0};

	va_list argp;
	va_start(argp, lpszFormat);

	vsprintf(localBuff, lpszFormat, argp);

	va_end(argp);

	mbstowcs(widemes,localBuff,2*LOG_FILE_ENTRY_LEN);
	Log(loglevel,widemes);
	}catch(...){}
}


void PLog::Text(char * strText)
{
	wchar_t widemes[LOG_FILE_ENTRY_LEN+1]={0};

	mbstowcs(widemes,strText,LOG_FILE_ENTRY_LEN);

	Log(PLDEBUG,widemes);


}
void PLog::Text(char * strText,string strTexttwo)
{
	char totalstr[LOG_FILE_ENTRY_LEN+1]={0};
	wchar_t widemes[LOG_FILE_ENTRY_LEN+1]={0};

	sprintf(totalstr,"%s %s",strText,strTexttwo.c_str());

	mbstowcs(widemes,totalstr,LOG_FILE_ENTRY_LEN);

	Log(PLDEBUG,widemes);
}
void PLog::Text(char * strText, char * strTexttwo)
{
	char totalstr[LOG_FILE_ENTRY_LEN+1]={0};
	wchar_t widemes[LOG_FILE_ENTRY_LEN+1]={0};

	sprintf(totalstr,"%s %s",strText,strTexttwo);

	mbstowcs(widemes,totalstr,LOG_FILE_ENTRY_LEN);

	Log(PLDEBUG,widemes);

}

void PLog::Text(char *  strText,int data)
{
	char totalstr[LOG_FILE_ENTRY_LEN+1]={0};
	wchar_t widemes[LOG_FILE_ENTRY_LEN+1]={0};

	sprintf(totalstr,"%s %d",strText,data);

	mbstowcs(widemes,totalstr,LOG_FILE_ENTRY_LEN);

	Log(PLDEBUG,widemes);

}
void PLog::Text(int level,char * strText)
{
	wchar_t widemes[LOG_FILE_ENTRY_LEN+1]={0};

	mbstowcs(widemes,strText,LOG_FILE_ENTRY_LEN);

	Log(level,widemes);

}

#endif


void PLog::Log(int loglevel,const TCHAR * lpszFormat,  ...)
{

	FILE*		pLog;
	FILE*		fpLogEvents;

	if(loglevel < mLogLevel)
		return;

	try{	

		SYSTEMTIME SystemTime;
		TCHAR	SystemDateString[MAX_PATH]={0};
		TCHAR	SystemTimeString[MAX_PATH]={0};

		GetLocalTime(&SystemTime);
		GetDateFormat( LOCALE_USER_DEFAULT, 0, &SystemTime, NULL, 
			SystemDateString, sizeof(SystemDateString)/sizeof(TCHAR));
		GetTimeFormat( LOCALE_USER_DEFAULT, 0, &SystemTime, NULL, 
			SystemTimeString, sizeof(SystemTimeString)/sizeof(TCHAR));


		EnterCriticalSection(&logfileCriticalSection);

#ifdef _UNICODE
		pLog = _wfopen(logFileFullPath.c_str(),_T("a+"));
#else
		pLog = fopen(logFileFullPath.c_str(),"a+");
#endif

		if(pLog != NULL)
		{
			_tfpintf(pLog, _T("%s "),SystemDateString); 	
			_tfpintf(pLog, _T("%s "),SystemTimeString);
			_tfpintf(pLog, _T("[%d] "), GetCurrentThreadId());

			switch(loglevel)
			{
			case PLDEBUG:
				_tfpintf(pLog,  _T("[DEBUG] "));
				break;
			case PLINFO:
				_tfpintf(pLog,  _T("[INFO ] "));
				break;
			case PLERROR:
				_tfpintf(pLog,  _T("[ERROR] "));
				break;
			default:
				break;

			}

			va_list	args;
			va_start(args, lpszFormat);
			_vstprintf(szBuffer, lpszFormat, args);

			_tfpintf(pLog,  _T("%s\n"),szBuffer);


			size_t filesize = ftell(pLog);
			fclose(pLog);

			RotateFile(logFileFullPath.c_str(), filesize);

		}
	
#ifdef _UNICODE
		fpLogEvents = _wfopen(eventlogFileFullPath.c_str(),_T("a+"));	
#else
		fpLogEvents = fopen(eventlogFileFullPath.c_str(),"a+");
#endif

		if(fpLogEvents != NULL)
		{

			switch(loglevel)
			{
			case PLINFO:
#ifdef UNICODE 
				_tfpintf(fpLogEvents, _T("%S "),SystemDateString); 	
				_tfpintf(fpLogEvents, _T("%S "),SystemTimeString);
				_tfpintf(fpLogEvents, _T("[%d] "), GetCurrentThreadId());
				_tfpintf(fpLogEvents,  _T("[INFO ] "));
				_tfpintf(fpLogEvents,  _T("%S\n"),szBuffer);
#else
				_tfpintf(fpLogEvents, _T("%s "),SystemDateString); 	
				_tfpintf(fpLogEvents, _T("%s "),SystemTimeString);
				_tfpintf(fpLogEvents, _T("[%d] "), GetCurrentThreadId());
				_tfpintf(fpLogEvents,  _T("[INFO ] "));
				_tfpintf(fpLogEvents,  _T("%s\n"),szBuffer);			
#endif
				break;
			case PLERROR:
#ifdef UNICODE 
				_tfpintf(fpLogEvents, _T("%S "),SystemDateString); 	
				_tfpintf(fpLogEvents, _T("%S "),SystemTimeString);
				_tfpintf(fpLogEvents, _T("[%d] "), GetCurrentThreadId());
				_tfpintf(fpLogEvents,  _T("[ERROR] "));
				_tfpintf(fpLogEvents,  _T("%S\n"),szBuffer);

#else
				_tfpintf(fpLogEvents, _T("%s "),SystemDateString); 	
				_tfpintf(fpLogEvents, _T("%s "),SystemTimeString);
				_tfpintf(fpLogEvents, _T("[%d] "), GetCurrentThreadId());
				_tfpintf(fpLogEvents,  _T("[ERROR] "));
				_tfpintf(fpLogEvents,  _T("%s\n"),szBuffer);
#endif
				break;
			default:
				break;

			}
			size_t efilesize = ftell(fpLogEvents);
			fclose(fpLogEvents);

			RotateEventFile(eventlogFileFullPath.c_str(), efilesize);
		}

		LeaveCriticalSection(&logfileCriticalSection);

	}catch(...)
	{
	}
}

#ifdef UNICODE

void PLog::LogError(int errorValue,CHAR * lpszFormat,  ...)
{
	CHAR localBuff[2*LOG_FILE_ENTRY_LEN+1]={0};

	wchar_t widemes[2*LOG_FILE_ENTRY_LEN+1]={0};


	va_list argp;
	va_start(argp, lpszFormat);

	vsprintf(localBuff, lpszFormat, argp);

	va_end(argp);

	mbstowcs(widemes,localBuff,2*LOG_FILE_ENTRY_LEN);


	LogError(errorValue,widemes);

}
#endif


void PLog::GetRecentLogEvents(_tstring & rdata)
{

	FILE *fpLogEvents;
	TCHAR streambuff[4097] ={0};
	

	try{
#if UNICODE
		//EnterCriticalSection(&logfileCriticalSection);
		
		/*_wfopen_s(&fpLogEvents,eventlogFileFullPath.c_str(),_T("a+"));
		if( fpLogEvents == NULL )
		{
			// cant open
		}
		else
		{		
			while(feof( fpLogEvents ) == 0)
			{
				fgetws(streambuff,4096,fpLogEvents);
				wrdata.append(streambuff);
				memset(streambuff,0,4096);
			}
		}
		if(fpLogEvents!=NULL)
		fclose( fpLogEvents );
		fpLogEvents=NULL;

		//delete file when datacopy over
		DeleteFile(eventlogFileFullPath.c_str());*/
	
		EnterCriticalSection(&logfileCriticalSection);

		if(eventlogFileFullPath.empty()==true)
		{
			LeaveCriticalSection(&logfileCriticalSection);
			return ;//rdata;
		}

		string eventpath(eventlogFileFullPath.begin(),eventlogFileFullPath.end());
		
		fopen_s(&fpLogEvents,eventpath.c_str(),"a+");
		if( fpLogEvents == NULL )
		{
			// cant open
		}
		else
		{		
			while(feof( fpLogEvents ) == 0)
			{
				fgets((char *)streambuff,4096,fpLogEvents);
				rdata.append(streambuff);
				memset(streambuff,0,4096);
			}
		}
		if(fpLogEvents!=NULL)
		fclose( fpLogEvents );
		fpLogEvents=NULL;

		//delete file when datacopy over
		DeleteFile(eventlogFileFullPath.c_str());		
	
		LeaveCriticalSection(&logfileCriticalSection);

		//return rdata;
		//string retdata(wrdata.begin(),wrdata.end());
		//return retdata;


#else	

		EnterCriticalSection(&logfileCriticalSection);
	

		if(eventlogFileFullPath.empty()==true)
		{
			LeaveCriticalSection(&logfileCriticalSection);
			return;// rdata;
		}


		fopen_s(&fpLogEvents,eventlogFileFullPath.c_str(),"a+");		
		if( fpLogEvents == NULL )
		{
			// cant open
		}
		else
		{		
			while(feof( fpLogEvents ) == 0)
			{
				fgets(streambuff,4096,fpLogEvents);
				rdata.append(streambuff);
				memset(streambuff,0,4096);
			}
		}
		if(fpLogEvents!=NULL)
		fclose( fpLogEvents );
		fpLogEvents=NULL;

		//delete file when datacopy over
		DeleteFile(eventlogFileFullPath.c_str());
		
		LeaveCriticalSection(&logfileCriticalSection);
		//return rdata;

#endif

	}catch(...)
	{}

}


void PLog::GetRecentLogEventsFilePath(_tstring & rdata)
{


	TCHAR streambuff[4097] ={0};
	

	try{

	

		EnterCriticalSection(&logfileCriticalSection);
	

		if(eventlogFileFullPath.empty()==true)
		{
			LeaveCriticalSection(&logfileCriticalSection);
			return;// rdata;
		}

		_tstring newName = eventlogFileFullPath;
		newName.append(_T(".copy"));

		CopyFile(eventlogFileFullPath.c_str(),newName.c_str(),false);

		//delete file when datacopy over
		DeleteFile(eventlogFileFullPath.c_str());
		
		rdata = newName;

		LeaveCriticalSection(&logfileCriticalSection);
		//return rdata;



	}catch(...)
	{}

}


void PLog::GetRecentLogEventsFromFile(_tstring & filePath, _tstring & rdata)
{

	FILE *fpLogEvents;
	TCHAR streambuff[4097] ={0};
	

	try{
#if UNICODE


		if(filePath.empty()==true)
		{
	
			return ;
		}

		_tstring eventpath(filePath.begin(),filePath.end());
		
		fopen_s(&fpLogEvents,(char *)eventpath.c_str(),"a+");

		if( fpLogEvents == NULL )
		{
			// cant open

			return;
		}
		else
		{		
			while(feof( fpLogEvents ) == 0)
			{
				fgets((char *)streambuff,4096,fpLogEvents);
				rdata.append(streambuff);
				memset(streambuff,0,4096);
			}
		}
		if(fpLogEvents!=NULL)
		fclose( fpLogEvents );
		fpLogEvents=NULL;

		//delete file when datacopy over
		DeleteFile(filePath.c_str());		
	
#else	

		
	

		if(filePath.empty()==true)
		{
			
			return;// rdata;
		}


		fopen_s(&fpLogEvents,filePath.c_str(),"a+");		
		if( fpLogEvents == NULL )
		{
			// cant open
			return;
		}
		else
		{		
			while(feof( fpLogEvents ) == 0)
			{
				fgets(streambuff,4096,fpLogEvents);
				rdata.append(streambuff);
				memset(streambuff,0,4096);
			}
		}
		if(fpLogEvents!=NULL)
		fclose( fpLogEvents );
		fpLogEvents=NULL;

		//delete file when datacopy over
		DeleteFile(filePath.c_str());
		
	

#endif

	}catch(...)
	{}

}

void PLog::LogError(int errorValue,TCHAR * lpszFormat,  ...)
{

	TCHAR logEntry[2*LOG_FILE_ENTRY_LEN]={0};
	va_list	args;
	//TCHAR szBuffer[2*1024]={0};

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;


	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorValue,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
		(lstrlen((LPCTSTR)lpMsgBuf)  + 40) * sizeof(TCHAR)); 
	StringCchPrintf((LPTSTR)lpDisplayBuf, 
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("Error %d: %s"), 
		errorValue, lpMsgBuf);

	va_start(args, lpszFormat);

	_vstprintf(szBuffer, lpszFormat, args);

	StringCchCopy(logEntry,ARRAYSIZE(logEntry),szBuffer);
	StringCchCat(logEntry,ARRAYSIZE(logEntry), _T(" Error String: "));
	StringCchCat(logEntry,ARRAYSIZE(logEntry), szBuffer);


	Log(PLERROR,logEntry);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);

}

void PLog::PrintBanner(TCHAR * banner)
{
	Log(PLOG_MSG_LEVEL,_T("///////////////////////////////////////////"));
	Log(PLOG_MSG_LEVEL,banner);
	Log(PLOG_MSG_LEVEL,_T("///////////////////////////////////////////"));
}

void PLog::RotateFile(_tstring OrgName, size_t filesize)
{
	if(filesize >= maxLogFileSize)
	{
		_tstring newName;
		char Buff[50];
		SYSTEMTIME curTime;
		GetLocalTime(&curTime);


		sprintf_s(Buff, sizeof(Buff)/sizeof(Buff[0]), "_%02hu%02hu%04hu-%02hu%02hu%02hu",
			curTime.wDay, curTime.wMonth, curTime.wYear, 
			curTime.wHour, 	curTime.wMinute, curTime.wSecond);


		newName = OrgName.substr(0, OrgName.rfind(_T(".")));
#ifdef _UNICODE
		TCHAR tempStr[50+1]={0};
		mbstowcs(tempStr,Buff,50);	

		newName+= _tstring(tempStr);
#else
		newName+= _tstring(Buff);
#endif
		newName+= OrgName.substr(OrgName.rfind(_T(".")));
		MoveFileEx(OrgName.c_str(), newName.c_str(), MOVEFILE_REPLACE_EXISTING);
	}
}
void PLog::RotateEventFile(_tstring OrgName, size_t filesize)
{
	long mb= 1 *1024*1024; //1 MB
	if(filesize >= mb)
	{
		_tstring newName;
		char Buff[50];
		SYSTEMTIME curTime;
		GetLocalTime(&curTime);


		sprintf_s(Buff, sizeof(Buff)/sizeof(Buff[0]), "_%02hu%02hu%04hu-%02hu%02hu%02hu",
			curTime.wDay, curTime.wMonth, curTime.wYear, 
			curTime.wHour, 	curTime.wMinute, curTime.wSecond);


		newName = OrgName.substr(0, OrgName.rfind(_T(".")));
#ifdef _UNICODE
		TCHAR tempStr[50+1]={0};
		mbstowcs(tempStr,Buff,50);	

		newName+= _tstring(tempStr);
#else
		newName+= _tstring(Buff);
#endif
		newName+= OrgName.substr(OrgName.rfind(_T(".")));
		MoveFileEx(OrgName.c_str(), newName.c_str(), MOVEFILE_REPLACE_EXISTING);

		//delete newNamefile
	}
}

void PLog::InitLogFile(_tstring strLoc, _tstring filename, DWORD maxSize)
{
	try{
		logFileLocation = strLoc;
		logFileName = filename;
		logFileFullPath = strLoc;

		if((logFileFullPath.c_str())[logFileFullPath.size()-1] != _T('\\'))
			logFileFullPath += _T("\\");

		//ms:code to create path for event log 
		eventlogFileLocation = strLoc;
		eventlogFileName = filename;
		eventlogFileName.append(_T(".event"));
		eventlogFileFullPath.append(logFileFullPath);
		eventlogFileFullPath.append(eventlogFileName);


		logFileFullPath += filename;

		//mLogLevel = GetLogLevelFromRegistry();
		mLogLevel = 0;

		if(maxSize)
			maxLogFileSize = maxSize;
		else
			maxLogFileSize = DEFAULT_LOG_FILE_SIZE_THRESHOLD;

#ifdef UNICODE
		Log(PLDEBUG,"Log file [%S\\%S] initialized with log level [%d]",strLoc.c_str(),filename.c_str(),mLogLevel);
		Log(PLDEBUG,"Log file [%S\\%S] log event path [%d]",eventlogFileLocation.c_str(),eventlogFileName.c_str(),mLogLevel);
#else
		Log(PLDEBUG,"Log file [%s\\%s] initialized with log level [%d]",strLoc.c_str(),filename.c_str(),mLogLevel);
		Log(PLDEBUG,"Log file [%s\\%s] log event path [%d]",eventlogFileLocation.c_str(),eventlogFileName.c_str(),mLogLevel);
#endif
	}catch(...)
	{}
}



PLog::PLog()
{
	logFileLocation=_T("");
	logFileName=_T("");
	logFileFullPath=_T("");

	eventlogFileLocation=_T("");
	eventlogFileName=_T("");
	eventlogFileFullPath=_T("");

	mLogLevel = PLOG_DEBUG_LEVEL;

	maxLogFileSize=DEFAULT_LOG_FILE_SIZE_THRESHOLD;

	InitializeCriticalSection(&logfileCriticalSection);
	


}
PLog::~PLog()
{
	DeleteCriticalSection(&logfileCriticalSection);
	//DeleteCriticalSection(&logeventfileCriticalSection);
}

#define LOG_LEVEL_ENTRY _T("loglevel")
#define PDA_T _T("SOFTWARE\\PDA")
//int PLog::GetLogLevelFromRegistry()
//{
//	DWORD dwStatus;
//	DWORD DataRet=0;
//	DWORD dwDataRetLen=sizeof(DataRet);
//
//	dwStatus = GetKeyValue(HKEY_LOCAL_MACHINE,PDA_T,LOG_LEVEL_ENTRY,REG_DWORD,&DataRet,&dwDataRetLen);	
//
//	if(DataRet)
//	{
//		mLogLevel = DataRet;
//	}
//	else
//	{
//		mLogLevel = PLOG_DEBUG_LEVEL;
//	}
//	return mLogLevel;
//}


//later

#if 0
void PLog::Text(int level,string strText)
{
	char strCurrTime[255] = {0};

	if(level)
	{
		EnterCriticalSection(&logfileCriticalSection);
		ofstream logFile;
		string path = logFileFullPath;
		logFile.open(path.c_str(), ios::out | ios::app );
		if(logFile.is_open())
		{
			GetSystemTimeString(strCurrTime);
			logFile<<endl<<"("<<strCurrTime<<")"<<"["<<GetCurrentThreadId()<<"]"<<" "<<"DEBUG"<<strText;
			size_t filesize = logFile.tellp();
			logFile.close();
			RotateFile(path, filesize);
		}
		LeaveCriticalSection(&logfileCriticalSection);

	}
}
void PLog::Text(string strText)
{
	char strCurrTime[255] = {0};

	EnterCriticalSection(&logfileCriticalSection);
	ofstream logFile;
	//cout<<"In create xml";
	string path = logFileFullPath;
	logFile.open(path.c_str(), ios::out | ios::app );
	if(logFile.is_open())
	{
		GetSystemTimeString(strCurrTime);
		logFile<<endl<<"("<<strCurrTime<<")"<<"["<<GetCurrentThreadId()<<"]"<<" "<<strText;
		size_t filesize = logFile.tellp();
		logFile.close();
		RotateFile(path, filesize);
	}
	LeaveCriticalSection(&logfileCriticalSection);
}
void PLog::Text(string strText,int data)
{
	char strCurrTime[255] = {0};

	EnterCriticalSection(&logfileCriticalSection);
	ofstream logFile;
	string path = logFileFullPath;
	logFile.open(path.c_str(), ios::out | ios::app );
	if(logFile.is_open())
	{
		GetSystemTimeString(strCurrTime);
		logFile<<endl<<"("<<strCurrTime<<")"<<"["<<GetCurrentThreadId()<<"]"<<" "<<strText<<" "<<data;
		size_t filesize = logFile.tellp();
		logFile.close();
		RotateFile(path, filesize);
	}
	LeaveCriticalSection(&logfileCriticalSection);
}







void PLog::Text(string strText,string strTexttwo)
{
	char strCurrTime[255] = {0};

	EnterCriticalSection(&logfileCriticalSection);
	ofstream logFile;
	string path = logFileFullPath;
	logFile.open(path.c_str(), ios::out | ios::app );
	if(logFile.is_open())
	{

		GetSystemTimeString(strCurrTime);
		logFile<<endl<<"("<<strCurrTime<<")"<<"["<<GetCurrentThreadId()<<"]"<<" "<<strText<<" "<<strTexttwo;
		size_t filesize = logFile.tellp();
		logFile.close();
		RotateFile(path, filesize);
	}
	LeaveCriticalSection(&logfileCriticalSection);
}

#endif

