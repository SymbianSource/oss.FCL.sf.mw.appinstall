/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* SCR Data Layer API which performs all interaction with the underlying database.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SCRSCRDATABASE_H
#define SCRSCRDATABASE_H

#include <e32base.h>
#include <f32file.h>

namespace Usif
	{
	// Forward declarations
	class CDatabaseImplementation;
	class CStatementImplementation;
	class CStatement;
	
	NONSHARABLE_CLASS(CDatabase) : public CBase
	/**
		This class provides the means to connect the SCR database and execute all SQL statements.
		It is intended to be used as a singleton instance by the SCR sessions. 
	 */
		{
		friend class CStatement; // CStatement class needs to access CheckSqlErrCodeL
	public:
		/**
		 	Creates a new database object with an handle to the given database file.
		 	
			@param aDatabaseFile The database file handle.
			@param aJournalNameZ The journal file handle.
		 	@leave KErrNotFound The file opened is not a database file.
		 	@leave KErrUnknown The database file is missing. 
		 	@leave KErrCorrupt The database disk image is malformed.
		 	@leave KErrNoMemory An attempt to allocate memory has failed.
		 	@leave KErrInUse The database file is locked by another thread.
		 	@leave KErrAccessDenied Unable to open the database file. Access permission may be denied.
		 	@leave Or other system wide error codes
		 */
		IMPORT_C static CDatabase* NewL(RFile& aDatabaseFile, RFile& aJournalFile);
		
		/**
			Creates a new database object with an handle to the given database file and leaves
			the newly created object on the cleanup stack.
			
			@param aDatabaseFile The database file handle.
			@param aJournalNameZ The journal file handle.
		 	@see CDatabase::NewL
		 */
		IMPORT_C static CDatabase* NewLC(RFile& aDatabaseFile, RFile& aJournalFile);
		
		/**
			Destructor. The handle to the database file is closed.
		 */
		IMPORT_C ~CDatabase();
		
		/**
		    Prepares the provided SQL statement for execution and returns 
		    the prepared statement object which can be executed later.  

			The SQL statement must contain a single statement and end with semicolon(;).
			
			@param aStatementStr The satement which will be prepared.
			@return A pointer to the statement object which has got a handle to the result rows set.
			        The returned object is left on the cleanup stack.
			@leave KErrNotFound The database file could not be opened for some reason OR the table 
			                    or the requested record could not be found OR the database is empty. 
			@leave KErrCorrupt The database schema has changed.
			@leave KErrNoMemory An attempt to allocate memory has failed OR some kind of disk I/O 
			                    operation couldn't be performed (probably no space left on the disk)
			                    OR the database is too big (larger than 2GB in size).
			@leave KErrInUse A table in the database is locked.
			@leave KErrOverflow Too much data(probably more than 1 megabyte in a single row).
			@leave KErrCancel The operation in progress has been terminated externally.
			@leave KErrNotSupported The library has been used incorrectly,
			@leave KErrAbort Abort due to constraint violation.
			@leave KErrArgument This error value indicates that there was an error in the SQL
			                    statement that was passed into. Data type mismatch may have been
			                    occurred OR the provided SQL was badly constructed OR contains 
			                    more than one statement.
			@leave KErrUnknown An internal error in the underlaying database engine occurred.
			@leave KErrGeneral An unspecified error ocurred in the database engine.		
		 */
		IMPORT_C CStatement* PrepareStatementLC(const TDesC& aStatementStr);
			
		/**
			@return The row id of the most recent successful insert into the database from this connection.
			@leave KErrNotFound If no successful inserts have ever occurred on this database connection.
		 */
		IMPORT_C TInt LastInsertedIdL();
		
	private:
		CDatabase();
		void ConstructL(RFile& aDatabaseFile, RFile& aJournalFile);
		void CheckSqlErrCodeL(TInt aErr);
		
	private:
		CDatabaseImplementation* iDbImpl;	///< Pointer to the database implementation object.
		};
	
	
	
	NONSHARABLE_CLASS(CStatement) : public CBase
	/**
		An instance of this class is used to execute all types of SQL statements with or without
		parameters. 
	 */
		{
		friend class CDatabase; // Only CDatabase can construct an object of this class.
	public:
		IMPORT_C ~CStatement();
		
		/** 
		 	If the SQL statement being executed returns any data, this function makes
		 	a new row of data ready for processing. The values may be accessed using
		 	the column access functions (@see CStatement::StrColumnL and @see CStatement::IntColumnL).
		 	
			When this function is called again to retrieve the next row of data, the previous row data
			is not accessible any more.
			
			If the caller wants to close the statement before retrieving all the rows, it needs to
			just destroy the CStatement object.
			
			@return Returns EFalse if no more rows are available. 
			@leave Leaves with one of the error codes listed in @see CDatabase::PerformStatementLC
		 */
		IMPORT_C TBool ProcessNextRowL();
		
		/**
			Eexecutes the prepared SQL statement. This function is appropriate to execute
			SQL statements which do NOT return a result row set (e.g. INSERT and UPDATE).
			If the SQL satetement contains parameters, the prepared statement can be bound
			and executed many times.
				 	
			@param aStatement The satement which will be executed.
			@leave Leaves with one of the leave codes given in @see CStatement::PrepareStatementLC.
		 */
		IMPORT_C void ExecuteStatementL();
		
		/**
			Sets the parameter given with the index value to the specified 32-bit integer value.
			A parameter value can be set:
			- immediately after this object has been created
			- after a call to @see CStatement::Reset 
			@param aParameterIndex The index value identifying the parameter; the first parameter 
			       has an index of 1.
			@param The 32-bit integer value to be assigned to the parameter.
			@leave Leaves with one of the error codes listed in @see CDatabase::PerformStatementLC 
		 */
		IMPORT_C void BindIntL(TInt aParameterIndex, TInt aParameterValue);
		
		/**
			Sets the parameter given with the index value to the specified 64-bit integer value.
			A parameter value can be set:
			- immediately after this object has been created
			- after a call to @see CStatement::Reset 
			@param aParameterIndex The index value identifying the parameter; the first parameter 
			       has an index of 1.
			@param The 64-bit integer value to be assigned to the parameter.
			@leave Leaves with one of the error codes listed in @see CDatabase::PerformStatementLC 
		 */
		IMPORT_C void BindInt64L(TInt aParameterIndex, TInt64 aParameterValue);
		
		/**
			Sets the parameter given with the index value to the specified 16-bit descriptor.
			A parameter value can be set:
			- immediately after this object has been created
			- after a call to @see CStatement::Reset 
			@param aParameterIndex The index value identifying the parameter; the first parameter 
			       has an index of 1.
			@param aParameterStr The 16-bit descriptor whose content is to be assigned to the parameter.
			@leave KErrArgument If the input string's length is more than 512 characters
			@leave Leaves with one of the error codes listed in @see CDatabase::PerformStatementLC 
		 */
		IMPORT_C void BindStrL(TInt aParameterIndex, const TDesC &aParameterStr);
		
		/**
			Sets the parameter given with the index value to the specified 8-bit descriptor.
			A parameter value can be set:
			- immediately after this object has been created
			- after a call to @see CStatement::Reset 
			@param aParameterIndex The index value identifying the parameter; the first parameter 
			       has an index of 1.
			@param aParameterStr The 8-bit descriptor whose content is to be assigned to the parameter.
			@leave KErrArgument If the input string's length is more than 512 characters			
			@leave Leaves with one of the error codes listed in @see CDatabase::PerformStatementLC 
		 */
		IMPORT_C void BindBinaryL(TInt aParameterIndex, const TDesC8 &aParameterStr);		

		/**
            Sets the parameter given with the index value to the specified 8-bit descriptor.
            A parameter value can be set:
            - immediately after this object has been created
            - after a call to @see CStatement::Reset
             
            @param aParameterIndex The index value identifying the parameter; the first parameter 
                   has an index of 1.
            @param aParameterStr The 8-bit descriptor whose content is to be assigned to the parameter.
            @param aCustomLength The maximum characters allowed
            
            @leave KErrArgument If the input string's length is more than aCustomLength            
            @leave Leaves with one of the error codes listed in @see CDatabase::PerformStatementLC 
         */
		IMPORT_C void BindBinaryL(TInt aParameterIndex, const TDesC8 &aParameterStr, TUint aCustomLength);
		
		/** 
			Retrieves the value of a string column. The caller must know the string column index.
			In addition, the caller must make the copy if they want to process multiple rows at once.
			
			@param aColIdx The index of the column in the result set.
			@return The string value of the column given.
			@leave KErrArgument The supplied column index is not valid or its type is not string.
			@leave Or one of the error codes listed in @see CDatabase::PerformStatementLC
		*/
		IMPORT_C TPtrC StrColumnL(TInt aColIdx) const;
		
		/** 
			Retrieves the value of a raw binary data column. The caller must know the column index.
			In addition, the caller must make the copy if they want to process multiple rows at once.
			
			@param aColIdx The index of the column in the result set.
			@return The binary value of the column given.
			@leave KErrArgument The supplied column index is not valid or its type is not string.
			@leave Or one of the error codes listed in @see CDatabase::PerformStatementLC
		*/
		IMPORT_C TPtrC8 BinaryColumnL(TInt aColIdx) const;		
		
		/** 
			Retrieves the value of a integer column. The caller must know the integer column index.
			The return value is 64-bit integer.
			
			@param aColIdx The index of the column in the result set.
			@return The integer value of the column given.
			@leave KErrArgument The supplied column index is not valid or its type is not integer.
			@leave Or one of the error codes listed in @see CDatabase::PerformStatementLC
		*/
		IMPORT_C TInt64 Int64ColumnL(TInt aColIdx) const;
		
		/** 
			Retrieves the value of an integer column. The caller must know the integer column index.
			The return value is 32-bit integer. 
					
			@param aColIdx The index of the column in the result set.
			@return The integer value of the column given.
			@leave KErrArgument The supplied column index is not valid or its type is not integer.
			@leave Or one of the error codes listed in @see CDatabase::PerformStatementLC
		 */
		IMPORT_C TInt IntColumnL(TInt aColIdx) const;
				
		/**
			Resets this SQL statement object to its initial state and makes it ready to be executed again.
			Any SQL statement parameters that had values bound to them, retain their values.
			If this object processes a parameterised SQL statement, then the parameter values 
			can be bound after the call to Reset().
			@leave Leaves with one of the error codes listed in @see CDatabase::PerformStatementLC
		 */
		IMPORT_C void ResetL();
		
		/**
			Returns whether the specified field (column) of a row is NULL.
			Note that if the field is NULL, it does NOT mean that the corresponding fields of 
			other rows are NULL as well.
			@param aColIdx The index of the column in the result set.
			@return ETrue, if the field is NULL, otherwise EFalse.
			@leave KErrArgument The supplied column index is not valid.
		 */
		IMPORT_C TBool IsFieldNullL(TInt aColIdx) const;
		
	private:
		/**
			Creates a new CStatement object on the heap.
			
			@param aDb Refrence to the database object.
			@param aStmtImpl Pointer to the statement implementation. The ownership is also passed
			                 to the CStatement object.
			@return The newly created CStatement object.
			
		 */
		static CStatement* NewL(const CDatabase& aDb, CStatementImplementation* aStmtImpl);
		
		CStatement(const CDatabase& aDb, CStatementImplementation* aStmtImpl);
		void ValidateRequestedColumnL(TInt aColIdx, TInt& aColumnType) const;
		
	private:
		const CDatabase& iDb; ///< Reference to the database object which creates this statement object.
		CStatementImplementation* iStmtImpl; ///< Pointer to the SQL statement implementation
		};	
	} // namespace Usif

#endif // SCRSCRDATABASE_H
