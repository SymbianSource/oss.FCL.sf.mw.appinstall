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
* Description:   CNcdEntityRefParser implementation
*
*/


#include "ncdunknownparser.h"
#include "ncdprotocolwords.h"
#include "ncdprotocolutils.h"
#include "ncd_parser_pp_entityref.h"
#include "ncd_pp_itemrefimpl.h"
#include "ncd_pp_folderrefimpl.h"
#include "ncdparserobserver.h"

#include "catalogsdebug.h"

// static void DebugPrintStuff( const Xml::RTagInfo& aElement,
//                              const Xml::RAttributeArray& aAttributes )
//     {
// //     DLINFO(("localname %S",&aElement.LocalName().DesC()));
// //     DLINFO(("      uri %S",&aElement.Uri().DesC()));
// //     DLINFO(("   prefix %S",&aElement.Prefix().DesC()));
// //     for( TInt i = 0; i < aAttributes.Count(); i++ )
// //         {
// //         DLINFO(("-attribute %d",i));
// //         DLINFO(("-      uri %S",  &aAttributes[i].Attribute().Uri().DesC() ));
// //         DLINFO(("-localname %S",  &aAttributes[i].Attribute().LocalName().DesC() ));
// //         DLINFO(("-   prefix %S",  &aAttributes[i].Attribute().Prefix().DesC() ));
// //         DLINFO(("-    value %S",  &aAttributes[i].Value().DesC() ));
// //         DLINFO(("-     type %d",  aAttributes[i].Type() ));
// //         }
//     }

CNcdPreminetProtocolEntityRefParser* 
CNcdPreminetProtocolEntityRefParser::NewL( MNcdParserObserverBundle& aObservers,
                                           MNcdSubParserObserver& aSubParserObserver,
                                           TInt aDepth,
                                           const Xml::RTagInfo& aElement,
                                           const Xml::RAttributeArray& aAttributes,
                                           const TDesC& aParentId,
                                           const TDesC& aParentNamespace )
    {
    CNcdPreminetProtocolEntityRefParser* self 
        = new(ELeave) CNcdPreminetProtocolEntityRefParser( aObservers,
                                                           aSubParserObserver,
                                                           aDepth );
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aAttributes, aParentId, aParentNamespace );
    CleanupStack::Pop( self );
    return self;
    }

CNcdPreminetProtocolEntityRefParser::CNcdPreminetProtocolEntityRefParser( MNcdParserObserverBundle& aObservers,
                                                                          MNcdSubParserObserver& aSubParserObserver,
                                                                          TInt aDepth )
    : CNcdSubParser( aObservers, aSubParserObserver, aDepth, EParserEntityRef )
    {

    }

CNcdPreminetProtocolEntityRefParser::~CNcdPreminetProtocolEntityRefParser()
    {
    DLTRACEIN((""));
    delete iItemRef;
    delete iFolderRef;
    delete iParentNamespace;
    }

