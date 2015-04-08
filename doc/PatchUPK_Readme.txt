-----------------------------------------------------------------------------------------------------------------
    PatchUPK documentation file
-----------------------------------------------------------------------------------------------------------------

PatchUPK is a console application for patching XCOM packages (upk files). Packages must be decompressed first
(using DecompressLZO or Gildor's decompress.exe).

PatchUPK operates on special script files and applies subsequent changes to packages.

-----------------------------------------------------------------------------------------------------------------
    Script commands basics
-----------------------------------------------------------------------------------------------------------------

There are two types of commands: keys and sections. Keys are followed by '=' character to separate them from
values and sections are enclosed into brackets '[' and ']'.

Keys can have additional specifiers separated by ':' character.

-----------------------------------------------------------------------------------------------------------------
    Comments
-----------------------------------------------------------------------------------------------------------------

PatchUPK supports two types of comments: curly brackets (text enclosed in '{' and '}') and C-style comments.
You can use comments anywhere inside a file. Examples:
/* Multi-line
   comment */
// Line comment
{ Curly brackets comment }

-----------------------------------------------------------------------------------------------------------------
    Controlling patcher behavior
-----------------------------------------------------------------------------------------------------------------

There are currently two keys to modify Patcher behavior.

UNINSTALL key is used to switch uninstall feature on and off: UNINSTALL=FALSE will switch uninstall feature off
and UNINSTALL=TRUE will switch it back on. By default uninstall feature is on. You can use this key in the middle
of the file to select what parts of your mod can be uninstalled safely and what parts can not.

UPDATE_REL key allows to switch relative offset auto-update on and off. Default behavior is off. UPDATE_REL=TRUE
forces Patcher to update relative offset after each write operation. UPDATE_REL=FALSE tells Patcher to keep
relative offset unchanged.

-----------------------------------------------------------------------------------------------------------------
    Optional keys
-----------------------------------------------------------------------------------------------------------------

MOD_NAME, AUTHOR and DESCRIPTION keys are used to provide author and mod information and can be used as mod's
readme.

-----------------------------------------------------------------------------------------------------------------
    Loading a package
-----------------------------------------------------------------------------------------------------------------

GUID key adds the GUID to list of allowed GUIDs. Example:
GUID=3F3B9C3140E45D9C8E92AFABF2746525:XComStrategyGame.upk
GUID value should correspond with the output of ExtractNameLists program! You can add multiple GUIDs for the same
package and multiple GUIDs for multiple packages. Declare allowed GUIDs before you start loading a package!

UPK_FILE key loads a package. Example:
UPK_FILE=XComGame.upk
Patcher opens the package, reads its header info, reconstructs full object names and additional info for "smart"
patching operations, so this command may take a while to complete. If the package is already opened, patcher
will not try to re-open it.
    
-----------------------------------------------------------------------------------------------------------------
    Setting a scope
-----------------------------------------------------------------------------------------------------------------

There currently are five different scopes:
- Package
- Name Table Entry
- Import Table Entry
- Export Table Entry
- Object: serialized data referenced in the Export Table Entry

OFFSET key sets the scope to an entire package (excluding first 8 bytes as changing package signature and version
is not allowed). The key sets a stream position to an ABSOLUTE file offset (i.e. from the very beginning - byte 0).
You can use dec and hex value representation.
OFFSET=0xA1B2
OFFSET=12345

NAME_ENTRY, IMPORT_ENTRY and EXPORT_ENTRY keys set the scope to a table entry: name entry for NAME_ENTRY
and object entry for IMPORT_ENTRY and EXPORT_ENTRY:
NAME_ENTRY=GetAltWeapon // use unique name here
IMPORT_ENTRY=XComGame.XComGameInfo.PerkContents // use full object name here
EXPORT_ENTRY=XGStrategyAI.GetAltWeapon // use full object name here

OBJECT key sets the scope to an export object:
OBJECT=XGStrategyAI.GetAltWeapon
This key has specifiers:
OBJECT=XGStrategyAI.GetAltWeapon:KEEP
OBJECT=XGStrategyAI.GetAltWeapon:MOVE
OBJECT=XGStrategyAI.GetAltWeapon:AUTO
OBJECT=XGStrategyAI.GetAltWeapon:INPL
If specifier is empty, KEEP is assumed. KEEP provides the most safest behavior: write operations will be performed
inside the scope only. It means, if a data chunk exceeds object's size, write operation will fail.
MOVE specifier forces moving object's data before applying any changes.
AUTO specifier allows program to auto-move/resize object when needed.
INPL specifier works similar to AUTO, but resizes object in-place instead of moving it.
Note that specifiers themselves do not make any changes: they work when you start to write actual data.
    
-----------------------------------------------------------------------------------------------------------------
    Setting a relative offset
-----------------------------------------------------------------------------------------------------------------

REL_OFFSET key is used to set a relative offset inside the scope. Example:
REL_OFFSET=0x30
REL_OFFSET=48

-----------------------------------------------------------------------------------------------------------------
    Finding a binary data
-----------------------------------------------------------------------------------------------------------------

FIND_HEX key is used to find the data and set a relative offset to the beginning of that data:
FIND_HEX=0A 0B 12 34
The value of FIND_HEX is a space-separated hex-represented string of bytes. You can use comments and multiline
strings inside FIND_HEX value.

FIND_HEX has two specifiers: BEG and END. Examples:
FIND_HEX=0A 0B 12 34:BEG
FIND_HEX=0A 0B 12 34:END
Using BEG will set the offset to the beginning of the search string and using END will set the offset to the
end of it. If no specifier is present, BEG is assumed.

-----------------------------------------------------------------------------------------------------------------
    Writing a binary data
-----------------------------------------------------------------------------------------------------------------

MODDED_HEX key is used to write the data at relative offset inside current scope. The value is the same as for
FIND_HEX: space-separated hex-represented string of bytes, can be multiline, can use comments.
MODDED_HEX=0A 0B 12 34
If AUTO or MOVE object specifier is used, program will attempt to auto-expand current object to fit MODDED_HEX
data. Auto-expand works for export object's serial data only, table entries won't be expanded.

MODDED_FILE key is used to read modded binary data from separate file.
MODDED_FILE=path-to-file.ext

MODDED_FILE is considered a total replacement for current scope: i.e. it should contain full object data. It is
also required to use Full.Object.Name.ext pattern for file name, so program could auto-set scope, based on file
name. Object data files should use .Type as their extension and table entries should use .NameEntry,
.ImportEntry and .ExportEntry respectively. Examples:
MODDED_FILE=XGStrategyAI.GetAltWeapon.Function
MODDED_FILE=GetAltWeapon.NameEntry
MODDED_FILE=XComGame.XComGameInfo.PerkContents.ImportEntry
MODDED_FILE=XGStrategyAI.GetAltWeapon.ExportEntry
You can use the same specifiers as for OBJECT key with MODDED_FILE key, although they will work for objects only
and will be ignored for table entries. After successful patching current scope will be set to the object. Since
modded file contains an object replacement code, export object data will be resized (expanded or shrunk)
accordingly if AUTO or MOVE specifier is set.

BYTE, FLOAT, INTEGER and UNSIGNED keys are used to set corresponding values. BYTE value is unsigned dec or hex
represented 1-byte value. FLOAT and INTEGER are signed dec-represented 4-byte values. UNSIGNED is unsigned dec
or hex represented 4-byte value.

NAMEIDX key is used to write UNameIndex 8-byte value determined by the name. Example:
NAMEIDX=GetAltWeapon

OBJIDX key is used to write UObjectReference 4-byte signed value determined by the full name. Example:
OBJIDX=XGStrategyAI.GetAltWeapon
   
-----------------------------------------------------------------------------------------------------------------
    Modifying object table entry
-----------------------------------------------------------------------------------------------------------------

RENAME key is used to rename an entry in the Name Table:
RENAME=GetAltWeapon:#GetPodProgs
Changing entry size is not allowed, so new name length should be equal to old name length. After successful
renaming current scope will be set to new Name Table name.

If entry is already renamed program will just set current scope to it and continue patching.

-----------------------------------------------------------------------------------------------------------------
    Expanding a function
-----------------------------------------------------------------------------------------------------------------

EXPAND_FUNCTION key is used to expand functions:
EXPAND_FUNCTION=XGStrategyAI.GetPossibleAliens:5828
It will expand function code by adding 0B (nop) tokens and modifying serial and memory size accordingly and set
current scope to the object after successful patching. If function already is of requested size, it will do
nothing.
EXPAND_FUNCTION will also expand other objects by simply appending zeroes to the end of object data (for backward-
compatibility reasons).

EXPAND_UNDO key is used to undo move/expand operation:
EXPAND_UNDO=XGStrategyAI.GetAltWeapon
As move/expand operation keeps original data in place, EXPAND_UNDO simply restores ExportTable references.
No other changes are made and no "garbage" (expanded object binary) is collected. Used mostly for uninstall
purposes.
    
-----------------------------------------------------------------------------------------------------------------
    Section-style patching
-----------------------------------------------------------------------------------------------------------------

All the keys can also be written as sections (for compatibility reasons and because IDIC matters).

[MODDED_HEX] section is an equivalent of MODDED_HEX key. Example:
[MODDED_HEX]
AB CD 12 34
You can use [/MODDED_HEX] to visually mark section end, but it is purely optional.

[FIND_HEX] (with optional [/FIND_HEX]) is an equivalent of FIND_HEX key.
    
-----------------------------------------------------------------------------------------------------------------
    Find-and-replace (FNR) style patching (UPKModder compatibility)
-----------------------------------------------------------------------------------------------------------------

BEFORE_HEX and AFTER_HEX are unique commands that behave similar to those of UPKModder.

[BEFORE_HEX] (with optional [/BEFORE_HEX]) sets current scope to search string.

[AFTER_HEX] (with optional [/AFTER_HEX]) string is considered a replacement for BEFORE_HEX string and can not be
used without calling BEFORE_HEX first.

If FNR operation is performed within export object scope it will respect OBJECT key specifiers: for KEEP it will
require AFTER_HEX data length be equal to BEFORE_HEX data length and for AUTO and MOVE it will auto-expand
(or shrink) BEFORE_HEX scope to fit AFTER_HEX data. Program will auto-adjust script serial size for objects with
scripts (functions and states), but it won't do anything about script memory size as it doesn't know anything
about it (see "Using pseudo-code" section below for pseudo-code version which does calculate and rewrite both
serial and memory sizes).

If FNR operation is performed within global scope or name/import/export tables, it will require AFTER_HEX data
length to match with BEFORE_HEX data length.

See "Using pseudo-code" section for BEFORE_CODE/AFTER_CODE combination, which can auto-update both serial and
memory sizes.

REPLACE_HEX/REPLACE_CODE keys are used to batch-replace specified data. They perform subsequent BEFORE/AFTER
patching inside current scope until all the blocks of before data are found and replaced.

Example:
REPLACE_CODE=<.LocalVarA>:<.LocalVarB>

-----------------------------------------------------------------------------------------------------------------
    Writing bulk data
-----------------------------------------------------------------------------------------------------------------

BULK_DATA/BULK_FILE keys allow to write BulkDataMirror objects which use absolute file offset to mark raw data
file position. BulkDataMirror objects are used to store textures and sounds inside cooked packages.

Examples:
BULK_DATA=01 02 03 04
BULK_FILE=path/to/binaryfile.ext

-----------------------------------------------------------------------------------------------------------------
    Using pseudo-code
-----------------------------------------------------------------------------------------------------------------

As PatchUPK can auto-convert object/name references to hex code, you can write scripts (and plain references) in
pseudo-code.

Pseudo-code is enclosed in '<' and '>' brackets. You can use any type of white-spaces and comments inside brackets.
Example:
<%b
// user-changeable section: set sniper class probability
25
// user-changeable section end
>

-------
Syntax:
-------
1. Numeric constants: <%c Value>, where c is a single-character format specifier:
        f - float (4 bytes)
        i - signed integer (4 bytes)
        u - unsigned integer (4 bytes)
        s - short unsigned integer (2 bytes)
        b - byte (1 byte)
        t - text (variable-size string)
    Examples:
        <%f  1.0>
        <%i -1>
        <%s  1>
        <%u  1>
        <%b  1>
        <%t  "text">
        <%t  "text with spaces "and inner quotes"">
2. Object references:
    <Class.ClassName> - class reference, should begin with "Class."!
    <ClassName.ObjName> - object reference (full object name).
    <.LocalObjName> - local object reference, should begin with ".". Full name is reconstructed by prepending
	local name with full name of current export object. Can not be used outside of export object scope.
    <@MemberObjName> - member object reference, full object name is reconstructed by prepending member name
	with class name determined from current export object name. Can not be used outside of export object scope.
    <!AliasName> - alias reference (see ALIAS key description below).
    <NullRef> - null object reference (0x00000000) with 4 serial and 8 memory size.
3. Name reference:
    <Name> - plain simple name.
4. [@label_name] and [#label_name], [@] and () - labels and references for auto-calculating memory offsets
   (see below).

------------------
Keys and sections:
------------------
MODDED_CODE (key and section) - used to write a modded code at current scope, equivalent of MODDED_HEX.
FIND_CODE (key and section) - used to find the code, equivalent of FIND_HEX.
INSERT_CODE (key and section) - used to insert a code at current position. Requires AUTO behavior, as it will
resize current object to insert new data. Works with export object data only. Does not change memory/file size
of a script! Useful for inserting additional default properties into default property list. Warning! Using
this function for mod distribution is not recommended, as if applied twice it will add the code twice!
BEFORE_CODE and AFTER_CODE (sections) - behave like BEFORE_HEX and AFTER_HEX, but calculate and write correct
serial and memory sizes in case of expanding/shrinking the object.

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

Aliases are searched scope-wise: if alias is defined inside object scope it is considered local to that
scope. While searching for alias replacement code program first looks in the local scope and then in
the global scope. This means you can have different aliases with the same names inside different scopes.

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
only with objects which have scripts (like functions and states). Will expand/shrink object if necessary and
if current behavior is set to "AUTO" or "MOVE". Will auto-calculate and re-write memory and serial script size.

You can use CODE keys/sections with pure/partial hex data, but in this case the memory size will be wrong and
you will need to set it manually. If you choose to do this, you should do it after REPLACEMENT_CODE call.

----------------------
Labels and references:
----------------------

You can label a token inside a code with [#label] and reference it with [@label]. Label names must be unique
(see example below). Don't use labels if you mix pure hex with pseudo-code, as references will be wrong!

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

You can use empty reference [@] and a pair of parentheses to auto-calculate memory size/skip size. Example:
// if(Game().GetDifficulty() >= 2)
07 [@label1] 99 19 1B <Game> 16 [@] <XGStrategy.GetDifficulty.ReturnValue> 00 ( 1B <GetDifficulty> 16 ) 2C <%b2> 16

[@] marks the place of memory size to calculate and parentheses mark the beginning and the end of the code
which size needs to be auto-calculated.

-----------------------------------------------------------------------------------------------------------------
    Adding new names and objects
-----------------------------------------------------------------------------------------------------------------

Warning! This feature is highly experimental and can completely mess up your packages! Use with caution!

There are three main keys/sections for adding new objects:
1. ADD_NAME_ENTRY - adds a name entry into the name table.
2. ADD_IMPORT_ENTRY - adds an import entry into the import table.
3. ADD_EXPORT_ENTRY - adds an export entry into the export table and links the new object to specified owner.
                      Always writes at least 16 bytes long serialized data:
					  PrevObjRef + Empty DefaultProperties List + NextObjRef
   
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

Note that all entries are actually raw data written in PatchUPK pseudo-code. Patcher will get it as pure HEX
code, deserialize it and link to owner if necessary. Patcher will not construct proper serial data for you,
you should do it yourself after adding an object!

-----------------------------------------------------------------------------------------------------------------
    Legacy support: deprecated/renamed keys
-----------------------------------------------------------------------------------------------------------------

You can still use those, but it is recommended to move on to new commands.

FUNCTION key is an alias to OBJECT key. Renamed for obvious reasons: not all objects are functions.

FUNCTION_FILE key is an alias to MODDED_FILE key.

NAMELIST_NAME key is an alias to much shorter RENAME key.

-----------------------------------------------------------------------------------------------------------------
    Uninstall scripts
-----------------------------------------------------------------------------------------------------------------

When installing mod PatchUPK automatically generates uninstall script and writes it to
your_mod_file_name.uninstall.txt. "Installing" .uninstall.txt "mod" will not generate another uninstall file.
In case your_mod_file_name.uninstall.txt already exists, program will generate your_mod_file_name.uninstall1.txt
and so on.

Uninstall data are taken directly from existing package before rewriting them.

-----------------------------------------------------------------------------------------------------------------
    TO DO
-----------------------------------------------------------------------------------------------------------------

Script compiler? :)
