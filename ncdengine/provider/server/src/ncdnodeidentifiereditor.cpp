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
* Description:   Implements NcdNodeIdentifierEditor namespace
*
*/


#include "ncdnodeidentifiereditor.h"
#include "ncdnodeidentifier.h"
#include "ncdproviderdefines.h"
#include "catalogsutils.h"
//#include "catalogsdebug.h"

// disable logging for this file
#undef DLTRACEIN
#define DLTRACEIN( x )

#undef DLTRACEOUT
#define DLTRACEOUT( x )

#undef DLTRACE
#define DLTRACE( x )

#undef DLINFO
#define DLINFO( x )

#undef DLERROR
#define DLERROR( x )

#undef DASSERT
#define DASSERT( x )

// This is used to inform if the current part of the identifier is namespace instead
// of the metadata id. This value is added after the length info before the length separator
// text.
const TText KNameSpaceCheck = 'N';

// This character is used to separate the length information from the
// actual descriptor text. 
const TText KNumSeparator = '_';


// ---------------------------------------------------------------------------
// Functions to create correct node identifier
// ---------------------------------------------------------------------------

CNcdNodeIdentifier* NcdNodeIdentifierEditor::CreateNodeIdentifierLC( const CNcdNodeIdentifier& aParentIdentifier,
                                                                     const CNcdNodeIdentifier& aMetaDataIdentifier )
    {
    DLTRACEIN(( _L("MetaId: %S, ParentId: %S"), 
                &aMetaDataIdentifier.NodeId(),
                &aParentIdentifier.NodeId() ));
    
    // The id descriptor is going to be of the following format:
    // When the namespace of the child differs from the namespace of the parent,
    // a new namespace will be included into the node id before the normal metadata id
    // is inserted. This way all the node identifiers will always be unique.
    // 1. The length of the descriptor
    // 2. If the descriptor describes namespace, then KNameSpaceCheck.
    // 3. KNumSeparator
    // 4. Id descriptor
    // 5. Loop to 1. if necessary

    // Create newId with the maximum length possibility.
    // The id may contain parent namespace and metadata namespace if
    // root or content source is handled. Also, remember the length infos.
    const TInt KNumOfLengthInfos( 2 );
    const TInt KMaxLengthInfoLength( 12 );    
    HBufC* newId =
        HBufC::NewLC( aParentIdentifier.NodeId().Length()
                      + aMetaDataIdentifier.NodeId().Length()
                      + aParentIdentifier.NodeNameSpace().Length()
                      + aMetaDataIdentifier.NodeNameSpace().Length()
                      + KMaxLengthInfoLength * KNumOfLengthInfos );

    if ( aParentIdentifier.NodeId().Length() > 0 )
        {
        // Insert the parent information into stream if
        // it exists. If the current node is root, then the parent
        // is empty and empty info should not be added in front.
        // Notice that here we do not insert the length info in front
        // because it has already been inserted before.
        newId->Des().Copy( aParentIdentifier.NodeId() );
        }

    DLINFO((_L("New id after parent: %S"), newId));
    
    // Check if the metadata or parent describes the root node.
    // If this is case, then the namespace has to be inserted before the
    // actual id.
    // Because temporary nodes do not have parent. Then, if the given parent is empty,
    // Also, insert the namespace.
    //
    
    // Always insert namespace if it differs from parent's namespace
    if ( aParentIdentifier.NodeNameSpace() != aMetaDataIdentifier.NodeNameSpace() )
        {
        DLINFO(("Identifies root or temporary node or catalog"));
        // Namespace has to be inserted also. So, the
        // node name will be unique
        newId->Des().AppendNum( aMetaDataIdentifier.NodeNameSpace().Length() );
        newId->Des().Append( KNameSpaceCheck );
        newId->Des().Append( KNumSeparator );
        newId->Des().Append( aMetaDataIdentifier.NodeNameSpace() );        
        }

    DLINFO((_L("New id after root check: %S"), newId));

    // Now that the beginning of the nodeid is of the correct form,
    // we can insert the actual id information.
    newId->Des().AppendNum( aMetaDataIdentifier.NodeId().Length() );
    newId->Des().Append( KNumSeparator );
    newId->Des().Append( aMetaDataIdentifier.NodeId() );


#ifdef COMPONENT_CATALOGSSERVEREXE  
    
    // Due to reference counting, copying is way faster than creating
    // a new identifier
    CNcdNodeIdentifier* identifier =
        CNcdNodeIdentifier::NewLC( aMetaDataIdentifier );
    identifier->SetNodeIdL( *newId );    

    CleanupStack::Pop( identifier );
    
#else

    CNcdNodeIdentifier* identifier =
        CNcdNodeIdentifier::NewL( aMetaDataIdentifier.NodeNameSpace(), 
                                  *newId,
                                  aMetaDataIdentifier.ServerUri(), 
                                  aMetaDataIdentifier.ClientUid() );
    
#endif    
    
    DLINFO((_L("New id: %S"), newId ));
    
    // Delete unnecessary descriptor
    CleanupStack::PopAndDestroy( newId );  

    // Insert the identifier into the stack
    CleanupStack::PushL( identifier );
    
    DLTRACEOUT((""));
    
    return identifier;
    }

