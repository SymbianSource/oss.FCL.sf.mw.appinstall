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
* Description:   MNcdProtocolElementEntity declaration
*
*/


#ifndef NcdPREMINEPROTOCOLLOCALENTITY_H
#define NcdPREMINEPROTOCOLLOCALENTITY_H

#include <e32base.h>

class MNcdPreminetProtocolIcon;


/**
 * Base interface for some entities
 */
class MNcdPreminetProtocolLocalEntity
    {
public:
    enum TLocalEntityType
        {
        EDataEntity
//         EActionData,
//         EPromoData
        };

    /**
     * Destructor
     */
    virtual ~MNcdPreminetProtocolLocalEntity() {}

    /**
     * Returns the type of this reference entity.
     * @return Type
     */
    virtual TLocalEntityType Type() const = 0;

    /**
     * Returns the ID of this entity.
     * @return Id
     */
    virtual const TDesC& Id() const = 0;
    /**
     * Returns the last modified date for this entity.
     * @return Last modified time, or 0 if never modified.
     */
    virtual const TTime TimeStamp() const = 0;
    /**
     * Returns the namespace for this entity
     * @return namespace or KNullDesC 
     */
    virtual const TDesC& NameSpace() const = 0;

    /**
     * Entity name, localized.
     * @return Name, never KNullDesC
     */
    virtual const TDesC& Name() const = 0;

    /**
     * Retuns the description for this entity, localized.
     * @return Description or KNullDesC
     */
    virtual const TDesC& Description() const = 0;

    /**
     * Returns icon information for the entity.
     * @return Icon info or null if icon not available.
     */
    virtual const MNcdPreminetProtocolIcon* Icon() const = 0;
    
    /**
     * Returns the disclaimer for the entity.
     * @return Disclaimer text or KNullDesC if no disclaimer.
     */
    virtual const TDesC& Disclaimer() const = 0;

    

    };


#endif
