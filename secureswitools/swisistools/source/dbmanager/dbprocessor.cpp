/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* scrdatabase.cpp
* Implements the database handling which is mostly a replica of the SCR Data Layer API 
* which performs all interaction with the underlying database.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#include "dbprocessor.h"
#include "exception.h"
#include "logs.h"
#include "util.h"
#include "symbiantypes.h"
#include "utf8_wrapper.h"
#include "../sisxlibrary/utility.h"
#include <string>
#include <cassert>

#ifdef __linux__
#include <dlfcn.h>


void* GetProcAddress(HINSTANCE aHandle, const char* aSymbol)
	{
	return dlsym(aHandle, aSymbol);
	}

HINSTANCE LoadLibraryA(const char* aLibraryName)
	{
	HINSTANCE handleUsingDefaultSearchPath = dlopen(aLibraryName, RTLD_LAZY);

	if( handleUsingDefaultSearchPath == NULL )
	{
		// Once the dlopen() fails by not finding the aLibraryName in the default
		// path specified by LD_LIBRARY_PATH, we will look in the epoc32/tools 
		// path as the second option.

		const char* epocRoot = getenv("EPOCROOT");		
		if(NULL == epocRoot)
			{
			throw CException("EPOCROOT environment variable not specified.", ExceptionCodes::EEnvNotSpecified);
			}
		std::string epocRootStr(epocRoot); 

		std::string absPathToLibrary = epocRootStr + std::string("epoc32/tools/") + std::string(aLibraryName);
		HINSTANCE handleUsingAbsSearchPath = dlopen(absPathToLibrary.c_str(), RTLD_LAZY);

		return handleUsingAbsSearchPath;
	}

	return handleUsingDefaultSearchPath;
	}

int FreeLibrary(HINSTANCE aHandle)
	{
	// FreeLibrary returns non-zero value on success whereas
	// dlcose returns zero on success.
	return (dlclose(aHandle) == 0)? true: false;
	}

std::string GetErrorMessage()
	{
	return dlerror();
	}

static utf16WString utf32WString2utf16WString(std::wstring& aParameter)
{
	int strLen = aParameter.length();
	const wchar_t * source = aParameter.c_str();
	unsigned short int* buffer = new unsigned short int[(strLen + 1) * 2];

	// Using a temp variable in place of buffer as ConvertUTF32toUTF16 modifies the source pointer passed.
	unsigned short int* temp = buffer;

	ConvertUTF32toUTF16(&source, source + strLen, &temp,  temp + strLen, lenientConversion);

	// Appending NUL to the converted buffer.
	*temp = 0;

	utf16WString utf16Ws;
	utf16Ws.resize(strLen);

	// The built-in basic_string template class copy operation
	// truncates when a NUL is encountered when a c_str() is
	// used to construct the required string.
	// So, if aParameter is any hashable string having the
	// syntax : swtypeName + L'\0' + someId then, we will end
	// up returning only part of the converted UTF-16 string.
	// Hence, we resort to the explicit copy operation with
	// two bytes at a time.
	while( strLen-- )
	{
		utf16Ws[ strLen ] = buffer[ strLen ];
	}

	delete[] buffer;

	return utf16Ws;
}

static std::wstring utf16WString2utf32WString(utf16WString& aParameter)
{	
	int strLen = aParameter.length();
	const unsigned short int* source = aParameter.c_str();
	wchar_t* buffer = new wchar_t[ strLen + 1 ];

	// Using a temp variable in place of buffer as ConvertUTF16toUCS4 modifies the source pointer passed.
	wchar_t* temp = buffer;

	ConvertUTF16toUCS4(&source, source + strLen, &temp, temp + strLen);

	// Appending NUL to the converted buffer.
	*temp = 0;

	std::wstring utf32Ws(buffer);

	delete[] buffer;

	return utf32Ws;
}