void NcdNodeIdentifierEditor::MarkMetaDataPartL( TLex& aMetaId )
    {
    // This sets the next character (in this case the first) to be marked.
    aMetaId.Mark();

    // Find the metadata part of the identifier id
    TInt tmpLength( 0 );
    
    while( !aMetaId.Eos() )
        {            
        if ( aMetaId.Peek() == KNumSeparator
             || aMetaId.Peek() == KNameSpaceCheck )
            {
            // Now we are in the end of the number value, because
            // next mark is some separator.
            TLex valueLex( aMetaId.MarkedToken() );  
            
            User::LeaveIfError( valueLex.Val( tmpLength ) );

            if ( aMetaId.Peek() == KNameSpaceCheck )
                {
                // This means that the namespace instead of the id value follows next.
                aMetaId.Inc();
                
                // Now we are at the namespace check and the next character should be
                // the separator.
                }

            // Increase the current place by one,
            // so the current place is the separator.
            aMetaId.Inc();

            // Mark the beginning of the metadata identifier.
            // This will be changed to the beginning of the next length
            // info below if necessary.
            aMetaId.Mark();
                
            // Skip to the end of the id, now that we know the length
            // of the actual identifier. This way we will get
            // the next length.
            aMetaId.Inc( tmpLength );

            if ( aMetaId.Eos() )
                {
                // We are at the end So, no need to continue.
                // Do not mark the end, because we want the mark
                // to be in the start of the metadata id text.
                break;
                }
            else
                {
                // Mark the next character that is the beginning of the actual
                // identifier.
                aMetaId.Mark();                
                }
            }
        else if ( !aMetaId.Peek().IsDigit() )
            {
            DLERROR(("Should be number but is not."));
            User::Leave( KErrArgument );
            }
        else
            {
            // Find the end of the length number by peeking until separator
            // is found
            aMetaId.Inc();
            }
        }
    }
    
