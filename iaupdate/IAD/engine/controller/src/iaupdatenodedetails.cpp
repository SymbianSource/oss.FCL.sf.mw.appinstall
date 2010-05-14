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

#include <ncdnodemetadata.h>
#include <ncdutils.h>

#include "iaupdatenodedetails.h"
#include "iaupdatenodedependencyimpl.h"
#include "iaupdateplatformdependency.h"
#include "iaupdateutils.h"
#include "iaupdatedependencyxmlparser.h"
#include "iaupdateprotocolconsts.h"

#include "iaupdatedebug.h"


CIAUpdateNodeDetails* CIAUpdateNodeDetails::NewL( const MNcdNodeMetadata* aData )
    {
    CIAUpdateNodeDetails* self =
         CIAUpdateNodeDetails::NewLC( aData );
    CleanupStack::Pop( self );
    return self;        
    }

CIAUpdateNodeDetails* CIAUpdateNodeDetails::NewLC( const MNcdNodeMetadata* aData )
    {
    CIAUpdateNodeDetails* self =
        new( ELeave )  CIAUpdateNodeDetails();
    CleanupStack::PushL( self );
    self->ConstructL( aData );
    return self;        
    }


CIAUpdateNodeDetails::~CIAUpdateNodeDetails()
    {
    ClearAll();
    delete iPlatformDependency;
    }


CIAUpdateNodeDetails::CIAUpdateNodeDetails()
: CBase(),
  iType( MIAUpdateNode::EPackageTypeSA ),
  iImportance( MIAUpdateBaseNode::ENormal )
    {
    
    }
    
void CIAUpdateNodeDetails::ConstructL( const MNcdNodeMetadata* aData )
    {
    iPlatformDependency = 
        CIAUpdatePlatformDependency::NewL();
    SetDetailsL( aData );
    }


void CIAUpdateNodeDetails::SetDetailsL( const MNcdNodeMetadata* aData )
    {
    ClearAll();
    
    // Get information from metadata.
    // Notice, ownership of the aData is not transferred here.
    if( aData )
        {
        // Get the reference to the key value pair array
        // that contains all the details of the metadata.
        const RPointerArray< CNcdKeyValuePair >& details( aData->Details() );
        
        for( TInt i = 0; i < details.Count(); ++i )
            {
            // Get the first key value pair from the list.
            CNcdKeyValuePair* pair( details[ i ] );
            
            if( pair->Key() == IAUpdateProtocolConsts::KImportanceKey() )
                {
                // The pair describes the importance of the node.
                // Check and set the correct importance.
                if( pair->Value().CompareF( 
                        IAUpdateProtocolConsts::KImportanceMandatory() ) == 0 )
                    {
                    iImportance = MIAUpdateBaseNode::EMandatory;
                    }
                else if( pair->Value().CompareF( 
                            IAUpdateProtocolConsts::KImportanceCritical() ) == 0 )
                    {
                    iImportance = MIAUpdateBaseNode::ECritical;
                    }
                else if( pair->Value().CompareF( 
                            IAUpdateProtocolConsts::KImportanceRecommended() ) == 0 )
                    {
                    iImportance = MIAUpdateBaseNode::ERecommended;
                    }
                else if( pair->Value().CompareF( 
                            IAUpdateProtocolConsts::KImportanceHidden() ) == 0 )
                    {
                    iImportance = MIAUpdateBaseNode::EHidden;
                    }
                else
                    {
                    iImportance = MIAUpdateBaseNode::ENormal;
                    }
                }
            else if( pair->Key() == IAUpdateProtocolConsts::KPackageTypeKey() )
                {
                // The pair describes the content package type.
                // Check and set the correct type.
                if( pair->Value().CompareF( IAUpdateProtocolConsts::KPackageTypeSP() ) == 0 )
                    {
                    iType = MIAUpdateNode::EPackageTypeSP;
                    }
                else if( pair->Value().CompareF( 
                            IAUpdateProtocolConsts::KPackageTypePU() ) == 0 )
                    {
                    iType = MIAUpdateNode::EPackageTypePU;
                    }
                else
                    {
                    iType = MIAUpdateNode::EPackageTypeSA;
                    }
                }
            else if ( pair->Key() == IAUpdateProtocolConsts::KSearchCriteriaKey() )
                {
                // The pair describes the search criteria.
                // Get the criteria.
                delete iSearchCriteria;
                iSearchCriteria = NULL;
                iSearchCriteria = pair->Value().AllocL();
                }
            else if ( pair->Key() == IAUpdateProtocolConsts::KDependencyKey() )
                {
                // Create the descriptor for the dependencies data.
                // In order to make the data to be inside one root XML element,
                // insert required XML element tags to the beginning and to the end.
                const TDesC& dependenciesXmlData( pair->Value() );
                HBufC* data( HBufC::NewLC( 
                                IAUpdateProtocolConsts::KDependenciesPrefix().Length()
                                + dependenciesXmlData.Length() 
                                + IAUpdateProtocolConsts::KDependenciesPostfix().Length() ) );
                TPtr ptrData( data->Des() );
                ptrData.Copy( IAUpdateProtocolConsts::KDependenciesPrefix() );
                ptrData.Append( dependenciesXmlData );
                ptrData.Append( IAUpdateProtocolConsts::KDependenciesPostfix() ); 

                // Create parser that will insert dependency information to the
                // given parameter objects.
                // Notice, that ClearAll() was called in the beginning of this function. 
                // So, now dependency array and platform dependency contain their defaults.
                CIAUpdateDependencyXmlParser* parser(
                    CIAUpdateDependencyXmlParser::NewLC( iDependencies, 
                                                         *iPlatformDependency ) );
                parser->ParseL( ptrData );
                CleanupStack::PopAndDestroy( parser );

                CleanupStack::PopAndDestroy( data );
                }
            else if ( pair->Key() == IAUpdateProtocolConsts::KFirmwareVersion1Key )
                {
                // The pair describes the firmware version 1.
                // Get the value.
                delete iFwVersion1;
                iFwVersion1 = NULL;
                iFwVersion1 = pair->Value().AllocL();
                }
            else if ( pair->Key() == IAUpdateProtocolConsts::KFirmwareVersion2Key )
                {
                // The pair describes the firmware version 2.
                // Get the value.
                delete iFwVersion2;
                iFwVersion2 = NULL;
                iFwVersion2 = pair->Value().AllocL();                
                }
            else if ( pair->Key() == IAUpdateProtocolConsts::KFirmwareVersion3Key )
                {
                // The pair describes the firmware version 3.
                // Get the value.
                delete iFwVersion3;
                iFwVersion3 = NULL;
                iFwVersion3 = pair->Value().AllocL();                
                }
            else if ( pair->Key ()== IAUpdateProtocolConsts::KRebootAfterInstallKey )
                {
                if( pair->Value() == IAUpdateProtocolConsts::KRebootAfterInstallNeeded )
                    {
                    iRebootAfterInstall = ETrue;
                    }
                }
            }        
        }    
    }
    
    
