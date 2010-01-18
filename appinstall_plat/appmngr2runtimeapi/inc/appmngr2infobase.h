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
* Description:   Base class definitions for items displayed in AppMngr2
*
*/


#ifndef C_APPMNGR2INFOBASE_H
#define C_APPMNGR2INFOBASE_H

#include <e32base.h>                    // CBase
#include <eikmenup.h>                   // CEikMenuPaneItem::SData

class CAppMngr2Runtime;

enum TAppMngr2Location
    {
    EAppMngr2LocationPhone,
    EAppMngr2LocationMemoryCard,
    EAppMngr2LocationMassStorage
    };

/**
 * CAppMngr2InfoBase is the base class for objects representing:
 * - installed applications and
 * - installation packages (aka installation files).
 * 
 * Installed applications and installation packages are displayed in the
 * Appliction Manager UI using the data that Runtime plug-ins provide with
 * CAppMngr2InfoBase derived objects. Application Manager gets the data via
 * GetInstalledAppsL() and GetInstallationFilesL() metods in CAppMngr2Runtime
 * class.
 * 
 * CAppMngr2InfoBase contains the common functionality for both installed
 * applications and installation packages. Derived classes CAppMngr2AppInfo
 * and CAppMngr2PackageInfo are placeholders for more specific functionality.  
 * 
 * CAppMngr2InfoBase, as well as CAppMngr2AppInfo and CAppMngr2PackageInfo,
 * are abstracts classes. Runtime plug-ins must provide the actual implementation
 * by using derived classes.
 * 
 * @lib appmngr2pluginapi.lib
 * @since S60 v5.1
 */
