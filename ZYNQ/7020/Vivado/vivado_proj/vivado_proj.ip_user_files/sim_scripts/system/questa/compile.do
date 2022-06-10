vlib questa_lib/work
vlib questa_lib/msim

vlib questa_lib/msim/xilinx_vip
vlib questa_lib/msim/axi_infrastructure_v1_1_0
vlib questa_lib/msim/axi_vip_v1_1_8
vlib questa_lib/msim/processing_system7_vip_v1_0_10
vlib questa_lib/msim/xil_defaultlib

vmap xilinx_vip questa_lib/msim/xilinx_vip
vmap axi_infrastructure_v1_1_0 questa_lib/msim/axi_infrastructure_v1_1_0
vmap axi_vip_v1_1_8 questa_lib/msim/axi_vip_v1_1_8
vmap processing_system7_vip_v1_0_10 questa_lib/msim/processing_system7_vip_v1_0_10
vmap xil_defaultlib questa_lib/msim/xil_defaultlib

vlog -work xilinx_vip  -sv -L axi_vip_v1_1_8 -L processing_system7_vip_v1_0_10 -L xilinx_vip "+incdir+D:/Vivado/Vivado/2020.2/data/xilinx_vip/include" \
"D:/Vivado/Vivado/2020.2/data/xilinx_vip/hdl/axi4stream_vip_axi4streampc.sv" \
"D:/Vivado/Vivado/2020.2/data/xilinx_vip/hdl/axi_vip_axi4pc.sv" \
"D:/Vivado/Vivado/2020.2/data/xilinx_vip/hdl/xil_common_vip_pkg.sv" \
"D:/Vivado/Vivado/2020.2/data/xilinx_vip/hdl/axi4stream_vip_pkg.sv" \
"D:/Vivado/Vivado/2020.2/data/xilinx_vip/hdl/axi_vip_pkg.sv" \
"D:/Vivado/Vivado/2020.2/data/xilinx_vip/hdl/axi4stream_vip_if.sv" \
"D:/Vivado/Vivado/2020.2/data/xilinx_vip/hdl/axi_vip_if.sv" \
"D:/Vivado/Vivado/2020.2/data/xilinx_vip/hdl/clk_vip_if.sv" \
"D:/Vivado/Vivado/2020.2/data/xilinx_vip/hdl/rst_vip_if.sv" \

vlog -work axi_infrastructure_v1_1_0  "+incdir+../../../../vivado_proj.gen/sources_1/bd/system/ipshared/ec67/hdl" "+incdir+../../../../vivado_proj.gen/sources_1/bd/system/ipshared/34f8/hdl" "+incdir+D:/Vivado/Vivado/2020.2/data/xilinx_vip/include" \
"../../../../vivado_proj.gen/sources_1/bd/system/ipshared/ec67/hdl/axi_infrastructure_v1_1_vl_rfs.v" \

vlog -work axi_vip_v1_1_8  -sv -L axi_vip_v1_1_8 -L processing_system7_vip_v1_0_10 -L xilinx_vip "+incdir+../../../../vivado_proj.gen/sources_1/bd/system/ipshared/ec67/hdl" "+incdir+../../../../vivado_proj.gen/sources_1/bd/system/ipshared/34f8/hdl" "+incdir+D:/Vivado/Vivado/2020.2/data/xilinx_vip/include" \
"../../../../vivado_proj.gen/sources_1/bd/system/ipshared/94c3/hdl/axi_vip_v1_1_vl_rfs.sv" \

vlog -work processing_system7_vip_v1_0_10  -sv -L axi_vip_v1_1_8 -L processing_system7_vip_v1_0_10 -L xilinx_vip "+incdir+../../../../vivado_proj.gen/sources_1/bd/system/ipshared/ec67/hdl" "+incdir+../../../../vivado_proj.gen/sources_1/bd/system/ipshared/34f8/hdl" "+incdir+D:/Vivado/Vivado/2020.2/data/xilinx_vip/include" \
"../../../../vivado_proj.gen/sources_1/bd/system/ipshared/34f8/hdl/processing_system7_vip_v1_0_vl_rfs.sv" \

vlog -work xil_defaultlib  "+incdir+../../../../vivado_proj.gen/sources_1/bd/system/ipshared/ec67/hdl" "+incdir+../../../../vivado_proj.gen/sources_1/bd/system/ipshared/34f8/hdl" "+incdir+D:/Vivado/Vivado/2020.2/data/xilinx_vip/include" \
"../../../bd/system/ip/system_processing_system7_0_0/sim/system_processing_system7_0_0.v" \
"../../../bd/system/sim/system.v" \

vlog -work xil_defaultlib \
"glbl.v"

