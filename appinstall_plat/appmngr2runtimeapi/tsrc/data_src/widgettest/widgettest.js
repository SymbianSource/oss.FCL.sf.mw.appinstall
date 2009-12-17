/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Basic widget for installation testing
*
*/


function showHelloView()
{
	// hide the main view and show the result view
	document.getElementById("mainView").style.display = "none";
	document.getElementById("resultView").style.display = "block";

	// detect if the output <p> tag has a child
	var child = document.getElementById('output').firstChild;
	// remove the child if exists
	if( child != null )
		document.getElementById('output').removeChild(child);

	// create a new text node
	var newText = document.createTextNode( "Hello, World!" );
	// append to the output <p> tag
	document.getElementById('output').appendChild(newText);

	// change the right softkey label and assign a custom callback function
	window.menu.setRightSoftKeyLabel('Back', showMainView);
}


function showMainView()
{
	// hide the result view and show the main view
	document.getElementById("resultView").style.display = "none";
	document.getElementById("mainView").style.display = "block";
	
	// restore the right softkey 
	window.menu.setRightSoftKeyLabel('', null);
}