class CAppMngr2InfoBase : public CBase
    {
public:     // constructor and destructor
    void ConstructL();
    ~CAppMngr2InfoBase();

public:     // new functions
    /**
     * Reference to the CAppMngr2Runtime object of this plugin.
     * 
     * @return CAppMngr2Runtime&  Runtime object
     */
    IMPORT_C CAppMngr2Runtime& Runtime() const;
    
    /**
     * Icon index of this item (installed application or installation package).
     * 
     * Items are displayed in a list in the UI. Each item has an icon image,
     * two labels (name and details) and an optional small indicator icon.
     * IconIndex() method provides index for the icon image.
     * 
     * Icon index can be either:
     * - index to icon array loaded in CAppMngr2Runtime::LoadIconsL() method
     * - special value EAppMngr2UseSpecificIcon defined in AppMngr2Common.hrh
     * 
     * If item has a specific icon that no other items use, return value
     * EAppMngr2UseSpecificIcon and load the specific icon via SpecificIconL().
     * 
     * There are no default icons, each plug-in must provide implementation
     * for IconIndex() method.
     * 
     * @return TInt  Icon index
     */
    virtual TInt IconIndex() const = 0;

    /**
     * Optional specific icon for this item.
     * 
     * Returns new icon specific for the item. Note that this method is not
     * used unless IconIndex() method returns EAppMngr2UseSpecificIcon value.
     * 
     * The caller of this method is responsible to delete the returned
     * icon object.
     * 
     * Default implementation leaves with KErrNotSupported. Plug-in must
     * override it if IconIndex() returns EAppMngr2UseSpecificIcon value.
     * 
     * If the same icon bitmap is used in many items, it is more efficient
     * to load it once in CAppMngr2Runtime::LoadIconsL() and return icon
     * indexes form IconIndex() method.
     *  
     * @return CGulIcon  Item specific icon
     */
    IMPORT_C virtual CGulIcon* SpecificIconL() const;

    /**
     * Name of this item. 
     * 
     * Items are displayed in a list in the UI. The list uses two labels
     * (name and details) for each item. Descriptor that the Name() method
     * returns is displayed as the first label (the 1st line of the item).
     * 
     * @return const TDesC&  Displayable name
     */
    virtual const TDesC& Name() const = 0;

    /**
     * Details (size) of this item.
     * 
     * Items are displayed in a list in the UI. The list uses two labels
     * (name and details) for each item. Descriptor that the Details() method
     * returns is displayed as the second label (the 2nd line of the item).
     * 
     * Plug-ins can use SizeStringWithUnitsL() method to create details
     * string to display the item size in UI.
     * 
     * @returns const TDesC&  Displayable additional information line
     */
    virtual const TDesC& Details() const = 0;

    /**
     * Indicator icon index of this item.
     * 
     * Icon index can be either:
     * - default icon index defined in TAppMngr2IconIndex in AppMngr2Common.hrh
     * - index to icon array loaded in CAppMngr2Runtime::LoadIconsL() method
     * - special value EAppMngr2UseSpecificIcon defined in AppMngr2Common.hrh
     * 
     * Default implementation returns the default icon indexes based on
     * the value of iLocation member variable, so plug-ins usually does not
     * need to override this method.
     * 
     * If EAppMngr2UseSpecificIcon value is returned, SpecificIndicatorIconL()
     * method is  called to get the indicator icon.
     * 
     * @return TInt  Index to the icon array, or a TAppMngr2IconIndex value
     */
    IMPORT_C virtual TInt IndicatorIconIndex() const;

    /**
     * Optional specific indicator icon for this item.
     * 
     * Returns new indicator icon specific for the item. Note that this
     * method is not used unless IndicatorIconIndex() method returns
     * special EAppMngr2UseSpecificIcon value.
     * 
     * The caller of this method is responsible to delete the returned
     * icon object.
     * 
     * Default implementation leaves with KErrNotSupported. Plug-in must
     * override it if IndicatorIconIndex() returns EAppMngr2UseSpecificIcon.
     * 
     * If the same icon bitmap is used in many items, it is more efficient
     * to load it once in CAppMngr2Runtime::LoadIconsL() and return icon
     * indexes form IconIndex() method.
     * 
     * @return CGulIcon  Item specific icon
     */
    IMPORT_C virtual CGulIcon* SpecificIndicatorIconL() const;

    /**
     * ShowOnTop highlight status of this item.
     * 
     * Returns ETrue if this item should be highlighted (e.g. displayed
     * separately on top of other items in installation files/installed
     * applications list). All items are displayed in alphabetical order.
     * If ShowOnTop flag is set, then the item is displayed before other
     * items (or in completely separate list of highligted items). All
     * separately displayed items are listed in alphabetical order too. 
     * 
     * @return TBool  ETrue, if this item should be displayed separately
     */
    IMPORT_C TBool IsShowOnTop() const;

    /**
     * Optional specific menu items for this item.
     * 
     * If the item supports specific menu commands, return the menu pane
     * data. Plug-in can use ReadMenuItemDataFromResourceL() method to
     * read menu pane data from resources. Menu commands are run via
     * HandleCommandL() method.
     * 
     * The caller of this method is responsible to delete the returned
     * CEikMenuPaneItem::SData data structs.
     * 
     * Default implementation is empty. 
     * 
     * @param aMenuCmds  Array where plug-in specific menu items are added
     */
    IMPORT_C virtual void GetMenuItemsL( RPointerArray<CEikMenuPaneItem::SData>& aMenuCmds );

    /**
     * Enable generic menu commands for this item.
     * 
     * Generic menu commands are defined in TAppMngr2GenericCommands enum in
     * AppMngr2Common.hrh. Generic commands are hidden from the menu if the
     * currently selected item does not support them. Return ETrue for those
     * command ids that are supported by this item. Generic commands are
     * run via HandleCommandL() method.
     * 
     * @param aCmdId Generic command id
     * @return TBool ETrue if command aCmdId is supported
     */
    IMPORT_C virtual TBool SupportsGenericCommand( TInt aCmdId );

    /**
     * Optional specific middle softkey command for this item.
     *
     * If the item supports specific middle softkey command, return TBUF
     * resource id (for the command label) and command id. Item specific
     * command is run via HandleCommandL() method when user presses the
     * middle softkey. 
     * 
     * Default middle softkey command is used, if plug-in does not override it.
     *
     * @param aResourceId  Command label to be displayed in UI (TBUF resource)
     * @param aCommandId  Command id to be passed to HandleCommandL 
     */
    IMPORT_C virtual void GetMiddleSoftkeyCommandL( TInt& aResourceId, TInt& aCommandId );

    /**
     * Starts asynchronously a user invoked command.
     * 
     * The command can be a generic one or specific to the plug-in. The ids
     * for the generic commands are defined in AppMngr2Common.hrh (see the 
     * enumeration TAppMngr2GenericCommand). The plug-in specific commands 
     * are defined via GetMenuItemsL() and GetMiddleSoftkeyCommandL() methods. 
     *
     * The caller of this method must call HandleCommandResultL() after the
     * asynchronous request has been completed.
     *
     * This asynchronous request must be completed properly using the method 
     * User::RequestComplete() even if the command itself has been implemented 
     * in synchronous manner.
     *
     * @param aCommandId  Id of the command to be run
     * @param aStatus  Active object request status
     */
    virtual void HandleCommandL( TInt aCommandId, TRequestStatus& aStatus ) = 0;

    /**
     * Handles the completion result of an asynchronous command. 
     *
     * This method is provided so that the plug-in may decide upon handling
     * possible errors encountered during the command processing (note that 
     * the completion status is received directly by the caller of the method
     * HandleCommandL() and not by this plug-in). These actions may include 
     * for example closing open sessions and deleting objects.
     *
     * This function should leave if an error note should be displayed in
     * the UI (e.g. KErrNoMemory). Note that the result may be KErrCancel
     * or SwiUI::KSWInstErrUserCancel if the user cancelled the operation.
     *
     * @param aResult  Completion code, KErrNone or some error code
     */
    virtual void HandleCommandResultL( TInt aResult ) = 0;

    /**
     * Cancels the current asynchronous command.
     * 
     * This method may be called at any time when an asynchronous command
     * is started using HandleCommandL() method. It must cancel the running
     * command as quickly as possible.  
     */
    virtual void CancelCommand() = 0;
    
    /**
     * Utility function to create displayable string that contains size
     * followed by kilobyte (kB), megabyte (MB) or gigabyte (GB) units.
     * 
     * Size is rounded and formatted using the relevant unit, for example
     * SizeStringWithUnitsL( 5120 ) returns "5 kB".
     * 
     * The caller of this method is responsible to delete the returned string.
     * 
     * @param aSizeInBytes  Item size
     * @return HBufC*  New string that contains displayable size string 
     */
    IMPORT_C HBufC* SizeStringWithUnitsL( TInt64 aSizeInBytes );

    /**
     * Utility function to construct CEikMenuPaneItem from MENU_ITEM resource.
     * 
     * @param aResourceId  MENU_ITEM resource id
     * @param aMenuItem  CEikMenuPaneItem::SData struct that is filled in
     */ 
    IMPORT_C void ReadMenuItemDataFromResourceL( TInt aResourceId,
            CEikMenuPaneItem::SData& aMenuItem );

    /**
     * Returns the location of installation file or installed application.
     * 
     * Remote drives are not supported. Derived class must fill in the
     * iLocation member variable to provide data for this function.
     * 
     * @return TAppMngr2Location  Location of the item
     */ 
    IMPORT_C TAppMngr2Location Location() const;

    /**
     * Returns the drive where installation file is located, or where
     * installed application is installed.
     * 
     * Derived class must fill in the iLocationDrive member variable
     * to provide data for this function.
     * 
     * @return TDriveUnit  Drive where the item is located
     */
    IMPORT_C TDriveUnit LocationDrive() const;
    
protected:  // new functions
    /**
     * Constructor, not exported because used via CAppMngr2AppInfo and CAppMngr2PackageInfo
     */
    CAppMngr2InfoBase( CAppMngr2Runtime& aRuntime, RFs& aFsSession );

protected:  // data
    /**
     * Location of this item.
     * 
     * Default implementation of IndicatorIconIndex() function uses this
     * value to return the default indicator icon index. See also the
     * Location() function defined above.
     */
    TAppMngr2Location iLocation;
    
    /**
     * Drive of this item.
     * 
     * Location drive defines the drive where this item resides (either
     * installation package is stored, or application is installed).
     * CAppMngr2InfoIterator uses this value to display the drive
     * in "Details" dialog. See also the LocationDrive() function
     * defined above.
     */
    TDriveUnit iLocationDrive;
    
    /**
     * ShowOnTop (highlight) flag.
     * 
     * If ShowOnTop flag is ETrue, the item is highlighted in UI. It
     * may be displayed on top of other items in installed application
     * or installation files list, or it may be displayed in separate
     * list of highlighted items.
     * 
     * For example, new or untrusted installed applications can be highlighted.
     * When set, the item needs to show the reason why iShowOnTop is enabled
     * (e.g. by using "new" or "untrusted" icon). See also the IsShowOnTop()
     * function defined above.
     */
    TBool iShowOnTop;

    /**
     * File server session, provided by framework when item is created.
     */
    RFs& iFs;
    
private:    // data
    /**
     * Reference to CAppMngr2Runtime class for this item.
     * See also the Runtime() function defined above. 
     */
    CAppMngr2Runtime& iRuntime;
    };

#endif // C_APPMNGR2INFOBASE_H

