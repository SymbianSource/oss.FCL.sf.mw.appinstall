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


#ifndef __OBJECTEXCHANGESERVICESEARCHER_H__
#define __OBJECTEXCHANGESERVICESEARCHER_H__

#include <e32base.h>
#include "BTServiceSearcher.h"
#include "ObjectExchangeProtocolConstants.h"


/*! 
  @class CObjectExchangeServiceSearcher
  
  @discussion Searches the remote SDP database for suitable OBEX servers.
  */
class CObjectExchangeServiceSearcher : public CBTServiceSearcher
    {
public:
/*!
  @function NewL
  
  @discussion Construct a CObjectExchangeServiceSearcher
  @result a pointer to the created instance of CObjectExchangeServiceSearcher
  */
    static CObjectExchangeServiceSearcher* NewL();

/*!
  @function NewLC
  
  @discussion Construct a CObjectExchangeServiceSearcher  
  @result a pointer to the created instance of CObjectExchangeServiceSearcher
  */
    static CObjectExchangeServiceSearcher* NewLC();

/*!
  @function ~CObjectExchangeServiceSearcher
  
  @discussion Destroy the object and release all memory objects
  */
    ~CObjectExchangeServiceSearcher();

/*!
  @function Port
  
  @result the port to connect to on the remote machine
  */
    TInt Port();

protected:

/*!
  @function ServiceClass
  
  @discussion The service class to search for
  @result the service class UUID
  */
    const TUUID& ServiceClass() const;

/*!
  @function ProtocolList
  
  @discussion The list of Protocols required by the service.
  */
    const TSdpAttributeParser::TSdpAttributeList& ProtocolList() const;

/*!
  @function FoundElementL
  
  @discussion Read the data element
  @param aKey a key that identifies the element
  @param aValue the data element
  */
    virtual void FoundElementL(TInt aKey, CSdpAttrValue& aValue);

private:
/*!
  @function CObjectExchangeServer
  
  @discussion Constructs this object
  */
    CObjectExchangeServiceSearcher();

/*!
  @function ConstructL
  
  @discussion 2nd phase construction of this object
  */
    void ConstructL();

private:
    /*! @var iServiceClass the service class to search for */
    TUUID iServiceClass;

    /*! @var iPort the port to connect to on the remote machine */
    TInt iPort;
    };

#endif // __OBJECTEXCHANGESERVICESEARCHER_H__

