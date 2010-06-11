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
* Description:   CNcdRequestBrowseSearch implementation
*
*/


#include "ncdrequestbrowsesearch.h"
#include "ncdrequestbase.h"
#include "ncdrequestconfigurationdata.h"
#include "catalogsdebug.h"
#include "ncdprotocolutils.h"
#include "ncdprotocoltypes.h"
#include "ncdprotocolwords.h"

CNcdRequestBrowseSearch* CNcdRequestBrowseSearch::NewL()
    {
    CNcdRequestBrowseSearch* self = 
        CNcdRequestBrowseSearch::NewLC( );
    CleanupStack::Pop();
    return self;
    }

CNcdRequestBrowseSearch* CNcdRequestBrowseSearch::NewLC()
    {
    CNcdRequestBrowseSearch* self = 
        new (ELeave) CNcdRequestBrowseSearch();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CNcdRequestBrowseSearch::ConstructL()
    {
    DLTRACEIN((""));
    CNcdRequestBase::ConstructL( KTagPreminetRequest );
    iName.SetL(KTagBrowse);
    iNamespaceUri.SetL(KDefaultNamespaceUri);
    iPrefix.SetL(KAttrCdpNamespacePrefix);
    iVersion.SetL(KAttrPreminetVersion);
    DLTRACEOUT((""));
    }
    
CNcdRequestBrowseSearch::CNcdRequestBrowseSearch()
: CNcdRequestBase(), iDoSearch(EFalse)
    {
    }

CNcdRequestBrowseSearch::~CNcdRequestBrowseSearch()
    {
    DLTRACEIN((""));
    if (iName.NotNull())
        iName.Free();
    if (iNamespaceUri.NotNull())
        iNamespaceUri.Free();
    if (iPrefix.NotNull())
        iPrefix.Free();
    if (iType.NotNull())
        iType.Free();
    
    ResetAndCloseArray( iEntities );
    
    iEntityFilter.Close();    
    
    iResponseFilter.Close();
        
    DLTRACEOUT((""));
    }


void CNcdRequestBrowseSearch::SetSearch(TBool aDoSearch) 
    {
    iDoSearch = aDoSearch;
    }


void CNcdRequestBrowseSearch::AddEntityL(
    const TDesC& aId, 
    TBool aIncludeMetadata) 
    {
    DLTRACEIN((_L("entry")));
    AddEntityL(aId, KNullDesC, aIncludeMetadata);
    }
    
void CNcdRequestBrowseSearch::AddEntityL(
    const TDesC& aId, 
    const TDesC& aTimestamp, 
    TBool aIncludeMetadata) 
    {
    DLTRACEIN((_L("entry")));
    TNcdRequestRemoteEntity ent;
    ent.id.SetL(aId);
    
    if (aTimestamp != KNullDesC)
        ent.timeStamp.SetL(aTimestamp);
    
    ent.includeMetaData = 
        aIncludeMetadata ? EValueTrue : EValueFalse;
    iEntities.AppendL(ent);
    }

void CNcdRequestBrowseSearch::AddEntityFilterL(
    const MDesC16Array& aIncludedKeywords,
    const MDesC16Array& aIncludedContentPurposes,    
    TBool         aSubscribableContent,
    TBool         aFreeContent) 
    {
    DLTRACEIN(("entry"));
    iEntityFilter.keywords.Reset();
    for (TInt i = 0; i < aIncludedKeywords.MdcaCount(); ++i) 
        {
        TPtrC16 ptr = aIncludedKeywords.MdcaPoint(i);
        if ( ptr.Length() > 0) 
            {
            TNcdRequestFilter keyword;
            keyword.text.SetL(ptr);
            keyword.exclude = EValueFalse;
            iEntityFilter.keywords.Append( keyword );
            }
        }
        

    DLTRACE((""));
    iEntityFilter.contentPurposes.Reset();
    for (TInt i = 0; i < aIncludedContentPurposes.MdcaCount(); ++i) 
        {
        TPtrC16 ptr = aIncludedContentPurposes.MdcaPoint(i);
        if ( ptr.Length() > 0) 
            {
            TNcdRequestFilter contentPurpose;
            contentPurpose.text.SetL(ptr);
            contentPurpose.exclude = EValueFalse;
            iEntityFilter.contentPurposes.Append( contentPurpose );
            }
        }

    DLTRACE((""));

    iEntityFilter.subscribableContent = 
        aSubscribableContent ? EValueTrue : EValueFalse;
    iEntityFilter.freeContent = 
        aFreeContent ? EValueTrue : EValueFalse;
    
    // Should add details
    iEntityFilterEnabled = ETrue;
    }
    

void CNcdRequestBrowseSearch::AddResponseFilterL(
    TInt          aPageSize,
    TInt          aPageStart,
    TInt          aStructureDepth,
    TInt          aMetaDataDepth,
    TInt          aMetaDataPerLevel,
    const MDesC16Array& aIncludedElements,
    const MDesC16Array& aExcludedElements )
    {
    DLTRACEIN((_L("entry")));
    iResponseFilter.pageSize = aPageSize;
    iResponseFilter.pageStart = aPageStart;
    iResponseFilter.structureDepth = aStructureDepth;
    iResponseFilter.metadataDepth = aMetaDataDepth;
    iResponseFilter.metadataPerLevel = aMetaDataPerLevel;
    
    iResponseFilter.includedElements.Reset();
    for (TInt i = 0; i < aIncludedElements.MdcaCount(); ++i) 
        {
        TPtrC16 ptr = aIncludedElements.MdcaPoint(i);
        if ( ptr.Length() > 0) 
            {
            TXmlEngString element;
            element.SetL(ptr);
            iResponseFilter.includedElements.Append( element );
            }
        }

    iResponseFilter.excludedElements.Reset();
    for (TInt i = 0; i < aExcludedElements.MdcaCount(); ++i) 
        {
        TPtrC16 ptr = aExcludedElements.MdcaPoint(i);
        if ( ptr.Length() > 0) 
            {
            TXmlEngString element;
            element.SetL(ptr);
            iResponseFilter.excludedElements.Append( element );
            }
        }
        
    iResponseFilterEnabled = ETrue;
    }

// generates the dom nodes
HBufC8* CNcdRequestBrowseSearch::CreateRequestL()
    {
    DLTRACEIN((_L("entry")));
    if ( iDoSearch ) 
        iRequestElement = NcdProtocolUtils::NewElementL(iDocument, KTagSearch);
    else 
        iRequestElement = NcdProtocolUtils::NewElementL(iDocument, KTagBrowse);
        
    if (iConfiguration)
        iConfiguration->SetNamespacePrefixL( KAttrCdpNamespacePrefix );

    // entity
    for (TInt i = 0; i < iEntities.Count(); ++i)
        {
        // create remote entity elements
        TNcdRequestRemoteEntity entity = iEntities[i];
        TXmlEngElement entityElement = 
            NcdProtocolUtils::NewElementL( 
                iDocument, iRequestElement, KTagEntity );

        NcdProtocolUtils::NewAttributeL( 
            entityElement, KAttrId, entity.id);

        if (entity.timeStamp.NotNull())
            NcdProtocolUtils::NewAttributeL( 
                entityElement, KAttrTimeStamp, entity.timeStamp);

        if (entity.includeMetaData != EValueNotSet) 
            {
            NcdProtocolUtils::NewNcdBoolAttributeL( 
                entityElement, KAttrIncludeMetaData, 
                entity.includeMetaData);
            }
        }
    
    // responseFilter
    if (iResponseFilterEnabled)
        {
        // create element for response filter
        TXmlEngElement responseFilter = NcdProtocolUtils::NewElementL( 
            iDocument, iRequestElement, KTagResponseFilter );
        NcdProtocolUtils::NewAttributeL( 
            responseFilter, KAttrPageSize, iResponseFilter.pageSize );
        NcdProtocolUtils::NewAttributeL( 
            responseFilter, KAttrPageStart, iResponseFilter.pageStart );
        NcdProtocolUtils::NewAttributeL( 
            responseFilter, KAttrStructureDepth, 
            iResponseFilter.structureDepth );
        NcdProtocolUtils::NewAttributeL( 
            responseFilter, KAttrMetadataDepth, 
            iResponseFilter.metadataDepth );
        NcdProtocolUtils::NewAttributeL( 
            responseFilter, KAttrMetadataPerLevel, 
            iResponseFilter.metadataPerLevel );

        TBool inc = EFalse;
        for ( TInt i = 0; i < iResponseFilter.includedElements.Count(); ++i )
            {
            TXmlEngElement includeElement = NcdProtocolUtils::NewElementL( 
                iDocument, responseFilter, KTagIncludeElement );
            NcdProtocolUtils::NewAttributeL( 
                includeElement, KAttrName, iResponseFilter.includedElements[i] );
            inc = ETrue;
            }
        for ( TInt i = 0; !inc && i < iResponseFilter.excludedElements.Count(); ++i )
            {
            TXmlEngElement excludeElement = NcdProtocolUtils::NewElementL( 
                iDocument, responseFilter, KTagExcludeElement );
            NcdProtocolUtils::NewAttributeL( 
                excludeElement, KAttrName, iResponseFilter.excludedElements[i] );
            }

        }

    
    // entityFilter
    if (iEntityFilterEnabled) 
        {
        // create element for entity filter
        TXmlEngElement entityFilter = NcdProtocolUtils::NewElementL( 
                iDocument, iRequestElement, KTagEntityFilter );

        NcdProtocolUtils::NewNcdBoolAttributeL( 
            entityFilter, KAttrSubscribableContent, 
            iEntityFilter.subscribableContent );
        NcdProtocolUtils::NewNcdBoolAttributeL( 
            entityFilter, KAttrFreeContent, iEntityFilter.freeContent );

        if ( iEntityFilter.keywords.Count() > 0 ) 
            {
            TXmlEngElement keywords = NcdProtocolUtils::NewElementL( 
                    iDocument, entityFilter, KTagKeywords );
            for ( TInt i = 0; i < iEntityFilter.keywords.Count(); ++i ) 
                {
                TNcdRequestFilter filter = iEntityFilter.keywords[i];
                TXmlEngElement keyword = 
                    NcdProtocolUtils::NewElementL( 
                        iDocument, keywords, KTagKeyword );
                keyword.SetTextL(filter.text.PtrC8());
                NcdProtocolUtils::NewNcdBoolAttributeL( 
                    keyword, KAttrExclude, filter.exclude );
                }
            }
                        
        if ( iEntityFilter.contentPurposes.Count() > 0 ) 
            {
            TXmlEngElement contentPurposes = NcdProtocolUtils::NewElementL( 
                    iDocument, entityFilter, KTagContentPurposes );
            for ( TInt i = 0; i < iEntityFilter.contentPurposes.Count(); ++i ) 
                {
                TNcdRequestFilter filter = iEntityFilter.contentPurposes[i];
                TXmlEngElement contentPurpose = NcdProtocolUtils::NewElementL( 
                    iDocument, contentPurposes, KTagContentPurpose );
                contentPurpose.SetTextL(filter.text.PtrC8());
                NcdProtocolUtils::NewNcdBoolAttributeL( 
                    contentPurpose, KAttrExclude, filter.exclude );
                }
            }
        }

    DLTRACEOUT((_L("exit")));
    
    // base class generates the complete request
    return CNcdRequestBase::CreateRequestL();
    }