void NcdNodeIdentifierEditor::GetParentIdAndNsL( const TDesC& aChildId,
    TPtrC& aParentId, TPtrC& aParentNs )
    {
    DLTRACEIN((""));
    TLex childIdLex( aChildId );
    // This sets the next character (in this case the first) to be marked.
    childIdLex.Mark();

    // Find the metadata part of the identifier id
    TInt tmpLength( 0 );    
    
    TInt nextIdStartPos = -1;
    TInt latestIdStartPos = -1;
    TInt latestNsStartPos = -1;
    TInt latestNsLength = -1;
    TInt previousNsStartPos = -1;
    TInt previousNsLength = -1;
    
    while( !childIdLex.Eos() )
        {
            
        if ( childIdLex.Peek() == KNumSeparator
             || childIdLex.Peek() == KNameSpaceCheck )
            {
            // Now we are in the end of the number value, because
            // next mark is some separator.
            TLex valueLex( childIdLex.MarkedToken() );    
            User::LeaveIfError( valueLex.Val( tmpLength ) );

            if ( childIdLex.Peek() == KNameSpaceCheck )
                {
                // This means that the namespace instead of the id value follows next.
                childIdLex.Inc();
                
                // Now we are at the namespace check and the next character should be
                // the separator.
                
                // update ns positions
                previousNsStartPos = latestNsStartPos;
                previousNsLength = latestNsLength;
                latestNsStartPos = childIdLex.Offset() + 1;
                latestNsLength = tmpLength;
                }
            else
                {
                // update id positions
                latestIdStartPos = nextIdStartPos;
                nextIdStartPos = childIdLex.Offset() + 1 + tmpLength;
                }

            // Increase the current place by one,
            // so the current place is the separator.
            childIdLex.Inc();

            // Mark the beginning of the metadata identifier.
            // This will be changed to the beginning of the next length
            // info below if necessary.
            childIdLex.Mark();
                
            // Skip to the end of the id, now that we know the length
            // of the actual identifier. This way we will get
            // the next length.
            childIdLex.Inc( tmpLength );

            if ( childIdLex.Eos() )
                {
                // We are at the end So, no need to continue.
                // Do not mark the end, because we want the mark
                // to be in the start of the metadata id text.
                break;
                }
            else
                {
                // Mark the next character that is the beginning of the actual
                // identifier.
                childIdLex.Mark();                
                }
            }
        else if ( !childIdLex.Peek().IsDigit() )
            {
            DLERROR(("Should be number but is not."));
            User::Leave( KErrArgument );
            }
        else
            {
            // Find the end of the length number by peeking until separator
            // is found
            childIdLex.Inc();
            }
        }
    
    if( previousNsStartPos < 0 || previousNsStartPos < 0)
        {
        // no parent found
        aParentId.Set( KNullDesC );
        aParentId.Set( KNullDesC );
        }
    else
        {
        // latest ns is part of latest id (child has own ns)
        if( latestIdStartPos < latestNsStartPos )
            {
            // previous ns is parent's ns
            // and latest ns is child's ns
            aParentNs.Set( aChildId.Mid( previousNsStartPos, previousNsLength ) );
            }
        // latest ns not part of latest id (child is of same ns)
        else
            {
            // latest ns is parent's ns
            aParentNs.Set( aChildId.Mid( latestNsStartPos, latestNsLength ) );
            }
        aParentId.Set( aChildId.Mid( 0, latestIdStartPos ) );
        }

    DLINFO((_L("parent id:%S"),&aParentId));
    DLINFO((_L("parent ns:%S"),&aParentNs));
    }

CNcdNodeIdentifier* NcdNodeIdentifierEditor::CreateMetaDataIdentifierL( const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((""));
    
    CNcdNodeIdentifier* metaIdentifier( 
        CreateMetaDataIdentifierLC( aNodeIdentifier ) );
    
    CleanupStack::Pop( metaIdentifier );
    DLTRACEOUT((""));
    return metaIdentifier;
    }


CNcdNodeIdentifier* NcdNodeIdentifierEditor::CreateMetaDataIdentifierLC( const CNcdNodeIdentifier& aNodeIdentifier )
    {
    DLTRACEIN((_L("NodeIdentifier: %S"), &aNodeIdentifier.NodeId()));
    
    if ( aNodeIdentifier.ContainsEmptyFields() )
        {
        DLERROR(("Node identifier was empty"));
        User::Leave( KErrArgument );
        }
        
    TLex metaId( aNodeIdentifier.NodeId() );
    NcdNodeIdentifierEditor::MarkMetaDataPartL( metaId );
    
#ifdef COMPONENT_CATALOGSSERVEREXE
    
    // Due to reference counting copying a nodeidentifier is a lot faster
    // than creating a new one so we first copy and then set the new value
    CNcdNodeIdentifier* metaIdentifier = 
            CNcdNodeIdentifier::NewLC( aNodeIdentifier );
    metaIdentifier->SetNodeIdL( metaId.RemainderFromMark() );

#else

    CNcdNodeIdentifier* metaIdentifier = 
        CNcdNodeIdentifier::NewLC( aNodeIdentifier.NodeNameSpace(),
                                   metaId.RemainderFromMark(),
                                   aNodeIdentifier.ServerUri(),
                                   aNodeIdentifier.ClientUid() );
    
#endif    
    DLTRACEOUT((_L("parsed metaIdentifier: %S"), 
                &metaIdentifier->NodeId())); 
           
    return metaIdentifier;
    }

