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
* Description:   Class CCatalogsAccessPoint declation
*
*/


#ifndef C_CATALOGSACCESSPOINT_H
#define C_CATALOGSACCESSPOINT_H

#include <s32strm.h>
#include "catalogsaccesspointsettings.h"

class CCatalogsAccessPoint: public CCatalogsAccessPointSettings
    {
public:

    /**
     * Two phase constructor.
     * 
     * @param aApNcdId The id of the access point (obtained from CDB).
     * @return A pointer to the created object.
     */
    static CCatalogsAccessPoint* NewL(const TDesC& aApNcdId, const TDesC& aName);
    
    /**
     * Two phase constructor. Object is constructed from a stream.
     * 
     * @param aSettings Stream which can be used to initialize object's state.
     * 
     * @return A pointer to the created object.
     */
    static CCatalogsAccessPoint* NewL( RReadStream& aStream );

    /**
     * Seconds phase constructor.
     * 
     * @param aApNcdId The id of the access point (obtained from CDB).
     * @return A pointer to the created object. Object is in cleanupstack.
     */
    static CCatalogsAccessPoint* NewLC(const TDesC& aApNcdId, const TDesC& aName);
    
    /**
     * Seconds phase constructor. Object is constructed from a stream.
     * 
     * @param aSettings Stream which can be used to initialize object's state.
     * 
     * @return A pointer to the created object. Object is in cleanupstack.
     */
    static CCatalogsAccessPoint* NewLC( RReadStream& aStream );
    
    /**
     * Destructor.
     */
    virtual ~CCatalogsAccessPoint();
    
    /**
     * Writes objects state to a stream.
     * 
     * @param aStream Opened write stream.
     */
    void ExternalizeL( RWriteStream& aStream );
    
    /**
     * Initializes objects state from a stream. 
     * 
     * @param aStream Opened read stream.
     */
    void InternalizeL( RReadStream& aStream );
        
    /**
     * Sets the name of the access point.
     *
     * @param aName The name.
     */
    void SetNameL(const TDesC& aName);
    
    /**
     * Get the name of the access point.
     *
     * @return The name.
     */
    const TDesC& Name() const;
    
    /**
     * Get accesspoint's id in the commsdatabase.
     * 
     * @return Id of the accesspoint.
     */
    TUint32 AccessPointId() const;
    
    /**
     * Get the accesspoint's id used in NCD (given by CDB).
     *
     * @return NCD id of the accesspoint.
     */
    const TDesC& NcdAccessPointId() const;
    
    /**
     * Set accesspoints id in the commsdatabase
     * 
     * @param aId    Id of the accesspoint.
     */
    void SetAccessPointId( TUint32 aId );

    /**
     * Get accesspoint's iCreatedByManager flag. If true, accesspoint is deleted in the manager's destructor.
     * 
     * @return Boolean flag value.
     */
    TBool CreatedByManager () const;
    
    /**
     * Set accesspoint's iCreatedByManager flag to true, so that it gets deleted in the manager's destructor.
     */
    void SetCreatedByManager();
    
protected:

    /**
     * Constructor.
     */
    CCatalogsAccessPoint();

    /**
     * Performs seconds phase construction.
     */
    void ConstructL(const TDesC& aApNcdId, const TDesC& aName);
    
    /**
     * Constructs the object from the given stream data.
     *
     * @param aStream The stream.
     */
    void ConstructL(RReadStream& aStream);

    /**
     * Compare two accesspoints.
     * 
     * @param aAccessPoint
     *               A Reference to another accesspoint.
     * 
     * @return ETrue if settings are same, EFalse otherwise.
     */
    TBool MatchingAccessPoint( const CCatalogsAccessPoint& aAccessPoint );

private:

    HBufC*  iName;
    HBufC*  iNcdAccessPointId;
    TUint32 iAccessPointId;
       
    // Boolean flag to avoid deleting pre-existing access points in ~CCatalogsAccessPointManager().
    // Fix for [#PRECLI-1596]
    TBool iCreatedByManager;
    };



#endif

