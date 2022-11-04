project('UPKUtils',
  [ 'cpp', 'c' ],
  default_options : [ 'cpp_std=c++14' ],
  version : '1.6',
  meson_version : '>=0.49')

common_src = [
  'CustomTFC.cpp',
  'LzoUtils.cpp',
  'minilzo.c',
  'UFlags.cpp',
  'UObject.cpp',
  'UObjectFactory.cpp',
  'UPKInfo.cpp',
  'UPKUtils.cpp',
  'UToken.cpp',
  'UTokenFactory.cpp',
]
libcommon = static_library('common', common_src)

# required by some of the binaries
wx_dep = dependency('wxwidgets',
  version : '>=3.0.0',
  required : true,
  modules : ['std', 'stc'])

if wx_dep.found()
  executable('DecompressLZO',
    ['DecompressLZO.cpp'],
    link_with: libcommon,
    dependencies : wx_dep)

  executable('DeserializeAll',
    ['DeserializeAll.cpp'],
    link_with: libcommon,
    dependencies : wx_dep)

  executable('ExportTexturesToDDS',
    ['ExportTexturesToDDS.cpp', 'dds.cpp'],
    link_with: libcommon,
    dependencies : wx_dep)

  executable('ImportTexturesFromDDS',
    ['ImportTexturesFromDDS.cpp', 'dds.cpp'],
    link_with: libcommon,
    dependencies : wx_dep)
endif

executable('CompareUPK',
  ['CompareUPK.cpp', 'UPKInfo.cpp', 'UFlags.cpp'])

executable('ExtractNameLists',
  ['ExtractNameLists.cpp'],
  link_with: libcommon)

executable('FindObjectByOffset',
  ['FindObjectByOffset.cpp'],
  link_with: libcommon)

executable('FindObjectEntry',
  ['FindObjectEntry.cpp'],
  link_with: libcommon)

executable('HexToPseudoCode',
  ['HexToPseudoCode.cpp'],
  link_with: libcommon)

executable('MoveExpandFunction',
  ['MoveExpandFunction.cpp'],
  link_with: libcommon)

executable('ParseTfc',
  ['ParseTfc.cpp', 'dds.cpp'],
  link_with: libcommon)

executable('PatchUPK',
  ['PatchUPK.cpp', 'ModParser.cpp', 'ModScript.cpp'],
  link_with: libcommon)

executable('UENativeTablesReader', ['UENativeTablesReader.cpp'])
