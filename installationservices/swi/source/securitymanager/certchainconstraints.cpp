/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Developer mode certificate constraints implementation.
*
*/


/**
 @file 
 @released
 @internalTechnology
*/

#include "certchainconstraints.h"
#include "x509constraintext.h"
#include <x509certext.h>
//#include "log.h"

using namespace Swi;

//
//  CCertChainConstraints methods.
//

EXPORT_C CCertChainConstraints* CCertChainConstraints::NewL(RPointerArray<CPKIXCertChainBase>& aValidCerts)
	{
	CCertChainConstraints* self = CCertChainConstraints::NewLC(aValidCerts);
	CleanupStack::Pop(self);
	return self;	
	}
	
EXPORT_C CCertChainConstraints* CCertChainConstraints::NewLC(RPointerArray<CPKIXCertChainBase>& aValidCerts)
	{
	CCertChainConstraints* self = new(ELeave) CCertChainConstraints();
	CleanupStack::PushL(self);
	self->ConstructL(aValidCerts);
	return self;	
	}

EXPORT_C CCertChainConstraints* CCertChainConstraints::NewL()
	{
	CCertChainConstraints* self = new(ELeave) CCertChainConstraints();
	return self;	
	}
	
CCertChainConstraints::~CCertChainConstraints()
	{
	//Release the resources
	iValidSIDs.Close();
	iValidVIDs.Close();
	iValidDeviceIDs.ResetAndDestroy();
	}
	
EXPORT_C TBool CCertChainConstraints::SIDIsValid(TSecureId aRequestSID) const
	{
	TBool ret=ETrue;
	if (iSIDsAreConstrained && (aRequestSID.iId & 0x80000000)==0 && KErrNotFound==iValidSIDs.Find(aRequestSID))
		{
		ret=EFalse;			
		}
	return ret;
	}
	
EXPORT_C TBool CCertChainConstraints::VIDIsValid(TVendorId aRequestVID) const
	{
	TBool ret=ETrue;
	if (iVIDsAreConstrained && aRequestVID!=0 && KErrNotFound==iValidVIDs.Find(aRequestVID))
		{
		ret=EFalse;		
		}
	return ret;	
	}

EXPORT_C TBool CCertChainConstraints::CapabilitiesAreValid(TCapabilitySet& aRequestCapabilities) const
	{
	return iValidCapabilities.HasCapabilities(aRequestCapabilities);
	}
	
EXPORT_C TBool CCertChainConstraints::DeviceIDIsValid(const HBufC* aRequestDeviceID) const
	{
	TBool ret=EFalse;
	if (iDeviceIDsAreConstrained)
		{
		TInt deviceIDCount=iValidDeviceIDs.Count();
		//Check if request Device ID is in the valid device ID list
		for(TInt i=0; i<deviceIDCount; i++)
			{
			if (iValidDeviceIDs[i]->CompareF(*aRequestDeviceID)==0)
				{
				ret=ETrue;
				break;
				}
			}		
		}
	else
		{
		//No constaints on Device ID at all
		ret=ETrue;				
		}
	return ret;		
	}

EXPORT_C TBool CCertChainConstraints::SIDsAreConstrained() const
	{
	return iSIDsAreConstrained;
	}
	
EXPORT_C TBool CCertChainConstraints::VIDsAreConstrained() const
	{
	return iVIDsAreConstrained; 
	}
	
EXPORT_C TBool CCertChainConstraints::DeviceIDsAreConstrained() const
	{
	return iDeviceIDsAreConstrained; 
	}
	
EXPORT_C TBool CCertChainConstraints::CapabilitiesAreConstrained() const
	{
	return iCapabilitiesAreConstrained;		
	}
	
EXPORT_C const TCapabilitySet& CCertChainConstraints::ValidCapabilities() const
	{
	return iValidCapabilities;
	}

EXPORT_C void CCertChainConstraints::SetValidCapabilities(const TCapabilitySet& aValidCapabilities)
	{
	iValidCapabilities=aValidCapabilities;
	}
	
CCertChainConstraints::CCertChainConstraints()
	{
	//Pre-initialise the valid Capability to all capability supported	
	iValidCapabilities.SetAllSupported();
	}

void CCertChainConstraints::ConstructL(RPointerArray<CPKIXCertChainBase>& aValidCerts)
	{
	//Get the Cert Chain count
	TInt certChainCount=aValidCerts.Count();
	
	//Go through the certificate chains
	for(TInt i=0; i<certChainCount; i++)
		{
		TInt certCount=aValidCerts[i]->Count();
		//Go through the certificate in one certificate chain
		for (TInt j=0; j<certCount; j++)
			{
			const CX509Certificate& validCert=aValidCerts[i]->Cert(j);
			
			//Retrieve the DeviceIDs and build the list
			RetrieveExtensionDeviceIDListL(validCert);
			
			//Retrieve the Capabilities and build capability constraints
			RetrieveExtensionCapabilitySetL(validCert);
			
			//Retrieve the SIDs and build the list
			RetrieveExtensionSIDListL(validCert);
			
			//Retrieve the VIDs and build the list
			RetrieveExtensionVIDListL(validCert);
			}
		}
	}

