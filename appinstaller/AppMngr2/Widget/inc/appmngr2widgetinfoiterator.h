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
* Description:   Implements MCUIInfoIterator for Widgets to use CCUIDetailsDialog
*
*/


#ifndef C_APPMNGR2WIDGETINFOITERATOR_H
#define C_APPMNGR2WIDGETINFOITERATOR_H

#include <appmngr2infoiterator.h>       // CAppMngr2InfoIterator


class CAppMngr2WidgetInfoIterator : public CAppMngr2InfoIterator
    {
public:     // constructors and destructor
    static CAppMngr2WidgetInfoIterator* NewL( CAppMngr2InfoBase& aWidget,
            TAppMngr2InfoType aInfoType );
    ~CAppMngr2WidgetInfoIterator();

protected:  // new functions
    void ConstructL();
    void SetOtherFieldsL();

private:    // new functions
    CAppMngr2WidgetInfoIterator( CAppMngr2InfoBase& aWidget,
            TAppMngr2InfoType aInfoType );
    };

#endif  // C_APPMNGR2WIDGETINFOITERATOR_H

