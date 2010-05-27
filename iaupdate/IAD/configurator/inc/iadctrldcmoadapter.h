/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  header file for iadCtrl plug-in adapter
*
*/

#ifndef _IADCTRLDCMOADAPTER__
#define _IADCTRLDCMOADAPTER__

#include <dcmointerface.h>

// An implementation of the CDCMOInterface definition
class CiadCtrlDCMOAdapter : public CDCMOInterface
	{
public:
	// Standardised safe construction which leaves nothing the cleanup stack.
	static CiadCtrlDCMOAdapter* NewL(TAny* aInitParams);
	// Destructor	
	~CiadCtrlDCMOAdapter();

	// Implementation of CDCMOInterface
 /**
  * Get the interger type value
	* @param aId
	* @param aValue
	* @return TDCMOStatus 
	*/ 
	TDCMOStatus  GetDCMOPluginIntAttributeValueL(TDCMONode aId, TInt& aValue);
	
 /**
  * Get the string type value
	* @param aId
	* @param aStrValue
	* @return TDCMOStatus
	*/	
	TDCMOStatus  GetDCMOPluginStrAttributeValueL(TDCMONode aId, TDes& aStrValue);
	
 /**
  * Set the interger type value
	* @param aId
	* @param aValue
	* @return TDCMOStatus
	*/
	TDCMOStatus  SetDCMOPluginIntAttributeValueL(TDCMONode aId, TInt aValue);
	
 /**
	* Set the string type value
	* @param aId
	* @param aStrValue
	* @return TDCMOStatus
	*/
	TDCMOStatus  SetDCMOPluginStrAttributeValueL(TDCMONode aId, const TDes& aStrValue);
	
 /**
	* Gets the localized name of the plug-in adapter
	* @param aLocName to get the localized name
	* @return none
	*/
	void GetLocalizedNameL(HBufC*& aLocName);
	
private:
	// Construction
	CiadCtrlDCMOAdapter(TAny* aInitParams);
	
 /**
	* second phase constructor
	*/
	void ConstructL();

private:
	// Data to pass back from implementation to client
	HBufC* iProperty;
	HBufC* iDescription;
	// Parameters taken from client
	CDCMOInterface::TDCMOInterfaceInitParams* iInitParams;
	};  

#endif //_IADCTRLDCMOADAPTER__
