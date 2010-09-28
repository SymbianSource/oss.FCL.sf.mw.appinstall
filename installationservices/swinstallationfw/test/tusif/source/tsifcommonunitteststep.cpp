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


/**
 @file
 @internalTechnology 
*/

#include "tsifcommonunitteststep.h"
#include "tsifsuitedefs.h"
#include <usif/sif/sifcommon.h>
#include <ct/rcpointerarray.h>
#include <scs/cleanuputils.h>
#include <s32mem.h> 

using namespace Usif;

namespace
    {
    TInt PackCapabilitySet(const TCapabilitySet& aCapSet)
        {
        TInt caps=0;
        for (TInt c=0; c<ECapability_Limit; ++c)
            {
            if (aCapSet.HasCapability(TCapability(c)))
                {
                caps += (1<<c);
                }
            }
        return caps;
        }
    }

CSifCommonUnitTestStep::~CSifCommonUnitTestStep()
/**
* Destructor
*/
    {
    INFO_PRINTF1(_L("Cleanup in CSifCommonUnitTestStep::~CSifCommonUnitTestStep()"));
    }

CSifCommonUnitTestStep::CSifCommonUnitTestStep()
/**
* Constructor
*/
    {
    }

void CSifCommonUnitTestStep::ImplTestStepL()
/**
* @return - TVerdict code
* Override of base class pure virtual
* Our implementation only gets called if the base class doTestStepPreambleL() did
* not leave. That being the case, the current test result value will be EPass.
*/
    {
    INFO_PRINTF1(_L("I am in CSifCommonUnitTestStep::doTestStep()."));

    SetTestStepResult(EFail);
    TestComponentInfoL();

    TestOpaqueNamedParamsL();

    SetTestStepResult(EPass);
    }



void CSifCommonUnitTestStep::ImplTestStepPostambleL()
/**
* @return - TVerdict code
* Override of base class virtual
*/
    {
    }


void CSifCommonUnitTestStep::ImplTestStepPreambleL()
/**
* @return - TVerdict code
* Override of base class virtual
*/
    {
    }