void CNcdPreminetProtocolEntityRefParser::ConstructL( const Xml::RTagInfo& aElement,
                                                      const Xml::RAttributeArray& aAttributes,
                                                      const TDesC& aParentId,
                                                      const TDesC& aParentNamespace )
    {
    CNcdSubParser::ConstructL( aElement );
    
    DLTRACEIN(("depth=%d tag=%S",iDepth,iTag));
    DLINFO((_L("parent=%S namespace=%S"),&aParentId,&aParentNamespace));
    
    NcdProtocolUtils::AssignDesL( iParentNamespace, aParentNamespace );


    TPtrC8 type = AttributeValue( KAttrType, aAttributes );
    if( type == KNullDesC8 )
        {
        type.Set( AttributePrefix( KAttrXsiType, aAttributes ) );
        }

    if( type == KValueFolderRef )
        {
        CNcdPreminetProtocolFolderRefImpl* folderRef = CNcdPreminetProtocolFolderRefImpl::NewL();
        iFolderRef = folderRef;
        DLINFO(("New Folder ref!"));
        TPtrC8 count = AttributeValue( KAttrCount, aAttributes );
        if( count != KNullDesC8 )
            {
            folderRef->iCount = NcdProtocolUtils::DesDecToIntL( count );
            }
        TPtrC8 authorizedCount = AttributeValue( KAttrAuthorizedCount, aAttributes );
        if( count != KNullDesC8 )
            {
            folderRef->iAuthorizedCount = NcdProtocolUtils::DesDecToIntL( count );
            }

        // @ Description
        NcdProtocolUtils::AssignDesL( iFolderRef->iParentId, aParentId );
        NcdProtocolUtils::AssignDesL( iFolderRef->iId, AttributeValue( KAttrId, aAttributes ) );
        NcdProtocolUtils::AssignDesL( iFolderRef->iNamespace, AttributeValue( KAttrNamespace, aAttributes ) );
        NcdProtocolUtils::AssignDesL( iFolderRef->iTimestamp, AttributeValue( KAttrTimestamp, aAttributes ) );
        NcdProtocolUtils::AssignDesL( iFolderRef->iServerUri, iObservers->SessionOrigin() );
        NcdProtocolUtils::AssignDesL( iFolderRef->iParentNamespace, *iParentNamespace );
        NcdProtocolUtils::AssignDesL( iFolderRef->iRemoteUri, AttributeValue( KAttrUri, aAttributes ) );

        if( *iFolderRef->iNamespace == KNullDesC )
            {
            // Namespace was not set, use parent.
            NcdProtocolUtils::AssignDesL( iFolderRef->iNamespace, *iParentNamespace );
            }

        }
    else if( type == KValueItemRef || type == KValueActionRef )
        {
        DLINFO(("New Item ref!"));
        iItemRef = CNcdPreminetProtocolItemRefImpl::NewL();
        
        // @ Description
        NcdProtocolUtils::AssignDesL( iItemRef->iParentId, aParentId );
        NcdProtocolUtils::AssignDesL( iItemRef->iId, AttributeValue( KAttrId, aAttributes ) );
        NcdProtocolUtils::AssignDesL( iItemRef->iNamespace, AttributeValue( KAttrNamespace, aAttributes ) );
        NcdProtocolUtils::AssignDesL( iItemRef->iTimestamp, AttributeValue( KAttrTimestamp, aAttributes ) );
        NcdProtocolUtils::AssignDesL( iItemRef->iServerUri, iObservers->SessionOrigin() );
        NcdProtocolUtils::AssignDesL( iItemRef->iParentNamespace, *iParentNamespace );
        NcdProtocolUtils::AssignDesL( iItemRef->iRemoteUri, AttributeValue( KAttrUri, aAttributes ) );

        if( *iItemRef->iNamespace == KNullDesC )
            {
            // Namespace was not set, use parent.
            NcdProtocolUtils::AssignDesL( iItemRef->iNamespace, *iParentNamespace );
            }
 
       }
    else
        {
        // unknown type!
        DLERROR(("Unknown item type=%S",&type));
        // Should there be more some informatic error?
        }

    DLTRACEOUT((""));
    }


void CNcdPreminetProtocolEntityRefParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                                           const Xml::RAttributeArray& aAttributes, 
                                                           TInt aErrorCode) 
    {

    DLTRACE((""));

    if( iSubParser )
        {
        // Use existing subparser.
        iSubParser->OnStartElementL( aElement, aAttributes, aErrorCode );
        }
    else if( iFolderRef == 0 && iItemRef == 0 )
        {
        // Skip everything if there was no supported type found earlier.
        DLERROR(("Skipping unknown entityref"));
        iSubParser = CNcdUnknownParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
        }
    else
        {
        // If there is no subparser, we should handle all tags ourselves.
        TPtrC8 tag( aElement.LocalName().DesC() );
        DLTRACE(("entity start tag=%S error=%d depth=%d",&aElement.LocalName().DesC(),aErrorCode,iDepth));

        // First handle simple tags if we can.
        if( tag == KTagValidUntil )
            {
            DLTRACE(("validuntil delta=%S",&AttributeValue(KAttrDelta,aAttributes)));
            if( iItemRef )
                {
                iItemRef->iValidUntilDelta = 
                    NcdProtocolUtils::DesDecToIntL( AttributeValue( KAttrDelta, aAttributes ) );
                iItemRef->iValidUntilAutoUpdate = EFalse;
                NcdProtocolUtils::DesToBool( iItemRef->iValidUntilAutoUpdate,
                                              AttributeValue( KAttrAutoUpdate, aAttributes ) );
                }
            else
                {
                DASSERT( iFolderRef );
                iFolderRef->iValidUntilDelta = 
                    NcdProtocolUtils::DesDecToIntL( AttributeValue( KAttrDelta, aAttributes ) );
                iFolderRef->iValidUntilAutoUpdate = EFalse;
                NcdProtocolUtils::DesToBool( iFolderRef->iValidUntilAutoUpdate,
                                              AttributeValue( KAttrAutoUpdate, aAttributes ) );
                }
            }
        else if( tag == KTagQueries )
            {
            DLTRACE(("queries"));
            // We don't really need to handle this tag at all, it suffices to react to
            // individual <query> tags. This assumes that <query> tags don't come anywhere else
            // in this entity.
            }
        else if( tag == KTagQuery )
            {
            TInt queryId = NcdProtocolUtils::DesDecToIntL( AttributeValue( KAttrId, aAttributes ) );
            DLTRACE(("query id=%d",queryId));
            if( iItemRef )
                {
                User::LeaveIfError( iItemRef->iQueries.Append( queryId ) );
                }
            else
                {
                User::LeaveIfError( iFolderRef->iQueries.Append( queryId ) );
                }
            }
        else if( tag == KTagEntity && iFolderRef )
            {
            DLTRACE(("subentity for folder"));
            // Subentities, this is allowed for folderrefs.
            // : Observer callback here, the parent entity is finished?
            // This might result in child entities finishing before the parent?
            iSubParser = CNcdPreminetProtocolEntityRefParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes,
                                                                    iFolderRef->Id() /*parent entity id*/, *iParentNamespace );
            }
        else
            {
            iSubParser = CNcdUnknownParser::NewL( *iObservers, *this, iDepth+1, aElement, aAttributes );
            }

        }
    }


