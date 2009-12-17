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
* Description:   Implements MCUIInfoIterator for SISX to use CCUIDetailsDialog
*
*/


#ifndef C_APPMNGR2SISXINFOITERATOR_H
#define C_APPMNGR2SISXINFOITERATOR_H

#include <appmngr2infoiterator.h>       // CAppMngr2InfoIterator


class CAppMngr2SisxInfoIterator : public CAppMngr2InfoIterator
    {
public:     // constructors and destructor
    static CAppMngr2SisxInfoIterator* NewL( CAppMngr2InfoBase& aSisx,
            TAppMngr2InfoType aInfoType );
    ~CAppMngr2SisxInfoIterator();

protected:  // new functions
    void ConstructL();
    void SetAllFieldsL();
    void SetVersionL();
    void SetSupplierL();
    void SetTypeL();

private:    // new functions
    CAppMngr2SisxInfoIterator( CAppMngr2InfoBase& aSisx,
            TAppMngr2InfoType aInfoType );
    };

#endif  // C_APPMNGR2SISXINFOITERATOR_H

