/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Defines common classes which are used to describe observed SWI events.
*
*/


/**
 @file
 @internalComponent
 @released
*/
  
 #ifndef SWIOBSERVEDINFO_H
 #define SWIOBSERVEDINFO_H
 
 #include <e32cmn.h>
 #include <swi/swiobcommon.h>
 
 namespace Swi
 	{
 	NONSHARABLE_CLASS(CObservationData) : public CBase
	/**
		Class for a single observation record data. Contains the basic information about
		a changed file by the SWI operation. Each observed SWI event in an observation log
		corresponds to a single observation data.	
	 */
		{
	public:
		IMPORT_C static CObservationData* NewLC();

		IMPORT_C static CObservationData* NewL(const TDesC& aFileName, const TSecureId& aSid, const TUint8 aFileFlag);
		IMPORT_C static CObservationData* NewLC(const TDesC& aFileName, const TSecureId& aSid, const TUint8 aFileFlag);
		
		IMPORT_C void InternalizeL(RReadStream& aStream);
		IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
			
		~CObservationData();
			
	private:
		CObservationData();
		CObservationData(const TSecureId& aSid, const TUint8 aFileFlag);
		void ConstructL(const TDesC& aFileName);
			
	public:
		HBufC* iFileName; 	///< Fully qualified file path.
		TSecureId iSid;   	///< Secure Id of the file.
		/**
			Gives information about the file: whether the file has been removed 
			from the system and the type of the file (exe or dll).
		 */
		TUint8 iFileFlag;
		};
			
		
	NONSHARABLE_CLASS(CObservationHeader) : public CBase
	/**
		Class that contains all observation records related with a specific package. 
		In an observation log file, each header info corresponds to a specific package.	
	 */
		{
	public:
		IMPORT_C static CObservationHeader* NewLC();

		IMPORT_C static CObservationHeader* NewL(const TUid& aPckgUid, const TPackageType aPckgType, const TOperationType aOpType);
		IMPORT_C static CObservationHeader* NewLC(const TUid& aPckgUid, const TPackageType aPckgType, const TOperationType aOpType);
			
		IMPORT_C void InternalizeL(RReadStream& aStream);
		IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
				
		~CObservationHeader();
			
	private:
		CObservationHeader();
		CObservationHeader(const TUid& aPckgUid, const TPackageType aPckgType, const TOperationType aOpType);
			
	public:
		TUid iPckgUid;				///< The package Uid of the package which copies/deletes the file.
		TPackageType iPckgType;		///< The type of package (SA, SP, PU, etc.)
		TOperationType iOpType;     ///< Operation type: Install, Uninstall, Restore etc.
		};
		
		
	NONSHARABLE_CLASS(CObservationFilter) : public CBase
	/**
		Class that represents a filter list which is used to filter SWI observation.
		SWI Observer ECOM plug-in implementations declares the folders in which they
		are intrested by using this class.
	 */
		{
	public:
		IMPORT_C static CObservationFilter* NewL();	
		IMPORT_C static CObservationFilter* NewLC();
		
		IMPORT_C void AddFilterL(const HBufC* aFilter);
		IMPORT_C TBool FindF(const HBufC* aFilter) const;
		
		~CObservationFilter();
			
	private:
		CObservationFilter();
			
	private:
		RPointerArray<HBufC> iFilterList; ///< The list of filters used to filter the observations.
		};
 
 	} // End of namespace Swi
 
 #endif