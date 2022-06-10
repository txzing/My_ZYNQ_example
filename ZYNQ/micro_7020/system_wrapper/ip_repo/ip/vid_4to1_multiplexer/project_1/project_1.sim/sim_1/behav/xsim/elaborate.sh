#!/bin/bash -f
# ****************************************************************************
# Vivado (TM) v2020.2 (64-bit)
#
# Filename    : elaborate.sh
# Simulator   : Xilinx Vivado Simulator
# Description : Script for elaborating the compiled design
#
# Generated by Vivado on Thu Apr 28 12:48:06 CST 2022
# SW Build 3064766 on Wed Nov 18 09:12:47 MST 2020
#
# Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
#
# usage: elaborate.sh
#
# ****************************************************************************
set -Eeuo pipefail
# elaborate design
echo "xelab -wto d1bf9dc51a4c4c7dbe320672d340661d --incr --debug typical --relax --mt 8 -L xil_defaultlib -L unisims_ver -L unimacro_ver -L secureip --snapshot vid_multiplexer_tb_behav xil_defaultlib.vid_multiplexer_tb xil_defaultlib.glbl -log elaborate.log"
xelab -wto d1bf9dc51a4c4c7dbe320672d340661d --incr --debug typical --relax --mt 8 -L xil_defaultlib -L unisims_ver -L unimacro_ver -L secureip --snapshot vid_multiplexer_tb_behav xil_defaultlib.vid_multiplexer_tb xil_defaultlib.glbl -log elaborate.log
