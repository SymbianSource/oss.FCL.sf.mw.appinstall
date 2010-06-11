/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/



#include <xml/documentparameters.h>
#include <xml/taginfo.h>
#include <xml/attribute.h>

#include "iaupdatexmlsubparser.h"


EXPORT_C CIAUpdateXmlSubParser::CIAUpdateXmlSubParser()
: CBase(),
  iAcceptData( ETrue )
    {
    
    }

    
EXPORT_C void CIAUpdateXmlSubParser::ConstructL( const TDesC8& aElementLocalName )
    {
    iElementLocalName = aElementLocalName.AllocL();
    }


EXPORT_C CIAUpdateXmlSubParser::~CIAUpdateXmlSubParser()
    {
    // Notice, this array also owns the iCurrentSubParser.
    // So, do not delete it twice. ResetAndDestroy here is enough.
    iSubParsers.ResetAndDestroy();

    delete iElementLocalName;
    }


EXPORT_C void CIAUpdateXmlSubParser::OnStartDocumentL( 
    const Xml::RDocumentParameters& aDocParam, 
    TInt aErrorCode )
    {
    iAcceptData = ETrue;
    iUnknownElementCounter = 0;
    iIsElementStarted = EFalse;
    iIsElementEnded = EFalse;
    for ( TInt i = 0; i < iSubParsers.Count(); ++i )
        {
        iSubParsers[ i ]->OnStartDocumentL( aDocParam, aErrorCode );
        }
    }
                                   
EXPORT_C void CIAUpdateXmlSubParser::OnEndDocumentL( TInt /*aErrorCode*/ )
    {
    // Nothing to do here.
    // Child class may add more functionality in its own implementation
    // if necessary.    
    }

EXPORT_C void CIAUpdateXmlSubParser::OnStartElementL( const Xml::RTagInfo& aElement, 
                                                      const Xml::RAttributeArray& aAttributes, 
                                                      TInt aErrorCode )
    {
    if ( CurrentSubParser() )
        {
        // Because current sub parser has been set, it is its responsibility to handle
        // the element.
        CurrentSubParser()->OnStartElementL( aElement, aAttributes, aErrorCode );
        }
    else if ( !IsElementStarted()
              && ElementTagInfoEquals( aElement ) )
        {
        // The starting element for this sub parser element was called.
        // Reset the flags just in case.
        // And, set the started flag to ETrue;
        iIsElementStarted = ETrue;
        iAcceptData = ETrue;
        iIsElementEnded = EFalse;
        iUnknownElementCounter = 0;
        }
    else if ( IsElementStarted()
              && AcceptData() )
        {
        // First time case has already been handled.
        // Because flags suggests that the handling of element is
        // job of the sub parser of this sub parser, try to check 
        // if we can delegate it to some sub parsers.
        // Also, set the AcceptData flag accordingly.
        // If sub parser is found then, then the job is its responsibility and
        // if sub parser is not found, then some unknown element is at hand.
        // So, in both cases set the flag to false.
        iAcceptData = EFalse;
        
        // Sub parser has not been set. So, check if we can find a subparser for the job.
        for ( TInt i = 0; i < iSubParsers.Count(); ++i )
            {
            if ( iSubParsers[ i ]->ElementTagInfoEquals( aElement ) )
                {
                // A correct sub parser was found because element tag matched
                // to the given element.
                iCurrentSubParser = iSubParsers[ i ];
                
                // Now, let the sub parser handle the element.
                CurrentSubParser()->OnStartElementL( aElement, aAttributes, aErrorCode );

                // No need to check sub parsers any more.
                // Notice, that in the scheme there will be only one element of a kind but
                // in action some elements may contain multiple instances of the same element.
                // But, this will be handled inside container element separately.
                break;
                }
            }

        if ( !iCurrentSubParser )
            {
            // Element was unknown.
            ++iUnknownElementCounter;
            }
        }
    else
        {
        // If we got some unknown element. 
        // Set the AcceptData flag to EFalse. 
        iAcceptData = EFalse;
        ++iUnknownElementCounter;
        }
    }
    
EXPORT_C void CIAUpdateXmlSubParser::OnEndElementL( const Xml::RTagInfo& aElement, 
                                                    TInt aErrorCode )
    {
    if ( CurrentSubParser() )
        {
        // Because current sub parser has been set
        // and it has not finished its job yet,
        // it is its responsibility to handle the element.
        CurrentSubParser()->OnEndElementL( aElement, aErrorCode );        

        if ( CurrentSubParser()->IsElementEnded() )
            {
            // Because current sub parser has finished its job.
            // This parser should not have any sub parser set any more.
            iCurrentSubParser = NULL;
            
            // Because the sub parser has finished it job,
            // this parser can now accept new sub elements if necessary.
            // Note, that this does not mean that this element would
            // finish next.
            iAcceptData = ETrue;
            }
        }
    else if( UnknownElementCounter() == 0 )
        {
        // This sub parser has finished its job.
        // Set accept flag to its default now because 
        // the element has been handled.
        iAcceptData = ETrue;
        iIsElementStarted = EFalse;
        iIsElementEnded = ETrue;
        }
    else if( UnknownElementCounter() == 1 )
        {
        --iUnknownElementCounter;
        // We have handled all the unknowns now.
        iAcceptData = ETrue;
        }
    else
        {
        // We got the unknown element end now for this sub parser.
        --iUnknownElementCounter;
        }
    }

