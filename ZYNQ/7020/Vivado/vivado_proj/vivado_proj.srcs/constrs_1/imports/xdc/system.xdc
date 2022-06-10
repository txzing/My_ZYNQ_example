set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]

# set_property  PULLUP true [get_ports {iic_rtl_0_scl_io}]
# set_property  PULLUP true [get_ports {iic_rtl_0_sda_io}]

# PL LED 0
set_property -dict {PACKAGE_PIN J18 IOSTANDARD LVCMOS33} [get_ports {EMIO_tri_io[0]}]
# PL LED 1
set_property -dict {PACKAGE_PIN H18 IOSTANDARD LVCMOS33} [get_ports {EMIO_tri_io[1]}]
# PL key 0
set_property -dict {PACKAGE_PIN L20 IOSTANDARD LVCMOS33} [get_ports {EMIO_tri_io[2]}]

# # PL key 1
# set_property -dict {PACKAGE_PIN J20 IOSTANDARD LVCMOS33} [get_ports {EMIO_tri_io[3]}]