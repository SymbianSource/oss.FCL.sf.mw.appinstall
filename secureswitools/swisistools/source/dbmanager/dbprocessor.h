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
* scrdatabase.h
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#ifndef DBPROCESSOR_H
#define DBPROCESSOR_H

#pragma warning(disable: 4786)
#pragma warning(disable: 4273)

#include "./sqlite/sqlite3.h"
#include "symbiantypes.h"
#include <string>

#ifdef __linux__
typedef void* HINSTANCE;
#else
#include <windows.h>
#endif // __linux__

// Forward declarations
class CStatement;

typedef int (*FnPtr_sqlite3_open)(const char *filename, sqlite3 **ppDb );
typedef int (*FnPtr_sqlite3_prepare_v2)(sqlite3 *db, const char *zSql, int nByte, sqlite3_stmt **ppStmt, const char **pzTail );
typedef int (*FnPtr_sqlite3_step)(sqlite3_stmt*);
typedef int (*FnPtr_sqlite3_finalize)(sqlite3_stmt *pStmt);
typedef int (*FnPtr_sqlite3_prepare_v2)(sqlite3 *db, const char *zSql, int nByte, sqlite3_stmt **ppStmt, const char **pzTail );
typedef int (*FnPtr_sqlite3_finalize)(sqlite3_stmt *pStmt);
typedef int (*FnPtr_sqlite3_bind_text)(sqlite3_stmt*, int, const char*, int n, void(*)(void*));
typedef int (*FnPtr_sqlite3_bind_text16)(sqlite3_stmt*, int, const void*, int, void(*)(void*));
typedef int (*FnPtr_sqlite3_bind_int)(sqlite3_stmt*, int, int);
typedef int (*FnPtr_sqlite3_step)(sqlite3_stmt*);
typedef int (*FnPtr_sqlite3_reset)(sqlite3_stmt*);
typedef int (*FnPtr_sqlite3_clear_bindings)(sqlite3_stmt*);
typedef sqlite3_int64 (*FnPtr_sqlite3_last_insert_rowid)(sqlite3*);
typedef int (*FnPtr_sqlite3_extended_result_codes) (sqlite3*, int onoff);
typedef int (*FnPtr_sqlite3_close) (sqlite3 *);
typedef const char* (*FnPtr_sqlite3_errmsg) (sqlite3*);
typedef int (*FnPtr_sqlite3_errcode) (sqlite3 *db);
typedef int (*FnPtr_sqlite3_bind_int64) (sqlite3_stmt*, int, sqlite3_int64);
typedef const void* (*FnPtr_sqlite3_column_text16) (sqlite3_stmt*, int iCol);
typedef int (*FnPtr_sqlite3_column_bytes16) (sqlite3_stmt*, int iCol);
typedef sqlite3_int64 (*FnPtr_sqlite3_column_int64) (sqlite3_stmt*, int iCol);
typedef int (*FnPtr_sqlite3_column_int) (sqlite3_stmt*, int iCol);
typedef int (*FnPtr_sqlite3_column_count) (sqlite3_stmt *pStmt);
typedef int (*FnPtr_sqlite3_column_type) (sqlite3_stmt*, int iCol);
typedef int (*FnPtr_sqlite3_prepare16_v2) (sqlite3 *db, const void *zSql,int nByte,sqlite3_stmt **ppStmt, const void **pzTail );
typedef int (*FnPtr_sqlite3_bind_blob) (sqlite3_stmt*, int, const void*, int n, void(*)(void*));