EXPORT_C void CIAUpdateXmlSubParser::OnContentL( const TDesC8& aBytes, 
                                                 TInt aErrorCode )
    {
    if ( CurrentSubParser() )
        {
        // Because current sub parser has been set, it is its responsibility to handle
        // the element.
        CurrentSubParser()->OnContentL( aBytes, aErrorCode );
        }
    }
    
EXPORT_C void CIAUpdateXmlSubParser::OnStartPrefixMappingL( const RString& /*aPrefix*/, 
                                                            const RString& /*aUri*/, 
                                                            TInt /*aErrorCode*/ )
    {
    // Nothing to do here.
    // Child class may add more functionality in its own implementation
    // if necessary.    
    }
                                   
EXPORT_C void CIAUpdateXmlSubParser::OnEndPrefixMappingL( const RString& /*aPrefix*/, 
                                                          TInt /*aErrorCode*/ )
    {
    // Nothing to do here.
    // Child class may add more functionality in its own implementation
    // if necessary.    
    }
                                   
EXPORT_C void CIAUpdateXmlSubParser::OnIgnorableWhiteSpaceL( const TDesC8& /*aBytes*/, 
                                                             TInt /*aErrorCode*/ )
    {
    // Nothing to do here.
    // Child class may add more functionality in its own implementation
    // if necessary.    
    }

EXPORT_C void CIAUpdateXmlSubParser::OnSkippedEntityL( const RString& /*aName*/, 
                                                       TInt /*aErrorCode*/ )
    {
    // Nothing to do here.
    // Child class may add more functionality in its own implementation
    // if necessary.    
    }
                                   
EXPORT_C void CIAUpdateXmlSubParser::OnProcessingInstructionL( const TDesC8& /*aTarget*/, 
                                                               const TDesC8& /*aData*/, 
                                                               TInt /*aErrorCode*/ )
    {
    // Nothing to do here.
    // Child class may add more functionality in its own implementation
    // if necessary.    
    }
                                   
EXPORT_C void CIAUpdateXmlSubParser::OnError( TInt /*aErrorCode*/ )
    {
    // Nothing to do here.
    // Child class may add more functionality in its own implementation
    // if necessary.    
    }
                                   
EXPORT_C TAny* CIAUpdateXmlSubParser::GetExtendedInterface( const TInt32 /*aUid*/ )
    {
    // Nothing to do here.
    // Child class may add more functionality in its own implementation
    // if necessary.    
    return NULL;
    }


EXPORT_C const TDesC8& CIAUpdateXmlSubParser::LocalName() const
    {
    return *iElementLocalName;
    }
    
    
EXPORT_C CIAUpdateXmlSubParser* CIAUpdateXmlSubParser::CurrentSubParser() const
    {
    return iCurrentSubParser;    
    }


EXPORT_C RPointerArray< CIAUpdateXmlSubParser >& CIAUpdateXmlSubParser::SubParsers()
    {
    return iSubParsers;
    }


EXPORT_C TBool CIAUpdateXmlSubParser::ElementTagInfoEquals( const Xml::RTagInfo& aElement ) const
    {
    // Get the element local name from the given parameter.
    const TDesC8& element( aElement.LocalName().DesC() );
    
    // Now, chek if the given element has the same name with the element that this
    // parser is created for. Notice, that the element names are specified as case sensitive.
    // But, element values are not specified as case sensitive. 
    if ( element == LocalName() )
        {
        // The given element tag matched with the local name of this class object.
        return ETrue;
        }
    else
        {
        // The local names did not match.
        return EFalse;
        }
    }


EXPORT_C TBool CIAUpdateXmlSubParser::AcceptData() const
    {
    return iAcceptData;
    }


EXPORT_C TBool CIAUpdateXmlSubParser::IsElementStarted() const
    {
    return iIsElementStarted;
    }


EXPORT_C TBool CIAUpdateXmlSubParser::IsElementEnded() const
    {
    return iIsElementEnded;
    }


EXPORT_C TInt CIAUpdateXmlSubParser::UnknownElementCounter() const
    {
    return iUnknownElementCounter;
    }

