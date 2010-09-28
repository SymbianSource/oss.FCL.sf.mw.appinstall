/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "swtypereginfoparser.h"
#include <scs/cleanuputils.h>
#include <xml/parser.h>
#include <xml/parserfeature.h>
#include <swi/sisinstallerrors.h>

using namespace Swi;

namespace
	{
	_LIT8(KXmlMimeType, "text/xml");
	
	_LIT8(KNodeSoftwareTypeRegistrationData, "LayeredExecutionEnvironments");
	_LIT8(KNodeSoftwareType, "SoftwareType");
	_LIT8(KNodeLocalizedName, "LocalizedName");
	_LIT8(KNodeMimeType, "MimeType");
	_LIT8(KNodeSifPluginUid, "SifPluginUid");
    _LIT8(KNodeCustomAccess, "CustomAccess");
    _LIT8(KNodeSecureId, "SecureId");
    _LIT8(KNodeAccessMode, "AccessMode");
    _LIT8(KNodeLauncherExecutable, "LauncherExecutable");
    
	_LIT8(KAttrSoftwareTypeName, "Name");
	_LIT8(KAttrLanguage, "Language");

// ##########################################################################################

	TUint Str2IntL(const TDesC8& aStr, TRadix aRadix = EDecimal)
		{
		TLex8 lex(aStr);
		TUint i(0);
		if (lex.Val(i, aRadix) != KErrNone)
			{
			User::Leave(KErrInvalidSoftwareTypeRegistrationFile);
			}
		return i;
		}

	TUint Str2IntL(const TDesC& aStr, TRadix aRadix = EDecimal)
		{
		TLex lex(aStr);
		TUint i(0);
		if (lex.Val(i, aRadix) != KErrNone)
			{
			User::Leave(KErrInvalidSoftwareTypeRegistrationFile);
			}
		return i;
		}

	TUid Str2UidL(const TDesC8& aStr)
		{
		TUid uid = TUid::Uid(Str2IntL(aStr, EHex));
		return uid;
		}

	HBufC* AttributeLC(const Xml::RAttributeArray& aAttributes, const TDesC8& aAttrName)
		{
		HBufC* attrValue = NULL;

		TInt numAttrs = aAttributes.Count();
		for(TInt i=0; i<numAttrs; ++i)
			{
			const Xml::RAttribute& attribute = aAttributes[i];
			const Xml::RTagInfo& nameInfo = attribute.Attribute();
			const TDesC8& localName8 = nameInfo.LocalName().DesC();
			if (aAttrName == localName8)
				{
				const TDesC8& value8 = attribute.Value().DesC();
				attrValue = ConvertBufferTo16bitL(value8);
				CleanupStack::PushL(attrValue);
				break;
				}
			}
		
		if (attrValue == NULL)
			{
			User::Leave(KErrInvalidSoftwareTypeRegistrationFile);
			}
		
		return attrValue;
		}
	}

// ##########################################################################################

CSoftwareTypeRegInfoParser* CSoftwareTypeRegInfoParser::NewL()
	{
	CSoftwareTypeRegInfoParser* self = new (ELeave) CSoftwareTypeRegInfoParser();
	CleanupStack::PushL(self);
	self->iNodes.AppendL(ENodeNone);
	CleanupStack::Pop();
	return self;
	}

CSoftwareTypeRegInfoParser::CSoftwareTypeRegInfoParser()
	{
	}

CSoftwareTypeRegInfoParser::~CSoftwareTypeRegInfoParser()
	{
	delete iContentChunks;
	iNodes.Close();
	}

void CSoftwareTypeRegInfoParser::ParseL(const TDesC8& aDocument, RPointerArray<Usif::CSoftwareTypeRegInfo>& aSwTypeRegInfoArray)
	{
	iSwTypeRegInfoArray = &aSwTypeRegInfoArray;
	
	Xml::CParser* parser = Xml::CParser::NewLC(KXmlMimeType, *this);
	parser->EnableFeature(Xml::ESendFullContentInOneChunk); // Use ESendFullContentInOneChunk if the parser supports it
	parser->ParseL(aDocument);
	parser->ParseEndL();
	CleanupStack::PopAndDestroy(parser);

	if (iContentChunks != NULL)
		{
		iContentChunks->Des().Zero();
		}
	iNodes.Reset();
	}

