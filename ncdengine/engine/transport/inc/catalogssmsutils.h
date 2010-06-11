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
* Description:  
*
*/


#ifndef C_CATALOGSSMSUTILS_H
#define C_CATALOGSSMSUTILS_H

#include <msvapi.h> // MMsvSessionObserver

class CCatalogsSmsUtils : public CBase,
                          public MMsvSessionObserver
    {
public:
    static CCatalogsSmsUtils* NewL();
    ~CCatalogsSmsUtils();
    
    HBufC* SmsCenterNumberLC();

private: // from MMsvSessionObserver
 
    void HandleSessionEventL( TMsvSessionEvent aEvent, 
        TAny* aArg1, TAny* aArg2, TAny* aArg3 );

    
protected:

    CCatalogsSmsUtils();    
    };
    
#endif // C_CATALOGSMSUTILS_H    