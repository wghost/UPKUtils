#include "UFlags.h"

/// helper functions

std::string FormatHEX(uint32_t val)
{
    char ch[255];
    sprintf(ch, "0x%08X", val);
    return std::string(ch);
}

std::string FormatHEX(uint16_t val)
{
    char ch[255];
    sprintf(ch, "0x%04X", val);
    return std::string(ch);
}

std::string FormatHEX(uint8_t val)
{
    char ch[255];
    sprintf(ch, "0x%02X", val);
    return std::string(ch);
}

std::string FormatHEX(float val)
{
    std::string ret = "0x";
    uint8_t *p = reinterpret_cast<uint8_t*>(&val);
    for (unsigned i = 0; i < 4; ++i)
    {
        char ch[255];
        sprintf(ch, "%02X", p[3 - i]);
        ret += ch;
    }
    return ret;
}

std::string FormatHEX(uint32_t L, uint32_t H)
{
    char ch[255];
    sprintf(ch, "0x%08X%08X", H, L);
    return std::string(ch);
}

std::string FormatHEX(std::vector<char> DataChunk)
{
    std::string ret;
    for (unsigned i = 0; i < DataChunk.size(); ++i)
    {
        char ch[255];
        sprintf(ch, "%02X", (uint8_t)DataChunk[i]);
        ret += std::string(ch) + " ";
    }
    return ret;
}

std::string FormatHEX(char* DataChunk, size_t size)
{
    std::string ret;
    for (unsigned i = 0; i < size; ++i)
    {
        char ch[255];
        sprintf(ch, "%02X", (uint8_t)DataChunk[i]);
        ret += std::string(ch) + " ";
    }
    return ret;
}

std::string FormatHEX(std::string DataString)
{
    std::string ret;
    for (unsigned i = 0; i < DataString.size(); ++i)
    {
        char ch[255];
        sprintf(ch, "%02X", (uint8_t)DataString[i]);
        ret += std::string(ch) + " ";
    }
    return ret;
}

/// format flags

std::string FormatPackageFlags(uint32_t flags)
{
    std::ostringstream ss;
    if (flags & (uint32_t)UPackageFlags::AllowDownload)
    {
        ss << "\t" << FormatHEX((uint32_t)UPackageFlags::AllowDownload) << ": AllowDownload" << std::endl;
    }
    if (flags & (uint32_t)UPackageFlags::ClientOptional)
    {
        ss << "\t" << FormatHEX((uint32_t)UPackageFlags::ClientOptional) << ": ClientOptional" << std::endl;
    }
    if (flags & (uint32_t)UPackageFlags::ServerSideOnly)
    {
        ss << "\t" << FormatHEX((uint32_t)UPackageFlags::ServerSideOnly) << ": ServerSideOnly" << std::endl;
    }
    if (flags & (uint32_t)UPackageFlags::BrokenLinks)
    {
        ss << "\t" << FormatHEX((uint32_t)UPackageFlags::BrokenLinks) << ": BrokenLinks" << std::endl;
    }
    if (flags & (uint32_t)UPackageFlags::Cooked)
    {
        ss << "\t" << FormatHEX((uint32_t)UPackageFlags::Cooked) << ": Cooked" << std::endl;
    }
    if (flags & (uint32_t)UPackageFlags::Unsecure)
    {
        ss << "\t" << FormatHEX((uint32_t)UPackageFlags::Unsecure) << ": Unsecure" << std::endl;
    }
    if (flags & (uint32_t)UPackageFlags::Encrypted)
    {
        ss << "\t" << FormatHEX((uint32_t)UPackageFlags::Encrypted) << ": Encrypted" << std::endl;
    }
    if (flags & (uint32_t)UPackageFlags::Need)
    {
        ss << "\t" << FormatHEX((uint32_t)UPackageFlags::Need) << ": Need" << std::endl;
    }
    if (flags & (uint32_t)UPackageFlags::Map)
    {
        ss << "\t" << FormatHEX((uint32_t)UPackageFlags::Map) << ": Map" << std::endl;
    }
    if (flags & (uint32_t)UPackageFlags::Script)
    {
        ss << "\t" << FormatHEX((uint32_t)UPackageFlags::Script) << ": Script" << std::endl;
    }
    if (flags & (uint32_t)UPackageFlags::Debug)
    {
        ss << "\t" << FormatHEX((uint32_t)UPackageFlags::Debug) << ": Debug" << std::endl;
    }
    if (flags & (uint32_t)UPackageFlags::Imports)
    {
        ss << "\t" << FormatHEX((uint32_t)UPackageFlags::Imports) << ": Imports" << std::endl;
    }
    if (flags & (uint32_t)UPackageFlags::Compressed)
    {
        ss << "\t" << FormatHEX((uint32_t)UPackageFlags::Compressed) << ": Compressed" << std::endl;
    }
    if (flags & (uint32_t)UPackageFlags::FullyCompressed)
    {
        ss << "\t" << FormatHEX((uint32_t)UPackageFlags::FullyCompressed) << ": FullyCompressed" << std::endl;
    }
    if (flags & (uint32_t)UPackageFlags::NoExportsData)
    {
        ss << "\t" << FormatHEX((uint32_t)UPackageFlags::NoExportsData) << ": NoExportsData" << std::endl;
    }
    if (flags & (uint32_t)UPackageFlags::Stripped)
    {
        ss << "\t" << FormatHEX((uint32_t)UPackageFlags::Stripped) << ": Stripped" << std::endl;
    }
    if (flags & (uint32_t)UPackageFlags::Protected)
    {
        ss << "\t" << FormatHEX((uint32_t)UPackageFlags::Protected) << ": Protected" << std::endl;
    }
    return ss.str();
}

