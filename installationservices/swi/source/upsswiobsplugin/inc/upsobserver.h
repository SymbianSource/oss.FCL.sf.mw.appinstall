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
* Interface definition for UPS SWI Observer ECOM plug-in.
*
*/


/**
 @file
 @internalComponent
 @prototype
*/

#ifndef UPSOBSERVER_H
#define UPSOBSERVER_H

#include <ecom/implementationproxy.h>
#include "swiobserverplugin.h"
#include <ups/upsclient.h>
#include "log.h"

using namespace Swi;
using namespace UserPromptService;


_LIT(KFilterSysBin,  "!:\\sys\\bin");					  //EFilterSysBin
_LIT(KFilterPlugins, "!:\\resource\\plugins");			  //EFilterPlugins
_LIT(KFilterPolicies,"!:\\private\\10283558\\policies\\");//EFilterPolicies


NONSHARABLE_CLASS(CUpsObserver) : public CSwiObserverPlugin
	{
public:
	/**
		A set of enumerated values which is used to indicate which notifications
		the plug-in has received
	 */
	enum TNotifications
		{
		EPluginChanged = 0x01,  ///< An ECOM plug-in has added to or deleted from the system.
		EPolicyChanged = 0x02   ///< A Ups policy file has changed.
		}; 
	
	/**
		A set of enumerated values which is used to indicate the order of the filters
		in the filter list.
		
		Please note that if a new filter is added or the order of the filters are changed,
		this enumeration must be updated.
	 */
	enum TFilterIndex
		{
		EFilterSysBin=0,	///< The index of the sys\bin filter.
		EFilterPlugins,		///< The index of the plugins filter.
		EFilterPolicies		///< The index of policies filter.
		};
		
public:
	static CSwiObserverPlugin* CreateUpsObserverL();
		
	void StartL(RFs& aFs);
 	void NextObservedHeaderL(const CObservationHeader& aHeader, TRequestStatus& aStatus);	
 	void NextObservedDataL(const CObservationData& aData, TInt aFilterIndex, TRequestStatus& aStatus);	
 	void FinalizeL(TRequestStatus& aStatus);
 	CObservationFilter* GetFilterL() const;
 		
protected:
	// From CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
		
	~CUpsObserver();
		
private:
	CUpsObserver();
	void AddFilterL(CObservationFilter& aFilterList, const TDesC& aFilter) const;
		
private:
	/** The request status object owned by the SWI Observer. */
	TRequestStatus* iClientStatus;
	/** User Prompt Server session handle. */
	RUpsManagement iUpsManager;
	/** Handle to the file server session. */
	RFs* iFs;
	/** Notification flag indicating the recevied notifications. */
	TUint8 iNotifyFlag;
	/** The list of UIDs whose decisions will be deleted from Ups Db.*/
	RArray<TSecureId> iSidList;
	};

#endif