#else
std::string GetErrorMessage()
	{
	LPCVOID lpMsgBuf;
		
	DWORD err = GetLastError();
	FormatMessage	(	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
						FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf,
						0, NULL 
					);
	std::wstring wErrMsg((wchar_t*)lpMsgBuf);
	return wstring2string(wErrMsg);
	}
#endif // __linux__

TDbLibrary* iLibraryHandler = NULL;

TDbLibrary::TDbLibrary(const std::string& aDllPath)
	{
	LoadSqlLibrary(aDllPath);
	LoadFunctions();
	}

/** 
* Failing to unload the library is not really a critical failure, when a dll is unloaded the 
* current process sends a notification to the dll to detach itself from the process, which 
* means the dll can process memory cleanup operations before it unloads. The only case where 
* freelibrary might throw an error is if the library was not loaded in the first place, an 
* error which would be caught beforehand. Hence if the library cannot be unloaded corresponding 
* error message is logged.
*/
TDbLibrary::~TDbLibrary()
	{
	int retCode = FreeLibrary(sqLiteHndl);
	if(retCode == 0)
		{
		//LOGERROR(GetErrorMessage());
		}
	}

void TDbLibrary::LoadSqlLibrary(const std::string& aDllPath)
	{
	sqLiteHndl = LoadLibraryA(aDllPath.c_str());
 
	// Check to see if the library was loaded successfully 

	if (sqLiteHndl != 0)
		{
		//LOGINFO("Library successfully loaded!");
		}
	else
		{
		std::string errMsg("Failed to load SQLite library - ");
		errMsg.append(aDllPath);
		//LOGERROR(errMsg);
		throw CException(errMsg,ExceptionCodes::ELibraryLoadError);
		}
	}

