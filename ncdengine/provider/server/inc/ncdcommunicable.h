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
* Description:   Contains CNcdCommunicable class
*
*/


#ifndef NCD_COMMUNICABLE_H
#define NCD_COMMUNICABLE_H


#include <e32def.h>
#include "catalogscommunicable.h"


/**
 * This class object provides additional functionality that
 * CCatalogsCommunicable objects may need. For example,
 * CCatalogsCommunicable class only concentrates on the sessions of
 * proxies and server objects. This class object provides
 * functions that can be used to check the general states 
 * of the server side objects.
 *
 * @lib ?library
 * @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdCommunicable: public CCatalogsCommunicable
{
public:

 /**
  * @param aObsolete Informs if the object should be set as obsolete.
  */
 void SetAsObsolete( TBool aObsolete );

 
 /**
  * @return TBool ETrue if the object in server side is obsolete and
  * should not be used in proxy side anymore.
  */
 TBool IsObsolete() const;


protected:

    /**
     * Constructor
     */
    CNcdCommunicable();

    /**
     * Destructor
     * 
     * @note that instead of using delete, Close() should be called.
     */
    virtual ~CNcdCommunicable();


private:
 // Prevent these if not implemented
 CNcdCommunicable( const CNcdCommunicable& aObject );
 CNcdCommunicable& operator =( const CNcdCommunicable& aObject );


private: // data

    TBool iObsolete;

};

#endif // NCD_COMMUNICABLE_H