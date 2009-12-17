/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* The file contains the declaration of the SecurityInfo namespace.
*
*/


/**
 @file 
 @released
 @internalTechnology 
*/

#ifndef __SECURITYINFO_H__
#define __SECURITYINFO_H__



namespace SecurityInfo
{

extern void RetrieveExecutableSecurityInfoL(RFs& aFs, const TDesC& aCurrentFileName, TSecurityInfo& aSecurityInfo);

} // namespace SecurityInfo

#endif 