class TDbLibrary
	{
	public:
		~TDbLibrary();
		TDbLibrary(const std::string& aDllPath);

	private:
		void LoadSqlLibrary(const std::string& aDllPath);
		void LoadFunctions();
		void VerifyLoadedFunction(void* aFnPtr);

	public:
		FnPtr_sqlite3_open			sqlite3_open;
		FnPtr_sqlite3_prepare_v2		sqlite3_prepare_v2;
		FnPtr_sqlite3_step			sqlite3_step;
		FnPtr_sqlite3_finalize		sqlite3_finalize;
		FnPtr_sqlite3_bind_text 		sqlite3_bind_text;
		FnPtr_sqlite3_bind_text16	sqlite3_bind_text16;
		FnPtr_sqlite3_bind_int 		sqlite3_bind_int;
		FnPtr_sqlite3_reset 			sqlite3_reset;
		FnPtr_sqlite3_clear_bindings	sqlite3_clear_bindings;
		FnPtr_sqlite3_last_insert_rowid		sqlite3_last_insert_rowid;
		FnPtr_sqlite3_extended_result_codes	sqlite3_extended_result_codes;

		FnPtr_sqlite3_close		sqlite3_close;
		FnPtr_sqlite3_errmsg		sqlite3_errmsg;
		FnPtr_sqlite3_errcode	sqlite3_errcode;
		FnPtr_sqlite3_bind_int64 sqlite3_bind_int64;
		FnPtr_sqlite3_column_text16	sqlite3_column_text16;
		FnPtr_sqlite3_column_bytes16 sqlite3_column_bytes16;
		FnPtr_sqlite3_column_int64	sqlite3_column_int64;
		FnPtr_sqlite3_column_int		sqlite3_column_int;
		FnPtr_sqlite3_column_count	sqlite3_column_count;
		FnPtr_sqlite3_column_type	sqlite3_column_type;
		FnPtr_sqlite3_prepare16_v2	sqlite3_prepare16_v2;
		FnPtr_sqlite3_bind_blob		sqlite3_bind_blob;

	private:
		HINSTANCE sqLiteHndl;
		
	};


#ifdef __TOOLS2_LINUX__
typedef enum {
	allowSlashConversion = 0,
	avoidSlashConversion
} SlashConversionFlags;

// utf16WString represents the UTF-16 data(WINDOWS wstring).
typedef std::basic_string<unsigned short int> utf16WString;


inline void ConvertToWindowsSpecificPaths(std::wstring& aPath)
{
		std::wstring::size_type idx = 0;
		while( (idx = aPath.find(L"//", idx)) != std::wstring::npos)
		{
			aPath.replace( idx, 2, L"\\\\" );
		}

		idx = 0;

		while( (idx = aPath.find(L"/", idx)) != std::wstring::npos)
		{
			aPath.replace( idx, 1, L"\\" );
		}
}

inline void ConvertToLinuxSpecificPaths(std::wstring& aPath)
{
	 std::wstring::size_type idx = 0;
     while( (idx = aPath.find(L"\\\\", idx)) != std::wstring::npos)
     {
             aPath.replace( idx, 2, L"//" );
     }

     idx = 0;

     while( (idx = aPath.find(L"\\", idx)) != std::wstring::npos)
     {
             aPath.replace( idx, 1, L"/" );
     }
}

#endif

/**
	An instance of this class is used to execute all types of SQL statements with or without
	parameters. 
*/
	