TBool NcdNodeIdentifierEditor::DoesMetaDataIdentifierMatchL( const CNcdNodeIdentifier& aNodeIdentifier,
    const TDesC& aMetaId, const TDesC& aMetaNameSpace, const TUid& aMetaUid )
    {
    if ( aMetaUid != aNodeIdentifier.ClientUid() ||
         aMetaNameSpace != aNodeIdentifier.NodeNameSpace() )
        {
        // no need to parse id if namespace and uid don't match
        return EFalse;
        }
    TLex metaId( aNodeIdentifier.NodeId() );
    NcdNodeIdentifierEditor::MarkMetaDataPartL( metaId );
    
    return aMetaId == metaId.RemainderFromMark();
    }

TBool NcdNodeIdentifierEditor::DoesMetaDataIdentifierMatchL( const CNcdNodeIdentifier& aMetaDataIdentifier,
    const CNcdNodeIdentifier& aNodeIdentifier )
    {
    return DoesMetaDataIdentifierMatchL( aNodeIdentifier, 
        aMetaDataIdentifier.NodeId(),
        aMetaDataIdentifier.NodeNameSpace(),
        aMetaDataIdentifier.ClientUid() );
    }

CNcdNodeIdentifier* NcdNodeIdentifierEditor::CreateRootIdentifierForClientLC( const TUid& aUid )
    {
    DLTRACEIN((""));

    // The root has an empty parent.    
    CNcdNodeIdentifier* empty( CNcdNodeIdentifier::NewLC() );

    // All the roots have the same namespace, but their ids are
    // the client UIDs.        
    CNcdNodeIdentifier* initRoot( 
        CNcdNodeIdentifier::NewLC( NcdProviderDefines::KRootNodeNameSpace,
                                   CleanUidName( aUid ),
                                   KNullDesC,
                                   aUid ) );
                                   
    // Create the actual root identifier from the initial version
    CNcdNodeIdentifier* root( 
        CreateNodeIdentifierLC( *empty, *initRoot ) );
    CleanupStack::Pop( root );
    
    // Delete temporary identifiers
    CleanupStack::PopAndDestroy( initRoot );
    CleanupStack::PopAndDestroy( empty );
    
    // Push the root back to the stack
    CleanupStack::PushL( root );        

    DLTRACEOUT((""));
    
    return root;
    }


CNcdNodeIdentifier* NcdNodeIdentifierEditor::CreateSearchRootIdentifierForClientLC( const TUid& aUid )
    {
    DLTRACEIN((""));

    // The root has an empty parent.    
    CNcdNodeIdentifier* empty( CNcdNodeIdentifier::NewLC() );

    // All the roots have the same namespace, but their ids are
    // the client UIDs.        
    CNcdNodeIdentifier* initRoot( 
        CNcdNodeIdentifier::NewLC( NcdProviderDefines::KSearchRootNameSpace,
                                   CleanUidName( aUid ),
                                   KNullDesC,
                                   aUid ) );
                                   
    // Create the actual root identifier from the initial version
    CNcdNodeIdentifier* root( 
        CreateNodeIdentifierLC( *empty, *initRoot ) );
    CleanupStack::Pop( root );
    
    // Delete temporary identifiers
    CleanupStack::PopAndDestroy( initRoot );
    CleanupStack::PopAndDestroy( empty );
    
    // Push the root back to the stack
    CleanupStack::PushL( root );        

    DLTRACEOUT((""));
    
    return root;
    }                                   


CNcdNodeIdentifier* NcdNodeIdentifierEditor::CreateTemporaryNodeIdentifierLC( const CNcdNodeIdentifier& aMetaDataIdentifier )
    {
    DLTRACEIN((""));

    CNcdNodeIdentifier* identifier( NULL );

    CNcdNodeIdentifier* empty =
        CNcdNodeIdentifier::NewLC();
    
    // Just create the node identifier that has no parent. Notice that
    // even the namespace will not be included into the identifier id
    // because this is not a root identifier. So, this id differs from the
    // meta id just by the length info in front of the id.
    identifier = CreateNodeIdentifierLC( *empty, aMetaDataIdentifier );
    CleanupStack::Pop( identifier );
        
    CleanupStack::PopAndDestroy( empty );
    
    CleanupStack::PushL( identifier );
        
    DLTRACEOUT((""));
    
    return identifier;
    }
    
    

