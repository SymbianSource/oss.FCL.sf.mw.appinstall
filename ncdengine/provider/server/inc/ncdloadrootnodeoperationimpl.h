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


#ifndef C_NCDLOADROOTNODEOPERATIONIMPL_H
#define C_NCDLOADROOTNODEOPERATIONIMPL_H

#include <e32base.h>

#include "ncdbaseoperation.h"
#include "ncdoperationobserver.h"
#include "ncdparserobserver.h"
#include "catalogshttpobserver.h"
#include "catalogshttptypes.h"
#include "catalogshttpsession.h"
#include "catalogsutils.h"
#include "ncdnodeidentifier.h"
#include "ncdconfigurationobserver.h"
#include "ncdproviderdefines.h"

class CDesC16Array;
class CDesC16ArrayFlat;
class CDesC8ArrayFlat;
class MNcdParser;
class CNcdNodeManager;
class MNcdProtocol;
class CNcdNodeIdentifier;
class CNcdLoadNodeOperationImpl;
class CCatalogsContext;
class MCatalogsAccessPointManager;
class MNcdConfigurationManager;
class MNcdOperationRemoveHandler;
class CNcdContentSourceMap;
class CNcdChildEntityMap;

// ======== CONSTANTS ========


class CNcdContentSource : public CBase
    {
public:
    
    static CNcdContentSource* NewLC( const CNcdNodeIdentifier& aParentIdentifier )
        {
        CNcdContentSource* self =
        new( ELeave ) CNcdContentSource;
        CleanupStack::PushL( self );
        self->ConstructL( aParentIdentifier );
        return self;
        }
        
    static CNcdContentSource* NewL( RReadStream& aStream ) 
        {
        CNcdContentSource* self = new ( ELeave ) CNcdContentSource;        
        CleanupStack::PushL( self );
        self->ConstructL( aStream );
        CleanupStack::Pop( self );
        return self;
        }
        
    ~CNcdContentSource()
        {
        delete iParentIdentifier;
        delete iNameSpace;
        delete iProvider;
        delete iUri;
        delete iNodeId;
        }
    
    const TDesC& NameSpace() const
        {
        return *iNameSpace;
        }
    void SetNameSpaceL( const TDesC& aNameSpace )
        {
        AssignDesL( iNameSpace, aNameSpace );
        }
    const TDesC& Provider() const
        {
        return *iProvider;
        }
    void SetProviderL( const TDesC& aProvider )
        {
        AssignDesL( iProvider, aProvider );
        }
    const TDesC& Uri() const
        {
        return *iUri;
        }
    void SetUriL( const TDesC& aUri )
        {
        AssignDesL( iUri, aUri );
        }
    /**
     * Getter for node id.
     *     
     * @return const TDesC
     */
    const TDesC& NodeId() const
        {
        return *iNodeId;
        }
    void SetNodeIdL( const TDesC& aNodeId )
        {
        AssignDesL( iNodeId, aNodeId );
        }
        
    void SetBroken( TBool aBroken ) 
        {
        DLTRACEIN(("broken: %d", aBroken));
        iBroken = aBroken;
        }
        
    TBool IsBroken() const 
        {
        DLTRACEIN(("broken: %d", iBroken));
        return iBroken;
        }
        
    void SetAlwaysVisible( TBool aValue ) 
        {
        iAlwaysVisible = aValue;
        }
        
    TBool AlwaysVisible() const 
        {
        return iAlwaysVisible;
        }
        
    void SetTransparent( TBool aValue )
        {
        iTransparent = aValue;
        }
        
    TBool IsTransparent() const
        {
        return iTransparent;
        }
        
        
    CNcdNodeIdentifier& ParentIdentifier() const 
        {
        return *iParentIdentifier;
        }
                
    CNcdContentSource* CopyL() const
        {
        CNcdContentSource* copy = CNcdContentSource::NewLC( *iParentIdentifier );
        copy->SetNameSpaceL( *iNameSpace );
        copy->SetProviderL( *iProvider );
        copy->SetUriL( *iUri );
        copy->SetNodeIdL( *iNodeId );
        copy->SetBroken( iBroken );
        copy->SetAlwaysVisible( iAlwaysVisible );
        copy->SetTransparent( iTransparent );
        CleanupStack::Pop( copy );
        return copy;
        }
        
    TBool Equals( const CNcdContentSource& aContentSource ) const 
        {
        if ( aContentSource.NameSpace() == NameSpace() &&
             aContentSource.Provider() == Provider() &&
             aContentSource.Uri() == Uri() &&
             aContentSource.NodeId() == NodeId() ) 
            {
            return ETrue;
            }
        else 
            {
            return EFalse;
            }
        }
             

        
    void ExternalizeL( RWriteStream& aStream ) 
        {
        iParentIdentifier->ExternalizeL( aStream );
        ExternalizeDesL( NameSpace(), aStream );
        ExternalizeDesL( Provider(), aStream );
        ExternalizeDesL( Uri(), aStream );
        ExternalizeDesL( NodeId(), aStream );
        aStream.WriteInt8L( iBroken );
        aStream.WriteInt8L( iAlwaysVisible );
        aStream.WriteInt8L( iTransparent );
        }

private:
    CNcdContentSource()
    : iBroken( EFalse ),
      iAlwaysVisible( EFalse ),
      iTransparent( EFalse )
        {
        }
        
        
    void ConstructL( const CNcdNodeIdentifier& aParentIdentifier )
        {
        iParentIdentifier = CNcdNodeIdentifier::NewL( aParentIdentifier );
        iNameSpace = KNullDesC().AllocL();
        iProvider = KNullDesC().AllocL();
        iUri = KNullDesC().AllocL();
        iNodeId = KNullDesC().AllocL();
        }
        
    void ConstructL( RReadStream& aStream ) 
        {
        iParentIdentifier = CNcdNodeIdentifier::NewL( aStream );
        InternalizeDesL( iNameSpace, aStream );
        InternalizeDesL( iProvider, aStream );
        InternalizeDesL( iUri, aStream );
        InternalizeDesL( iNodeId, aStream );
        iBroken = aStream.ReadInt8L();
        iAlwaysVisible = aStream.ReadInt8L();
        iTransparent = aStream.ReadInt8L();
        }

private:
    CNcdNodeIdentifier* iParentIdentifier;
    HBufC* iNameSpace;
    HBufC* iProvider;
    HBufC* iUri;
    HBufC* iNodeId;
    TBool iBroken;
    TBool iAlwaysVisible;
    TBool iTransparent;
    };

