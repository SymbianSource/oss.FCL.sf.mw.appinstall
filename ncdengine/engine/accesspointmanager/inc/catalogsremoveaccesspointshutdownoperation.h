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
* Description:   Class CCatalogsRemoveAccesspointShutdownOperation declaration
*
*/


#ifndef C_CATALOGSREMOVEACCESSPOINTSHUTDOWNOPERATION_H
#define C_CATALOGSREMOVEACCESSPOINTSHUTDOWNOPERATION_H

#include <e32base.h>

#include "catalogsshutdownoperation.h"
#include "catalogsaccesspointobserver.h"

class TCatalogsConnectionMethod;
class CCatalogsNetworkManager;


class CCatalogsRemoveAccesspointShutdownOperation
    : public CCatalogsShutdownOperation,
      public MCatalogsAccessPointObserver
    {
public:
    
    static CCatalogsRemoveAccesspointShutdownOperation* NewL( 
        const TUid& aFamilyUid,
        const TUint32 aApnId );
	
	virtual ~CCatalogsRemoveAccesspointShutdownOperation();

protected: // MCatalogsAccessPointObserver
    
    void HandleAccessPointEventL( 
        const TCatalogsConnectionMethod& aAp,
        const TCatalogsAccessPointEvent& aEvent );
    
protected:
    
    static TInt CallbackRemoveAccessPoint( TAny* aData );
    
    CCatalogsRemoveAccesspointShutdownOperation( 
        const TUid& aFamilyUid,
        const TUint32 aApnId );    
    
protected: // CCatalogsShutdownOperation    
    
    void DoExecuteL();
    void DoCancel();

private:
    
    void RemoveAccessPoint();
    
private:
    
    TUint32 iApnId;    
    CCatalogsNetworkManager* iNetworkManager; // not owned
    CPeriodic* iTimer; // owned
    };

#endif /* C_CATALOGSREMOVEACCESSPOINTSHUTDOWNOPERATION_H */