std::string FormatCompressionFlags(uint32_t flags)
{
    std::ostringstream ss;
    if (flags & (uint32_t)UCompressionFlags::ZLIB)
    {
        ss << "\t" << FormatHEX((uint32_t)UCompressionFlags::ZLIB) << ": ZLIB" << std::endl;
    }
    if (flags & (uint32_t)UCompressionFlags::LZO)
    {
        ss << "\t" << FormatHEX((uint32_t)UCompressionFlags::LZO) << ": LZO" << std::endl;
    }
    if (flags & (uint32_t)UCompressionFlags::LZX)
    {
        ss << "\t" << FormatHEX((uint32_t)UCompressionFlags::LZX) << ": LZX" << std::endl;
    }
    return ss.str();
}

std::string FormatObjectFlagsL(uint32_t flags)
{
    std::ostringstream ss;
    if (flags & (uint32_t)UObjectFlagsL::Transactional)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsL::Transactional) << ": Transactional" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsL::Unreachable)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsL::Unreachable) << ": Unreachable" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsL::Public)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsL::Public) << ": Public" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsL::Private)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsL::Private) << ": Private" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsL::Automated)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsL::Automated) << ": Automated" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsL::Transient)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsL::Transient) << ": Transient" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsL::Preloading)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsL::Preloading) << ": Preloading" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsL::LoadForClient)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsL::LoadForClient) << ": LoadForClient" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsL::LoadForServer)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsL::LoadForServer) << ": LoadForServer" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsL::LoadForEdit)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsL::LoadForEdit) << ": LoadForEdit" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsL::Standalone)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsL::Standalone) << ": Standalone" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsL::NotForClient)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsL::NotForClient) << ": NotForClient" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsL::NotForServer)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsL::NotForServer) << ": NotForServer" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsL::NotForEdit)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsL::NotForEdit) << ": NotForEdit" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsL::NeedPostLoad)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsL::NeedPostLoad) << ": NeedPostLoad" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsL::HasStack)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsL::HasStack) << ": HasStack" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsL::Native)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsL::Native) << ": Native" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsL::Marked)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsL::Marked) << ": Marked" << std::endl;
    }
    return ss.str();
}