void TDbLibrary::LoadFunctions()
	{
	sqlite3_open = (FnPtr_sqlite3_open)GetProcAddress(sqLiteHndl,"sqlite3_open");
	VerifyLoadedFunction(sqlite3_open);
	
	sqlite3_prepare_v2	= (FnPtr_sqlite3_prepare_v2) GetProcAddress(sqLiteHndl,"sqlite3_prepare_v2");	
	VerifyLoadedFunction(sqlite3_prepare_v2);
	
	sqlite3_step		= (FnPtr_sqlite3_step) GetProcAddress(sqLiteHndl,"sqlite3_step");
	VerifyLoadedFunction(sqlite3_step);
	
	sqlite3_finalize	= (FnPtr_sqlite3_finalize) GetProcAddress(sqLiteHndl,"sqlite3_finalize");
	VerifyLoadedFunction(sqlite3_finalize);
	
	sqlite3_bind_text	= (FnPtr_sqlite3_bind_text) GetProcAddress(sqLiteHndl,"sqlite3_bind_text");
	VerifyLoadedFunction(sqlite3_bind_text);
	
	sqlite3_bind_text16 = (FnPtr_sqlite3_bind_text16) GetProcAddress(sqLiteHndl,"sqlite3_bind_text16");
	VerifyLoadedFunction(sqlite3_bind_text16);
	
	sqlite3_bind_int	= (FnPtr_sqlite3_bind_int) GetProcAddress(sqLiteHndl,"sqlite3_bind_int");
	VerifyLoadedFunction(sqlite3_bind_int);
	
	sqlite3_reset		= (FnPtr_sqlite3_reset) GetProcAddress(sqLiteHndl,"sqlite3_reset");
	VerifyLoadedFunction(sqlite3_reset);
	
	sqlite3_clear_bindings		= (FnPtr_sqlite3_clear_bindings) GetProcAddress(sqLiteHndl,"sqlite3_clear_bindings");
	VerifyLoadedFunction(sqlite3_clear_bindings);
	
	sqlite3_last_insert_rowid	= (FnPtr_sqlite3_last_insert_rowid) GetProcAddress(sqLiteHndl,"sqlite3_last_insert_rowid");
	VerifyLoadedFunction(sqlite3_last_insert_rowid);
	
	sqlite3_extended_result_codes = (FnPtr_sqlite3_extended_result_codes) GetProcAddress(sqLiteHndl, "sqlite3_extended_result_codes" );
	VerifyLoadedFunction(sqlite3_extended_result_codes);
	
	sqlite3_close			= (FnPtr_sqlite3_close)GetProcAddress(sqLiteHndl,"sqlite3_close");
	VerifyLoadedFunction(sqlite3_close);
	
	sqlite3_errmsg			= (FnPtr_sqlite3_errmsg)GetProcAddress(sqLiteHndl,"sqlite3_errmsg");
	VerifyLoadedFunction(sqlite3_errmsg);
	
	sqlite3_errcode			= (FnPtr_sqlite3_errcode)GetProcAddress(sqLiteHndl,"sqlite3_errcode");
	VerifyLoadedFunction(sqlite3_errcode);
	
	sqlite3_bind_int64		= (FnPtr_sqlite3_bind_int64)GetProcAddress(sqLiteHndl,"sqlite3_bind_int64");
	VerifyLoadedFunction(sqlite3_bind_int64);
	
	sqlite3_column_text16	= (FnPtr_sqlite3_column_text16)GetProcAddress(sqLiteHndl,"sqlite3_column_text16");
	VerifyLoadedFunction(sqlite3_column_text16);
	
	sqlite3_column_bytes16	= (FnPtr_sqlite3_column_bytes16)GetProcAddress(sqLiteHndl,"sqlite3_column_bytes16");
	VerifyLoadedFunction(sqlite3_column_bytes16);
	
	sqlite3_column_int64	= (FnPtr_sqlite3_column_int64)GetProcAddress(sqLiteHndl,"sqlite3_column_int64");
	VerifyLoadedFunction(sqlite3_column_int64);
	
	sqlite3_column_int		= (FnPtr_sqlite3_column_int)GetProcAddress(sqLiteHndl,"sqlite3_column_int");
	VerifyLoadedFunction(sqlite3_column_int);
	
	sqlite3_column_count	= (FnPtr_sqlite3_column_count)GetProcAddress(sqLiteHndl,"sqlite3_column_count");
	VerifyLoadedFunction(sqlite3_column_count);
	
	sqlite3_column_type		= (FnPtr_sqlite3_column_type)GetProcAddress(sqLiteHndl,"sqlite3_column_type");
	VerifyLoadedFunction(sqlite3_column_type);
	
	sqlite3_prepare16_v2	= (FnPtr_sqlite3_prepare16_v2)GetProcAddress(sqLiteHndl,"sqlite3_prepare16_v2");
	VerifyLoadedFunction(sqlite3_prepare16_v2);

	sqlite3_bind_blob = (FnPtr_sqlite3_bind_blob)GetProcAddress(sqLiteHndl,"sqlite3_bind_blob");
	VerifyLoadedFunction(sqlite3_bind_blob);

	}

void TDbLibrary::VerifyLoadedFunction(void* aFnPtr)
	{
	if(aFnPtr != NULL)
		return;

	std::string errMsg = GetErrorMessage();
	//LOGERROR(errMsg);
	throw CException(errMsg,ExceptionCodes::ELibraryLoadError);
	}


//
// CDbProcessor
// 

// Constructor
CDbProcessor::CDbProcessor(const std::string& aDllPath, const std::string& aDbName )
	:iLibraryHandler(aDllPath)
	{
	
	// Then open the SCR database
	TInt err = iLibraryHandler.sqlite3_open(aDbName.c_str(), &iDbHandle);
	
	// Check the returned error code
	CheckSqlErrCode(err);
	// If comes here, means the database file has been opened successfully
	// Now, enable the extended result codes feature of SQLite. In SQLite, this feature is 
	// disabled by default for historical compatibility.
	err = iLibraryHandler.sqlite3_extended_result_codes(iDbHandle, 0);
	// Check the returned error code
	CheckSqlErrCode(err,"Failed to activate the extended error mechanism.");
	}

