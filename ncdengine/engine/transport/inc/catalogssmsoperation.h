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
* Description:  
*
*/


#ifndef M_CATALOGSSMSOPERATION_H
#define M_CATALOGSSMSOPERATION_H

#include "catalogstransportoperation.h"
#include "catalogstransportoperationid.h"

/**
* Interface for SMS operations
*/
class MCatalogsSmsOperation : public MCatalogsTransportOperation
    {	
    public: // New operations
    
        /**
        * Returns operation's ID
        *
        * @return Operation ID
        */
        virtual const TCatalogsTransportOperationId& OperationId() const = 0;
    
    };



#endif // M_CATALOGSSMSOPERATION_H