void CNcdPreminetProtocolEntityRefParser::OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode) 
    {
    DLTRACEIN((""));

    if( iSubParser == 0 && iTag && *iTag == aElement.LocalName().DesC() )
        {
        DLTRACE(("end tag=%S",&aElement.LocalName().DesC()));
        DLINFO(("itemref %X folderref %X",iItemRef,iFolderRef));
        iSubParserObserver->SubParserFinishedL( aElement.LocalName().DesC(), aErrorCode );
        }
    else if( iSubParser )
        {
        iSubParser->OnEndElementL( aElement, aErrorCode );
        }
    else
        {
        DLWARNING(("end tag ignored, tag=%S",&aElement.LocalName().DesC()));
        }
    }


void CNcdPreminetProtocolEntityRefParser::SubParserFinishedL( const TDesC8& aTag, 
                                                              TInt /*aErrorCode*/ )
    {
    DLTRACEIN(("tag=%S subparser=%X",&aTag,iSubParser));
    (void) aTag; // suppresses compiler warning

    // Subparser finished, what was it doing?
    if( iSubParser->Type() == EParserEntityRef &&
        iSubParser->Tag() == KTagEntity )
        {
        DLTRACE(("subentity ready"));
        // A subentity is now ready. Report it.
        if( iObservers->EntityObserver() )
            {
            MNcdPreminetProtocolItemRef* itemRef =
                static_cast<CNcdPreminetProtocolEntityRefParser*>( iSubParser )->ItemRef();
            MNcdPreminetProtocolFolderRef* folderRef = 
                static_cast<CNcdPreminetProtocolEntityRefParser*>( iSubParser )->FolderRef();
            DLINFO(("subparser itemref=%X folderref=%X",itemRef,folderRef));
            if( itemRef )
                {
                iObservers->EntityObserver()->ItemRefL( itemRef );
                }
            if( folderRef )
                {
                iObservers->EntityObserver()->FolderRefL( folderRef );
                }
            }
        else
            {
            DLWARNING(("Child entity %S ignored, no observer",&aTag));
            }
        }

    delete iSubParser; 
    iSubParser = 0;
    }

MNcdPreminetProtocolFolderRef*
CNcdPreminetProtocolEntityRefParser::FolderRef()
    {
    DLTRACEIN(("iFolderRef=%X",iFolderRef));
    MNcdPreminetProtocolFolderRef* ref = iFolderRef;
    iFolderRef = 0;
    return ref;
    }

MNcdPreminetProtocolItemRef*
CNcdPreminetProtocolEntityRefParser::ItemRef()
    {
    DLTRACEIN(("iItemRef=%X",iItemRef));
    MNcdPreminetProtocolItemRef* ref = iItemRef;
    iItemRef = 0;
    return ref;
    }