// Destructor
CDbProcessor::~CDbProcessor()
	{
	//LOGINFO("Closing the SCR database connection.");

	// Close the db handle
	TInt err = iLibraryHandler.sqlite3_close(iDbHandle);
	
	if(err != SQLITE_OK)
		{
		std::string errMsg("Failed to close the database handle.");
		//LOGERROR(errMsg);
		}
	}


CStatement* CDbProcessor::PrepareStatement(const std::string& aStatementStr)
	{
	// For statements which start with the SELECT key word, this function creates an sql statement  
	// object and returns it.
	sqlite3_stmt* stmtHandle = NULL; // Temporary statement handle
	const char* stmtTail = NULL;	 // Pointer to unused portion of Sql statement.                         
	TInt err = iLibraryHandler.sqlite3_prepare_v2(iDbHandle, aStatementStr.c_str(), aStatementStr.size(), &stmtHandle, &stmtTail);
	
	// Check the returned error code
	CheckSqlErrCode(err);
	// Since we expect single statement, stmtTail pointer should be NULL or point to zero.
	if(stmtTail && stmtTail[0] != 0)
		{
		err = iLibraryHandler.sqlite3_finalize(stmtHandle);
		std::string errMessage = "There is a problem with the provided SQL statement. It may contain more \
								than one statement. Or It may not be terminated with semicolon. \
								Or It may contain a space or invalid char after semicolon.";
		//LOGERROR(errMessage);
		CheckSqlErrCode(err,errMessage);
		}
	// stmtHandle can be NULL for statements like this: ";"
	if(!stmtHandle)
		{
		throw CException(ExceptionCodes::ESqlArgumentError);
		}

	std::string str = "Query:";
	//LOGINFO(str+aStatementStr);
	// The statement object which carries handle to the result set of the sql statement
	CStatement* stmtObj = new CStatement(stmtHandle,iLibraryHandler,*iDbHandle); 
	
	return stmtObj;	
	}


TInt64 CDbProcessor::LastInsertedId()
	{
	TInt retVal = (TInt)iLibraryHandler.sqlite3_last_insert_rowid(iDbHandle); 
	// it is now expected that row ids in scr will require 64-bit storage, so cast the return value to TInt
	if(retVal <= 0)
		{
		throw CException(ExceptionCodes::ESqlNotFoundError);
		}
	return retVal;
	}

void CDbProcessor::CheckSqlErrCode(TInt aErrorCode, std::string& aErrorMessage ) 
	{
	if(aErrorCode != SQLITE_OK)
		{
		TInt errorCode = iLibraryHandler.sqlite3_errcode(iDbHandle);
		//LOGERROR(aErrorMessage);
		throw CException(aErrorMessage,aErrorCode);
		}
	}

void CDbProcessor::CheckSqlErrCode(TInt aErrorCode) 
	{

	if(aErrorCode != SQLITE_OK)
		{
		TInt errorCode = iLibraryHandler.sqlite3_errcode(iDbHandle);
		const char* errMsg = iLibraryHandler.sqlite3_errmsg(iDbHandle);
		CheckSqlErrCode(errorCode,errMsg);
		}
	}

void CDbProcessor::CheckSqlErrCode(TInt aErrorCode, const char* aErrorMessage ) 
	{
	std::string errMsg(aErrorMessage);
	CheckSqlErrCode(aErrorCode,errMsg);
	}

//
// CStatement
//

// Constructor
CStatement::CStatement(sqlite3_stmt* aStmtHandle, const TDbLibrary& aLibraryHandler , sqlite3& aDbHandle)
	:iStmtHandle(aStmtHandle),
	iLibraryHandler(aLibraryHandler),
	iDbHandle(aDbHandle)
	{
	// Make sure that the statement handle is never NULL.
	assert(iStmtHandle != NULL);
	}

