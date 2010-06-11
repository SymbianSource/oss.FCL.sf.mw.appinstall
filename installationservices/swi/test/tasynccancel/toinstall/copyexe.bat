@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of the License "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem

call echo Y | del /F data\*.exe

call copy \epoc32\release\%1\%2\tswinocapabilityVFP.exe	data\toinstall-exe.exe
call copy \epoc32\release\%1\%2\tswidrmcapability.exe	data\toinstall-exe1.exe
call copy \epoc32\release\%1\%2\createprivatefile.exe	data\toinstall-exe2.exe
call copy \epoc32\release\%1\%2\testbigexe.exe		data\toinstall-exe-big.exe
