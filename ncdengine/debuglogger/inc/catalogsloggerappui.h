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
* Description:   ?description
*
*/


#ifndef C_CATALOGSLOGGERAPPUI_H
#define C_CATALOGSLOGGERAPPUI_H

#include <aknappui.h>

class CCatalogsLoggerAppView;
class CCatalogsLoggerMsgQueueObserver;
class COsmDelay;

class CCatalogsLoggerAppUi : public CAknAppUi
    {
public:
    void ConstructL();

    CCatalogsLoggerAppUi();

    ~CCatalogsLoggerAppUi();

    void TimerEvent();
    
    void StartupTimerEvent();
    
public: // from CAknAppUi
    void HandleCommandL(TInt aCommand);

    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane);

private:
    CCatalogsLoggerAppView* iAppView;
    CCatalogsLoggerMsgQueueObserver* iMsgObserver;
    COsmDelay* iDelay;
    COsmDelay* iStartupActionDelay;
    };


#endif // C_CATALOGSLOGGERAPPUI_H