// ---------------------------------------------------------------------------
// Functions that can provide information that is included to the identifiers
// ---------------------------------------------------------------------------

TInt NcdNodeIdentifierEditor::NodeDepthL( const CNcdNodeIdentifier& aNodeIdentifier )
    {
    //DLTRACEIN((""));
    
    if ( aNodeIdentifier.ContainsEmptyFields() )
        {
        DLERROR(("Node identifier was empty"));
        User::Leave( KErrArgument );
        }

    TInt depth( 0 );

    TLex metaId( aNodeIdentifier.NodeId() );
    // This sets the next character (in this case the first) to be marked.
    metaId.Mark();

    // Find the metadata part of the identifier id
    TInt tmpLength( 0 );
    
    while( ETrue )
        {
        if ( metaId.Eos() )
            {
            // The whole thing has been checked.
            break;
            }

        if ( metaId.Peek() == KNumSeparator
             || metaId.Peek() == KNameSpaceCheck )
            {
            // Now we are in the end of the number value, because
            // next mark is one of the separators.
            TLex valueLex( metaId.MarkedToken() );    
            User::LeaveIfError( valueLex.Val( tmpLength ) );

            if ( metaId.Peek() == KNameSpaceCheck )
                {
                // Because the namespaces should be skipped when
                // depth is calculated decrease the depth because it will
                // be increased later.
                --depth;
                metaId.Inc();
                // Now we are at the namespace separator value and the
                // next character should be the separator.   
                }

            // Increase the current place by one,
            // so the current place is the separator.
            metaId.Inc();
            
            // Skip to the end of the id, now that we know the length
            // of the actual identifier. This way we will get
            // the next length.
            metaId.Inc( tmpLength );

            // Start the next phase from the beginning of the new
            // length info.
            metaId.Mark();
            
            // Also, update depth value, because we had some namespace or
            // id descriptor here.
            ++depth;
            }
        else if ( !metaId.Peek().IsDigit() )
            {
            DLERROR((_L("Should be number but is not. Most likely not identifier of node. Ns: %S, Id: %S"),
                     &aNodeIdentifier.NodeNameSpace(), &aNodeIdentifier.NodeId()));
            // Most likely we have gotten an identifier that is not used for node.
            // This may be case for example when a metadata identifier has been given here.
            // Leave with error argument. So, user may do what ever it wants with the error.
            User::Leave( KErrArgument );
            }
        else
            {
            // Find the end of the length number by peeking until separator
            // is found
            metaId.Inc();
            }
        }

    // Notice that the depth does not contain the possible namespaces.
    // Also, because root depth should be zero, decrease the given depth info
    // by one.
    --depth;
/*            
    DLTRACEOUT((_L("Node depth: %d, id: %S"), 
                depth, &aNodeIdentifier.NodeId()));
  */          
    return depth;
    }


TBool NcdNodeIdentifierEditor::IdentifiesSomeRoot( const CNcdNodeIdentifier& aIdentifier )
    {
    const TDesC& nameSpace = aIdentifier.NodeNameSpace();
    
    if ( nameSpace == NcdProviderDefines::KRootNodeNameSpace
         || nameSpace == NcdProviderDefines::KSearchRootNameSpace )
        {
        // Identifier identified some root.
        return ETrue;
        }
    else
        {
        // Identifier does not identify root.
        return EFalse;
        }
    }