std::string FormatObjectFlagsH(uint32_t flags)
{
    std::ostringstream ss;
    if (flags & (uint32_t)UObjectFlagsH::Obsolete)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsH::Obsolete) << ": Obsolete" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsH::Final)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsH::Final) << ": Final" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsH::PerObjectLocalized)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsH::PerObjectLocalized) << ": PerObjectLocalized" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsH::PropertiesObject)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsH::PropertiesObject) << ": PropertiesObject" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsH::ArchetypeObject)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsH::ArchetypeObject) << ": ArchetypeObject" << std::endl;
    }
    if (flags & (uint32_t)UObjectFlagsH::RemappedName)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UObjectFlagsH::RemappedName) << ": RemappedName" << std::endl;
    }
    return ss.str();
}

std::string FormatExportFlags(uint32_t flags)
{
    std::ostringstream ss;
    if (flags & (uint32_t)UExportFlags::ForcedExport)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UExportFlags::ForcedExport) << ": ForcedExport" << std::endl;
    }
    return ss.str();
}

std::string FormatFunctionFlags(uint32_t flags)
{
    std::ostringstream ss;
    if (flags & (uint32_t)UFunctionFlags::Final)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::Final) << ": Final" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::Defined)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::Defined) << ": Defined" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::Iterator)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::Iterator) << ": Iterator" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::Latent)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::Latent) << ": Latent" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::PreOperator)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::PreOperator) << ": PreOperator" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::Singular)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::Singular) << ": Singular" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::Net)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::Net) << ": Net" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::NetReliable)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::NetReliable) << ": NetReliable" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::Simulated)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::Simulated) << ": Simulated" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::Exec)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::Exec) << ": Exec" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::Native)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::Native) << ": Native" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::Event)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::Event) << ": Event" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::Operator)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::Operator) << ": Operator" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::Static)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::Static) << ": Static" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::NoExport)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::NoExport) << ": NoExport" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::OptionalParameters)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::OptionalParameters) << ": OptionalParameters" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::Const)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::Const) << ": Const" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::Invariant)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::Invariant) << ": Invariant" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::Public)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::Public) << ": Public" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::Private)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::Private) << ": Private" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::Protected)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::Protected) << ": Protected" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::Delegate)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::Delegate) << ": Delegate" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::NetServer)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::NetServer) << ": NetServer" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::NetClient)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::NetClient) << ": NetClient" << std::endl;
    }
    if (flags & (uint32_t)UFunctionFlags::DLLImport)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UFunctionFlags::DLLImport) << ": DLLImport" << std::endl;
    }
    return ss.str();
}

std::string FormatStructFlags(uint32_t flags)
{
    std::ostringstream ss;
    if (flags & (uint32_t)UStructFlags::Native)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UStructFlags::Native) << ": Native" << std::endl;
    }
    if (flags & (uint32_t)UStructFlags::Export)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UStructFlags::Export) << ": Export" << std::endl;
    }
    if (flags & (uint32_t)UStructFlags::Long)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UStructFlags::Long) << ": Long" << std::endl;
    }
    if (flags & (uint32_t)UStructFlags::HasComponents)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UStructFlags::HasComponents) << ": HasComponents" << std::endl;
    }
    if (flags & (uint32_t)UStructFlags::Init)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UStructFlags::Init) << ": Init" << std::endl;
    }
    if (flags & (uint32_t)UStructFlags::Transient)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UStructFlags::Transient) << ": Transient" << std::endl;
    }
    if (flags & (uint32_t)UStructFlags::Atomic)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UStructFlags::Atomic) << ": Atomic" << std::endl;
    }
    if (flags & (uint32_t)UStructFlags::Immutable)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UStructFlags::Immutable) << ": Immutable" << std::endl;
    }
    if (flags & (uint32_t)UStructFlags::StrictConfig)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UStructFlags::StrictConfig) << ": StrictConfig" << std::endl;
    }
    if (flags & (uint32_t)UStructFlags::ImmutableWhenCooked)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UStructFlags::ImmutableWhenCooked) << ": ImmutableWhenCooked" << std::endl;
    }
    if (flags & (uint32_t)UStructFlags::AtomicWhenCooked)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UStructFlags::AtomicWhenCooked) << ": AtomicWhenCooked" << std::endl;
    }
    return ss.str();
}