class CNcdNodeMap : public CBase 
    {
public:
    CNcdNodeMap( CNcdContentSource* aContentSource )
    : iContentSource( aContentSource )
        {
        }
        
    static CNcdNodeMap* NewL( RReadStream& aStream ) 
        {
        CNcdNodeMap* self = new ( ELeave ) CNcdNodeMap;
        CleanupStack::PushL( self );
        self->ConstructL( aStream );
        CleanupStack::Pop( self );
        return self;
        }
    
    virtual ~CNcdNodeMap() 
        {
        delete iContentSource;
        iNodes.ResetAndDestroy();
        }
        
    TBool ContainsNode( const CNcdNodeIdentifier& aNodeIdentifier ) 
        {
        for ( TInt i = 0; i < iNodes.Count(); i++ ) 
            {
            if ( iNodes[i]->Equals( aNodeIdentifier ) ) 
                {
                return ETrue;
                }
            }
        return EFalse;
        }
        
    void ExternalizeL( RWriteStream& aStream ) 
        {
        iContentSource->ExternalizeL( aStream );
        aStream.WriteInt32L( iNodes.Count() );
        for ( TInt i = 0; i < iNodes.Count(); i++ ) 
            {
            iNodes[i]->ExternalizeL( aStream );
            }
        }
        
private:
    CNcdNodeMap() 
        {
        }
        
    void ConstructL( RReadStream& aStream ) 
        {
        DLTRACEIN((""));
        iContentSource = CNcdContentSource::NewL( aStream );
        TInt32 nodeCount = aStream.ReadInt32L();
        for ( TInt i = 0; i < nodeCount; i++ ) 
            {
            CNcdNodeIdentifier* nodeId = CNcdNodeIdentifier::NewLC( aStream );
            iNodes.AppendL( nodeId );
            CleanupStack::Pop( nodeId );
            }
        }
        
public:
    CNcdContentSource* iContentSource;
    RPointerArray<CNcdNodeIdentifier> iNodes;
    };
    
