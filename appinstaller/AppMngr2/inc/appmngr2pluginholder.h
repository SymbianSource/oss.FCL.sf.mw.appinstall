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
* Description:   Container class to hold Runtime plugin and related data
*
*/


#ifndef C_APPMNGR2PLUGINHOLDER_H
#define C_APPMNGR2PLUGINHOLDER_H

#include <e32base.h>                    // CBase
#include <apmrec.h>                     // CDataTypeArray

class CAppMngr2Runtime;
class CAknIconArray;


class CAppMngr2PluginHolder : public CBase
    {
public:     // constructor and destructor
    CAppMngr2PluginHolder( CAppMngr2Runtime* aRuntime );    // takes ownership
    ~CAppMngr2PluginHolder();

public:     // new functions
    CAppMngr2Runtime& Runtime();
    CDataTypeArray& DataTypes();
    void LoadIconsL( CAknIconArray& aIconArray );
    void FetchDataTypesL();
    TInt IconIndexBase();
    TInt IconIndexMax();

private:    // data
    CAppMngr2Runtime* iRuntime;
    CDataTypeArray* iDataTypes;
    TInt iIconIndexBase;
    TInt iIconCount;
    };

#endif  // C_APPMNGR2PLUGINHOLDER_H

