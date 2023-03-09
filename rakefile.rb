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
        'Formatf.cpp',
        'ObjectBase.cpp',
        'RcArray.cpp',
        'RcString.cpp',
        'cstring_util.cpp',
        'utf8util.cpp'
    );

    setupCppConfig :targetType =>'DLL' do |cfg|
    end
end

end # module Rakish