class CNcdFolderContent : public CBase
    {
public:
    static CNcdFolderContent* NewL(
        CNcdNodeIdentifier* aFolderIdentifier, TInt aPositionInGrid ) 
        {
        CNcdFolderContent* self =
            new ( ELeave ) CNcdFolderContent( aFolderIdentifier, aPositionInGrid );
        return self;
        }
        
    static CNcdFolderContent* NewLC( RReadStream& aStream ) 
        {
        CNcdFolderContent* self = new ( ELeave ) CNcdFolderContent;
        CleanupStack::PushL( self );
        self->InternalizeL( aStream );
        return self;
        }
        
    ~CNcdFolderContent() 
        {
        delete iFolderIdentifier;
        iContentSources.ResetAndDestroy();
        }

    TInt Find( const CNcdContentSource& aContentSource ) const 
        {
        for ( TInt i = 0; i < iContentSources.Count(); i++ ) 
            {
            if ( iContentSources[i]->Equals( aContentSource ) ) 
                {
                return i;
                }
            }
        return KErrNotFound;
        }
        
    const CNcdNodeIdentifier& FolderIdentifier() const 
        {
        DASSERT( iFolderIdentifier );
        return *iFolderIdentifier;
        }
        
    TInt PositionInGrid() const 
        {
        return iPositionInGrid;
        }
        
    CNcdContentSource& ContentSource( TInt aIndex ) const 
        {
        DASSERT( aIndex >= 0 && aIndex < iContentSources.Count() );
        return *iContentSources[aIndex];
        }
    
    TInt ContentSourceCount() const 
        {
        return iContentSources.Count();
        }
        
    void AppendContentSourceL( CNcdContentSource* aContentSource ) 
        {
        iContentSources.AppendL( aContentSource );
        }
        
    void ClearContentSources() 
        {
        iContentSources.ResetAndDestroy();
        }
        
    void ExternalizeL( RWriteStream& aStream ) 
        {
        DLTRACEIN((""));
        aStream.WriteInt32L( iPositionInGrid );
        iFolderIdentifier->ExternalizeL( aStream );
        aStream.WriteInt32L( iContentSources.Count() );
        for ( TInt i = 0; i < iContentSources.Count(); i++ ) 
            {
            iContentSources[i]->ExternalizeL( aStream );
            }
        }
        
        
protected:
    CNcdFolderContent( CNcdNodeIdentifier* aFolderIdentifier, TInt aPositionInGrid ) :
        iPositionInGrid( aPositionInGrid ), iFolderIdentifier( aFolderIdentifier ) 
        {
        }
        
    CNcdFolderContent() 
        {
        }
        
    void InternalizeL( RReadStream& aStream ) 
        {
        DLTRACEIN((""));
        iPositionInGrid = aStream.ReadInt32L();
        delete iFolderIdentifier;
        iFolderIdentifier = NULL;
        iFolderIdentifier = CNcdNodeIdentifier::NewL( aStream );
        
        iContentSources.ResetAndDestroy();
        TInt contentSourceCount = aStream.ReadInt32L();
        for ( TInt i = 0; i < contentSourceCount; i++ ) 
            {
            CNcdContentSource* contentSource = CNcdContentSource::NewL( aStream );
            CleanupStack::PushL( contentSource );
            iContentSources.AppendL( contentSource );
            CleanupStack::Pop( contentSource );
            }
        }
        
    
private:
    TInt iPositionInGrid;
    CNcdNodeIdentifier* iFolderIdentifier;
    RPointerArray<CNcdContentSource> iContentSources;    
    };
    
        
