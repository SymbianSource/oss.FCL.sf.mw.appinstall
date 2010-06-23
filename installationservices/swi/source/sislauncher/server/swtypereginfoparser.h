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
* An XML parser for the CSoftwareTypeRegInfo class used for installation of a Layered Eexecution Environment.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SWTYPEREGINFOPARSER_H
#define SWTYPEREGINFOPARSER_H

#include "swtypereginfo.h"
#include <xml/contenthandler.h>

namespace Swi
	{

	class CSoftwareTypeRegInfoParser : public CBase, public Xml::MContentHandler
		{
	public:
		static CSoftwareTypeRegInfoParser* NewL();
		virtual ~CSoftwareTypeRegInfoParser();
		
		void ParseL(const TDesC8& aDocument, RPointerArray<Usif::CSoftwareTypeRegInfo>& aSwTypeRegInfoArray);

	private:
		// From MContentHandler
		virtual void OnStartDocumentL(const Xml::RDocumentParameters& aDocParam, TInt aErrorCode);
		virtual void OnEndDocumentL(TInt aErrorCode);
		virtual void OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, TInt aErrorCode);
		virtual void OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode);
		virtual void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
		virtual void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode);
		virtual void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
		virtual void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);
		virtual void OnSkippedEntityL(const RString& aName, TInt aErrorCode);
		virtual void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode);
		virtual void OnError(TInt aErrorCode);
		virtual TAny* GetExtendedInterface(const TInt32 aUid);

	private:
		CSoftwareTypeRegInfoParser();
		
		RPointerArray<Usif::CSoftwareTypeRegInfo>* iSwTypeRegInfoArray;
		HBufC8* iContentChunks;
		TLanguage iLocalizedNameLanguage;
		TSecureId iSecureId;
		TInt iAccessMode;
		
		enum TXmlNode
			{
			ENodeNone,
			ENodeSoftwareTypeRegistrationData,
			ENodeSoftwareType,
			ENodeLocalizedName,
			ENodeMimeType,
			ENodeSifPluginUid,
			ENodeCustomAccess,
			ENodeLauncherExecutable
			};
		RArray<TXmlNode> iNodes;
		
		TXmlNode ElementNameToNode(const TDesC8& aName) const;
		void PushNodeL(TXmlNode aNew, TXmlNode aExpectedParent);
		void AddContentChunkL(const TDesC8& aChunk);
		};

	}

#endif // SWTYPEREGINFOPARSER_H
