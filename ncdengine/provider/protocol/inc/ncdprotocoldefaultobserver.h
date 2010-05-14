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
* Description:   MNcdProtocolDefaultObserver declaration
*
*/


#ifndef M_NCDPROTOCOLDEFAULTOBSERVER_H
#define M_NCDPROTOCOLDEFAULTOBSERVER_H

#include <e32base.h>
#include "ncdparserobserver.h"


/**
 * Default protocol event observer.
 * All protocol events not handled elsewhere should be passed here,
 * where they are processed further.
 * @ Add other protocol observer interfaces.
 */
class MNcdProtocolDefaultObserver : public MNcdParserConfigurationProtocolObserver,
//                                     public MNcdParserEntityObserver,
//                                     public MNCdParserPurchaseObserver,
//                                     public MNcdParserSessionObserver,
                                    public MNcdParserInformationObserver,
                                    public MNcdParserDataBlocksObserver,
                                    public MNcdParserErrorObserver,
                                    public MNcdParserQueryObserver,
                                    public MNcdParserSubscriptionObserver
                  
                 
    {
public: // Implementing object can be deleted through this interface
    virtual ~MNcdProtocolDefaultObserver() {}
    
    };



// /**
//  * Default protocol event observer observer.
//  * Used to inform the owner of the default observer that the default observer
//  * can be safely deleted
//  */
// class MNcdProtocolDefaultObserverOwner
//     {
// public:
//     virtual void DefaultObserverFinished( 
//         MNcdProtocolDefaultObserver& aObserver ) = 0;

// protected:
//     virtual ~MNcdProtocolDefaultObserverOwner() 
//         {
//         }
//     };

#endif