class CNcdContentSourceMap : public CBase
    {
public:
    static CNcdContentSourceMap* NewL()
        {
        CNcdContentSourceMap* self =
            new( ELeave ) CNcdContentSourceMap;
        CleanupStack::PushL( self );
        self->ConstructL();
        CleanupStack::Pop( self );
        return self;
        }
        
    static CNcdContentSourceMap* NewL( RReadStream& aStream ) 
        {
        CNcdContentSourceMap* self = new ( ELeave ) CNcdContentSourceMap;
        CleanupStack::PushL( self );
        self->ConstructL( aStream );
        CleanupStack::Pop( self );
        return self;
        }
        
        
    ~CNcdContentSourceMap()
        {
        iNodeMaps.ResetAndDestroy();
        iFolderContents.ResetAndDestroy();
        }
        

    TInt GetInsertIndexL(
        const CNcdContentSource& aContentSource,
        const CNcdNodeIdentifier& aParentIdentifier )
        {
        DLTRACEIN((""));
        TInt folderIndex = FindFolder( aParentIdentifier );
        User::LeaveIfError( folderIndex );        
        CNcdFolderContent& folderContent = FolderContent( folderIndex );
                
        TInt csIndex = folderContent.Find( aContentSource );
        User::LeaveIfError( csIndex );
        TInt insertIndex = 0;
        for ( TInt i = 0; i <= csIndex; i++ ) 
            {
            CNcdContentSource& contentSource = folderContent.ContentSource( i );            
            insertIndex += NodeCount( contentSource );
            }
            
        if ( aParentIdentifier.NodeNameSpace() == 
                NcdProviderDefines::KRootNodeNameSpace ) 
            {
            // There may be bundles before this content source, so the index
            // must be increased possibly.
            for ( TInt i = 0; i < iFolderContents.Count(); i++ ) 
                {
                if ( iFolderContents[i]->FolderIdentifier().NodeNameSpace() != 
                     NcdProviderDefines::KRootNodeNameSpace && 
                     iFolderContents[i]->PositionInGrid() <= csIndex ) 
                    {
                    insertIndex++;
                    }
                }
            return insertIndex;             
            }
         else 
            {
            return insertIndex;
            }
        }        
        
    void AppendContentSourceL(
        CNcdContentSource* aContentSource,
        TInt aPositionInGrid = 0 ) 
        {
        DLTRACEIN((""));
        CNcdNodeMap* map = new ( ELeave ) CNcdNodeMap( aContentSource->CopyL() );
        CleanupStack::PushL( map );                
        iNodeMaps.AppendL( map );        
        CleanupStack::Pop( map );
        
        TInt folderIndex = FindFolder( aContentSource->ParentIdentifier() );
        if ( folderIndex != KErrNotFound ) 
            {
            CNcdFolderContent& folderContent = FolderContent( folderIndex );
            folderContent.AppendContentSourceL( aContentSource );
            }
        else 
            {
            CNcdNodeIdentifier* parentCopy = 
                CNcdNodeIdentifier::NewLC( aContentSource->ParentIdentifier() );
            CNcdFolderContent* folderContent = CNcdFolderContent::NewL(
                parentCopy, aPositionInGrid );
            CleanupStack::Pop( parentCopy );
            CleanupStack::PushL( folderContent );
            iFolderContents.AppendL( folderContent );
            CleanupStack::Pop( folderContent );
            folderContent->AppendContentSourceL( aContentSource );
            }
        }
        
    void AppendFolderL( const CNcdNodeIdentifier& aNodeIdentifier,
        TInt aPositionInGrid = 0 )
        {
        TInt folderIndex = FindFolder( aNodeIdentifier );
        if ( folderIndex == KErrNotFound )
            {
            CNcdNodeIdentifier* parentCopy = 
                CNcdNodeIdentifier::NewLC( aNodeIdentifier );
            CNcdFolderContent* folderContent = CNcdFolderContent::NewL(
                parentCopy, aPositionInGrid );
            CleanupStack::Pop( parentCopy );
            CleanupStack::PushL( folderContent );
            iFolderContents.AppendL( folderContent );
            CleanupStack::Pop( folderContent );
            }
        }
        
    void AddNodeToContentSourceL(
        CNcdNodeIdentifier* aNodeIdentifier,
        CNcdContentSource& aContentSource ) 
        {
        DLTRACEIN((""));
        TInt index = Find( aContentSource );
        User::LeaveIfError( index );
        CNcdNodeMap* map = iNodeMaps[index];
        if ( !map->ContainsNode( *aNodeIdentifier ) ) 
            {
            map->iNodes.AppendL( aNodeIdentifier );
            }
        }

    TInt ContentSourceCount()
        {
        return iNodeMaps.Count();
        }
        
    CNcdContentSource& ContentSource( TInt aIndex )
        {
        return *iNodeMaps[aIndex]->iContentSource;
        }
        
    TBool HasContentSource( const CNcdContentSource& aContentSource ) 
        {
        TInt index = Find( aContentSource );
        return index != KErrNotFound;
        }
        
    CNcdContentSource& ContentSourceL( const CNcdContentSource& aContentSource ) 
        {
        TInt index = Find( aContentSource );
        if ( index == KErrNotFound ) 
            {
            User::Leave( KErrNotFound );
            }

        return *iNodeMaps[index]->iContentSource;
        }
        
    RPointerArray<CNcdNodeIdentifier>& NodesL(
        const CNcdContentSource& aContentSource ) 
        {
        
        TInt index = Find( aContentSource );
        if ( index == KErrNotFound ) 
            {
            User::Leave( KErrNotFound );
            }
        return iNodeMaps[index]->iNodes;
        }

    TInt BundleFolderCount() const 
        {
        TInt count( 0 );
        for ( TInt i = 0; i < iFolderContents.Count(); i++ ) 
            {
            if ( iFolderContents[i]->FolderIdentifier().NodeNameSpace() !=
                 NcdProviderDefines::KRootNodeNameSpace ) 
                {
                count++;
                }
            }
        return count;
        }

    const CNcdNodeIdentifier& BundleFolder( TInt aIndex ) const 
        {
        TInt bundleIndex( 0 );
        for ( TInt i = 0; i < iFolderContents.Count(); i++ ) 
            {
            if ( iFolderContents[i]->FolderIdentifier().NodeNameSpace() != NcdProviderDefines::KRootNodeNameSpace ) 
                {
                if ( aIndex == bundleIndex ) 
                    {
                    return iFolderContents[i]->FolderIdentifier();
                    }
                bundleIndex++;
                }
            }
            
        DASSERT( EFalse );
        return iFolderContents[0]->FolderIdentifier();
        }
        
    TBool HasBundleFolder( const CNcdNodeIdentifier& aNodeIdentifier ) 
        {
        DLTRACEIN((""));
        TInt index = FindFolder( aNodeIdentifier );
        if ( index != KErrNotFound && aNodeIdentifier.NodeNameSpace() != NcdProviderDefines::KRootNodeNameSpace ) 
            {
            return ETrue;
            }
        return EFalse;
        }
                
    TInt FindFolder( const CNcdNodeIdentifier& aFolderIdentifier ) const 
        {
        for ( TInt i = 0; i < iFolderContents.Count(); i++ ) 
            {
            if ( iFolderContents[i]->FolderIdentifier().Equals( aFolderIdentifier ) ) 
                {
                return i;
                }
            }
            
        return KErrNotFound;
        }
        
    CNcdFolderContent& FolderContent( TInt aIndex ) const 
        {
        DASSERT( aIndex >= 0 && aIndex < iFolderContents.Count() );
        return *iFolderContents[aIndex];
        }

    void ExternalizeL( RWriteStream& aStream ) 
        {
        DLTRACEIN(( ( "Content source count=%d" ), iNodeMaps.Count() ));
        aStream.WriteInt32L( iNodeMaps.Count() );
        for ( TInt i = 0; i < iNodeMaps.Count(); i++ ) 
            {
            iNodeMaps[i]->ExternalizeL( aStream );
            }
        DLINFO(( ( "Folder content count=%d" ), iFolderContents.Count() ));
        aStream.WriteInt32L( iFolderContents.Count() );
        for ( TInt i = 0; i < iFolderContents.Count(); i++ ) 
            {
            iFolderContents[i]->ExternalizeL( aStream );
            }
        }
    
private:
    CNcdContentSourceMap(){}
    void ConstructL(){}
    
    void ConstructL( RReadStream& aStream ) 
        {
        DLTRACEIN((""));
        TInt32 nodeMapCount = aStream.ReadInt32L();
        DLINFO((("Content source count=%d"), nodeMapCount ));
        for ( TInt i = 0; i < nodeMapCount; i++ ) 
            {
            CNcdNodeMap* map = CNcdNodeMap::NewL( aStream );
            CleanupStack::PushL( map );
            iNodeMaps.AppendL( map );
            CleanupStack::Pop( map );
            }
            
        TInt32 folderContentCount = aStream.ReadInt32L();
        DLINFO((("Folder content count=%d"), folderContentCount ));
        for ( TInt i = 0; i < folderContentCount; i++ ) 
            {
            CNcdFolderContent* content = CNcdFolderContent::NewLC( aStream );
            iFolderContents.AppendL( content );
            CleanupStack::Pop( content );
            }
        }
        
    TInt Find( const CNcdContentSource& aContentSource ) const
        {
        for ( TInt i = 0; i < iNodeMaps.Count(); i++ ) 
            {
            if ( iNodeMaps[i]->iContentSource->Equals( aContentSource ) )
                {
                return i;
                }
            }
        return KErrNotFound;
        }
        
        
    TInt NodeCount( const CNcdContentSource& aContentSource ) const 
        {
        TInt index = Find( aContentSource );
        DASSERT( index >= 0 && index < iNodeMaps.Count() );
        CNcdNodeMap* map = iNodeMaps[index];
        return map->iNodes.Count();
        }
        
private:       
    RPointerArray<CNcdNodeMap> iNodeMaps;
    RPointerArray<CNcdFolderContent> iFolderContents;
    
    };


