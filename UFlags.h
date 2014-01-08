#ifndef UFLAGS_H_INCLUDED
#define UFLAGS_H_INCLUDED

/// flags
enum class UPackageFlags: uint32_t
{
    AllowDownload = 0x00000001,
    ClientOptional = 0x00000002,
    ServerSideOnly = 0x00000004,
    BrokenLinks = 0x00000008,
    Cooked = BrokenLinks,
    Unsecure = 0x00000010,
    Encrypted = 0x00000020,
    Need = 0x00008000,
    Map = 0x00020000,
    Script = 0x00200000,
    Debug = 0x00400000,
    Imports = 0x00800000,
    Compressed = 0x02000000,
    FullyCompressed = 0x04000000,
    NoExportsData = 0x20000000,
    Stripped = 0x40000000,
    Protected = 0x80000000
};

enum class UCompressionFlags: uint32_t
{
    ZLIB = 0x00000001,
    ZLO = 0x00000002,
    ZLX = 0x00000004
};

enum class UObjectFlagsL: uint32_t
{
    Transactional = 0x00000001,
    Unreachable = 0x00000002,
    Public = 0x00000004,
    Private = 0x00000080,
    Automated = 0x00000100,
    Transient = 0x00004000,
    Preloading = 0x00008000,
    LoadForClient = 0x00010000,
    LoadForServer = 0x00020000,
    LoadForEdit = 0x00040000,
    Standalone = 0x00080000,
    NotForClient = 0x00100000,
    NotForServer = 0x00200000,
    NotForEdit = 0x00400000,
    NeedPostLoad = 0x01000000,
    HasStack = 0x02000000,
    Native = 0x04000000,
    Marked = 0x08000000
};

enum class UObjectFlagsH: uint32_t
{
    Obsolete = 0x00000020,
    Final = 0x00000080,
    PerObjectLocalized = 0x00000100,
    PropertiesObject = 0x00000200,
    ArchetypeObject = 0x00000400,
    RemappedName = 0x00000800
};

enum class UExportFlags: uint32_t
{
    ForcedExport = 0x00000001
};

enum class UFunctionFlags: uint32_t
{
    Final = 0x00000001,
    Defined = 0x00000002,
    Iterator = 0x00000004,
    Latent = 0x00000008,
    PreOperator = 0x00000010,
    Singular = 0x00000020,
    Net = 0x00000040,
    NetReliable = 0x00000080,
    Simulated = 0x00000100,
    Exec = 0x00000200,
    Native = 0x00000400,
    Event = 0x00000800,
    Operator = 0x00001000,
    Static = 0x00002000,
    NoExport = 0x00004000,
    OptionalParameters = NoExport,
    Const = 0x00008000,
    Invariant = 0x00010000,
    Public = 0x00020000,
    Private = 0x00040000,
    Protected = 0x00080000,
    Delegate = 0x00100000,
    NetServer = 0x00200000,
    NetClient = 0x01000000,
    DLLImport = 0x02000000
};

enum class UStructFlags: uint32_t
{
    Native = 0x00000001,
    Export = 0x00000002,
    Long = 0x00000004,
    HasComponents = Long,
    Init = 0x00000008,
    Transient = Init,
    Atomic = 0x00000010,
    Immutable = 0x00000020,
    StrictConfig = 0x00000040,
    ImmutableWhenCooked = 0x00000080,
    AtomicWhenCooked = 0x00000100
};

enum class UClassFlags: uint32_t
{
    Abstract = 0x00000001,
    Compiled = 0x00000002,
    Config = 0x00000004,
    Transient = 0x00000008,
    Parsed = 0x00000010,
    Localized = 0x00000020,
    SafeReplace = 0x00000040,
    RuntimeStatic = 0x00000080,
    NoExport = 0x00000100,
    Placeable = 0x00000200,
    PerObjectConfig = 0x00000400,
    NativeReplication = 0x00000800,
    EditInlineNew = 0x00001000,
    CollapseCategories = 0x00002000,
    ExportStructs = 0x00004000,
    Instanced = 0x00200000,
    HideDropDown = 0x00400000,
    HasComponents = HideDropDown,
    CacheExempt = 0x00800000,
    Hidden = CacheExempt,
    ParseConfig = 0x01000000,
    Deprecated = ParseConfig,
    HideDropDown2 = 0x02000000,
    Exported = 0x04000000,
    NativeOnly = 0x20000000
};

enum class UStateFlags: uint32_t
{
    Editable = 0x00000001,
    Auto = 0x00000002,
    Simulated = 0x00000004
};

enum class UPropertyFlagsL: uint32_t
{
    Editable = 0x00000001,
    Const = 0x00000002,
    Input = 0x00000004,
    ExportObject = 0x00000008,
    OptionalParm = 0x00000010,
    Net = 0x00000020,
    EditConstArray = 0x00000040,
    EditFixedSize = EditConstArray,
    Parm = 0x00000080,
    OutParm = 0x00000100,
    SkipParm = 0x00000200,
    ReturnParm = 0x00000400,
    CoerceParm = 0x00000800,
    Native = 0x00001000,
    Transient = 0x00002000,
    Config = 0x00004000,
    Localized = 0x00008000,
    Travel = 0x00010000,
    EditConst = 0x00020000,
    GlobalConfig = 0x00040000,
    Component = 0x00080000,
    OnDemand = 0x00100000,
    Init = OnDemand,
    New = 0x00200000,
    DuplicateTransient = New,
    NeedCtorLink = 0x00400000,
    NoExport = 0x00800000,
    Cache = 0x01000000,
    NoImport = Cache,
    EditorData = 0x02000000,
    NoClear = EditorData,
    EditInline = 0x04000000,
    EdFindable = 0x08000000,
    EditInlineUse = 0x10000000,
    Deprecated = 0x20000000,
    EditInlineNotify = 0x40000000,
    DataBinding = EditInlineNotify,
    SerializeText = 0x80000000,
    Automated = SerializeText
};

enum class UPropertyFlagsH: uint32_t
{
    RepNotify = 0x00000001,
    Interp = 0x00000002,
    NonTransactional = 0x00000004,
    EditorOnly = 0x00000008,
    NotForConsole = 0x00000010,
    RepRetry = 0x00000020,
    PrivateWrite = 0x00000040,
    ProtectedWrite = 0x00000080,
    Archetype = 0x00000100,
    EditHide = 0x00000200,
    EditTextBox = 0x00000400,
    CrossLevelPassive = 0x00001000,
    CrossLevelActive = 0x00002000
};

#endif // UFLAGS_H_INCLUDED
