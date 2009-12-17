/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Interface for CCompressedDataProvider
*
*/


/**
 @file 
 @internalTechnology
*/

#ifndef __COMPRESSEDDATAPROVIDER_H__
#define __COMPRESSEDDATAPROVIDER_H__

#include <e32std.h>
#include <ezbufman.h>

#include "sisdataprovider.h"

class CEZDecompressor;

namespace Swi
{
  const TInt KDeflateBufferMaxSize = 0x8000;
	
  /**
   * This class is decorator over an MSisDataProvider. It adds 
   * an decompression layer so that users can access a compressed 
   * data stream transparently.
   *
   * @released
   * @internalTechnology
   */
  class CCompressedDataProvider : public CBase, public MSisDataProvider, public MEZBufferManager
  {
  public:
  
  	/**
  	 * Creates a new compressed data provider wrapping up the given data provider.
  	 *
  	 * @param aDataProvider The data provider from which we read the actual data.
  	 * @param abufferSize 	The size of the in-memory buffer used during the decompression.
  	 *
  	 * @return And instance of CCompressedDataProvider decorating the given data provider.
  	 */
    static CCompressedDataProvider* NewL(MSisDataProvider& aDataProvider, TInt abufferSize = KDeflateBufferMaxSize);

  	/**
  	 * Creates a new compressed data provider wrapping up the given data provider and 
  	 * leaves it on the cleanup stack.
  	 *
  	 * @param aDataProvider The data provider from which we read the actual data.
  	 * @param abufferSize 	The size of the in-memory buffer used during the decompression.
  	 *
  	 * @return And instance of CCompressedDataProvider decorating the given data provider.
  	 */
    static CCompressedDataProvider* NewLC(MSisDataProvider& aDataProvider, TInt abufferSize = KDeflateBufferMaxSize);

    ~CCompressedDataProvider();
    
  public: // from MSisDataProvider
    TInt Read(TDes8& aDes);
    TInt Read(TDes8& aDes, TInt aLength);
    TInt Seek(TSeek aMode, TInt64& aPos);    
    
  public: // from MEZBufferManager
    void InitializeL(CEZZStream &aZStream);
    void NeedInputL(CEZZStream &aZStream);
    void NeedOutputL(CEZZStream &aZStream);
    void FinalizeL(CEZZStream &aZStream);    
    
  protected:

  	/**
  	 * The constructor.
  	 *
  	 * @param aDataProvider The underlying data provider object.
  	 */
    CCompressedDataProvider(MSisDataProvider& aDataProvider);

	/**
	 * The second phase contructor.
	 *
	 * @param aBufferSize The size of the in-memory buffer used during decompression.
	 */
    void ConstructL(TInt aBufferSize);

  private:
  
    MSisDataProvider& iDataProvider;
    CEZDecompressor*  iDecompressor;
    HBufC8* iInputBuffer;
    HBufC8* iOutputBuffer;
    TPtr8   iInputDescriptor;
    TPtr8   iOutputDescriptor;
    TBool   iMore;
    CBufFlat* iBuffer;
  };

} // namespace Swi
#endif // __COMPRESSEDDATAPROVIDER_H__
