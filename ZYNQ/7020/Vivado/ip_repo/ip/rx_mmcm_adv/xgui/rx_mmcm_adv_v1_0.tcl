# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  ipgui::add_param $IPINST -name "MMCM_MODE" -parent ${Page_0} -widget comboBox
  ipgui::add_param $IPINST -name "CLKIN_PERIOD" -parent ${Page_0}
  ipgui::add_param $IPINST -name "USE_PLL" -parent ${Page_0} -widget comboBox
  ipgui::add_param $IPINST -name "PIXEL_CLOCK" -parent ${Page_0} -widget comboBox
  ipgui::add_param $IPINST -name "SAMPL_CLOCK" -parent ${Page_0} -widget comboBox


}

proc update_PARAM_VALUE.CLKIN_PERIOD { PARAM_VALUE.CLKIN_PERIOD } {
	# Procedure called to update CLKIN_PERIOD when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.CLKIN_PERIOD { PARAM_VALUE.CLKIN_PERIOD } {
	# Procedure called to validate CLKIN_PERIOD
	return true
}

proc update_PARAM_VALUE.MMCM_MODE { PARAM_VALUE.MMCM_MODE } {
	# Procedure called to update MMCM_MODE when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.MMCM_MODE { PARAM_VALUE.MMCM_MODE } {
	# Procedure called to validate MMCM_MODE
	return true
}

proc update_PARAM_VALUE.PIXEL_CLOCK { PARAM_VALUE.PIXEL_CLOCK } {
	# Procedure called to update PIXEL_CLOCK when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.PIXEL_CLOCK { PARAM_VALUE.PIXEL_CLOCK } {
	# Procedure called to validate PIXEL_CLOCK
	return true
}

proc update_PARAM_VALUE.SAMPL_CLOCK { PARAM_VALUE.SAMPL_CLOCK } {
	# Procedure called to update SAMPL_CLOCK when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.SAMPL_CLOCK { PARAM_VALUE.SAMPL_CLOCK } {
	# Procedure called to validate SAMPL_CLOCK
	return true
}

proc update_PARAM_VALUE.USE_PLL { PARAM_VALUE.USE_PLL } {
	# Procedure called to update USE_PLL when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.USE_PLL { PARAM_VALUE.USE_PLL } {
	# Procedure called to validate USE_PLL
	return true
}


proc update_MODELPARAM_VALUE.MMCM_MODE { MODELPARAM_VALUE.MMCM_MODE PARAM_VALUE.MMCM_MODE } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.MMCM_MODE}] ${MODELPARAM_VALUE.MMCM_MODE}
}

proc update_MODELPARAM_VALUE.CLKIN_PERIOD { MODELPARAM_VALUE.CLKIN_PERIOD PARAM_VALUE.CLKIN_PERIOD } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.CLKIN_PERIOD}] ${MODELPARAM_VALUE.CLKIN_PERIOD}
}

proc update_MODELPARAM_VALUE.USE_PLL { MODELPARAM_VALUE.USE_PLL PARAM_VALUE.USE_PLL } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.USE_PLL}] ${MODELPARAM_VALUE.USE_PLL}
}

proc update_MODELPARAM_VALUE.PIXEL_CLOCK { MODELPARAM_VALUE.PIXEL_CLOCK PARAM_VALUE.PIXEL_CLOCK } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.PIXEL_CLOCK}] ${MODELPARAM_VALUE.PIXEL_CLOCK}
}

proc update_MODELPARAM_VALUE.SAMPL_CLOCK { MODELPARAM_VALUE.SAMPL_CLOCK PARAM_VALUE.SAMPL_CLOCK } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.SAMPL_CLOCK}] ${MODELPARAM_VALUE.SAMPL_CLOCK}
}

