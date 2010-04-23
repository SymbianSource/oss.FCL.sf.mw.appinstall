/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains CNcdNodeIdentifierIdentifier class
*
*/


#ifndef NCD_NODE_IDENTIFIER_H
#define NCD_NODE_IDENTIFIER_H


#include <e32base.h>

class RReadStream; 
class RWriteStream;

class CCatalogsRefString;

#define DLNODEID( nodeId ) DLTRACE(( _L("id: %S, ns: %S, uid: %x, uri: %S"), &nodeId.NodeId(), &nodeId.NodeNameSpace(), nodeId.ClientUid().iUid, &nodeId.ServerUri() ))

/**
 *  ?one_line_short_description
 *
 *  ?more_complete_description
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdNodeIdentifier : public CBase
    {
    
public:

    /**
     * Destructor
     */
    virtual ~CNcdNodeIdentifier();

    /**
     * NewL
     * Sets the namespace, id and server URI values to KNullDesC and the
     * UID value to Null() value.
     *
     * @return CNcdNodeIdentifier* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeIdentifier* NewL();

    /**
     * NewLC
     * Sets the namespace, id and server URI values to KNullDesC and the
     * UID value to Null() value.
     *
     * @return CNcdNodeIdentifier* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeIdentifier* NewLC();


    /**
     * NewL
     * Server URI is set to KNullDesC.
     *
     * @param aNodeNameSpace
     * @param aNodeId This class does not take ownership.
     * @param aClientUid
     * @return CNcdNodeIdentifier* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeIdentifier* NewL( const TDesC& aNodeNameSpace,
                                     const TDesC& aNodeId,
                                     const TUid& aClientUid );

    /**
     * NewLC
     * Server URI is set to KNullDesC.
     *
     * @param aNodeNameSpace
     * @param aNodeId
     * @param aClientUid
     * @return CNcdNodeIdentifier* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeIdentifier* NewLC( const TDesC& aNodeNameSpace,
                                      const TDesC& aNodeId,
                                      const TUid& aClientUid );


    /**
     * NewL
     *
     * @param aNodeNameSpace
     * @param aNodeId
     * @param aServerUri
     * @param aClientUid
     * @return CNcdNodeIdentifier* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeIdentifier* NewL( const TDesC& aNodeNameSpace,
                                     const TDesC& aNodeId,
                                     const TDesC& aServerUri,
                                     const TUid& aClientUid );

    /**
     * NewLC
     *
     * @param aNodeNameSpace
     * @param aNodeId
     * @param aServerUri
     * @param aClientUid
     * @return CNcdNodeIdentifier* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeIdentifier* NewLC( const TDesC& aNodeNameSpace,
                                      const TDesC& aNodeId,
                                      const TDesC& aServerUri,
                                      const TUid& aClientUid );


    /**
     * NewL
     *
     * @param aNodeIdentifier This class does not take ownership.
     * @return CNcdNodeIdentifier* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeIdentifier* NewL( const CNcdNodeIdentifier& aNodeIdentifier );

    /**
     * NewLC
     *
     * @param aNodeIdentifier This class does not take ownership.
     * @return CNcdNodeIdentifier* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeIdentifier* NewLC( const CNcdNodeIdentifier& aNodeIdentifier );


    /**
     * NewL
     *
     * @param aNodeIdentifierData
     * @return CNcdNodeIdentifier* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeIdentifier* NewL( const TDesC8& aNodeIdentifierData );

    /**
     * NewLC
     *
     * @param aNodeIdentifierData
     * @return CNcdNodeIdentifier* Pointer to the created object 
     * of this class.
     */
    static CNcdNodeIdentifier* NewLC( const TDesC8& aNodeIdentifierData );


    static CNcdNodeIdentifier* NewL( RReadStream& aReadStream );
    static CNcdNodeIdentifier* NewLC( RReadStream& aReadStream );

    virtual void InternalizeL( RReadStream& aReadStream );
    virtual void ExternalizeL( RWriteStream& aWriteStream ) const;



    /**
     * @return TDesc& Namespace of the node
     */
    const TDesC& NodeNameSpace() const;

    /**
     * @return TDesc& Id of the node which is unique in a namespace
     */
    const TDesC& NodeId() const;

    /**
     * @return TDesc& Server URI of the node
     */
    const TDesC& ServerUri() const;

    /**
     * @return Client UID.
     */
    const TUid& ClientUid() const;


    /**
     * This function recreates the data everytime this function is asked.
     * The data will contain:
     * @return HBufC8* Identifying data that contains all the nodeidentifier data.
     * in one. Ownership is transferred.
     */
    HBufC8* NodeIdentifierDataL() const;


    /**
     * @note As a default the uri is not included for the checking.
     *
     * @param aCheckNameSpace Informs if the identifier namespace should be included
     * to the checking.
     * @param aCheckId Informs if the identifier id should be included
     * to the checking.
     * @param aCheckUri Informs if the identifier server uri should be included
     * to the checking.
     * @param aCheckUid Informs if the identifier Uid should be included
     * to the checking.
     * @return ETrue if at least one of the identifier fields
     * is empty. EFalse, if all of the fields contain some data.
     */
    TBool ContainsEmptyFields( TBool aCheckNameSpace = ETrue,
                               TBool aCheckId = ETrue,
                               TBool aCheckUri = EFalse,
                               TBool aCheckUid = ETrue ) const;


    /**
     * Can be used to compare this node identifier to another.
     * If all the identifier values are same then ETrue is returned.
     *
     * @note This compares also UID values. Different applications may have
     * same ids, namespaces and URIs for the nodes, but the UID can be used to
     * make corresponding nodes unique.
     *
     * @note As a default Uri is left out of the comparison.
     *
     * @param aObject1 Identifier object whose data is compared
     * @param aCompareNameSpace Informs if the identifier namespace should be
     * included to the comparison.
     * @param aCompareId Informs if the identifier id should be
     * included to the comparison.
     * @param aCompareUri Informs if the identifier server uri should be
     * included to the comparison.
     * @param aCompareUid Informs if the identifier Uid should be
     * included to the comparison.    
     * @return TBool ETrue if identifiers contains same info. Else EFalse.
     */    
    TBool Equals( const CNcdNodeIdentifier& aObject,
                  TBool aCompareNameSpace = ETrue,
                  TBool aCompareId = ETrue,
                  TBool aCompareUri = EFalse,
                  TBool aCompareUid = ETrue ) const;

    /**
     * Static function that can be used to compare two node identifiers.
     * If all the identifier values are same then ETrue is returned.
     *
     * @note This compares also UID values. Different applications may have
     * same ids, namespaces and URIs for the nodes, but the UID can be used to
     * make corresponding nodes unique.
     *
     * @note As a default Uri is left out of the comparison.
     *
     * @param aObject1 Identifier object whose data is compared
     * @param aObject2 Identifier object whose data is compared
     * @param aCompareNameSpace Informs if the identifier namespace should be
     * included to the comparison.
     * @param aCompareId Informs if the identifier id should be
     * included to the comparison.
     * @param aCompareUri Informs if the identifier server uri should be
     * included to the comparison.
     * @param aCompareUid Informs if the identifier Uid should be
     * included to the comparison.    
     * @return TBool ETrue if identifiers contains same info. Else EFalse.
     */    
    static TBool Equals( const CNcdNodeIdentifier& aObject1,
                         const CNcdNodeIdentifier& aObject2,
                         TBool aCompareNameSpace = ETrue,
                         TBool aCompareId = ETrue,
                         TBool aCompareUri = EFalse,
                         TBool aCompareUid = ETrue );


    /**
     * Node id comparison
     *
     * @param aObject1 First id
     * @param aObject2 Second id
     * @return a negative value if aObject1 < aObject2, 
     * 0 if aObject1 == aObject2 and a positive value if 
     * aObject1 > aObject2
     * @note Only node id, namespace and client UID are used for comparison
     */
    static TInt Compare( const CNcdNodeIdentifier& aObject1,
                         const CNcdNodeIdentifier& aObject2 );


    /**
     * Node id comparison
     *
     * Orders first by client UID then by namespace and finally node id
     *
     * @param aObject1 First id
     * @param aObject2 Second id
     * @return a negative value if aObject1 < aObject2, 
     * 0 if aObject1 == aObject2 and a positive value if 
     * aObject1 > aObject2
     * @note Only node id, namespace and client UID are used for comparison
     */
    static TInt CNcdNodeIdentifier::CompareOrderByUid( 
        const CNcdNodeIdentifier& aObject1,
        const CNcdNodeIdentifier& aObject2 );

    #if defined (COMPONENT_CATALOGSSERVEREXE) && defined (LOG_MEMORY_USAGE)
    
        static TInt64 iCurrentMemory;
        static TInt64 iMaxMemory;
        static TInt iIdCount;
        static TInt iMaxCount;
    
        static void AddId( const CNcdNodeIdentifier& aId, TInt64& aMemory );
        static void RemoveId( const CNcdNodeIdentifier& aId, const TInt64& aMemory );
    
        TInt64 iMyMemory;
    #endif    
        
    #ifdef COMPONENT_CATALOGSSERVEREXE
        /**
         * Sets node id
         */
        void SetNodeIdL( const TDesC& aNodeId );
    #endif