/**
 *  Load node operation implementation.
 *
 *  Handles the "loading of root node" i.e. creates
 *  a root node from a conf protocol response.
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CNcdLoadRootNodeOperation : public CNcdBaseOperation,
                                  public MNcdParserObserver,
                                  public MCatalogsHttpObserver,
                                  public MNcdParserConfigurationProtocolObserver,
                                  public MNcdConfigurationObserver
    {
    
public:

    /** Sub states of root node operation */
    enum TRootNodeState
        {
        EConfRequest,      // create and send a conf request
        EReceiveConf,      // receive and parse conf data
        EConfQuery,        // respond to conf query
        EBrowseRequest,    // create sub-operations 
        EReceiveBrowse,    // wait for sub-operations to complete
        EComplete,         // everything done
        EFailed            // operation failed
        };
    
    
    static CNcdLoadRootNodeOperation* NewL( 
        TInt aClientUid,
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdOperationRemoveHandler* aRemoveHandler,
        MCatalogsSession& aSession );

    static CNcdLoadRootNodeOperation* NewLC( 
        TInt aClientUid,
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdOperationRemoveHandler* aRemoveHandler,
        MCatalogsSession& aSession );

    virtual ~CNcdLoadRootNodeOperation();
    
    /**
     * Returns the identifier of the parent node this operation is loading.
     *
     * @return Identifier of the parent node.
     */
    const CNcdNodeIdentifier& NodeIdentifier() const;
    