MIAUpdateNode::TPackageType CIAUpdateNodeDetails::ContentType() const
    {
    return iType;
    }


MIAUpdateBaseNode::TImportance CIAUpdateNodeDetails::Importance() const
    {
    return iImportance;
    }

void CIAUpdateNodeDetails::SetImportance( MIAUpdateBaseNode::TImportance aImportance )
    {
    iImportance = aImportance;
    }
    
const TDesC& CIAUpdateNodeDetails::SearchCriteria() const
    {
    if ( iSearchCriteria )
        {
        return *iSearchCriteria;
        }
    else
        {
        return KNullDesC();
        }
    }


TBool CIAUpdateNodeDetails::EmbededDegrades() const
    {
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeDetails::EmbededDegrades() begin");
    
    for ( TInt i = 0; i < iDependencies.Count(); ++i )
        {
        CIAUpdateNodeDependency* dependency( iDependencies[ i ] );
        if ( dependency->IsEmbedded() )
            {
            // Because the dependency is embedded 
            // we have to make sure that embedding does not
            // create downgrading.
            TBool installed( EFalse );
            TIAUpdateVersion installedVersion;
            TRAP_IGNORE( 
                installed = 
                    IAUpdateUtils::IsAppInstalledL( dependency->Uid(), 
                                                    installedVersion ) );    
            if ( installed && installedVersion > dependency->VersionRoof() )
                {
                IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeDetails::EmbededDegrades() end: ETrue");
                return ETrue;
                }   
            }
        }
        
    IAUPDATE_TRACE("[IAUPDATE] CIAUpdateNodeDetails::EmbededDegrades() end: EFalse");
    
    return EFalse;
    }


void CIAUpdateNodeDetails::GetDependenciesL( 
    RPointerArray< CIAUpdateNodeDependency >& aDependencies ) const
    {
    aDependencies.ReserveL( iDependencies.Count() );
    for( TInt i = 0; i < iDependencies.Count(); ++i )
        {
        aDependencies.AppendL( iDependencies[ i ] );
        }    
    }


CIAUpdatePlatformDependency& CIAUpdateNodeDetails::PlatformDependency() const
    {
    return *iPlatformDependency;
    }


const TDesC& CIAUpdateNodeDetails::FwVersion1() const
    {
    if ( iFwVersion1 )
        {
        return *iFwVersion1;
        }
    else
        {
        return KNullDesC();
        }
    }


const TDesC& CIAUpdateNodeDetails::FwVersion2() const
    {
    if ( iFwVersion2 )
        {
        return *iFwVersion2;
        }
    else
        {
        return KNullDesC();
        }
    }


const TDesC& CIAUpdateNodeDetails::FwVersion3() const
    {
    if ( iFwVersion3 )
        {
        return *iFwVersion3;
        }
    else
        {
        return KNullDesC();
        }
    }


TBool CIAUpdateNodeDetails::RebootAfterInstall() const
    {
    return iRebootAfterInstall;
    }


void CIAUpdateNodeDetails::ClearAll()
    {
    delete iSearchCriteria;
    iSearchCriteria = NULL;

    delete iFwVersion1;
    iFwVersion1 = NULL;

    delete iFwVersion2;
    iFwVersion2 = NULL;

    delete iFwVersion3;
    iFwVersion3 = NULL;

    iDependencies.ResetAndDestroy();    

    iPlatformDependency->Reset();
    
    iRebootAfterInstall = EFalse;
    }
