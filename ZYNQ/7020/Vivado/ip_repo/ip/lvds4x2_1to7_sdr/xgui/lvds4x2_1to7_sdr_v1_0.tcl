# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  ipgui::add_param $IPINST -name "D" -parent ${Page_0}
  ipgui::add_param $IPINST -name "N" -parent ${Page_0}

  ipgui::add_param $IPINST -name "DIFF_TERM" -widget comboBox

}

proc update_PARAM_VALUE.D { PARAM_VALUE.D } {
	# Procedure called to update D when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.D { PARAM_VALUE.D } {
	# Procedure called to validate D
	return true
}

proc update_PARAM_VALUE.DIFF_TERM { PARAM_VALUE.DIFF_TERM } {
	# Procedure called to update DIFF_TERM when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.DIFF_TERM { PARAM_VALUE.DIFF_TERM } {
	# Procedure called to validate DIFF_TERM
	return true
}

proc update_PARAM_VALUE.N { PARAM_VALUE.N } {
	# Procedure called to update N when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.N { PARAM_VALUE.N } {
	# Procedure called to validate N
	return true
}


proc update_MODELPARAM_VALUE.D { MODELPARAM_VALUE.D PARAM_VALUE.D } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.D}] ${MODELPARAM_VALUE.D}
}

proc update_MODELPARAM_VALUE.N { MODELPARAM_VALUE.N PARAM_VALUE.N } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.N}] ${MODELPARAM_VALUE.N}
}

proc update_MODELPARAM_VALUE.DIFF_TERM { MODELPARAM_VALUE.DIFF_TERM PARAM_VALUE.DIFF_TERM } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.DIFF_TERM}] ${MODELPARAM_VALUE.DIFF_TERM}
}