public: //from CNcdBaseOperation

    /**
     * @see CNcdBaseOperation::Cancel
     */
    virtual void Cancel();
    
    /**
     * @see CNcdBaseOperation::HandleCancelMessage
     */
    virtual void HandleCancelMessage( MCatalogsBaseMessage* aMessage );
    
public: // from MCatalogsHttpObserver

    /**     
     * @see MCatalogsHttpObserver::HandleHttpEventL
     */
    virtual void HandleHttpEventL( 
        MCatalogsHttpOperation& aOperation, 
        TCatalogsHttpEvent aEvent );
        
    /**
     * @see MCatalogsHttpObserver::HandleHttpError()
     */
    TBool HandleHttpError(
        MCatalogsHttpOperation& aOperation,
        TCatalogsHttpError aError );
        
public: //from MNcdParserObserver

    /**
     * @see MNcdParserObserver
     */
    virtual void ParseError( TInt aErrorCode );

    /**
     * @see MNcdParserObserver
     */
    virtual void ParseCompleteL( TInt aError );
    
public: // from MNcdParserConfigurationProtocolObserver

    virtual void ConfigurationBeginL( const TDesC& aVersion, 
                                      TInt aExpirationDelta );
    virtual void ConfigurationQueryL( MNcdConfigurationProtocolQuery* aQuery );

    virtual void ClientConfigurationL(
        MNcdConfigurationProtocolClientConfiguration* aConfiguration );
    virtual void ConfigurationDetailsL( 
        CArrayPtr<MNcdConfigurationProtocolDetail>* aDetails );
    virtual void ConfigurationActionRequestL(
        MNcdConfigurationProtocolActionRequest* aActionRequest );
    virtual void ConfigurationErrorL( MNcdConfigurationProtocolError* aError );
    virtual void ConfigurationServerDetailsL( MNcdConfigurationProtocolServerDetails* aServerDetails );
    virtual void ConfigurationEndL();

