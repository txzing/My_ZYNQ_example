# Configuration - Can be modified by the user
#set xsa_filename [lindex $argv 1]
put [pwd]

set project_path [lindex $argv 0]
#set project_name vivado_proj
set project_name [lindex $argv 1]
#set part name
set part_name [lindex $argv 2]
#set BD_name system
set bd_filename [lindex $argv 3]
#set BD_name [file rootname [lrange [file split $bd_filename] end end]]
set BD_name [file rootname [file tail $bd_filename]]
set xdc_filename [lindex $argv 4]
#set design_name $BD_name
#set ip_repo_path [pwd]/ip_repo
set ip_repo_path [lindex $argv 5]
set ip_cache_path [pwd]/ip_cache
set current_vivado_version [version -short]

# Create a new project
create_project $project_name $project_path -part $part_name
set_property target_language Verilog [current_project]

# Create 'sources_1' fileset (if not found)
if {[string equal [get_filesets -quiet sources_1] ""]} {
  create_fileset -srcset sources_1
}
# Add hdl files
#add_files -fileset sources_1 -norecurse -scan_for_includes ./hdl
#add_files -fileset sources_1 -norecurse -scan_for_includes ./hdl/hdmi
#import_files -fileset sources_1 -norecurse ./hdl
#add_files -fileset sources_1 -norecurse ./hdl/ip/clk_200M/clk_200M.xci
#add_files -fileset sources_1 -norecurse ./hdl/ip/clk_94m/clk_94m.xci
#add_files -fileset sources_1 -norecurse ./hdl/ip/clk_pixel/clk_pixel.xci
#add_files -fileset sources_1 -norecurse ./hdl/clk_200M.xcix
#add_files -fileset sources_1 -norecurse ./hdl/clk_94m.xcix
#add_files -fileset sources_1 -norecurse ./hdl/clk_pixel.xcix
#add_files -fileset sources_1 -norecurse ./hdl/ip/mig_7series_0/mig_7series_0.xci

# Create 'constrs_1' fileset (if not found)
if {[string equal [get_filesets -quiet constrs_1] ""]} {
  create_fileset -constrset -quiet constrs_1
}
# Add constraint files
add_files -fileset constrs_1 -norecurse -scan_for_includes $xdc_filename
import_files -fileset constrs_1 -norecurse $xdc_filename

# Create 'sim_1' fileset (if not found)
if {[string equal [get_filesets -quiet sim_1] ""]} {
  create_fileset -simset sim_1
}
# Add simulation files
#add_files -fileset sim_1 -norecurse -scan_for_includes ./src/sim
#import_files -fileset sim_1 -norecurse ./src/sim


# Add ip repository
set_property ip_repo_paths $ip_repo_path [current_project]
config_ip_cache -use_cache_location $ip_cache_path
update_ip_catalog

# Build the Block Design
source ./bd/system.tcl

# Validate the BD
regenerate_bd_layout
validate_bd_design 
save_bd_design

# Create the HDL wrapper
export_ip_user_files -of_objects [get_files $project_path/$project_name.srcs/sources_1/bd/$BD_name/$BD_name.bd] -sync -no_script -force -quiet
make_wrapper -files [get_files $project_path/$project_name.srcs/sources_1/bd/$BD_name/$BD_name.bd] -top
#make_wrapper -files [get_files ./$project_name/$project_name.srcs/sources_1/top.v] -top


# Add the wrapper to the fileset
set obj [get_filesets sources_1]
if { [string first $current_vivado_version "2020.2 2021.1"] != -1 } {
    set files [list "[file normalize [glob "$project_path/$project_name.gen/sources_1/bd/$BD_name/hdl/*_wrapper*"]]"]
} elseif { [string first $current_vivado_version "2018.1 2018.2 2018.3 2019.1 2019.2 2020.1"] != -1 } {
    set files [list "[file normalize [glob "$project_path/$project_name.srcs/sources_1/bd/$BD_name/hdl/*_wrapper*"]]"]
}
#set files [list \
#               [file normalize $origin_dir/generated-src/Top.$CONFIG.v] \
#               [file normalize $base_dir/src/main/verilog/chip_top.sv] \
#               [file normalize $base_dir/socip/nasti/channel.sv] \
#               ...
#              ]
add_files -norecurse -fileset $obj $files


update_compile_order -fileset sources_1
#update_compile_order -fileset sim_1

#write_bd_layout -format pdf -orientation portrait -force ./$BD_name.pdf

#set_property STEPS.WRITE_BITSTREAM.ARGS.BIN_FILE true [get_runs impl_1]

######
#exit
######
