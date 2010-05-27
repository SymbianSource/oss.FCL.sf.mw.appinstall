/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Implementation of iadCtrl plug-in adapter for DCMO
*
*/

#include <featmgr.h>
#include <featurecontrol.h>
#include <f32file.h> 
#include <data_caging_path_literals.hrh>
#include "iadctrldcmoadapter.h"

_LIT( KiadCtrlProperty, "IAD");
_LIT( KiadCtrlDescription, "Used to enable/disable the IAD." ); // Description
_LIT( KRuntimeResourceFileName, "z:iaupdatedcmoadapter.rsc" );	    

// Construction and destruction functions

// ----------------------------------------------------------------------------------------
// CiadCtrlDCMOAdapter::NewL
// ----------------------------------------------------------------------------------------
CiadCtrlDCMOAdapter* CiadCtrlDCMOAdapter::NewL( TAny* aParams )
	{
	CiadCtrlDCMOAdapter* self=new(ELeave) CiadCtrlDCMOAdapter( aParams );  
	CleanupStack::PushL( self );
	self->ConstructL(); 
	CleanupStack::Pop();
	return self;
	}
// ----------------------------------------------------------------------------------------
// CiadCtrlDCMOAdapter::~CiadCtrlDCMOAdapter() 
// ----------------------------------------------------------------------------------------
CiadCtrlDCMOAdapter::~CiadCtrlDCMOAdapter()
	{	
	delete iProperty;
	delete iDescription;
	}

// ----------------------------------------------------------------------------------------
// CiadCtrlDCMOAdapter::CiadCtrlDCMOAdapter() 
// ----------------------------------------------------------------------------------------

CiadCtrlDCMOAdapter::CiadCtrlDCMOAdapter(TAny* aInitParams)
:  iInitParams((CDCMOInterface::TDCMOInterfaceInitParams*)aInitParams)
	{
	// See ConstructL() for initialisation completion.
	}

// ----------------------------------------------------------------------------------------
// CiadCtrlDCMOAdapter::ConstructL
// ----------------------------------------------------------------------------------------
void CiadCtrlDCMOAdapter::ConstructL()
// Safely complete the initialization of the constructed object	
	{	
	// Set up the data to pass back
	if(iInitParams->descriptor)
		iProperty = (iInitParams->descriptor)->AllocL();
	else
		iProperty = KiadCtrlProperty().AllocL();
	iDescription = KiadCtrlDescription().AllocL();
	}


// Implementation of CDCMOInterface

// ----------------------------------------------------------------------------------------
// CiadCtrlDCMOAdapter::GetDCMOPluginIntAttributeValueL
// Gets the integer attribute value.
// ----------------------------------------------------------------------------------------
TDCMOStatus CiadCtrlDCMOAdapter::GetDCMOPluginIntAttributeValueL(TDCMONode aId, TInt& aValue)
{
	TDCMOStatus status(EDcmoFail);
	TInt err(KErrNone);
	
	switch(aId)
	{
		case EGroup: aValue = EService;
				break;
		
		case EAttached:	aValue = 1; // Attached
				break;
					
		case EEnabled:	{		
				FeatureManager::InitializeLibL();	
				if( FeatureManager::FeatureSupported ( KFeatureIdIAUpdate ) )
					{							
					aValue = 1;
					}
				else 
					{
					aValue = 0;
					} 
				// uninitialize feature manager
				FeatureManager::UnInitializeLib();
				}							
				break;
		
		case EDenyUserEnable:	aValue = 1; // DenyUserEnable
				break;
						
		case ENotifyUser:	aValue = 1; // NotifyUser
				break;	
						
		default :
			{
				err = KErrNotFound;
				status = EDcmoNotFound; 
			} 				
	}
	if( err == KErrNone )
		status = EDcmoSuccess;
	
	return status;
}

// ----------------------------------------------------------------------------------------
// CiadCtrlDCMOAdapter::GetDCMOPluginStrAttributeValueL
// Gets the string attribute value.
// ----------------------------------------------------------------------------------------
TDCMOStatus CiadCtrlDCMOAdapter::GetDCMOPluginStrAttributeValueL(TDCMONode aId, TDes& aStrValue) 
{
	TDCMOStatus status( EDcmoSuccess );
	switch( aId )
	{
		case EProperty: 	aStrValue = *iProperty;
							break;
		
		case EDescription:	aStrValue = *iDescription;
						break;
		default:			status = EDcmoNotFound;
	
	}
		
	return status;
}

// ----------------------------------------------------------------------------------------
// CiadCtrlDCMOAdapter::SetDCMOPluginIntAttributeValueL
// Sets the integer attribute value.
// ----------------------------------------------------------------------------------------

TDCMOStatus CiadCtrlDCMOAdapter::SetDCMOPluginIntAttributeValueL(TDCMONode aId, TInt aValue)
{
	TDCMOStatus status( EDcmoFail );
	TInt err( KErrNone );
	
	switch(aId)
	{
		case EDenyUserEnable:
			break;
							
		case ENotifyUser:
			break;
							
		case EEnable:	{								
			RFeatureControl featureControl;
    		TInt err( KErrNone);
    		err = featureControl.Connect();
       		if ( err == KErrNone )
        		{
        		TUid featureId = TUid::Uid( KFeatureIdIAUpdate );
        		if( aValue == 1 )
					{
      				err = featureControl.EnableFeature( featureId );  
       				}
				else if(aValue == 0)
					{
					err = featureControl.DisableFeature( featureId );        					
					}
				}
			featureControl.Close(); 
			}
			break;
		default :
			{
			err = KErrNotFound;
			status = EDcmoNotFound; 
			} 		
	}
	if( err == KErrNone )
		status = EDcmoSuccess;
	
	return status;
}

// ----------------------------------------------------------------------------------------
// CiadCtrlDCMOAdapter::SetDCMOPluginStrAttributeValueL
// Sets the string attribute value.
// ----------------------------------------------------------------------------------------

TDCMOStatus CiadCtrlDCMOAdapter::SetDCMOPluginStrAttributeValueL( TDCMONode /*aId */, const TDes& /*aStrValue */)
{
	// Nothing to do
	TDCMOStatus status( EDcmoNotSupported );
	
	return status;	
}

// ----------------------------------------------------------------------------------------
// CiadCtrlDCMOAdapter::GetLocalizedNameL
// Returns the localized name of IAD plug-in Adapter.
// ----------------------------------------------------------------------------------------
void CiadCtrlDCMOAdapter::GetLocalizedNameL( HBufC*& /*aLocName*/ )
{
	TFileName myFileName;
    TParse parseObj;
    parseObj.Set( KRuntimeResourceFileName(), &KDC_RESOURCE_FILES_DIR,NULL );
    myFileName = parseObj.FullName();
/*	CStringResourceReader* test = CStringResourceReader::NewLC( myFileName );
	TPtrC buf;
	buf.Set(test->ReadResourceString( R_SWUPDATE_CONFIGURING_TITLE ) ); 
	aLocName = buf.AllocL() ; 
	CleanupStack::PopAndDestroy( test );*/
}