void CSifCommonUnitTestStep::TestComponentInfoL()
    {
    TCapabilitySet capSet(ECapabilityReadDeviceData);
    RPointerArray<Usif::CComponentInfo::CApplicationInfo>* applications = NULL;
    CComponentInfo* compInfo = CComponentInfo::NewLC();
    
    /* Internalize from an empty descriptor and read invalid root node */
    TIpcArgs ipcArgs;
    compInfo->PrepareForIpcL(ipcArgs, 0);
    TDes8* des = reinterpret_cast<TDes8*>(ipcArgs.iArgs[0]);
    des->FillZ(1);
    TRAPD(err, compInfo->RootNodeL());
    if (err != KErrNotFound)
        {
        INFO_PRINTF1(_L("Internalize from an empty descriptor and read invalid root node"));
        User::Leave(err);
        }
    CleanupStack::PopAndDestroy(compInfo);

    /* Add invalid child */
    CComponentInfo::CNode* node = CComponentInfo::CNode::NewLC(KNullDesC, KNullDesC, KNullDesC, KNullDesC, EDeactivated, ENewComponent, 12345678, KNullDesC, ENotAuthenticated, capSet, 1234, EFalse, EFalse, applications);
    TRAP(err, node->AddChildL(NULL));
    if (err != KErrArgument)
        {
        INFO_PRINTF1(_L("TestComponentInfoL: Add invalid child"));
        User::Leave(err);
        }
    /* Get empty GlobalComponentId */
    if (node->GlobalComponentId() != KNullDesC)
        {
        INFO_PRINTF1(_L("TestComponentInfoL: Get empty GlobalComponentId"));
        User::Leave(KErrGeneral);
        }
    CleanupStack::PopAndDestroy(node);

    /* Overflow detection */
    const TInt maxDescriptorLength = 256;
    const TInt tooBigLen = maxDescriptorLength + 1;
    const TInt maxSize = 1234;
    const TBool hasExe = EFalse;
    const TBool driveSelectionRequired = EFalse;

    HBufC* tooBigStr = HBufC::NewLC(tooBigLen);
    TPtr tooBigStrPtr = tooBigStr->Des();
    tooBigStrPtr.FillZ(tooBigLen);
    
    // for SoftwareTypeName
    TRAP(err, CComponentInfo::CNode::NewLC(*tooBigStr, KNullDesC, KNullDesC, KNullDesC, EDeactivated, ENewComponent, 12345678, KNullDesC, ENotAuthenticated, capSet, maxSize, hasExe, driveSelectionRequired, applications));
    if (err != KErrOverflow)
        {
        INFO_PRINTF1(_L("TestComponentInfoL: Overflow detection for SoftwareTypeName"));
        User::Leave(err);
        }

    // for ComponentName
    TRAP(err, CComponentInfo::CNode::NewLC(KNullDesC, *tooBigStr, KNullDesC, KNullDesC, EDeactivated, ENewComponent, 12345678, KNullDesC, ENotAuthenticated, capSet, maxSize, hasExe, driveSelectionRequired, applications));
    if (err != KErrOverflow)
        {
        INFO_PRINTF1(_L("TestComponentInfoL: Overflow detection for ComponentName"));
        User::Leave(err);
        }

    // for Version
    TRAP(err, CComponentInfo::CNode::NewLC(KNullDesC, KNullDesC, *tooBigStr, KNullDesC, EDeactivated, ENewComponent, 12345678, KNullDesC, ENotAuthenticated, capSet, maxSize, hasExe, driveSelectionRequired, applications));
    if (err != KErrOverflow)
        {
        INFO_PRINTF1(_L("TestComponentInfoL: Overflow detection for aVersion"));
        User::Leave(err);
        }

    // for Vendor
    TRAP(err, CComponentInfo::CNode::NewLC(KNullDesC, KNullDesC, KNullDesC, *tooBigStr, EDeactivated, ENewComponent, 12345678, KNullDesC, ENotAuthenticated, capSet, maxSize, hasExe, driveSelectionRequired, applications));
    if (err != KErrOverflow)
        {
        INFO_PRINTF1(_L("TestComponentInfoL: Overflow detection for aVendor"));
        User::Leave(err);
        }

    // for GlobalComponentId
    TRAP(err, CComponentInfo::CNode::NewLC(KNullDesC, KNullDesC, KNullDesC, KNullDesC, EDeactivated, ENewComponent, 12345678, *tooBigStr, ENotAuthenticated, capSet, maxSize, hasExe, driveSelectionRequired, applications));
    if (err != KErrOverflow)
        {
        INFO_PRINTF1(_L("TestComponentInfoL: Overflow detection for GlobalComponentId"));
        User::Leave(err);
        }

    CleanupStack::PopAndDestroy(tooBigStr);
    
    /* Testing embedded nodes */
    compInfo = CComponentInfo::NewLC();
    
    /* Set invalid root node */
    TRAP(err, compInfo->SetRootNodeL(NULL));
    if (err != KErrArgument)
        {
        INFO_PRINTF1(_L("TestComponentInfoL: Set invalid root node"));
        User::Leave(err);
        }

    CComponentInfo::CNode* emptyNode = CComponentInfo::CNode::NewLC(KNullDesC, KNullDesC, KNullDesC, KNullDesC, EDeactivated, ENewComponent, 12345678, KNullDesC, ENotAuthenticated, capSet, 1234, EFalse, EFalse, applications);
    const TDesC& globalComponentId(emptyNode->GlobalComponentId());
    ASSERT(globalComponentId.Length() == 0);
    TRAP(err, compInfo->SetRootNodeAsChildL(*emptyNode));
    if (err != KErrNotFound)
        {
        INFO_PRINTF1(_L("TestComponentInfoL: Set root node as child without a root node"));
        User::Leave(err);
        }
    CleanupStack::PopAndDestroy(emptyNode);
    
    // Create root node
    _LIT(KTxtRootSoftwareTypeName, "Test Software Type");
    _LIT(KTxtRootComponentName, "Root Component");
    _LIT(KTxtRootVersion, "1.0.0");
    _LIT(KTxtRootVendor, "Symbian");
    _LIT(KTxtRootGlobalComponentId, "Root Component Global Id");
    CComponentInfo::CNode* root = CComponentInfo::CNode::NewLC(KTxtRootSoftwareTypeName,
                KTxtRootComponentName, KTxtRootVersion, KTxtRootVendor, EDeactivated,
                ENewComponent, 12345678, KTxtRootGlobalComponentId, ENotAuthenticated, capSet, maxSize, hasExe, driveSelectionRequired, applications);
    
    // Create an array of nodes
    RCPointerArray<CComponentInfo::CNode> children;
    CleanupClosePushL(children);
    
    // Create second child node
    _LIT(KTxtChild2SoftwareTypeName, "Test Software Type");
    _LIT(KTxtChild2ComponentName, "Child2 Component");
    _LIT(KTxtChild2Version, "1.2.0");
    _LIT(KTxtChild2Vendor, "Symbian");
    _LIT(KTxtChild2GlobalComponentId, "Child2 Component Global Id");
    CComponentInfo::CNode* child2 = CComponentInfo::CNode::NewLC(KTxtChild2SoftwareTypeName,
                KTxtChild2ComponentName, KTxtChild2Version, KTxtChild2Vendor, EDeactivated,
                ENewComponent, 12345678, KTxtChild2GlobalComponentId, ENotAuthenticated, capSet, maxSize, hasExe, driveSelectionRequired, applications);
    children.AppendL(child2);
    CleanupStack::Pop(child2);
    
    // Create third child node
    _LIT(KTxtChild3SoftwareTypeName, "Test Software Type");
    _LIT(KTxtChild3ComponentName, "Child3 Component");
    _LIT(KTxtChild3Version, "1.3.0");
    _LIT(KTxtChild3Vendor, "Symbian");
    _LIT(KTxtChild3GlobalComponentId, "Child3 Component Global Id");
    CComponentInfo::CNode* child3 = CComponentInfo::CNode::NewLC(KTxtChild3SoftwareTypeName,
                KTxtChild3ComponentName, KTxtChild3Version, KTxtChild3Vendor, EDeactivated,
                ENewComponent, 12345678, KTxtChild3GlobalComponentId, ENotAuthenticated, capSet, maxSize, hasExe, driveSelectionRequired, applications);
    children.AppendL(child3);
    CleanupStack::Pop(child3);
    
    // Create first child node
    _LIT(KTxtChild1SoftwareTypeName, "Test Software Type");
    _LIT(KTxtChild1ComponentName, "Child1 Component");
    _LIT(KTxtChild1Version, "1.1.0");
    _LIT(KTxtChild1Vendor, "Symbian");
    _LIT(KTxtChild1GlobalComponentId, "Child1 Component Global Id");
    CComponentInfo::CNode* child1 = CComponentInfo::CNode::NewLC(KTxtChild1SoftwareTypeName,
                KTxtChild1ComponentName, KTxtChild1Version, KTxtChild1Vendor, EDeactivated,
                ENewComponent, 12345678, KTxtChild1GlobalComponentId, ENotAuthenticated, capSet,
                maxSize, hasExe, driveSelectionRequired, applications, &children);
    root->AddChildL(child1);
    CleanupStack::Pop(child1);
    CleanupStack::PopAndDestroy(&children);
    
    // Set the root node
    compInfo->SetRootNodeL(root);
    CleanupStack::Pop(root);
    
    // Check the content of the third node
    const CComponentInfo::CNode& rootNode = compInfo->RootNodeL();
    const CComponentInfo::CNode& child1Node = *rootNode.Children()[0];
    const CComponentInfo::CNode& child3Node = *child1Node.Children()[1];
    
    if (child3Node.SoftwareTypeName() != KTxtChild3SoftwareTypeName ||
        child3Node.ComponentName() != KTxtChild3ComponentName ||
        child3Node.Version() != KTxtChild3Version ||
        child3Node.Vendor() != KTxtChild3Vendor ||
        child3Node.ScomoState() != EDeactivated ||
        child3Node.InstallStatus() != ENewComponent ||
        child3Node.ComponentId() != 12345678 ||
        child3Node.GlobalComponentId() != KTxtChild3GlobalComponentId ||
        child3Node.Authenticity() != ENotAuthenticated ||
        PackCapabilitySet(child3Node.UserGrantableCaps()) != PackCapabilitySet(capSet) ||
        child3Node.MaxInstalledSize() != maxSize ||
        child3Node.Children().Count() != 0)
        {
        INFO_PRINTF1(_L("TestComponentInfoL: 'Testing embedded nodes' failed"));
        User::Leave(KErrGeneral);
        }
    
    CleanupStack::PopAndDestroy(compInfo);
    }

