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
* Description:   MNcdPreminetProtocolEntityRef declaration
*
*/


#ifndef NcdPREMINETPROTOCOLENTITYREF_H
#define NcdPREMINETPROTOCOLENTITYREF_H

#include <e32base.h>

class MNcdPreminetProtocolEntityRef
    {
public:

    enum TType
        {
        EFolderRef,
        EItemRef,
        EActionRef,
        EPromoRef
        };

    enum TDescription
        {
        ENormal,
        ESearchResults,
        EServices,
        ERecommendations
        };

    /**
     * Destructor
     */
    virtual ~MNcdPreminetProtocolEntityRef() {}

    /**
     * Returns the type of this reference entity.
     * @return TType
     */
    virtual TType Type() const = 0;

    /**
     * Returns the description of this reference entity.
     * @return TDescription
     */
    virtual TDescription Description() const = 0;

    /**
     * Returns the ID of the parent entity.
     * This entity should be added to the parent's child list,
     * @return Parent id or KNullDesC if parentless.
     */
    virtual const TDesC& ParentId() const = 0;
    
    /**
     * Returns the ID of this entity.
     * @return Id
     */
    virtual const TDesC& Id() const = 0;
    /**
     * Returns the last modified date for this entity.
     * @return Last modified time, or 0 if never modified.
     */
    virtual const TDesC& Timestamp() const = 0;

    /**
     * If this is a remote reference, the target uri is 
     * retrieved with this method. If the reference is local,
     * KNullDesC is returned.
     * @return Remote uri or KNullDesC.
     */
    virtual const TDesC& RemoteUri() const = 0;

    /**
     * Returns the namespace for this entity
     * @return namespace or KNullDesC if not available 
     */
    virtual const TDesC& Namespace() const = 0;

    /**
     * Returns the query ids related to this entity.
     * The array may be empty.
     * @return Query id array
     */
    virtual const RArray<TInt>& Queries() const = 0;

    /**
     * Retrieves the server uri from which this reference was received
     * @return Server URI
     */
    virtual const TDesC& ServerUri() const = 0;

    /**
     * Returns the namespace of the parent of this entity.
     * @return namespace or KNullDesC if not available
     */
    virtual const TDesC& ParentNamespace() const = 0;

    /**
     * Returns the validity time in minutes.
     * @return Validity time, 0 if not set.
     */
    virtual TInt ValidUntilDelta() const = 0;

    /**
     * Returns true if the client may update the entity automatically
     * @return true or false (default false)
     */
    virtual TBool ValidUntilAutoUpdate() const = 0;

    };

#endif