void CSoftwareTypeRegInfoParser::OnStartDocumentL(const Xml::RDocumentParameters& /*aDocParam*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}

void CSoftwareTypeRegInfoParser::OnEndDocumentL(TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}

void CSoftwareTypeRegInfoParser::PushNodeL(TXmlNode aNew, TXmlNode aExpectedParent)
	{
	ASSERT(iNodes.Count() > 0);

	const TXmlNode parent = iNodes[iNodes.Count()-1];
	if (aExpectedParent != parent)
		{
		User::Leave(KErrInvalidSoftwareTypeRegistrationFile);
		}
	iNodes.AppendL(aNew);
	}

void CSoftwareTypeRegInfoParser::OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	ASSERT(iSwTypeRegInfoArray != NULL);

	const TDesC8& localName8 = aElement.LocalName().DesC();
	const TXmlNode node = ElementNameToNode(localName8);

	switch (node)
		{
		case ENodeSoftwareTypeRegistrationData:
			PushNodeL(ENodeSoftwareTypeRegistrationData, ENodeNone);
			break;
		
		case ENodeSoftwareType:
			{
			PushNodeL(ENodeSoftwareType, ENodeSoftwareTypeRegistrationData);
			HBufC* softwareTypeName = AttributeLC(aAttributes, KAttrSoftwareTypeName);
			Usif::CSoftwareTypeRegInfo* regInfo = Usif::CSoftwareTypeRegInfo::NewL(*softwareTypeName);
			CleanupStack::PushL(regInfo);
			iSwTypeRegInfoArray->AppendL(regInfo);
			CleanupStack::Pop();
			CleanupStack::PopAndDestroy(softwareTypeName);
			}
			break;
		
		case ENodeLocalizedName:
			{
			ASSERT(iSwTypeRegInfoArray->Count() > 0);
			PushNodeL(ENodeLocalizedName, ENodeSoftwareType);
			HBufC* language = AttributeLC(aAttributes, KAttrLanguage);
			iLocalizedNameLanguage = static_cast<TLanguage>(Str2IntL(*language));
			CleanupStack::PopAndDestroy(language);
			}
			break;
		
		case ENodeMimeType:
			ASSERT(iSwTypeRegInfoArray->Count() > 0);
			PushNodeL(ENodeMimeType, ENodeSoftwareType);
			break;
		
		case ENodeSifPluginUid:
			ASSERT(iSwTypeRegInfoArray->Count() > 0);
			PushNodeL(ENodeSifPluginUid, ENodeSoftwareType);
			break;
			
		case ENodeCustomAccess:
		    {
		    ASSERT(iSwTypeRegInfoArray->Count() > 0);
		    PushNodeL(ENodeCustomAccess, ENodeSoftwareType);
		    HBufC* secureId = AttributeLC(aAttributes, KNodeSecureId);
		    secureId->Des().TrimAll();
		    iSecureId = TUid::Uid(Str2IntL(*secureId, EHex));
		    CleanupStack::PopAndDestroy(secureId);
		    
		    HBufC* accessMode = AttributeLC(aAttributes, KNodeAccessMode);
		    iAccessMode = static_cast<TInt>(Str2IntL(*accessMode));
		    CleanupStack::PopAndDestroy(accessMode);
		    }
		    break;
		   
		case ENodeLauncherExecutable:
		    {
		    ASSERT(iSwTypeRegInfoArray->Count() > 0);
		    PushNodeL(ENodeLauncherExecutable, ENodeSoftwareType);
		    }
		    break;

		default:
			User::Leave(KErrInvalidSoftwareTypeRegistrationFile);
		}
	
	if (iContentChunks != NULL)
		{
		iContentChunks->Des().Zero();
		}
	}

void CSoftwareTypeRegInfoParser::OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode)
	{
	ASSERT(iSwTypeRegInfoArray != NULL);
	User::LeaveIfError(aErrorCode);

	const TInt lastIdx = iNodes.Count()-1;
	const TXmlNode node = ElementNameToNode(aElement.LocalName().DesC());
	const TXmlNode lastNode = iNodes[lastIdx];
	if (node != lastNode)
		{
		User::Leave(KErrInvalidSoftwareTypeRegistrationFile);
		}
	iNodes.Remove(lastIdx);

	RPointerArray<Usif::CSoftwareTypeRegInfo>& infoArray = *iSwTypeRegInfoArray;
	switch (node)
		{
		case ENodeLocalizedName:
			{
			ASSERT(iSwTypeRegInfoArray->Count() > 0);
			Usif::CSoftwareTypeRegInfo& regInfo = *infoArray[infoArray.Count()-1];
			HBufC* name = ConvertBufferTo16bitL(*iContentChunks);
			CleanupStack::PushL(name);
			name->Des().TrimAll();
			regInfo.SetLocalizedSoftwareTypeNameL(iLocalizedNameLanguage, *name);
			CleanupStack::PopAndDestroy(name);
			}
			break;

		case ENodeMimeType:
			{
			ASSERT(iSwTypeRegInfoArray->Count() > 0);
			Usif::CSoftwareTypeRegInfo& regInfo = *infoArray[infoArray.Count()-1];
			HBufC* mimeType = ConvertBufferTo16bitL(*iContentChunks);
			CleanupStack::PushL(mimeType);
			mimeType->Des().TrimAll();
			regInfo.SetMimeTypeL(*mimeType);
			CleanupStack::PopAndDestroy(mimeType);
			}
			break;
		
		case ENodeSifPluginUid:
			{
			ASSERT(iSwTypeRegInfoArray->Count() > 0);
			iContentChunks->Des().TrimAll();
			Usif::CSoftwareTypeRegInfo& regInfo = *infoArray[infoArray.Count()-1];
			regInfo.SetSifPluginUid(Str2UidL(*iContentChunks));
			}
			break;

		case ENodeCustomAccess:
		    {
		    ASSERT(iSwTypeRegInfoArray->Count() > 0);
		    iContentChunks->Des().TrimAll();
		    Usif::CSoftwareTypeRegInfo& regInfo = *infoArray[infoArray.Count()-1];
		    regInfo.SetCustomAccessL(iSecureId, static_cast<Usif::TAccessMode>(iAccessMode));
		    }
		    break;
		    
		case ENodeLauncherExecutable:
		    {
		    ASSERT(iSwTypeRegInfoArray->Count() > 0);
		    Usif::CSoftwareTypeRegInfo& regInfo = *infoArray[infoArray.Count()-1];
		    HBufC* launcherExecutable = ConvertBufferTo16bitL(*iContentChunks);
		    CleanupStack::PushL(launcherExecutable);
		    launcherExecutable->Des().TrimAll();
		    regInfo.SetLauncherExecutableL(*launcherExecutable);
		    CleanupStack::PopAndDestroy(launcherExecutable);        
		    }
		    break;
		}
	
	if (iContentChunks != NULL)
		{
		iContentChunks->Des().Zero();
		}
	}

void CSoftwareTypeRegInfoParser::OnContentL(const TDesC8& aBytes, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	ASSERT(iNodes.Count() > 0);

	switch (iNodes[iNodes.Count()-1])
		{
		case ENodeSoftwareTypeRegistrationData:
		case ENodeSoftwareType:
			break;
		
		case ENodeLocalizedName:
		case ENodeMimeType:
		case ENodeSifPluginUid:
		case ENodeCustomAccess:  
		case ENodeLauncherExecutable:
			AddContentChunkL(aBytes);
			break;

		default:
			User::Leave(KErrInvalidSoftwareTypeRegistrationFile);
		}
	}

void CSoftwareTypeRegInfoParser::OnStartPrefixMappingL(const RString& /*aPrefix*/, const RString& /*aUri*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}

void CSoftwareTypeRegInfoParser::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}

void CSoftwareTypeRegInfoParser::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}

void CSoftwareTypeRegInfoParser::OnSkippedEntityL(const RString& /*aName*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}

void CSoftwareTypeRegInfoParser::OnProcessingInstructionL(const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}

void CSoftwareTypeRegInfoParser::OnError(TInt /*aErrorCode*/)
	{
	}