void CSifCommonUnitTestStep::TestOpaqueNamedParamsL()
    {
    _LIT(KParamName1, "param 1");
    _LIT(KParamName2, "param 2");
    _LIT(KParamName3, "param 3");
    _LIT(KParamName4, "param 4");
    _LIT(KParamName5, "param 5");
    
    _LIT(KStringValue1, "value 1");
    _LIT(KStringValue2, "value 2");
    _LIT(KStringValue3, "value 3");
    
    const TInt KIntValue1 = 111;
    const TInt KIntValue2 = 222;
    const TInt KIntValue3 = 333;
    
    //String Array
    RPointerArray<HBufC> stringArray;
    CleanupResetAndDestroyPushL(stringArray);
    
    HBufC* stringPtr = KStringValue1().AllocLC();
    stringArray.AppendL(stringPtr);
    CleanupStack::Pop();
    
    stringPtr = KStringValue2().AllocLC();
    stringArray.AppendL(stringPtr);
    CleanupStack::Pop();
    
    stringPtr = KStringValue3().AllocLC();
    stringArray.AppendL(stringPtr);
    CleanupStack::Pop(); 
    
    //Int Array
    RArray<TInt> intArray;
    CleanupClosePushL(intArray);
    intArray.AppendL(KIntValue1);
    intArray.AppendL(KIntValue2);
    intArray.AppendL(KIntValue3);
    
    /*  NewL */
    COpaqueNamedParams* params1 = COpaqueNamedParams::NewL();
    CleanupStack::PushL(params1);
    
    /* String operations */
    
    // AddStringL for a new param
    params1->AddStringL(KParamName1, KStringValue2);
    // AddStringL for an already existing param
    params1->AddStringL(KParamName1, KStringValue1);
    // StringByName for an existing param
    if (params1->StringByNameL(KParamName1) != KStringValue1)
        {
        INFO_PRINTF1(_L("TestOpaqueNamedParamsL: 'StringByName for an existing param' failed"));
        User::Leave(KErrGeneral);
        }
    // StringByName for a non-existing param
    if (params1->StringByNameL(KParamName2) != KNullDesC)
        {
        INFO_PRINTF1(_L("TestOpaqueNamedParamsL: 'StringByName for a non-existing param' failed"));
        User::Leave(KErrGeneral);
        }
   
    //Added as string , trying to retrieve as int.
    TRAPD(err, params1->IntByNameL(KParamName1));
    if(err != KErrNotFound)
        {
        INFO_PRINTF1(_L("TestOpaqueNamedParamsL: 'IntByNameL for an existing param' failed"));
        User::Leave(KErrGeneral);
        }   
    // StringByNameL for a non-existing param
    if (params1->StringByNameL(KParamName2) != KNullDesC)
        {
        INFO_PRINTF1(_L("TestOpaqueNamedParamsL: 'StringByNameL for a non-existing param' failed"));
        User::Leave(KErrGeneral);
        }
    
    CleanupStack::PopAndDestroy(params1);
    
    /* NewLC */
    COpaqueNamedParams* params2 = COpaqueNamedParams::NewLC();
    
    /* Int operations */
    
    // AddIntL for a new param
    params2->AddIntL(KParamName1, KIntValue2);
    // AddIntL for an already existing param
    params2->AddIntL(KParamName1, KIntValue1);
    // GetIntByName for an existing param
    TInt intValue1 = 0;
    if (!params2->GetIntByNameL(KParamName1, intValue1) || intValue1 != KIntValue1)
        {
        INFO_PRINTF1(_L("TestOpaqueNamedParamsL: 'GetIntByName for an existing param' failed"));
        User::Leave(KErrGeneral);
        }
    // GetIntByName for a non-existing param
    if (params2->GetIntByNameL(KParamName2, intValue1))
        {
        INFO_PRINTF1(_L("TestOpaqueNamedParamsL: 'GetIntByName for a non-existing param' failed"));
        User::Leave(KErrGeneral);
        }
    // IntByNameL for an existing param
    if (params2->IntByNameL(KParamName1) != KIntValue1)
        {
        INFO_PRINTF1(_L("TestOpaqueNamedParamsL: 'IntByNameL for an existing param' failed"));
        User::Leave(KErrGeneral);
        }
    // IntByNameL for a non-existing param
    TRAP(err, params2->IntByNameL(KParamName2));
    if (err != KErrNotFound)
        {
        INFO_PRINTF1(_L("TestOpaqueNamedParamsL: 'IntByNameL for a non-existing param' failed"));
        User::Leave(err);
        }
    
    /* GetNamesL & Count*/
    params2->AddStringL(KParamName2, KStringValue2);
    params2->AddIntL(KParamName3, KIntValue3);
    RCPointerArray<HBufC> names;
    CleanupClosePushL(names);
    params2->GetNamesL(names);
    if (params2->CountL() != 3 || names.Count() != 3 ||
        *names[0] != KParamName1 || *names[1] != KParamName2 || *names[2] != KParamName3)
        {
        INFO_PRINTF1(_L("TestOpaqueNamedParamsL: 'GetNamesL & Count' failed"));
        User::Leave(KErrGeneral);
        }
    
    /* Overflow detection */
    
    COpaqueNamedParams* params3 = COpaqueNamedParams::NewLC();
    
    const TInt bigLen = 1024;
    HBufC* bigStr = HBufC::NewLC(bigLen);
    TPtr bigStrPtr = bigStr->Des();
    bigStrPtr.FillZ(bigLen);
    
    // Max acceptable length of descriptor
    const TInt maxGoodLen = 1024/2; 
    TRAP(err, params3->AddStringL(KParamName1, bigStr->Left(maxGoodLen)));
    if (err != KErrNone)
        {
        INFO_PRINTF1(_L("TestOpaqueNamedParamsL: 'Overflow detection: Max acceptable length of descriptor'"));
        User::Leave(err);
        }
        
    // Too big descriptor - same param name
    const TInt tooBigLen = 1024/2+1;
    TRAP(err, params3->AddStringL(KParamName1, bigStr->Left(tooBigLen)));
    if (err != KErrOverflow)
        {
        INFO_PRINTF1(_L("TestOpaqueNamedParamsL: 'Overflow detection: Too big descriptor - updated param'"));
        User::Leave(err);
        }       
    
    // Too big descriptor - different param name
    TRAP(err, params3->AddStringL(KParamName2, bigStr->Left(tooBigLen)));
    if (err != KErrOverflow)
        {
        INFO_PRINTF1(_L("TestOpaqueNamedParamsL: 'Overflow detection: Too big descriptor - new param'"));
        User::Leave(err);
        }
    
    // Overflow in KMaxExternalizedSize
    
    params3->AddStringL(KParamName3, bigStr->Left(maxGoodLen));
    params3->AddStringL(KParamName4, bigStr->Left(maxGoodLen));
    params3->AddStringL(KParamName4, bigStr->Left(maxGoodLen)); // Add the same param to check overwriting
    TRAP(err, params3->AddStringL(KParamName5, bigStr->Left(maxGoodLen)));
    if (err != KErrOverflow)
        {
        INFO_PRINTF1(_L("TestOpaqueNamedParamsL: 'Overflow detection: Overflow in KMaxExternalizedSize'"));
        User::Leave(err);
        }
    
    CleanupStack::PopAndDestroy(4, params2);
    
    COpaqueNamedParams* params4 = COpaqueNamedParams::NewLC();
    
    params4->AddStringArrayL(KParamName1, stringArray);
    
    const RPointerArray<HBufC>& strArray = params4->StringArrayByNameL(KParamName1);
    if(*strArray[0] != KStringValue1 || *strArray[1] != KStringValue2 || *strArray[2] != KStringValue3)
        {
        INFO_PRINTF1(_L("TestOpaqueNamedParamsL: StringArrayByNameL failed"));
        User::Leave(err);
        }
    
    TRAP(err, const RArray<TInt>& intArray1 = params4->IntArrayByNameL(KParamName1));
    if(err != KErrNotFound)
        {
        INFO_PRINTF1(_L("TestOpaqueNamedParamsL: IntArrayByNameL failed"));
        User::Leave(err);  
        }   
    //Append a big string ( greater than 1024 bytes)
    HBufC* largeString = HBufC::NewLC(1000);
    largeString->Des().FillZ(1000);
    stringArray.AppendL(largeString);
    CleanupStack::Pop();
    
    TRAP(err, params4->AddStringArrayL(KParamName1, stringArray));
      
    if(err != KErrOverflow)
        {
        INFO_PRINTF1(_L("TestOpaqueNamedParamsL: AddStringArrayL overflow check failed"));
        User::Leave(err);       
        }
    
    stringArray.ResetAndDestroy();
    // Few large strings
    HBufC* bigString2 = HBufC::NewLC(1024);
    bigString2->Des().FillZ(1024);
    stringArray.AppendL(bigString2);
    CleanupStack::Pop();
    
    HBufC* bigString3 = HBufC::NewLC(1024);
    bigString3->Des().FillZ(1024);
    stringArray.AppendL(bigString3);
    CleanupStack::Pop();
    
    HBufC* bigString4 = HBufC::NewLC(1024);
    bigString4->Des().FillZ(1024);
    stringArray.AppendL(bigString4);
    CleanupStack::Pop();
    
    HBufC* bigString5 = HBufC::NewLC(1024);
    bigString5->Des().FillZ(1024);
    stringArray.AppendL(bigString5);
    CleanupStack::Pop();
     
    HBufC* bigString6 = HBufC::NewLC(256);
    bigString6->Des().FillZ(256);
    stringArray.AppendL(bigString6);
    CleanupStack::Pop();
     
    TRAP(err, params4->AddStringArrayL(KParamName1, stringArray));
      
    if(err != KErrOverflow)
        {
        INFO_PRINTF1(_L("TestOpaqueNamedParamsL: AddStringArrayL overflow check failed"));
        User::Leave(err);       
        }    
      
    params4->AddIntArrayL(KParamName1, intArray);
    const RArray<TInt>& RefIntArray = params4->IntArrayByNameL(KParamName1);
    if(RefIntArray[0] != KIntValue1 || RefIntArray[1] != KIntValue2 || RefIntArray[2] != KIntValue3)
        {
        INFO_PRINTF1(_L("TestOpaqueNamedParamsL: IntArrayByNameL failed"));
        User::Leave(err);
        }   
    
    TRAP(err, const RPointerArray<HBufC>& strArray1 = params4->StringArrayByNameL(KParamName1));
    if(err != KErrNotFound)
        {
        INFO_PRINTF1(_L("TestOpaqueNamedParamsL: StringArrayByNameL failed"));
        User::Leave(err);  
        }
    
    CleanupStack::PopAndDestroy(3, &stringArray);
    
    //Externalize, internalize tests.
    COpaqueNamedParams* params5 = COpaqueNamedParams::NewLC();
    HBufC* testString = _L("TestString").AllocLC();
    params5->AddStringL(KParamName1, *testString);
    params5->AddIntL(KParamName2, 23);
    CleanupStack::PopAndDestroy(testString);
    
    RArray<TInt> intArray2;
    CleanupClosePushL(intArray2);
    intArray2.AppendL(100);
    intArray2.AppendL(200);
    
    RPointerArray<HBufC> stringArray2;
    CleanupResetAndDestroyPushL(stringArray2);
    const HBufC16* testString1 = _L("TestString1").AllocLC();
    
    TInt err1 = stringArray2.Append(testString1);
    if (err1 != KErrNone)
        {
        CleanupStack::PopAndDestroy();
        }
    else
        {
        CleanupStack::Pop();
        }
    
    const HBufC16* testString2 = _L("TestString2").AllocLC();
    TInt err2 = stringArray2.Append(testString2);
    if (err2 != KErrNone)
        {
        CleanupStack::PopAndDestroy();
        }
    else
        {
        CleanupStack::Pop();
        }
    params5->AddStringArrayL(KParamName3, stringArray2);
    params5->AddIntArrayL(KParamName4, intArray2);
    
    CBufFlat* externalizedBuffer = CBufFlat::NewL(150);
    CleanupStack::PushL(externalizedBuffer);
    
    RBufWriteStream writeStream(*externalizedBuffer);
    CleanupClosePushL(writeStream);
    
    params5->ExternalizeL(writeStream);
    
    COpaqueNamedParams* params6 = COpaqueNamedParams::NewLC();
    
    RBufReadStream readStream(*externalizedBuffer);
    CleanupClosePushL(readStream);
    params6->InternalizeL(readStream);
    
    if(params6->IntByNameL(KParamName2)!= 23 || (params6->StringByNameL(KParamName1) != _L("TestString")))
        {
        INFO_PRINTF1(_L("TestOpaqueNamedParamsL: Internalize of int and string failed."));
        User::Leave(err);
        } 
    
    const RArray<TInt>& internalizedIntArray = params6->IntArrayByNameL(KParamName4);
    
    if(internalizedIntArray[0] != 100 && internalizedIntArray[1] != 200)
        {
        INFO_PRINTF1(_L("TestOpaqueNamedParamsL: Internalize of int array failed."));
        User::Leave(err);
        }   
    
    const RPointerArray<HBufC>& internalizedStringArray = params6->StringArrayByNameL(KParamName3);
    
    if (err1 == KErrNone)
        {
        if(*internalizedStringArray[0] != _L("TestString1") && *internalizedStringArray[1] != _L("TestString2"))
            {
            INFO_PRINTF1(_L("TestOpaqueNamedParamsL: Internalize of string array failed."));
            User::Leave(err);
            }
        }
    else if (err2 == KErrNone)
        {
        if(*internalizedStringArray[0] != _L("TestString2"))
            {
            INFO_PRINTF1(_L("TestOpaqueNamedParamsL: Internalize of string array failed."));
            User::Leave(err);
            }
        }

    CleanupStack::PopAndDestroy(7, params5);
    
    //Testing the large Param Name(more than 128 char)
    COpaqueNamedParams* params7 = COpaqueNamedParams::NewLC();
    
    //Adding an int as a opaque param
    HBufC* paramName = HBufC::NewLC(256);
    paramName->Des().FillZ(256);
       
    TRAP(err, params5->AddIntL(*paramName, 23));
    if(err != KErrOverflow)
       {
       INFO_PRINTF1(_L("TestOpaqueNamedParamsL: ParamName overflow check failed"));
       User::Leave(err);  
       }
    CleanupStack::PopAndDestroy(2, params7); //paramName
    }