class CStatement
	{
		
	public:

		/**
			Creates a new CStatement object on the heap.
			
			@param aStmtHandle Pointer to the statement handle.
			@return The newly created CStatement object.
			
		 */
		CStatement(sqlite3_stmt* aStmtHandle, const TDbLibrary& aLibraryHandler, sqlite3& aDbHandle);

		~CStatement();
		
		/** 
		 	If the SQL statement being executed returns any data, this function makes
		 	a new row of data ready for processing. The values may be accessed using
		 	the column access functions (@see CStatement::StrColumnL and @see CStatement::IntColumnL).
		 	
			When this function is called again to retrieve the next row of data, the previous row data
			is not accessible any more.
			
			If the caller wants to close the statement before retrieving all the rows, it needs to
			just destroy the CStatement object.
			
			@return Returns EFalse if no more rows are available. 
		 */
		bool ProcessNextRow();
		
		/**
			Eexecutes the prepared SQL statement. This function is appropriate to execute
			SQL statements which do NOT return a result row set (e.g. INSERT and UPDATE).
			If the SQL satetement contains parameters, the prepared statement can be bound
			and executed many times.
				 	
			@param aStatement The statement which will be executed.
		 */
		void ExecuteStatement();
		
		/**
			Sets the parameter given with the index value to the specified 32-bit integer value.
			A parameter value can be set:
			- immediately after this object has been created
			- after a call to @see CStatement::Reset 
			@param aParameterIndex The index value identifying the parameter; the first parameter 
			       has an index of 1.
			@param The 32-bit integer value to be assigned to the parameter.
		 */
		void BindInt(int aParameterIndex, int aParameterValue);
		
		/**
			Sets the parameter given with the index value to the specified 64-bit integer value.
			A parameter value can be set:
			- immediately after this object has been created
			- after a call to @see CStatement::Reset 
			@param aParameterIndex The index value identifying the parameter; the first parameter 
			       has an index of 1.
			@param The 64-bit integer value to be assigned to the parameter.
		 */
		void BindInt64(int aParameterIndex, TInt64 aParameterValue);

		/**
			Sets the parameter given with the index value to the specified 16-bit descriptor.
			A parameter value can be set:
			- immediately after this object has been created
			- after a call to @see CStatement::Reset 
			@param aParameterIndex The index value identifying the parameter; the first parameter 
			       has an index of 1.
			@param aParameterStr The 16-bit descriptor whose content is to be assigned to the parameter.
			@param aConvertSlash The integer value which is used only under LINUX platform specifies whether
								 to convert any LINUX specific paths to WINDOWS specific paths in aParameterStr.
								 This helps in avoiding the wrong interpretation of the DataType strings
								 having the syntax (DataType/Format) as PATHs.
		 */
		#ifdef __TOOLS2_LINUX__
		void BindStr(TInt aParameterIndex, const std::wstring &aParameterStr, int aConvertSlash=allowSlashConversion);
		#else
		void BindStr(TInt aParameterIndex, const std::wstring &aParameterStr);
		#endif
		
		void BindBinary(int aParameterIndex, const std::string &aParameterStr);

		void BindBinary(int aParameterIndex, const std::wstring &aParameterStr);

		/**
			Resets this SQL statement object to its initial state and makes it ready to be executed again.
			Any SQL statement parameters that had values bound to them, retain their values.
			If this object processes a parameterised SQL statement, then the parameter values 
			can be bound after the call to Reset().
		 */
		void Reset();

		int IntColumn(int aColumnId ) const;

		TInt64 Int64Column(int aColumnId ) const;

		std::wstring StrColumn(int ColmnnId) const;
		
	private:
		void CheckSqlErrCode(int aErrorCode);

	private:
		sqlite3_stmt* iStmtHandle; //SQL statement handle
		
		sqlite3& iDbHandle;	
		const TDbLibrary& iLibraryHandler;

		};	


/**
	This class is a wrapper over the SQLite interfaces for acessing the database. 
*/
class CDbProcessor

	{
	
	public:
		/**
		 	Creates a new database object with an handle to the given database file.
		 	
		 	@param aDbName The name of the database.
		 */
		CDbProcessor(const std::string& aDllPath, const std::string& aDbName);
		
		
		/**
			Destructor. The handle to the database file is closed.
		 */
		~CDbProcessor();
		
		/**
		    Prepares the provided SQL statement for execution and returns 
		    the prepared statement object which can be executed later.  

			The SQL statement must contain a single statement and end with semicolon(;).
			
			@param aStatementStr The statement which will be prepared.
			@return A pointer to the statement object which has got a handle to the result rows set.
			        The returned object is left on the cleanup stack.
		 */
		CStatement* PrepareStatement(const std::string& aStatementStr);
		
		/**
			@return The row id of the most recent successful insert into the database from this connection.
			@leave ESqlNotFoundError If no successful inserts have ever occurred on this database connection.
		 */
		TInt64 LastInsertedId();

	public:
		void CheckSqlErrCode(int aErrorCode);
		void CheckSqlErrCode(int aErrorCode, const char* aErrorMessage ) ;
		void CheckSqlErrCode(int aErrorCode, std::string& aErrorMessage ) ;

	private:
		sqlite3* iDbHandle;		///< The database handle, owned by CDatabaseImplementation.	
		TDbLibrary iLibraryHandler;
		};

#endif // DBPROCESSOR_H
