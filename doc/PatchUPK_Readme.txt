-----------------------------------------------------------------------------------------------------------------
    UPK Utils: PatchUPK documentation file
-----------------------------------------------------------------------------------------------------------------

PatchUPK is a Windows console utility to patch XCOM packages (upk files). Packages must be decompressed first
(using DecompressLZO or Gildor's decompress.exe for Windows).

PatchUPK operates on special script files and applies subsequent changes to packages.

Version 5.0, 07/30/2014

Wasteland Ghost aka wghost81 (wghost81@gmail.com).

-----------------------------------------------------------------------------------------------------------------
    Changelist:
-----------------------------------------------------------------------------------------------------------------

07/30/2014:
    - fixed bug with global FIND_HEX usage.
    - '<' and '>' symbols in text strings (pseudo-code) are now allowed, "> combination with any white-spaces in
      between is still disallowed inside a string (for the sake of backward compatibility).
    - uninstall script now uses object-oriented approach instead of global offset to ensure compatibility with
      new objects (for future mods).

04/24/2014:
    - white-spaces handling improved.
    - text constant code added (see using pseudo-code section).
    - added commands for adding new name, import and export entries. WARNING! Highly experimental!!!

04/07/2014:
    - <NullRef> pseudo code added for generating null object reference (0x00000000) with 4 serial and 8 memory
      size.
    - WARNING! Changed behavior of FIND_HEX/FIND_CODE and BEFORE/AFTER_HEX/CODE! See below for details.

03/31/2014:
    - member variable marker added (see script commands for detailed info).
    - ALIAS key added (see script commands for detailed info).
    - FIND_HEX/FIND_CODE sections added.

03/18/2014:
    - CODE keys and sections added (see script commands for detailed info).

02/19/2014:
    - C-style comments support added:
        /* comment */
        // comment
    - FIND_HEX operation now sets temporary scope, which limits the next write operation to FIND_HEX boundaries.
      This allows to auto-expand object (if AUTO or MOVE specifier is set) to fit in replacement data (specified
      by MODDED_HEX or AFTER_HEX). Also applies to BEFORE_HEX/AFTER_HEX.
    - MODDED_FILE is now considered a total replacement for current scope: i.e. it should contain full object
      data. Is also required now to use Full.Object.Name.ext pattern for file name, as program will auto-set
      scope, based on file name.
    - EXPAND_FUNCTION key now sets current scope to specified object.

-----------------------------------------------------------------------------------------------------------------
    Script commands basics
-----------------------------------------------------------------------------------------------------------------

There are two types of commands: keys and sections. Keys are followed by '=' sign to separate them from values
and sections are enclosed into brackets '[' and ']'.

-----------------------------------------------------------------------------------------------------------------
    Comments
-----------------------------------------------------------------------------------------------------------------

PatchUPK supports two types of comments: curly brackets comments (enclosed in  '{' and '}') and C-style comments.
You may use comments anywhere in the mod file. Examples:
/* Multi-line
   comment */
// Line comment
{ Curly brackets comment }

-----------------------------------------------------------------------------------------------------------------
    Optional keys
-----------------------------------------------------------------------------------------------------------------

MOD_NAME, AUTHOR and DESCRIPTION keys are used to provide author and mod information and may be used as mod's
readme.

-----------------------------------------------------------------------------------------------------------------
    Loading a package
-----------------------------------------------------------------------------------------------------------------

GUID key adds specified GUID to allowed GUIDs list. Example:
GUID=3F3B9C3140E45D9C8E92AFABF2746525:XComStrategyGame.upk
GUID value must correspond with the output of ExtractNameLists program! You may add multiple GUIDs for the same
package and multiple GUIDs for multiple packages. Use GUID key before you load a package with UPK_FILE key!

UPK_FILE key loads a package. Example:
UPK_FILE=XComGame.upk
Patcher opens the specified package, reads its header info, reconstructs full object names and additional info
for "smart" patching operations, so this command may take a while to complete.
If specified package is already opened, patcher will not try to re-open it.
    
-----------------------------------------------------------------------------------------------------------------
    Setting a scope
-----------------------------------------------------------------------------------------------------------------

There currently are four different scopes:
- Package
- Name Table Entry
- Import Table Entry
- Export Table Entry
- Object: serialized data, referenced in Export Table Entry

OFFSET key sets a scope to entire package, excluding first 8 bytes, as changing package signature and version
is not allowed. Offset sets write pointer to specified ABSOLUTE file offset (i.e. from the very beginning - 0 byte).
You may use dec and hex value representation.
OFFSET=0xA1B2
OFFSET=12345

NAME_ENTRY, IMPORT_ENTRY and EXPORT_ENTRY keys set scope to specified table entry: unique name for NAME_ENTRY
and full object name for IMPORT_ENTRY and EXPORT_ENTRY:
NAME_ENTRY=GetAltWeapon
IMPORT_ENTRY=XComGame.XComGameInfo.PerkContents
EXPORT_ENTRY=XGStrategyAI.GetAltWeapon

OBJECT key sets the current scope to specified export object:
OBJECT=XGStrategyAI.GetAltWeapon
Value of this key may be used with specifiers:
OBJECT=XGStrategyAI.GetAltWeapon:KEEP
OBJECT=XGStrategyAI.GetAltWeapon:MOVE
OBJECT=XGStrategyAI.GetAltWeapon:AUTO
If specifier is empty, KEEP is assumed. KEEP provides a most safest behavior: write operations will be performed
inside specified scope only. It means, if data chunk exceeds object's size, write operation will fail.
MOVE specifier forces object data move before applying any changes.
AUTO specifier allows program to auto-move/resize object when needed.
Note that modifiers themselves do not make any changes: they work when you start to write actual data.
    
-----------------------------------------------------------------------------------------------------------------
    Setting a relative offset
-----------------------------------------------------------------------------------------------------------------

REL_OFFSET key is used to set relative offset inside current scope. Example:
REL_OFFSET=0x30
REL_OFFSET=48

-----------------------------------------------------------------------------------------------------------------
    Finding the binary data
-----------------------------------------------------------------------------------------------------------------

FIND_HEX key is used to find specified data and set the relative offset to the beginning of that data:
FIND_HEX=0A 0B 12 34
Value of FIND_HEX is a space-separated hex-represented bytes string. You may use comments and multiline strings
inside FIND_HEX value.

Since v.3.2 FIND_HEX can be used with modifiers: BEG and END. Examples:
FIND_HEX=0A 0B 12 34:BEG
FIND_HEX=0A 0B 12 34:END
Using BEG will set current offset to the beginning of search string and using END will set the offset to the end
of search string. If no modifier is present, BEG is assumed.

WARNING! Since v.3.2 FIND_HEX no longer sets temporary scope! It sets current offset only, as the very first
version did!
    
-----------------------------------------------------------------------------------------------------------------
    Writing the binary data
-----------------------------------------------------------------------------------------------------------------

MODDED_HEX key is used to write specified data at current offset inside current scope. Value is the same as for
FIND_HEX: space-separated hex-represented bytes string, may be multiline, may use comments.
MODDED_HEX=0A 0B 12 34
If AUTO or MOVE object specifier is used, program will attempt to auto-expand current object to fit MODDED_HEX
data. Auto-expand works for export object serial data only, table entries won't be expanded.

MODDED_FILE key is used to read modded binary data from separate file.
MODDED_FILE=path-to-file.ext

MODDED_FILE is considered a total replacement for current scope: i.e. it should contain full object data. It is
also required to use Full.Object.Name.ext pattern for file name, so program could auto-set scope, based on file
name. Object data files must use .Type as their extension and table entries must use .NameEntry, .ImportEntry and
.ExportEntry respectively. Examples:
MODDED_FILE=XGStrategyAI.GetAltWeapon.Function
MODDED_FILE=GetAltWeapon.NameEntry
MODDED_FILE=XComGame.XComGameInfo.PerkContents.ImportEntry
MODDED_FILE=XGStrategyAI.GetAltWeapon.ExportEntry
You can use the same specifiers as for OBJECT key with MODDED_FILE key, although they will work for objects only
and will be ignored for table entries. After successful patching, current scope will be set to specified object.
Since modded file contains an object replacement code, export object data will be resized (expanded or shrunk)
accordingly if AUTO or MOVE modifier is set.

BYTE, FLOAT, INTEGER and UNSIGNED keys are used to set corresponding values. BYTE value is unsigned dec or hex
represented 1-byte value. FLOAT and INTEGER are signed dec-represented 4-byte values. UNSIGNED is unsigned dec
or hex represented 4-byte value.

NAMEIDX key is used to write UNameIndex 8-byte value, determined by specified name. Example:
NAMEIDX=GetAltWeapon

OBJIDX key is used to write UObjectReference 4-byte signed value, determined by object's full name. Example:
OBJIDX=XGStrategyAI.GetAltWeapon
   
-----------------------------------------------------------------------------------------------------------------
    Modifying object table entry
-----------------------------------------------------------------------------------------------------------------

RENAME key is used to rename an entry in NameTable:
RENAME=GetAltWeapon:#GetPodProgs
Changing entry size is not allowed, so new name length must be equal to old name length. After successful
renaming, current scope will be set to new Name Table name.

If entry is already renamed, program will just set current scope to it and continue patching.

-----------------------------------------------------------------------------------------------------------------
    Expanding a function
-----------------------------------------------------------------------------------------------------------------

EXPAND_FUNCTION is a key, used to expand functions:
EXPAND_FUNCTION=XGStrategyAI.GetPossibleAliens:5828
It will expand function code by adding 0B (nop) tokens and modifying serial and memory size accordingly and set
current scope to specified object after successful patching. If function already has specified size, it will do
nothing.
EXPAND_FUNCTION will also expand other objects by simply appending zeroes to the end of object data (for backward-
compatibility reasons).

EXPAND_UNDO key is used to undo move/expand operation:
EXPAND_UNDO=XGStrategyAI.GetAltWeapon
As move/expand operation keeps original data in place, EXPAND_UNDO simply restores ExportTable references to
the old data. No other changes are made and no "garbage" (expanded object binary) is collected. Used mostly
for uninstall purposes.
    
-----------------------------------------------------------------------------------------------------------------
    Section-style patching
-----------------------------------------------------------------------------------------------------------------

Some time ago I decided that sections are bulky and may be easily replaced with keys, while keeping all
functionality. But for the sake of backward-compatibility (and because IDIC matters!) program still supports
sections.

[MODDED_HEX] section is equal to MODDED_HEX key. Example:
[MODDED_HEX]
AB CD 12 34
You may use [/MODDED_HEX] to visually mark section end, but it is purely optional.

[FIND_HEX] (with optional [/FIND_HEX]) is equal to FIND_HEX key.
    
-----------------------------------------------------------------------------------------------------------------
    Find-and-replace (FNR) style patching (UPKModder compatibility)
-----------------------------------------------------------------------------------------------------------------

Since v.3.2 BEFORE_HEX and AFTER_HER are considered a unique commands and behave similar to those of UPKModder.

[BEFORE_HEX] (with optional [/BEFORE_HEX]) will set current scope to search string.

[AFTER_HEX] (with optional [/AFTER_HEX]) string is considered a replacement for BEFORE_HEX string and can not be
used without calling BEFORE_HEX first.

If FNR operation is performed within export object scope, it will respect OBJECT key modifiers: for KEEP it will
require AFTER_HEX data length be equal to BEFORE_HEX data length and for AUTO and MOVE it will auto-expand
(or shrink) BEFORE_HEX scope to fit AFTER_HEX data. Program will auto-adjust script serial size for objects with
scripts (functions and states), but it won't do anything about script memory size, as it doesn't know anything
about it (see "Using pseudo-code" section below for pseudo-code version which does calculate and rewrite both
serial and memory sizes).

If FNR operation is performed within global scope or name/import/export tables, it will require AFTER_HEX data
length to match with BEFORE_HEX data length.

-----------------------------------------------------------------------------------------------------------------
    Using pseudo-code
-----------------------------------------------------------------------------------------------------------------

As PatchUPK can auto-convert object/name references to hex code, you can write scripts (and plain references) in
pseudo-code.

Pseudo-code is enclosed in '<' and '>' brackets. Since v.4.0 you can use any type of white-spaces and comments
inside brackets. Example:
<%b
// user-changeable section: set sniper class probability
25
// user-changeable section end
>

-------
Syntax:
-------
1. Numeric constants:
    <%c Value>, where c is a single-character format specifier:
        f - float (4 bytes)
        i - signed integer (4 bytes)
        u - unsigned integer (4 bytes)
        s - short unsigned integer (2 bytes)
        b - byte (1 byte)
        t - text (variable-size string)
    and Value is a numeric.
    Examples:
        <%f  1.0>
        <%i -1>
        <%s  1>
        <%u  1>
        <%b  1>
        <%t  "text">
        <%t  "text with spaces "and inner quotes"">
2. Object references:
    <Class.ClassName> - class reference, must begin with "Class."!
    <ClassName.ObjName> - object reference (full object name).
    <.LocalObjName> - local object reference, must begin with ".". Full name is constructed by prepending local
    name with full name of current export object. Can not be used outside of export object scope.
    <@MemberObjName> - member object reference, full object name is constructed by prepending member name with
    class name, determined from current export object name. Can not be used outside of export object scope.
    <!AliasName> - alias reference (see ALIAS key description below).
    <NullRef> - null object reference (0x00000000) with 4 serial and 8 memory size.
3. Name reference:
    <Name> - plain simple name.

------------------
Keys and sections:
------------------
MODDED_CODE (key and section) - used to write a modded code at current scope, equivalent of MODDED_HEX.
FIND_CODE (key and section) - used to find specified code, equivalent of FIND_HEX.
INSERT_CODE (key and section) - used to insert a code at current position. Requires AUTO behavior, as it will
resize current object to insert new data. Works with export object data only. Does not change memory/file size
of a script! Useful for inserting additional default properties into default property list. Warning! Using
this function for mod distribution is dangerous, as if applied twice it will add specified code twice!
BEFORE_CODE and AFTER_CODE (sections) - behave like BEFORE_HEX and AFTER_HEX, but calculate and write correct
serial and memory sizes in case of expanding/shrinking the object with script.

Pseudo-code example:
[MODDED_CODE]
0F 00 <.arrRemove> 1B <ScaleRect> 00 <.arrRemove> 1E <%f0.75> 16

Adding default property example:
OBJECT=Default__XComMeldContainerSpawnPoint:AUTO
FIND_CODE=<None>
[INSERT_CODE]
<m_bBeginCountdownWhenSeen>
<BoolProperty>
<%u0>
<%u0>
<%b1>

FNR example (from Drakous79 "Default Helmet and Armor Tint" mod). This will correctly expand a function,
calculate and write script serial and memory sizes:
[BEFORE_CODE]
04 00 <.kSoldier>
[AFTER_CODE]
0F 35 <XGTacticalGameCoreNativeBase.TAppearance.iHaircut> <XGTacticalGameCoreNativeBase.TAppearance> 00 00
35 <XGTacticalGameCoreNativeBase.TSoldier.kAppearance> <XGTacticalGameCoreNativeBase.TSoldier> 00 01 00
<.kSoldier> 1D <%u305>
0F 35 <XGTacticalGameCoreNativeBase.TAppearance.iArmorTint> <XGTacticalGameCoreNativeBase.TAppearance> 00 00
35 <XGTacticalGameCoreNativeBase.TSoldier.kAppearance> <XGTacticalGameCoreNativeBase.TSoldier> 00 01 00
<.kSoldier> 93 2C <%b10> 26 16
04 00 <.kSoldier>

--------
Aliases:
--------

ALIAS (key) - used to define alias name for a portion of code. Syntax: 
    ALIAS=NAME:replacement code
    Example:
    ALIAS=GameDiff:19 1B <Game> 16 0A 00 <XGStrategy.GetDifficulty.ReturnValue> 00 1B <GetDifficulty> 16
    Usage:
    [MODDED_CODE]
    // if(Game().GetDifficulty() >= 2)
    07 00 00 99 <!GameDiff> 2C <%b2> 16

Aliases are searched scope-wise: if alias is defined inside object scope, it considered local to that scope.
While searching for alias replacement code, program first looks in the local scope and then in the global scope.
This means you may have different aliases with the same names inside different objects.

Example of locally defined alias:
OBJECT=XGStrategyAI.GetNumOutsiders:AUTO
ALIAS=GameDiff:19 1B <Game> 16 0A 00 <XGStrategy.GetDifficulty.ReturnValue> 00 1B <GetDifficulty> 16
[MODDED_CODE]
// if(Game().GetDifficulty() >= 2)
07 00 00 99 <!GameDiff> 2C <%b2> 16

Example of globally defined alias:
ALIAS=GameDiff:19 1B <Game> 16 0A 00 <XGStrategy.GetDifficulty.ReturnValue> 00 1B <GetDifficulty> 16
OBJECT=XGStrategyAI.GetNumOutsiders:AUTO
[MODDED_CODE]
// if(Game().GetDifficulty() >= 2)
07 00 00 99 <!GameDiff> 2C <%b2> 16

-----------------
Replacement code:
-----------------

REPLACEMENT_CODE (key and section) is used to completely replace export object script with the new one. Works
only with objects, which have scripts (like functions and states). Will expand/shrink object if necessary and if
current behavior is set to "AUTO" or "MOVE". Will auto-calculate and re-write memory and serial script size.

You can use CODE keys/sections with pure hex data, but in this case REPLACEMENT_CODE will set wrong memory size
(equal to serial script size). You may still set memory size manually, but you must do it after REPLACEMENT_CODE
call. Same way, memory size won't be calculated properly if you mix pure hex data with the pseudo-code.

You can label a token inside a code with [#label] and reference that label with [@label]. Label names must be
unique (see example below). Don't use labels if you mix pure hex with pseudo-code, as references will be wrong!

Example:
OBJECT=XGStrategyAI.GetNumOutsiders:AUTO
[REPLACEMENT_CODE]
// if(Game().GetDifficulty() >= 2)
07 [@label1] 99 19 1B <Game> 16 0A 00 <XGStrategy.GetDifficulty.ReturnValue> 00 1B <GetDifficulty> 16 2C <%b2> 16
	// return 2
	04 2C 02
// goto (else)
06 [@label2]
	[#label1]
	// return 1
	04 26
[#label2]
// return ReturnValue
04 3A <.ReturnValue>
// EOS
53

-----------------------------------------------------------------------------------------------------------------
    Adding new names and objects
-----------------------------------------------------------------------------------------------------------------

Warning! This feature is highly experimental, incompatible with uninstall feature and can completely mess up your
packages, so use this with caution and for experimentation purposes only! Don't use in actual mods!

There are three main keys/sections for adding new objects:
1. ADD_NAME_ENTRY - adds a name entry into name table.
2. ADD_IMPORT_ENTRY - adds import entry into import table.
3. ADD_EXPORT_ENTRY - adds export entry into export table and links new object to specified owner. Always writes
   at least 16 bytes long serialized data: PrevObjRef + Empty DefaultProperties List + NextObjRef
   
Example: adding a new WGhost81 local variable to XGStrategyAI.GetNumOutsiders function

[ADD_NAME_ENTRY]
<%u9>           // string length (including terminating null)
<%t"WGhost81">  // ASCII null-terminated string
<%u0x00000000>  // flags L (always the same)
<%u0x00070010>  // flags H (always the same)

[ADD_EXPORT_ENTRY]
<Core.IntProperty> // Type
<NullRef>          // ParentClassRef
<XGStrategyAI.GetNumOutsiders> // OwnerRef
<WGhost81>         // NameIdx
<NullRef>          // ArchetypeRef
<%u0x00000000>  // flags H
<%u0x00070004>  // flags L
<%u40>          // serial size
<%u0>           // serial offset
<%u0>           // export flags
<%u0>           // net objects count
<%u0>           // GUID1, zero if net objects count == 0
<%u0>           // GUID2, zero if net objects count == 0
<%u0>           // GUID3, zero if net objects count == 0
<%u0>           // GUID4, zero if net objects count == 0
<%u0>           // unknown, zero if net objects count == 0

OBJECT=XGStrategyAI.GetNumOutsiders.WGhost81
REL_OFFSET=16   // skip PrevObjRef + Empty DefaultProperties List + NextObjRef
[MODDED_CODE]
<%s1>		    // ArrayDim
<%s0>		    // ElementSize
<%u0x00000000>  // flags L
<%u0x00000000>  // flags H
<None>          // CategoryIndex
<NullRef>	    // ArrayEnumRef

Note that all entries are actually raw data, written in PatchUPK pseudo-code. Patcher will get it as pure HEX
code, deserialize it and link to owner if necessary. Patcher will not construct proper serial data for you,
you must do it yourself after adding an object!

-----------------------------------------------------------------------------------------------------------------
    Legacy support: deprecated/renamed keys
-----------------------------------------------------------------------------------------------------------------

You may still use those, but it is recommended to move to new commands.

FUNCTION key is an alias to OBJECT key. Renamed for obvious reasons: not all objects are functions.

FUNCTION_FILE key is an alias to MODDED_FILE key.

NAMELIST_NAME key is an alias to much shorter RENAME key.

-----------------------------------------------------------------------------------------------------------------
    Uninstall scripts
-----------------------------------------------------------------------------------------------------------------

When installing mod, PatchUPK automatically generates uninstall script and writes it to
your_mod_file_name.uninstall.txt. "Installing" .uninstall.txt "mod" will not generate another uninstall file.
In case your_mod_file_name.uninstall.txt already exists, program will generate your_mod_file_name.uninstall1.txt
and so on.

Uninstall data are taken directly from existing package before rewriting them.

-----------------------------------------------------------------------------------------------------------------
    TO DO
-----------------------------------------------------------------------------------------------------------------

Script compiler? :)
