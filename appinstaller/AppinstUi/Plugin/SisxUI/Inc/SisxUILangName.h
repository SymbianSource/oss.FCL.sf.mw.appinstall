/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Defines class for language id amd name pairs
*
*/


#ifndef SISXUILANGNAME_H
#define SISXUILANGNAME_H

#include <e32base.h>        // CBase

class TResourceReader;


class CLangName : public CBase
    {
public:     // constructors and destructor
    static CLangName* NewL( TResourceReader& aReader );
    ~CLangName();
    
public:     // new functions
    TLanguage Id() const;
    TPtrC Name() const;

private:    // new functions
    CLangName();
    void ConstructFromResourceL( TResourceReader& aReader );

private:    // data
    TLanguage iId;
    HBufC* iName;
    };

#endif // SISXUILANGNAME_H
