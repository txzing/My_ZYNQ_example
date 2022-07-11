set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]

set_property PACKAGE_PIN AG13 [get_ports uart_0_rxd]
set_property IOSTANDARD LVCMOS33 [get_ports uart_0_rxd]
set_property PACKAGE_PIN W11 [get_ports uart_0_txd]
set_property IOSTANDARD LVCMOS33 [get_ports uart_0_txd]
set_property PACKAGE_PIN AH13 [get_ports {uart_0_txen[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {uart_0_txen[0]}]

# Sil9136-3
## sda
set_property PACKAGE_PIN AA12 [get_ports i2c_0_sda_io]
set_property IOSTANDARD LVCMOS33 [get_ports i2c_0_sda_io]
## scl
set_property PACKAGE_PIN Y12 [get_ports i2c_0_scl_io]
set_property IOSTANDARD LVCMOS33 [get_ports i2c_0_scl_io]


# ch_0
## sda
set_property PACKAGE_PIN G5 [get_ports i2c_1_sda_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_1_sda_io]
## scl
set_property PACKAGE_PIN F5 [get_ports i2c_1_scl_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_1_scl_io]

# ch_1
## sda
set_property PACKAGE_PIN F3 [get_ports i2c_2_sda_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_2_sda_io]
## scl
set_property PACKAGE_PIN D4 [get_ports i2c_2_scl_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_2_scl_io]

# ch_2
## sda
set_property PACKAGE_PIN A1 [get_ports i2c_3_sda_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_3_sda_io]
## scl
set_property PACKAGE_PIN A2 [get_ports i2c_3_scl_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_3_scl_io]

# ch_3
## sda
set_property PACKAGE_PIN D5 [get_ports i2c_4_sda_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_4_sda_io]
## scl
set_property PACKAGE_PIN C8 [get_ports i2c_4_scl_io]
set_property IOSTANDARD LVCMOS18 [get_ports i2c_4_scl_io]

# SiI9136-3 timing
set_property PACKAGE_PIN AC13 [get_ports vid_clk]
set_property IOSTANDARD LVCMOS33 [get_ports vid_clk]

set_property PACKAGE_PIN Y14 [get_ports vid_de]
set_property IOSTANDARD LVCMOS33 [get_ports vid_de]

set_property PACKAGE_PIN AC14 [get_ports vid_vs]
set_property IOSTANDARD LVCMOS33 [get_ports vid_vs]

set_property PACKAGE_PIN Y13 [get_ports vid_hs]
set_property IOSTANDARD LVCMOS33 [get_ports vid_hs]

# SiI9136-3 data
set_property PACKAGE_PIN L13 [get_ports {vid_data[0]}]
set_property PACKAGE_PIN L14 [get_ports {vid_data[1]}]
set_property PACKAGE_PIN C14 [get_ports {vid_data[2]}]
set_property PACKAGE_PIN C13 [get_ports {vid_data[3]}]
set_property PACKAGE_PIN B14 [get_ports {vid_data[4]}]
set_property PACKAGE_PIN A14 [get_ports {vid_data[5]}]
set_property PACKAGE_PIN B13 [get_ports {vid_data[6]}]
set_property PACKAGE_PIN A13 [get_ports {vid_data[7]}]
set_property PACKAGE_PIN K14 [get_ports {vid_data[8]}]
set_property PACKAGE_PIN J14 [get_ports {vid_data[9]}]
set_property PACKAGE_PIN H13 [get_ports {vid_data[10]}]
set_property PACKAGE_PIN H14 [get_ports {vid_data[11]}]
set_property PACKAGE_PIN F13 [get_ports {vid_data[12]}]
set_property PACKAGE_PIN G13 [get_ports {vid_data[13]}]
set_property PACKAGE_PIN G14 [get_ports {vid_data[14]}]
set_property PACKAGE_PIN G15 [get_ports {vid_data[15]}]
set_property PACKAGE_PIN A11 [get_ports {vid_data[16]}]
set_property PACKAGE_PIN A12 [get_ports {vid_data[17]}]
set_property PACKAGE_PIN C12 [get_ports {vid_data[18]}]
set_property PACKAGE_PIN D12 [get_ports {vid_data[19]}]
set_property PACKAGE_PIN A10 [get_ports {vid_data[20]}]
set_property PACKAGE_PIN B11 [get_ports {vid_data[21]}]
set_property PACKAGE_PIN B10 [get_ports {vid_data[22]}]
set_property PACKAGE_PIN C11 [get_ports {vid_data[23]}]

set_property IOSTANDARD LVCMOS33 [get_ports {vid_data[*]}]

# relay_ctrl
set_property PACKAGE_PIN AG11 [get_ports {gpio_tri_io[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gpio_tri_io[0]}]

# SiI9136-3 reset
set_property PACKAGE_PIN AE15 [get_ports {gpio_tri_io[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gpio_tri_io[1]}]


#gpio for maxim chips

#chip0_mfp0(fsync)
set_property PACKAGE_PIN E2 [get_ports {gpio_tri_io[2]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[2]}]

#chi0_lock
set_property PACKAGE_PIN F1 [get_ports {gpio_tri_io[3]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[3]}]

#chip0_errb
set_property PACKAGE_PIN G1 [get_ports {gpio_tri_io[4]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[4]}]

#chip0_mfp6
set_property PACKAGE_PIN G8 [get_ports {gpio_tri_io[5]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[5]}]

#chip0_pwdnb
set_property PACKAGE_PIN F2 [get_ports {gpio_tri_io[6]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[6]}]

#chip1_mfp0(fsync)
set_property PACKAGE_PIN F7 [get_ports {gpio_tri_io[7]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[7]}]

#chip1_lock
set_property PACKAGE_PIN F6 [get_ports {gpio_tri_io[8]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[8]}]

#chip1_errb
set_property PACKAGE_PIN G3 [get_ports {gpio_tri_io[9]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[9]}]

#chip1_mfp6
set_property PACKAGE_PIN G4 [get_ports {gpio_tri_io[10]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[10]}]

#chip1_pwdnb
set_property PACKAGE_PIN G6 [get_ports {gpio_tri_io[11]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[11]}]

#chip2_mfp0(fsync)
set_property PACKAGE_PIN C2 [get_ports {gpio_tri_io[12]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[12]}]

#chip2_lock
set_property PACKAGE_PIN D6 [get_ports {gpio_tri_io[13]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[13]}]

#chip2_errb
set_property PACKAGE_PIN D7 [get_ports {gpio_tri_io[14]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[14]}]

#chip2_mfp6
set_property PACKAGE_PIN E8 [get_ports {gpio_tri_io[15]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[15]}]

#chip2_pwdnb
set_property PACKAGE_PIN C3 [get_ports {gpio_tri_io[16]}]
set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[16]}]

#chip3_mfp0(fsync)
#set_property PACKAGE_PIN A7 [get_ports {gpio_tri_io[17]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[17]}]

#chip3_lock
#set_property PACKAGE_PIN A6 [get_ports {gpio_tri_io[18]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[18]}]

#chip3_errb
#set_property PACKAGE_PIN E5 [get_ports {gpio_tri_io[19]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[19]}]

#chip3_mfp6
#set_property PACKAGE_PIN B8 [get_ports {gpio_tri_io[20]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[20]}]

#chip3_pwdnb
#set_property PACKAGE_PIN F8 [get_ports {gpio_tri_io[21]}]
#set_property IOSTANDARD LVCMOS18 [get_ports {gpio_tri_io[21]}]

# 0_792_CFG1_CTL
set_property PACKAGE_PIN AG14 [get_ports {gpio_tri_io[17]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gpio_tri_io[17]}]
# 1_792_CFG1_CTL
set_property PACKAGE_PIN AH14 [get_ports {gpio_tri_io[18]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gpio_tri_io[18]}]
# 0_9296_CFG1_CTL
set_property PACKAGE_PIN AC11 [get_ports {gpio_tri_io[19]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gpio_tri_io[19]}]
## 1_9296_CFG1_CTL  VALID
#set_property PACKAGE_PIN AB11 [get_ports {gpio_tri_io[20]}]
#set_property IOSTANDARD LVCMOS33 [get_ports {gpio_tri_io[20]}]

#
#set_property PACKAGE_PIN F8 [get_ports hdmi_tx0_clk_p]
#set_property IOSTANDARD LVDS [get_ports hdmi_tx0_clk_p]

#set_property PACKAGE_PIN B5 [get_ports {hdmi_tx0_data_p[0]}]
#set_property PACKAGE_PIN A9 [get_ports {hdmi_tx0_data_p[1]}]
#set_property PACKAGE_PIN E9 [get_ports {hdmi_tx0_data_p[2]}]
#set_property IOSTANDARD LVDS [get_ports {hdmi_tx0_data_p[*]}]

#
#set_property PACKAGE_PIN B4 [get_ports hdmi_tx1_clk_p]
#set_property IOSTANDARD LVDS [get_ports hdmi_tx1_clk_p]

#set_property PACKAGE_PIN C1 [get_ports {hdmi_tx1_data_p[0]}]
#set_property PACKAGE_PIN C6 [get_ports {hdmi_tx1_data_p[1]}]
#set_property PACKAGE_PIN E1 [get_ports {hdmi_tx1_data_p[2]}]
#set_property IOSTANDARD LVDS [get_ports {hdmi_tx1_data_p[*]}]

