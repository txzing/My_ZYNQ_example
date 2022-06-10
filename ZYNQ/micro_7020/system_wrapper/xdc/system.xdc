set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]

#set_property  PULLUP true [get_ports {iic_rtl_0_scl_io}]

#PL KEY0
set_property -dict {PACKAGE_PIN M19 IOSTANDARD LVCMOS33} [get_ports {GPIO_0_0_tri_io[0]}]
#PL KEY1
set_property -dict {PACKAGE_PIN M20 IOSTANDARD LVCMOS33} [get_ports {GPIO_0_0_tri_io[1]}]
#PL LED0
set_property -dict {PACKAGE_PIN M15 IOSTANDARD LVCMOS33} [get_ports {GPIO_0_0_tri_io[2]}]