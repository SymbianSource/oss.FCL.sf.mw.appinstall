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
* Description:   Class CCatalogsShutdownOperation declaration
*
*/


#ifndef C_CATALOGSSHUTDOWNOPERATION_H
#define C_CATALOGSSHUTDOWNOPERATION_H

#include <e32base.h>

class CCatalogsShutdownOperation;

class MCatalogsShutdownOperationObserver
    {
public:
    
    virtual void ShutdownOperationComplete( 
        CCatalogsShutdownOperation* aOperation, 
        TInt aError ) = 0;
    
protected:
    
    virtual ~MCatalogsShutdownOperationObserver()
        {        
        }
    };


class CCatalogsShutdownOperation : public CBase
    {
public:
	
	virtual ~CCatalogsShutdownOperation();
	
	void Execute();
	void SetObserver( MCatalogsShutdownOperationObserver& aObserver );
	
	const TUid& FamilyUid() const;
	
	void Cancel();
	
protected:
    
    CCatalogsShutdownOperation( const TUid& aFamilyUid );
    virtual void DoExecuteL() = 0; 
    virtual void DoCancel() = 0;
    
    void NotifyObserver( TInt aError );
    
private:
    
    MCatalogsShutdownOperationObserver* iObserver;
    TUid iFamilyUid;
    };

#endif /* C_CATALOGSSHUTDOWNOPERATION_H */
