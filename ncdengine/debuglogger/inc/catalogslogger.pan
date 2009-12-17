/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CatalogsLogger application panic codes
*
*/


#ifndef CATALOGSLOGGER_PAN
#define CATALOGSLOGGER_PAN

/** CatalogsLogger application panic codes */
enum TCatalogsLoggerPanics 
    {
    ECatalogsLoggerBasicUi = 1
    // add further panics here
    };

inline void Panic(TCatalogsLoggerPanics aReason)
    {
    _LIT(applicationName,"CatalogsLogger");
    User::Panic(applicationName, aReason);
    }

#endif // CATALOGSLOGGER_PAN
