/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Header file of CIAUpdateUiConfigData class
*
*/



#ifndef IA_UPDATE_UI_CONFIG_DATA_H
#define IA_UPDATE_UI_CONFIG_DATA_H


#include <e32base.h>


/**
 * CIAUpdateUiConfigData provides configuration data 
 * for the engine.
 *
 * @since S60 v3.2
 */
class CIAUpdateUiConfigData : public CBase
    {

public:

    /**
     * @return CIAUpdateUiConfigData*
     *
     * @since S60 v3.2
     */
    static CIAUpdateUiConfigData* NewL();

    /**
     * @return CIAUpdateUiConfigData*
     *
     * @since S60 v3.2
     */
    static CIAUpdateUiConfigData* NewLC();


    /**
     * Destructor
     *
     * @since S60 v3.2
     */
    virtual ~CIAUpdateUiConfigData();  


    /**
     * @return TInt Grid refresh delta hours.
     *
     * @since S60 v3.2
     */
    TInt GridRefreshDeltaHours() const;


    /**
     * @return TInt Query history delay hours.
     *
     * @since S60 v3.2
     */
    TInt QueryHistoryDelayHours() const;

private:

    CIAUpdateUiConfigData();
    void ConstructL();


private: // data

    TInt iGridRefreshDeltaHours;
    TInt iQueryHistoryDelayHours;
    
    };
    
#endif // IA_UPDATE_UI_CONFIG_DATA_H

