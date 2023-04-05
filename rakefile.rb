myDir = File.dirname(File.expand_path(__FILE__));
require "#{myDir}/../build-options.rb";

module Rakish

Rakish::CppProject.new(
	:name 			=> "artd-jlib-base",
	:package 		=> "artd",
	:id 			=> "",
	:dependsUpon 	=> [ "../artd-lib-logger"]
) do

	cppDefine('BUILDING_artd_jlib_base');
		
	addPublicIncludes('include/artd/*.h');

    addSourceFiles(
        'ArtdClassId.cpp',
        'Formatf.cpp',
        'HexFormatter.cpp',
        'IntrusiveList.cpp',
        'ObjectBase.cpp',
        'RcArray.cpp',
        'RcString.cpp',
        'base_types.cpp',
        'cstring_util.cpp',
        'Uuid.cpp',
        'utf8util.cpp'
    );

    setupCppConfig :targetType =>'DLL' do |cfg|
    end
end

end # module Rakish