public: // from MNcdOperationObserver
    
    /**
     * @see MNcdOperationObserver
     */
    virtual void Progress( CNcdBaseOperation& aOperation );
    
    /**
     * @see MNcdOperationObserver
     */
    virtual void QueryReceived( CNcdBaseOperation& aOperation,
                                 CNcdQuery* aQuery );

    /**
     * @see MNcdOperationObserver
     */
    virtual void OperationComplete( CNcdBaseOperation* aOperation,
                                    TInt aError );
                                    
public: // from MNcdConfigurationObserver

    virtual void ConfigurationChangedL();                                   
                                    
public: // from CCatalogsCommunicable
    
    /**
     * @see CCatalogsCommunicable
     */
    virtual void ReceiveMessage( 
        MCatalogsBaseMessage* aMessage,
        TInt aFunctionNumber );
        
public: // from MNcdParserErrorObserver

    virtual void ErrorL( MNcdPreminetProtocolError* aData );
                                    

protected:

    CNcdLoadRootNodeOperation( 
        TInt aClientUid,
        CNcdGeneralManager& aGeneralManager,
        MCatalogsHttpSession& aHttpSession,
        MNcdOperationRemoveHandler* aRemoveHandler,
        MCatalogsSession& aSession );
    
    void ConstructL();
    
    HBufC8* CreateConfRequestLC( CNcdQuery* aQuery = NULL );

    /**
     * Reverts the nodes which could not be retrieved due to broken content
     * source from NodeManager's temp cache to main RAM cache.
     */
    void RevertNodesOfBrokenSourcesToCacheL();
    
    /**
     * Reverts the given node belonging to given content source from
     * NodeManager's temp cache to the main cache.
     */
    void RevertNodeL(
        const CNcdNodeIdentifier& aNodeIdentifier,
        const CNcdContentSource& aContentSource );            
    
    /**
     * Checks the content source map for nodes which belong to several parent
     * nodes and adds them as child of the different parents.
     */
    void AddNodesToDifferentParentsL();
    
    /**
     * Sets always visible flags to the metadata of the nodes that belong to
     * content sources defined as always visible in client configuration.
     */
    void SetAlwaysVisibleFlagsL();
    
    void ParseCatalogBundleL( const MNcdConfigurationProtocolDetail& aDetail );
    
    static TBool ContainsNode(
        const RPointerArray<CNcdNodeIdentifier>& aNodes,
        const CNcdNodeIdentifier& aNode);

