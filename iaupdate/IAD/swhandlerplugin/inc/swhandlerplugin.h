/*
* Copyright (c) 2010- Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef __SWHANDLERPLUGIN_H__
#define __SWHANDLERPLUGIN_H__

//  Include Files

#include <e32base.h>	// CBase
#include <schemehandler.h>
#include <AknServerApp.h>

//  Constants

//  Class Definitions

class CSwHandlerPlugin : public CSchemeHandler
    {
public:
    // new functions
    static CSwHandlerPlugin* NewL();
    CSwHandlerPlugin();
    ~CSwHandlerPlugin();

public: // CSchemeHandler
    
    /**
    * Url Handler with embedding
    * @param -
    */
    virtual void HandleUrlEmbeddedL();

    /**
    * Url Handler without embedding
    * @param -
    */
    virtual void HandleUrlStandaloneL();

    /**
    * Observer
    * @param - aSchemeDoc
    */
    virtual void Observer( MAknServerAppExitObserver* aSchemeDoc );     
    
private:
    void ConstructL();

private: // data
    /**
     * Exit observer.
     */
    MAknServerAppExitObserver* iSchemeDoc; ///< Not owned.
    };

#endif  // __SWHANDLERPLUGIN_H__

