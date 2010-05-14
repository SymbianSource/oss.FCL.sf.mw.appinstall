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
rem Certstore Configuration 4 setup script

cd \

rem Test SWI cert store

perl %SECURITYSOURCEDIR%\certman\twtlscert\scripts\batchfiles\certstorePlugins disable_all
perl %SECURITYSOURCEDIR%\certman\twtlscert\scripts\batchfiles\certstorePlugins enable swicertstoreplugin.dll