protected:

    void HandleConfigurationDataRequestMessage( MCatalogsBaseMessage& aMessage );
    
    
protected: // from CNcdBaseOperation

    /**
     * @see CNcdBaseOperation::RunOperation
     */
    virtual TInt RunOperation();
    
    void DoRunOperationL();
    
    /**
     * @see CNcdBaseOperation::RunOperation
     */
    virtual void ChangeToPreviousStateL();
    
    virtual TBool QueryCompletedL( CNcdQuery* aQuery );
    
private: // type declarations
    
    /** Sub states of master server redirection process */
    enum TMasterServerRedirectionState 
        {
        EBegin, // Start state.
        ERedirecting, // Redirecting.
        EReverted // Returned to use previous master server address.
        };

private: // data
    /**
     * A sub-state of this operation's execution.
     */
    TRootNodeState iRootNodeState;
    
    /**
     * A sub-state of master server redirection.
     */
    TMasterServerRedirectionState iMasterServerRedirectionState;
    
    /**
     * Nodes that have been loaded. This is used to store loaded
     * nodes identifiers before they can be sent to the proxy in a message.
     */
    RPointerArray<CNcdNodeIdentifier> iLoadedNodes;
        
    /**
     * This flag is set to indicate that progress info should be sent
     * when the next message is received.
     */
    TBool iSendProgress;    
    
    CNcdNodeIdentifier* iRootNodeIdentifier;
    
    /**
     * Access point manager for creating access points.
     */
    MCatalogsAccessPointManager& iAccessPointManager;
    

    /**
     * Http session for sending requests and receiving responses.
     * Not own.
     */
    MCatalogsHttpSession& iHttpSession;

    MNcdProtocol& iProtocol;
    
    /**
     * Configuration manager for getting the Master Server Address
     */
    MNcdConfigurationManager& iConfigManager;
        
    /**
     * Http operation for current transaction.
     * Not own.
     */
    MCatalogsHttpOperation* iTransaction;
        
    /**
     * Sub-operations for browse requests.
     */
    RPointerArray<CNcdLoadNodeOperationImpl> iSubOps;
    RPointerArray<CNcdLoadNodeOperationImpl> iFailedSubOps;
    RPointerArray<CNcdLoadNodeOperationImpl> iCompletedSubOps;
    
    TInt32 iClientUid;
    
    /**
     * Content sources are stored here.
     */
    CNcdContentSourceMap* iContentSourceMap;
    
    CNcdQuery* iConfQuery;
    RPointerArray<CNcdQuery> iSubOpQuerys;
   
        
    MNcdParserConfigurationProtocolObserver*	iDefaultConfigurationProtocolObserver;
    
    HBufC* iServerUri;
    
    CBufFlat* iConfigResponseBuf;
    
    TInt iBundleInsertIndex;
    
    TBool iNodeDbLocked;
    
    /**
     * Maps of current children and their children for new checking.
     */
    RPointerArray<CNcdChildEntityMap> iChildEntityMaps;
    };

#endif // C_NCDLOADROOTNODEOPERATIONIMPL_H