TBool NcdNodeIdentifierEditor::IdentifiesTemporaryNodeL( const CNcdNodeIdentifier& aIdentifier )
    {
    TBool isTemporary( ETrue );
    
    CNcdNodeIdentifier* rootIdentifier =
        CreateRootIdentifierForClientLC( aIdentifier.ClientUid() );
    CNcdNodeIdentifier* searchRootIdentifier =
            CreateSearchRootIdentifierForClientLC( aIdentifier.ClientUid() );

    // All the nodes that do not start with the root id are thought to be
    // temporary nodes.
    
    
    if ( aIdentifier.NodeId().Length() >= rootIdentifier->NodeId().Length() &&
        aIdentifier.NodeId().Mid( 0, rootIdentifier->NodeId().Length() )
            == rootIdentifier->NodeId() )
        {
        isTemporary = EFalse;
        }
    else if ( aIdentifier.NodeId().Length() >= searchRootIdentifier->NodeId().Length() &&
        aIdentifier.NodeId().Mid( 0, searchRootIdentifier->NodeId().Length() )
            == searchRootIdentifier->NodeId() )
        {
        isTemporary = EFalse;
        }
    
    CleanupStack::PopAndDestroy( searchRootIdentifier );
    CleanupStack::PopAndDestroy( rootIdentifier );
    
    return isTemporary;
    }


TBool NcdNodeIdentifierEditor::ParentOf( const CNcdNodeIdentifier& aParentNodeIdentifier,
                                         const CNcdNodeIdentifier& aChildNodeIdentifier )
    {
    DLTRACEIN((""));
    
    // If the child identifier starts with the parent identifier id,
    // then the child is actually a real child.
    // Notice that the child can not be its own parent. So,
    // id lengths are also checked.
    if ( aParentNodeIdentifier.NodeId().Length()
            < aChildNodeIdentifier.NodeId().Length()
         && aChildNodeIdentifier.NodeId().Mid( 0, aParentNodeIdentifier.NodeId().Length() )
                == aParentNodeIdentifier.NodeId()
         && aChildNodeIdentifier.ClientUid() 
                == aParentNodeIdentifier.ClientUid() )
        {
        DLTRACEOUT((_L("%S::%S was parent of %S::%S"), 
                    &aParentNodeIdentifier.NodeNameSpace(),
                    &aParentNodeIdentifier.NodeId(),
                    &aChildNodeIdentifier.NodeNameSpace(),
                    &aChildNodeIdentifier.NodeId()));
        return ETrue;
        }
    else
        {
        DLTRACEOUT((_L("%S::%S was not parent of %S::%S"), 
                    &aParentNodeIdentifier.NodeNameSpace(),
                    &aParentNodeIdentifier.NodeId(),
                    &aChildNodeIdentifier.NodeNameSpace(),
                    &aChildNodeIdentifier.NodeId()));
        return EFalse;
        }
    }


CNcdNodeIdentifier* NcdNodeIdentifierEditor::ParentOfLC( const CNcdNodeIdentifier& aChildNodeIdentifier )
    {
    DLTRACEIN((_L("ChildNodeIdentifier: %S"), &aChildNodeIdentifier.NodeId()));
    
    if ( aChildNodeIdentifier.ContainsEmptyFields() )
        {
        DLERROR(("Node identifier was empty"));
        User::Leave( KErrArgument );
        }

    TPtrC parentId;
    TPtrC parentNs;
    GetParentIdAndNsL( aChildNodeIdentifier.NodeId(), parentId, parentNs );
    
    if( parentId == KNullDesC || parentId == KNullDesC )
        {
        DLERROR(("Node identifier contained no parent"));
        User::Leave( KErrArgument );
        }
    
    CNcdNodeIdentifier* parentIdentifier =
        CNcdNodeIdentifier::NewLC( parentNs,
                                  parentId,
                                  aChildNodeIdentifier.ServerUri(),
                                  aChildNodeIdentifier.ClientUid() );
                                   
    DLTRACEOUT((_L("parsed parent Identifier: %S"), 
                &parentIdentifier->NodeId())); 
           
    return parentIdentifier;
    }

TBool NcdNodeIdentifierEditor::IdentifiesSearchNodeL( const CNcdNodeIdentifier& aIdentifier )
    {
    TBool isSearch( EFalse );
    
    CNcdNodeIdentifier* searchRootIdentifier =
            CreateSearchRootIdentifierForClientLC( aIdentifier.ClientUid() );

    // All the nodes that do not start with the root id are thought to be
    // temporary nodes.
    if ( aIdentifier.NodeId().Mid( 0, searchRootIdentifier->NodeId().Length() )
            == searchRootIdentifier->NodeId() )
        {
        isSearch = ETrue;
        }
    
    CleanupStack::PopAndDestroy( searchRootIdentifier );
    return isSearch;
    }
