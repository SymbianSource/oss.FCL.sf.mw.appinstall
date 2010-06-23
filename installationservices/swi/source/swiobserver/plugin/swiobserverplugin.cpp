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
* Implements base SWI Observer ECOM plug-in interface.
*
*/


/**
 @file
 @internalTechnology
 @released
*/
 
#include "swiobserverplugin.h"

using namespace Swi;


//
//CSwiObserverPlugin
//


EXPORT_C CSwiObserverPlugin::CSwiObserverPlugin()
/**
	Constructor.
 */
	: CActive(CActive::EPriorityLow)	
		{
		CActiveScheduler::Add(this);
		}

	
EXPORT_C CSwiObserverPlugin::~CSwiObserverPlugin()
/**
	Destructor.
 */
	{
	Deque();
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}
	

EXPORT_C CSwiObserverPlugin* CSwiObserverPlugin::NewL(const TUid& aImplementationId)
/**
	Creates a new SWI Observer plug-in.					
	
	@param aImplementationId The UID of the SWI Observer plug-in implemenation
	@return A pointer to the new SWI Observer plug-in implementation.
*/
	{
	TAny* plugin(0);
	
	plugin = REComSession::CreateImplementationL(aImplementationId,
				_FOFF(CSwiObserverPlugin,iDtor_ID_Key));
			
	return reinterpret_cast<CSwiObserverPlugin*>(plugin);	
	}


EXPORT_C TInt CSwiObserverPlugin::GetExtension(TUint aExtensionId, TAny*& a0, TAny* a1)
/**
Allows the SWI Observer plug-in to support extra interface in future.
If the extension id is not recognised then KErrNotSupported should be returned.

@param aExtensionId		The UID of the interface to instantiate.
@param a0				A reference to a pointer that should be set to the newly
						instantiated object.
@param a1				Data specific to the instantiate of the specified interface.

@return KErrNone if the extension is supported or KErrNotSupported if the extension is not
		recognised; otherwise, a system wide error may be returned.
*/
	{
	(void) aExtensionId;
	a0 = 0;
	(void) a1;
	return KErrNotSupported;
	}

EXPORT_C TInt CSwiObserverPlugin::Extension_(TUint aExtensionId, TAny*& a0, TAny* a1)
	{
	return CActive::Extension_(aExtensionId, a0, a1);
	}