protected:

    /**
     * Constructor
     *
     */
    CNcdNodeIdentifier();

    /**
     * ConstructL
     */
    void ConstructL(
        const TDesC& aNodeNameSpace,
        const TDesC& aNodeId,
        const TDesC& aServerUri,
        const TUid& aClientUid );

    /**
     * ConstructL
     */
    void ConstructL( const TDesC8& aNodeIdentifierData );


private:

    // Prevent if not implemented
    //CNcdNodeIdentifier( const CNcdNodeIdentifier& aObject );
    CNcdNodeIdentifier& operator =( const CNcdNodeIdentifier& aObject );


protected: // data

#ifdef COMPONENT_CATALOGSSERVEREXE

    CNcdNodeIdentifier( const CNcdNodeIdentifier& aId );
    void ReleaseStrings();
    
    const CCatalogsRefString* iNodeNameSpace;
    const CCatalogsRefString* iNodeId;    
    const CCatalogsRefString* iServerUri;

#else
    // This is the name space of the node. Owns.
    HBufC* iNodeNameSpace;

    // This is the id of the node. Owns.
    HBufC* iNodeId;

    // This is the id of the node. Owns.
    HBufC* iServerUri;

#endif
    // This is the UID of the client.
    TUid iClientUid;
    
    };

#endif // NCD_NODE_IDENTIFIER_H