TAny* CSoftwareTypeRegInfoParser::GetExtendedInterface(const TInt32 /*aUid*/)
	{
	return NULL;
	}

CSoftwareTypeRegInfoParser::TXmlNode CSoftwareTypeRegInfoParser::ElementNameToNode(const TDesC8& aName) const
	{
	if (aName == KNodeLocalizedName)
		{
		return ENodeLocalizedName;
		}
	else if (aName == KNodeMimeType)
		{
		return ENodeMimeType;
		}
	else if (aName == KNodeSifPluginUid)
		{
		return ENodeSifPluginUid;
		}
	else if (aName == KNodeSoftwareType)
		{
		return ENodeSoftwareType;
		}
	else if (aName == KNodeSoftwareTypeRegistrationData)
		{
		return ENodeSoftwareTypeRegistrationData;
		}
	else if (aName == KNodeCustomAccess)
	    {
	    return ENodeCustomAccess;
	    }
	else if (aName == KNodeLauncherExecutable)
	    {
	    return ENodeLauncherExecutable;
	    }
	else
		{
		return ENodeNone;
		}
	}
	
void CSoftwareTypeRegInfoParser::AddContentChunkL(const TDesC8& aChunk)
	{
	if (iContentChunks == NULL)
		{
		iContentChunks = HBufC8::NewL(aChunk.Length());
		iContentChunks->Des().Copy(aChunk);
		}
	else
		{
		const TInt newLen = iContentChunks->Des().MaxLength() + aChunk.Length();
		iContentChunks = iContentChunks->ReAlloc(newLen);
		iContentChunks->Des().Append(aChunk);
		}
	}
