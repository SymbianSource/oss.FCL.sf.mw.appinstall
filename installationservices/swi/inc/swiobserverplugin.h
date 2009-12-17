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
* Interface definition for SWI Observer ECOM plug-in.
*
*/


/**
 @file
 @internalTechnology
 @released
*/
 
 #ifndef SWIOBSERVERPLUGIN_H
 #define SWIOBSERVERPLUGIN_H
 
 #include <e32base.h>
 #include <ecom/ecom.h>
 
 #include <swi/swiobcommon.h>
 #include <swi/swiobservedinfo.h>
 
 namespace Swi
 	{
	/**
		Abstract base class for a SWI Observer ECOM plug-in.
			
		SWI Observer plug-ins are primarily	responsible for submitting SWI event 
		notifications to the related system server asynchronously. This
		interface allows components to register their interest in SWI events.
		System servers which wish to register to the SWI Observer implement
		a concrete class derived from this abstract class.
	 */
	 class CSwiObserverPlugin : public CActive
	 	{
	 public:
	 	IMPORT_C static CSwiObserverPlugin* NewL( const TUid& aImplementationId);
	 		
	 	virtual void StartL(RFs& aFs) = 0;
	 	virtual void NextObservedHeaderL(const CObservationHeader& aHeader, TRequestStatus& aStatus) = 0;	
	 	virtual void NextObservedDataL(const CObservationData& aData, TInt aFilterIndex, TRequestStatus& aStatus) = 0;	
	 	virtual void FinalizeL(TRequestStatus& aStatus) = 0;
	 	virtual CObservationFilter* GetFilterL() const = 0;
	 		
	 	IMPORT_C virtual TInt GetExtension(TUint aExtensionId, TAny*& a0, TAny* a1);
	 		
	 	IMPORT_C ~CSwiObserverPlugin();
	 	
	 protected:
	 	IMPORT_C CSwiObserverPlugin();
	 	IMPORT_C virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
	 		
	 private:
		TUid iDtor_ID_Key;		// Required by ECOM
		TAny* iReserved;
	 	};
	 	
	 /** Hosting the SWI Observer Plugin Interface UID. */	
	 const TUid KUidSwiObserverPluginInterface = {0x10283724};
	 		
 	} // End of namespace Swi
 
 #endif
 
 