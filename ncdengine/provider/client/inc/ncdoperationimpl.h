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
* Description:   Contains MCatalogsBase implementation base class definition
*
*/


#ifndef C_NCDOPERATION_H
#define C_NCDOPERATION_H

#include <e32cmn.h>
#include <e32base.h>
#include "catalogsbase.h"
#include "ncdoperation.h"
#include "ncdbaseoperationproxy.h"

template< class T1 >
class CNcdOperation : public CNcdBaseOperationProxy, public T1
    {
public:

    /**
     * @see MNcdOperation::OperationStateL()
     */
    MNcdOperation::TState OperationStateL() const
        {
        return DoOperationStateL();
        }    
        
    /**
     * @see MNcdOperation::StartOperationL()
     */
    void StartOperationL()
        {
        DoStartOperationL();
        }
    
    /**
     * @see MNcdOperation::CancelOperation()
     */
    void CancelOperation()
        {
        DoCancelOperation();
        }

    /**
     * @see MNcdOperation::Progress()
     */
    TNcdProgress Progress() const
        {
        return DoProgress();
        }
    
    /**
     * @see MNcdOperation::CompleteQuery()
     */    
    void CompleteQueryL( MNcdQuery& aQuery )
        {
        DoCompleteQueryL( aQuery );
        }
    
    /**
     * @see MNcdOperation::Node()
     */
    MNcdNode* Node()
        {
        return DoNode();
        }
            
protected:
    /**
     * ConstructorL
     *
     * @note CNcdBaseOperationProxy::ConstructL also needs to be called when the child classes
     * initialize themselves in their own ConstructL functions.
     *
     * @see CNcdBaseOperationProxy::ConstructL
     */ 
    CNcdOperation( MNcdClientLocalizer* aLocalizer ) : 
        CNcdBaseOperationProxy( aLocalizer ) 
        {
        }

    const TAny* QueryInterfaceL( TInt aInterfaceId ) const
        {
        const TAny* result = NULL;
        switch( aInterfaceId )
            {
            case T1::KInterfaceUid:             result = static_cast< const T1* >( this ); break;
            case MNcdOperation::KInterfaceUid:  result = static_cast< const MNcdOperation* >( this ); break;
            case MCatalogsBase::KInterfaceUid:  result = static_cast< const MCatalogsBase* >( this ); break;                        
            default: break;
            }

        if( result != NULL )
            {
            AddRef();
            }

        return result;
        }
    };

#endif // C_NCDOPERATION_H