void CCertChainConstraints::RetrieveExtensionCapabilitySetL(const CX509Certificate& aCert)
	{
	const CX509CertExtension* certExt = aCert.Extension(KCapabilitiesConstraint);
	if (certExt)
		{
        CX509CapabilitySetExt* capSetExt=CX509CapabilitySetExt::NewL(certExt->Data());
		iValidCapabilities.Intersection(capSetExt->CapabilitySet());
		delete capSetExt;
		iCapabilitiesAreConstrained=ETrue;
		}
	}

TBool CompareInstance(const HBufC& aFirst, const HBufC& aSecond)
	{
	return (aFirst.CompareF(aSecond) == 0);
	}

void CCertChainConstraints::RetrieveExtensionDeviceIDListL(const CX509Certificate& aCert)
	{
	if (!iDeviceIDsAreConstrained || (iDeviceIDsAreConstrained && iValidDeviceIDs.Count()>0))
		{
		const CX509CertExtension* certExt = aCert.Extension(KDeviceIdListConstraint);
		if (certExt)
			{
			CX509Utf8StringListExt* deviceIdExt=CX509Utf8StringListExt::NewLC(certExt->Data());
			const RPointerArray<HBufC>& buf=deviceIdExt->StringArray();
			// iValidDeviceIDs intersect the constrained Device ID set in the certificate
			if (!iDeviceIDsAreConstrained)
				{
				TInt count=buf.Count();
				for (TInt i=0;i<count;i++)
					{
					HBufC* temp=buf[i]->AllocLC();
					iValidDeviceIDs.AppendL(temp);
					CleanupStack::Pop(temp);
					}
				iDeviceIDsAreConstrained=ETrue;					
				}
			else
				{
				for (TInt k=iValidDeviceIDs.Count()-1;k>=0;k--)
					{
					if(KErrNotFound==buf.Find(iValidDeviceIDs[k],TIdentityRelation<HBufC>(CompareInstance)))
						{
						HBufC* temp=iValidDeviceIDs[k];
						iValidDeviceIDs.Remove(k);
						delete temp;
						}					
					}
				}
			CleanupStack::PopAndDestroy(deviceIdExt);
			}
		}	
	}

void CCertChainConstraints::RetrieveExtensionSIDListL(const CX509Certificate& aCert)
	{
	if (!iSIDsAreConstrained || (iSIDsAreConstrained &&  iValidSIDs.Count()>0))
		{
		const CX509CertExtension* certExt=aCert.Extension(KSidListConstraint);
		if (certExt)
			{
			CX509IntListExt* intExt=CX509IntListExt::NewLC(certExt->Data());
			const RArray<TInt>& sidList=intExt->IntArray();
			// iValidSIDs intersect the constrained sid set in the certificate
			if (!iSIDsAreConstrained)
				{
				TInt count=sidList.Count();
				for (TInt i=0;i<count;i++)
					{
					iValidSIDs.AppendL(TSecureId(sidList[i]));
					}
				iSIDsAreConstrained=ETrue;						
				}
			else
				{
				for (TInt k=iValidSIDs.Count()-1;k>=0;k--)
					{
					if (sidList.Find(iValidSIDs[k].iId)==KErrNotFound)
						{
						iValidSIDs.Remove(k);
						}						
					}										
				}
			CleanupStack::PopAndDestroy(intExt);
			}				
		}
	}

void CCertChainConstraints::RetrieveExtensionVIDListL(const CX509Certificate& aCert)
	{
	if (!iVIDsAreConstrained || (iVIDsAreConstrained &&  iValidVIDs.Count()>0))
		{
		const CX509CertExtension* certExt=aCert.Extension(KVidListConstraint);
		if (certExt)
			{
			CX509IntListExt* intExt=CX509IntListExt::NewLC(certExt->Data());
			const RArray<TInt>& vidList=intExt->IntArray();
			// iValidVIDs intersect the constrained vid set in the certificate
			if (!iVIDsAreConstrained)
				{
				TInt count=vidList.Count();
				for (TInt i=0;i<count;i++)
					{
					iValidVIDs.AppendL(TVendorId(vidList[i]));
					}
				iVIDsAreConstrained=ETrue;
				}
			else
				{
				for (TInt k=iValidVIDs.Count()-1;k>=0;k--)
					{
					if (vidList.Find(iValidVIDs[k].iId)==KErrNotFound)
						{
						iValidVIDs.Remove(k);
						}						
					}					
				}
			CleanupStack::PopAndDestroy(intExt);
			}			
		}
	}

