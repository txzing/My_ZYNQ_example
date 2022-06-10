set tclPath [pwd]
cd $tclPath
#回到了上级路径
cd ..
set projpath [pwd]
puts "Current workspace is $projpath"
setws $projpath
getws
set xsaName system_wrapper

set appName test


#Create a new platform
platform create -name $xsaName -hw $projpath/xsa/$xsaName.xsa -proc ps7_cortexa9_0 -os standalone -arch 32-bit -out $projpath
importprojects $projpath/$xsaName
platform active $xsaName
repo -add-platforms $xsaName

#importsources -name $xsaName/zynqmp_fsbl -path $tclPath/src/fsbl -target-path ./

domain active
#Create a new application
app create -name $appName -platform $xsaName -domain standalone_domain -template "Empty Application"
importsources -name $appName -path $tclPath/src/
#Build platform project
platform generate
#Build application project
append appName "_system"
sysproj build -name $appName