std::string FormatClassFlags(uint32_t flags)
{
    std::ostringstream ss;
    if (flags & (uint32_t)UClassFlags::Abstract)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::Abstract) << ": Abstract" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::Compiled)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::Compiled) << ": Compiled" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::Config)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::Config) << ": Config" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::Transient)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::Transient) << ": Transient" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::Parsed)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::Parsed) << ": Parsed" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::Localized)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::Localized) << ": Localized" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::SafeReplace)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::SafeReplace) << ": SafeReplace" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::RuntimeStatic)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::RuntimeStatic) << ": RuntimeStatic" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::NoExport)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::NoExport) << ": NoExport" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::Placeable)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::Placeable) << ": Placeable" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::PerObjectConfig)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::PerObjectConfig) << ": PerObjectConfig" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::NativeReplication)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::NativeReplication) << ": NativeReplication" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::EditInlineNew)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::EditInlineNew) << ": EditInlineNew" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::CollapseCategories)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::CollapseCategories) << ": CollapseCategories" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::ExportStructs)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::ExportStructs) << ": ExportStructs" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::Instanced)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::Instanced) << ": Instanced" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::HideDropDown)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::HideDropDown) << ": HideDropDown" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::HasComponents)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::HasComponents) << ": HasComponents" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::CacheExempt)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::CacheExempt) << ": CacheExempt" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::Hidden)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::Hidden) << ": Hidden" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::ParseConfig)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::ParseConfig) << ": ParseConfig" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::Deprecated)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::Deprecated) << ": Deprecated" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::HideDropDown2)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::HideDropDown2) << ": HideDropDown2" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::Exported)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::Exported) << ": Exported" << std::endl;
    }
    if (flags & (uint32_t)UClassFlags::NativeOnly)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UClassFlags::NativeOnly) << ": NativeOnly" << std::endl;
    }
    return ss.str();
}

std::string FormatStateFlags(uint32_t flags)
{
    std::ostringstream ss;
    if (flags & (uint32_t)UStateFlags::Editable)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UStateFlags::Editable) << ": Editable" << std::endl;
    }
    if (flags & (uint32_t)UStateFlags::Auto)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UStateFlags::Auto) << ": Auto" << std::endl;
    }
    if (flags & (uint32_t)UStateFlags::Simulated)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UStateFlags::Simulated) << ": Simulated" << std::endl;
    }
    return ss.str();
}

