/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CSisLangChangeMonitor - class used to monitor the language change on the device
*							upudate the localized package names accordingly.
*
*/


/**
 @file 
 @released
 @internalComponent
*/
#ifndef __SISLANGCHANGEMONITOR_H__
#define __SISLANGCHANGEMONITOR_H__

//
// Class CSisLangChangeMonitor
//

namespace Swi
{
	class CSisRegistryCache :: CSisLangChangeMonitor : public CActive
 		{
 	/**
 	* Utility class used to monitor locale/language change event.
 	* @internalComponent
 	* 
 	* @released
 	*/
 	public:
 		static CSisLangChangeMonitor* NewL(CSisRegistryCache& aSisRegistryCache);
 		~CSisLangChangeMonitor();
 		
 	private:
 		CSisLangChangeMonitor(CSisRegistryCache& aSisRegistryCache);
 		void ConstructL();
 		
 	private:	//from CActive
 		void Start();
 		void RunL();
 		void DoCancel();
 		TInt RunError(TInt aError);
 		
 	private:
 		RChangeNotifier iLangNotifier;
 		CSisRegistryCache& iSisRegistryCache;
 		TLanguage iPrevLanguage;
	};
}
#endif