// Destructor
CStatement::~CStatement()
	{
	TInt err = iLibraryHandler.sqlite3_finalize(iStmtHandle);
	if(SQLITE_OK != err)
		{
		std::string errMsg = "Failed to finalize the statement object.";
		//LOGERROR(errMsg);
		throw CException(errMsg,err);
		}
	}

void CStatement::ExecuteStatement()
	{
	// If the statement doesn't return any result table, it should normally be executed
	// with sqlite3_exec. However, sqlite does not have a 16-bit version of sqlite3_exec.  
	// Therefore, the execution is made with PrepareStatementLC and ProcessNextRowL functions.
	//LOGINFO("Executing the prepared SCR SQL statement.");
	
	// Now, execute and check if the function has completed successfully by calling ProcessNextRowL.
	// If the function has failed, ProcessNextRowL will leave with one of the system wide error codes.
	while(ProcessNextRow())
		{}
	}


bool CStatement::ProcessNextRow()
	{
	TInt err = iLibraryHandler.sqlite3_step(iStmtHandle);
	
	switch(err)
		{
	case SQLITE_ROW: // A new row of data is ready for processing.
		return true;
	
	case SQLITE_DONE: // The statement has finished executing successfully.
		return false;
	default:		 
		CheckSqlErrCode(err);
		}// End of switch
	return false;
	}

void CStatement::BindInt(TInt aParameterIndex, TInt aParameterValue)
	{
	TInt err = iLibraryHandler.sqlite3_bind_int(iStmtHandle, aParameterIndex, aParameterValue);
	CheckSqlErrCode(err);
	}

void CStatement::BindInt64(TInt aParameterIndex, TInt64 aParameterValue)
	{
	TInt err = iLibraryHandler.sqlite3_bind_int64(iStmtHandle, aParameterIndex, aParameterValue);
	CheckSqlErrCode(err);
	}


#ifdef __TOOLS2_LINUX__
void CStatement::BindStr(TInt aParameterIndex, const std::wstring &aParameterStr, int aConvertSlash=allowSlashConversion)
#else
void CStatement::BindStr(TInt aParameterIndex, const std::wstring &aParameterStr)
#endif
	{
	/*
	 * Under LINUX : Because of the UTF-32 format of wstring, we can't directly use the sqlite3_bind_text16() which
	 * requires UTF-16 format. So, we convert the UTF-32 data into UTF-16 before using it.
	 *
	 * Under WINDOWS : No conversion required since wstring will be in UTF-16 format itself.
	 */

    #ifdef __LINUX__
	// Make sure that the wstring passed to this function is not having any trailing
	// explicit NUL( Preferably, pass c_str() part of wstring ).
    //
	// Only case in which you shouldn't pass c_str() is that the wstring has NUL as
	// part of its actual string content(like swtypename + L'\0' + someID etc).

	// In order to maintain the consistency of DB contents across the WINDOWS and LINUX platforms, before interacting
	// with the DB we convert the local paths into WINDOWS specific paths.
    //
	// If aParameterStr is not a PATH but contains a forward slash, we should restrain 
	// from the slash conversion. One such instance is MimeType.
	//
	std::wstring temp = aParameterStr;
	if( aConvertSlash == allowSlashConversion )
	{
	    ConvertToWindowsSpecificPaths(temp);
	}

    utf16WString utf16s = utf32WString2utf16WString(temp);

	TInt err = iLibraryHandler.sqlite3_bind_text16(iStmtHandle, aParameterIndex, utf16s.c_str(), aParameterStr.size()*2, SQLITE_TRANSIENT);

    #else
	TInt err = iLibraryHandler.sqlite3_bind_text16(iStmtHandle, aParameterIndex, aParameterStr.c_str(), aParameterStr.size()*2, SQLITE_TRANSIENT);
	#endif
	// The fifth argument has the value SQLITE_TRANSIENT, it means that SQLite makes its own private copy of the data immediately
	CheckSqlErrCode(err);
	}