std::string FormatPropertyFlagsL(uint32_t flags)
{
    std::ostringstream ss;
    if (flags & (uint32_t)UPropertyFlagsL::Editable)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::Editable) << ": Editable" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::Const)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::Const) << ": Const" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::Input)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::Input) << ": Input" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::ExportObject)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::ExportObject) << ": ExportObject" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::OptionalParm)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::OptionalParm) << ": OptionalParm" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::Net)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::Net) << ": Net" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::EditConstArray)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::EditConstArray) << ": EditConstArray" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::EditFixedSize)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::EditFixedSize) << ": EditFixedSize" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::Parm)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::Parm) << ": Parm" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::OutParm)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::OutParm) << ": OutParm" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::SkipParm)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::SkipParm) << ": SkipParm" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::ReturnParm)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::ReturnParm) << ": ReturnParm" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::CoerceParm)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::CoerceParm) << ": CoerceParm" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::Native)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::Native) << ": Native" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::Transient)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::Transient) << ": Transient" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::Config)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::Config) << ": Config" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::Localized)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::Localized) << ": Localized" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::Travel)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::Travel) << ": Travel" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::EditConst)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::EditConst) << ": EditConst" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::GlobalConfig)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::GlobalConfig) << ": GlobalConfig" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::Component)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::Component) << ": Component" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::OnDemand)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::OnDemand) << ": OnDemand" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::Init)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::Init) << ": Init" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::New)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::New) << ": New" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::DuplicateTransient)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::DuplicateTransient) << ": DuplicateTransient" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::NeedCtorLink)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::NeedCtorLink) << ": NeedCtorLink" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::NoExport)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::NoExport) << ": NoExport" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::Cache)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::Cache) << ": Cache" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::NoImport)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::NoImport) << ": NoImport" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::EditorData)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::EditorData) << ": EditorData" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::NoClear)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::NoClear) << ": NoClear" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::EditInline)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::EditInline) << ": EditInline" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::EdFindable)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::EdFindable) << ": EdFindable" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::EditInlineUse)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::EditInlineUse) << ": EditInlineUse" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::Deprecated)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::Deprecated) << ": Deprecated" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::EditInlineNotify)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::EditInlineNotify) << ": EditInlineNotify" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::DataBinding)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::DataBinding) << ": DataBinding" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::SerializeText)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::SerializeText) << ": SerializeText" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsL::Automated)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsL::Automated) << ": Automated" << std::endl;
    }
    return ss.str();
}

std::string FormatPropertyFlagsH(uint32_t flags)
{
    std::ostringstream ss;
    if (flags & (uint32_t)UPropertyFlagsH::RepNotify)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsH::RepNotify) << ": RepNotify" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsH::Interp)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsH::Interp) << ": Interp" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsH::NonTransactional)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsH::NonTransactional) << ": NonTransactional" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsH::EditorOnly)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsH::EditorOnly) << ": EditorOnly" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsH::NotForConsole)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsH::NotForConsole) << ": NotForConsole" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsH::RepRetry)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsH::RepRetry) << ": RepRetry" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsH::PrivateWrite)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsH::PrivateWrite) << ": PrivateWrite" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsH::ProtectedWrite)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsH::ProtectedWrite) << ": ProtectedWrite" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsH::Archetype)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsH::Archetype) << ": Archetype" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsH::EditHide)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsH::EditHide) << ": EditHide" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsH::EditTextBox)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsH::EditTextBox) << ": EditTextBox" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsH::CrossLevelPassive)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsH::CrossLevelPassive) << ": CrossLevelPassive" << std::endl;
    }
    if (flags & (uint32_t)UPropertyFlagsH::CrossLevelActive)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UPropertyFlagsH::CrossLevelActive) << ": CrossLevelActive" << std::endl;
    }

    return ss.str();
}

std::string FormatBulkDataFlags(uint32_t flags)
{
    std::ostringstream ss;
    if (flags & (uint32_t)UBulkDataFlags::StoredInSeparateFile)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UBulkDataFlags::StoredInSeparateFile) << ": StoredInSeparateFile" << std::endl;
    }
    if (flags & (uint32_t)UBulkDataFlags::StoredAsSeparateData)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UBulkDataFlags::StoredAsSeparateData) << ": StoredAsSeparateData" << std::endl;
    }
    if (flags & (uint32_t)UBulkDataFlags::EmptyData)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UBulkDataFlags::EmptyData) << ": EmptyData" << std::endl;
    }
    if (flags & (uint32_t)UBulkDataFlags::CompressedZlib)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UBulkDataFlags::CompressedZlib) << ": CompressedZlib" << std::endl;
    }
    if (flags & (uint32_t)UBulkDataFlags::CompressedLzo)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UBulkDataFlags::CompressedLzo) << ": CompressedLzo" << std::endl;
    }
    if (flags & (uint32_t)UBulkDataFlags::CompressedLzx)
    {
        ss << "\t\t" << FormatHEX((uint32_t)UBulkDataFlags::CompressedLzx) << ": CompressedLzx" << std::endl;
    }

    return ss.str();
}
