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
* Declares SCR subsession implementation classes on the server side.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SCRSUBSESSION_H
#define SCRSUBSESSION_H

#include "scrserver.h"
#include "usifcommon.h"

namespace Usif
	{
	// forward declarations
	class CStatement;
	class CComponentEntry;
	class CComponentFilter;
	
	class CScrSubsession : public CScsSubsession
	/**
		Base SCR Subsession. Owns handle to an existing database session.
	 */
		{
	public:
		virtual ~CScrSubsession();
		
	protected:
		CScrSubsession(CScrSession &aSession);
		void ConstructL();
		
	public:
		CStatement *iStatement;
		// Pointer to the server object. Owned by the session object. 
		// Do NOT delete. Added to make the code cleaner and more consistent.
		CScrServer *iServer; 
		// Pointer to the session object. The owner session passes its own pointer. 
		// Do NOT delete. Added to make the code cleaner and more consistent.
		CScrSession *iSession;
		};
		
	class CCompViewSubsessionContext : public CBase
		{
	friend class CScrRequestImpl;
	friend class CComponentViewSubsession;
	public:
		~CCompViewSubsessionContext();
	private:
		TInt iLastSoftwareTypeId;
		HBufC* iLastSoftwareTypeName;
		RArray<TComponentId> iComponentFilterSuperset; ///< A helper array for the component filter. The filter conditions are split into two, and this variable is used for maintaing all components matching the first set of conditions.
		TBool iFilterSupersetInUse; ///< Specifies whether the filter superset array should be used when iterating over the filter query.		
		};
	
	class CComponentViewSubsession : public CScrSubsession
	/**
		This object is created for each component view subsession opened by the SCR client.
		Handles component view creation by using a provided filter and returns the row(s) of the view.
	 */
		{
	public:
		static CComponentViewSubsession* NewL(CScrSession& aSession);
		~CComponentViewSubsession();
		// Implement CScsSession
		TBool DoServiceL(TInt aFunction, const RMessage2& aMessage);
			
	private:
		CComponentViewSubsession(CScrSession& aSession);
		void ConstructL();
			
	private:
		CComponentFilter* iFilter;
		CComponentEntry *iComponentEntry;
		RPointerArray<CComponentEntry> iComponentEntryList;
		CCompViewSubsessionContext* iSubsessionContext;
		};	
	
	class CFileListSubsession : public CScrSubsession
	/**
		This object is created for each file list subsession opened by the SCR client
		to retrieve the list of files owned by a given component.
	 */
		{
	public:
		static CFileListSubsession* NewL(CScrSession& aSession);
		~CFileListSubsession();
		// Implement CScsSession
		TBool DoServiceL(TInt aFunction, const RMessage2& aMessage);
				
	private:
		CFileListSubsession(CScrSession& aSession);
				
	private:
		HBufC *iFilePath;
		RPointerArray<HBufC> iFileList;
		};	
	
	}// End of namespace Usif


#endif /* SCRSUBSESSION_H */