void CStatement::BindBinary(TInt aParameterIndex, const std::string &aParameterStr)
	{
	TInt err = iLibraryHandler.sqlite3_bind_blob(iStmtHandle, aParameterIndex, aParameterStr.c_str(), aParameterStr.size(), SQLITE_TRANSIENT);
	// The fifth argument has the value SQLITE_TRANSIENT, it means that SQLite makes its own private copy of the data immediately
	CheckSqlErrCode(err);
	}

void CStatement::BindBinary(TInt aParameterIndex, const std::wstring &aParameterStr)
	{
	#ifdef __LINUX__
	// To maintain consistency of the binary equivalent of the wstring
	// being binded, we convert the wstring with UTF-32 encoding under LINUX
	// to UTF-16 encoding which is same as that of wstring under WINDOWS.
	
	std::wstring temp = aParameterStr;
	utf16WString utf16s = utf32WString2utf16WString(temp);

	TInt err = iLibraryHandler.sqlite3_bind_blob(iStmtHandle, aParameterIndex, utf16s.c_str(), aParameterStr.size()*2, SQLITE_TRANSIENT);
	#else
	TInt err = iLibraryHandler.sqlite3_bind_blob(iStmtHandle, aParameterIndex, aParameterStr.c_str(), aParameterStr.size()*2, SQLITE_TRANSIENT);
	#endif

	// The fifth argument has the value SQLITE_TRANSIENT, it means that SQLite makes its own private copy of the data immediately
	CheckSqlErrCode(err);
	}

void CStatement::Reset()
	{
	TInt err = iLibraryHandler.sqlite3_reset(iStmtHandle);
	CheckSqlErrCode(err);
	err = iLibraryHandler.sqlite3_clear_bindings(iStmtHandle);
	CheckSqlErrCode(err);
	}

void CStatement::CheckSqlErrCode(TInt aErrorCode) 
	{
	
	if(aErrorCode != SQLITE_OK)
		{
		TInt errorCode = iLibraryHandler.sqlite3_errcode(&iDbHandle);
		const char* errMsg = iLibraryHandler.sqlite3_errmsg(&iDbHandle);
		std::string errStr(errMsg);
		//LOGERROR(errStr);
		throw CException(errStr,errorCode);
		}
	}

int CStatement::IntColumn(int aColumnId ) const
	{
	return iLibraryHandler.sqlite3_column_int(iStmtHandle, aColumnId);
	}

std::wstring CStatement::StrColumn(int aColumnId ) const
	{
	/*
	 * Under LINUX : While writing onto DB, we bind the wstring after converting it into UTF-16 from
	 * UTF-32 format. So, now while reading we need to convert the UTF-16 data back to UTF-32
	 * format so that we can return the required UTF-32 wstring.
	 *
	 * Under WINDOWS : No conversion required since wstring will be in UTF-16 format itself.
	 */
	#ifdef __LINUX__

	utf16WString utf16S = iLibraryHandler.sqlite3_column_text16(iStmtHandle, aColumnId);
	std::wstring utf32S = utf16WString2utf32WString(utf16S);

	// The DB will have WINDOWS specific paths to maintain the consistency of DB contents across WINDOWS and LINUX platforms.
	// So, after reading under LINUX we will convert them into local paths.

    ConvertToLinuxSpecificPaths(utf32S);

	#else
	std::wstring utf32S(static_cast<const wchar_t*>(iLibraryHandler.sqlite3_column_text16(iStmtHandle, aColumnId)));
	#endif

	return utf32S;
	}

TInt64 CStatement::Int64Column(int aColumnId ) const
	{
	return iLibraryHandler.sqlite3_column_int64(iStmtHandle, aColumnId);
	}



