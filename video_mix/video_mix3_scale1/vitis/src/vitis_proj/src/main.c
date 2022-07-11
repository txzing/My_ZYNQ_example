#include <stdio.h>
#include "xparameters.h"
#include "platform.h"
#include "platform_config.h"
#include "xil_types.h"
#include "xstatus.h"
#include "xil_exception.h"
#include "sleep.h"
#include "xil_cache.h"
#if defined (__arm__) || defined(__aarch64__)
#include "xil_printf.h"
#endif
#if defined (__MICROBLAZE__)
#include "xintc.h"
#else
#include "xscugic.h"
#endif
#include "xgpio.h"
#include "xclk_wiz.h"
#include "xvtc.h"
#include "xaxivdma.h"
//#include "xv_frmbufrd_l2.h"
//#include "xv_frmbufwr_l2.h"
#include "xvprocss.h"
#include "xaxis_switch.h"
#include "xvidc.h"
#include "xv_tpg.h"
#include "tpg/tpg.h"
#include "vtc/video_resolution.h"
#include "vtc/vtiming_gen.h"
#include "xgpio_i2c/xgpio_i2c.h"
//#include "xiicps.h"
//#include "PS_i2c.h"
#include "clk_wiz/clk_wiz.h"

#include "bitmanip.h"
#include "trace_zzg_debug.h"
#include "config.h"


#include "axis_passthrough_monitor.h"


#define IIC_SCLK_RATE		100000

#if defined(__MICROBLAZE__)
#define DDR_BASEADDR XPAR_MIG7SERIES_0_BASEADDR
#else
#define DDR_BASEADDR XPAR_DDR_MEM_BASEADDR
#endif

#define FRAME_BUFFER_SIZE0      0x2000000    //0x2000000 for max 4KW RGB888 8bpc
//#define FRAME_BUFFER_SIZE0      0x600000    //0x600000 for max 1080p RGB888 8bpc
#define FRAME_BUFFER_BASE_ADDR  (DDR_BASEADDR + (0x10000000))
#define FRAME_BUFFER_1          FRAME_BUFFER_BASE_ADDR
#define FRAME_BUFFER_2          FRAME_BUFFER_BASE_ADDR + FRAME_BUFFER_SIZE0
#define FRAME_BUFFER_3          FRAME_BUFFER_BASE_ADDR + (FRAME_BUFFER_SIZE0*2)

#define TEST_3_MIX                0

#if !TEST_3_MIX
#define TEST_channel_4_scaler  1
#endif

//XAxis_Switch AxisSwitch4;

XClk_Wiz ClkWizInst0;

XVtc        VtcInst0;       /**< Instance of the VTC core. */
XVtc_Config *VtcConfig0;
XV_tpg tpg_inst0;
XV_tpg_Config *tpg_config0;
XV_tpg tpg_inst1;
XV_tpg_Config *tpg_config1;
XV_tpg tpg_inst2;
XV_tpg_Config *tpg_config2;
XV_tpg tpg_inst3;
XV_tpg_Config *tpg_config3;

XVidC_ColorFormat colorFmtIn0 = XVIDC_CSF_RGB;
//XVidC_ColorFormat colorFmtIn0 = XVIDC_CSF_YCRCB_422;
XVidC_ColorFormat colorFmtOut0 = XVIDC_CSF_RGB;
XVidC_ColorFormat colorFmtIn1 = XVIDC_CSF_RGB;
XVidC_ColorFormat colorFmtOut1 = XVIDC_CSF_RGB;
XVidC_ColorFormat colorFmtIn2 = XVIDC_CSF_RGB;
XVidC_ColorFormat colorFmtOut2 = XVIDC_CSF_RGB;
XVidC_ColorFormat colorFmtIn3 = XVIDC_CSF_RGB;
XVidC_ColorFormat colorFmtOut3 = XVIDC_CSF_RGB;
u32 bckgndId0=XTPG_BKGND_CHECKER_BOARD;
u32 bckgndId1=XTPG_BKGND_SOLID_BLUE;
u32 bckgndId2=XTPG_BKGND_DP_COLOR_SQUARE;
u32 bckgndId3=XTPG_BKGND_COLOR_BARS;

XVidC_VideoMode resId;
XVidC_VideoStream StreamIn, StreamOut;
XVidC_VideoTiming const *TimingPtr;

XVprocSs VprocInst0;
XVprocSs VprocInst1;
XVprocSs_Config *VprocCfgPtr;
XVidC_VideoMode resId;
XVidC_VideoStream StreamIn, StreamOut;
XVidC_VideoTiming const *TimingPtr;
XVidC_FrameRate fpsIn = XVIDC_FR_30HZ;
XVidC_FrameRate fpsOut = XVIDC_FR_30HZ;

/* Assign Mode ID Enumeration. First entry Must be > XVIDC_VM_CUSTOM */
typedef enum {
	XVIDC_VM_1280x3840_30_P = (XVIDC_VM_CUSTOM + 1),
	XVIDC_VM_320x960_30_P,
	XVIDC_VM_1920x2160_30_P,
	XVIDC_VM_960x1080_30_P,
	XVIDC_VM_1920x2560_30_P,
	XVIDC_VM_1600x2560_30_P,
	XVIDC_VM_670x2160_30_P,
    XVIDC_CM_NUM_SUPPORTED

} XVIDC_CUSTOM_MODES;

/* Create entry for each mode in the custom table */
const XVidC_VideoTimingMode XVidC_MyVideoTimingMode[(XVIDC_CM_NUM_SUPPORTED - (XVIDC_VM_CUSTOM + 1))] =
{
    { XVIDC_VM_1280x3840_30_P, "1280x3840@30Hz", XVIDC_FR_30HZ,
        {1280, 110, 40, 220, 1650, 1,
        		3840, 5, 5, 20, 3870, 0, 0, 0, 0, 1} },
    { XVIDC_VM_320x960_30_P, "320x960@30Hz", XVIDC_FR_30HZ,
        {320, 10, 10, 10, 350, 1,
        		960, 5, 5, 10, 980, 0, 0, 0, 0, 1} },
    { XVIDC_VM_1920x2160_30_P, "1920x2160@30Hz", XVIDC_FR_30HZ,
		{1920, 10, 10, 10, 1950, 1,
				2160, 5, 5, 10, 2180, 0, 0, 0, 0, 1} },
	{ XVIDC_VM_960x1080_30_P, "960x1080@30Hz", XVIDC_FR_30HZ,
		{960, 10, 10, 10, 990, 1,
				1080, 5, 5, 10, 1100, 0, 0, 0, 0, 1} },
    { XVIDC_VM_1920x2560_30_P, "1920x2560@30Hz", XVIDC_FR_30HZ,
		{1920, 10, 10, 10, 1950, 1,
				2560, 5, 5, 10, 2580, 0, 0, 0, 0, 1} },
    { XVIDC_VM_1600x2560_30_P, "1600x2560@30Hz", XVIDC_FR_30HZ,
		{1600, 10, 10, 10, 1630, 1,
				2560, 5, 5, 10, 2580, 0, 0, 0, 0, 1} },
    { XVIDC_VM_670x2160_30_P, "670x2160@30Hz", XVIDC_FR_30HZ,
		{670, 10, 10, 10, 700, 1,
				2160, 5, 5, 10, 2180, 0, 0, 0, 0, 1} }
};



void clkwiz_vtc_cfg(void)
{
    u32 Status;
    // dynamic config clkwiz
    Status = XClk_Wiz_dynamic_reconfig(&ClkWizInst0, XPAR_VID_OUT_SS_CLK_WIZ_0_DEVICE_ID);
    if (Status != XST_SUCCESS)
    {
      xil_printf("XClk_Wiz0 dynamic reconfig failed.\r\n");
//      return XST_FAILURE;
    }
    //xil_printf("XClk_Wiz0 dynamic reconfig ok\n\r");

    // vtc configuration
    VtcConfig0 = XVtc_LookupConfig(XPAR_VID_OUT_SS_V_TC_0_DEVICE_ID);
    Status = XVtc_CfgInitialize(&VtcInst0, VtcConfig0, VtcConfig0->BaseAddress);
    if(Status != XST_SUCCESS)
    {
        xil_printf("VTC0 Initialization failed %d\r\n", Status);
//      return(XST_FAILURE);
    }
    vtiming_gen_run(&VtcInst0, VIDEO_RESOLUTION_4K, 0);
}

void tpg_config()
{
    u32 Status;

#if TEST_3_MIX
    // tpg0
    //xil_printf("TPG0 Initializing\n\r");

    Status = XV_tpg_Initialize(&tpg_inst0, XPAR_TPG_V_TPG_0_DEVICE_ID);
    if(Status!= XST_SUCCESS)
    {
        xil_printf("TPG0 configuration failed\r\n");
//      return(XST_FAILURE);
    }

    //Configure the TPG0
    tpg_cfg(&tpg_inst0, 2160, 1920, colorFmtIn0, bckgndId0);

    //Configure the moving box of the TPG0
    tpg_box(&tpg_inst0, 50, 2);

    //Start the TPG0
    XV_tpg_EnableAutoRestart(&tpg_inst0);
    XV_tpg_Start(&tpg_inst0);
    //xil_printf("TPG0 started!\r\n");

//////////////////////////////////////////////////
    // tpg1
    //xil_printf("TPG1 Initializing\n\r");

    Status = XV_tpg_Initialize(&tpg_inst1, XPAR_TPG_V_TPG_1_DEVICE_ID);
    if(Status!= XST_SUCCESS)
    {
        xil_printf("TPG1 configuration failed\r\n");
//      return(XST_FAILURE);
    }

    //Configure the TPG1,4K
    tpg_cfg(&tpg_inst1, 2160, 3840, colorFmtIn1, bckgndId1);

    //Configure the moving box of the TPG1
    tpg_box(&tpg_inst1, 50, 2);
    XV_tpg_Set_boxColorR(&tpg_inst1,0xff);

    //Start the TPG1
    XV_tpg_EnableAutoRestart(&tpg_inst1);
    XV_tpg_Start(&tpg_inst1);
    //xil_printf("TPG1 started!\r\n");

//////////////////////////////////////////////////
    // tpg2
    //xil_printf("TPG2 Initializing\n\r");

    Status = XV_tpg_Initialize(&tpg_inst2, XPAR_TPG_V_TPG_2_DEVICE_ID);
    if(Status!= XST_SUCCESS)
    {
        xil_printf("TPG2 configuration failed\r\n");
//      return(XST_FAILURE);
    }

    //Configure the TPG2
    tpg_cfg(&tpg_inst2, 1080, 1920, colorFmtIn2, bckgndId2);

    //Configure the moving box of the TPG2
    tpg_box(&tpg_inst2, 50, 2);
    XV_tpg_Set_boxColorR(&tpg_inst2,0xff);
    XV_tpg_Set_boxColorG(&tpg_inst2,0xff);
    XV_tpg_Set_boxColorB(&tpg_inst2,0xff);

    //Start the TPG2
    XV_tpg_EnableAutoRestart(&tpg_inst2);
    XV_tpg_Start(&tpg_inst2);
    //xil_printf("TPG2 started!\r\n");

#endif

//////////////////////////////////////////////////
#if TEST_channel_4_scaler
    // tpg3
	//xil_printf("TPG2 Initializing\n\r");

	Status = XV_tpg_Initialize(&tpg_inst3, XPAR_TPG_V_TPG_3_DEVICE_ID);
	if(Status!= XST_SUCCESS)
	{
		xil_printf("TPG3 configuration failed\r\n");
//      return(XST_FAILURE);
	}

	//Configure the TPG3
	tpg_cfg(&tpg_inst3, 5120, 1600, colorFmtIn3, bckgndId3);

	//Configure the moving box of the TPG3
	tpg_box(&tpg_inst3, 50, 2);
	XV_tpg_Set_boxColorR(&tpg_inst3,0xff);
	XV_tpg_Set_boxColorG(&tpg_inst3,0xff);
	XV_tpg_Set_boxColorB(&tpg_inst3,0xff);

	//Start the TPG3
	XV_tpg_EnableAutoRestart(&tpg_inst3);
	XV_tpg_Start(&tpg_inst3);
	//xil_printf("TPG3 started!\r\n");
#endif

}


void vdma_config(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = 3;

#if TEST_3_MIX
    int offset0 = (0*3840+0)*3; // (y*w+x)*Bpp  channel_1 write
    u32 stride0 = 3840;
    u32 width0 = 1920;
    u32 height0 = 2160;

    int offset1 = (0*3840+1920)*3; // (y*w+x)*Bpp  channel_2 write
    u32 stride1 = 3840;
    u32 width1 = 1920;
    u32 height1 = 1080;

    int offset2 = (1080*3840+1920)*3; // (y*w+x)*Bpp  channel_3 write
    u32 stride2 = 3840;
    u32 width2 = 1920;
    u32 height2 = 1080;
#endif
#if TEST_channel_4_scaler
    int offset3 = (0*3840+1500)*3; // (y*w+x)*Bpp  channel_4 write
    u32 stride3 = 3840;
    u32 width3 = 670;
    u32 height3 = 2160;
#endif

    int offset4 = 0; // (y*w+x)*Bpp read
    u32 stride4 = 3840;  // crop keeps write Stride
    u32 width4 = 3840;
    u32 height4 = 2160;


#if TEST_3_MIX
/**************channel 1 write**************/
    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_0_BASEADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_0_BASEADDR + 0xAC, FRAME_BUFFER_1 + offset0);
    //S2MM Start Address 2
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_0_BASEADDR + 0xB0, FRAME_BUFFER_2 + offset0);
    //S2MM Start Address 3
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_0_BASEADDR + 0xB4, FRAME_BUFFER_3 + offset0);
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_0_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_0_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_0_BASEADDR + 0xA0, height0);

/**************channel 2 write**************/
    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_1_BASEADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_1_BASEADDR + 0xAC, FRAME_BUFFER_1 + offset1);
    //S2MM Start Address 2
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_1_BASEADDR + 0xB0, FRAME_BUFFER_2 + offset1);
    //S2MM Start Address 3
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_1_BASEADDR + 0xB4, FRAME_BUFFER_3 + offset1);
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_1_BASEADDR + 0xA8, stride1*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_1_BASEADDR + 0xA4, width1*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_1_BASEADDR + 0xA0, height1);

/**************channel 3 write**************/
    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_2_BASEADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_2_BASEADDR + 0xAC, FRAME_BUFFER_1 + offset2);
    //S2MM Start Address 2
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_2_BASEADDR + 0xB0, FRAME_BUFFER_2 + offset2);
    //S2MM Start Address 3
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_2_BASEADDR + 0xB4, FRAME_BUFFER_3 + offset2);
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_2_BASEADDR + 0xA8, stride2*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_2_BASEADDR + 0xA4, width2*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_2_BASEADDR + 0xA0, height2);

#endif

#if TEST_channel_4_scaler
/**************channel 4 write**************/
    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_3_BASEADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_3_BASEADDR + 0xAC, FRAME_BUFFER_1 + offset3);
    //S2MM Start Address 2
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_3_BASEADDR + 0xB0, FRAME_BUFFER_2 + offset3);
    //S2MM Start Address 3
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_3_BASEADDR + 0xB4, FRAME_BUFFER_3 + offset3);
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_3_BASEADDR + 0xA8, stride3*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_3_BASEADDR + 0xA4, width3*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_VDMA_WRITE_AXI_VDMA_3_BASEADDR + 0xA0, height3);
#endif

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
//    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0x00, 0x8B);
    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0x00, 0x83);//there should be no genlock as there is no S2MM

    // MM2S Start Address 1
    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0x5C, FRAME_BUFFER_1 + offset4);
    // MM2S Start Address 2
    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0x60, FRAME_BUFFER_2 + offset4);
    // MM2S Start Address 3
    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0x64, FRAME_BUFFER_3 + offset4);
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0x58, stride4*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0x54, width4*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0x50, height4);

    xil_printf("VDMA configuration OK!!!\r\n");
}


void vpss_config(void)
{
    u32 Status;

    /* User registers custom timing table */
	//xil_printf("INFO> Registering Custom Timing Table with %d entries \r\n", (XVIDC_CM_NUM_SUPPORTED - (XVIDC_VM_CUSTOM + 1)));
	Status = XVidC_RegisterCustomTimingModes(XVidC_MyVideoTimingMode, (XVIDC_CM_NUM_SUPPORTED - (XVIDC_VM_CUSTOM + 1)));
	//if (Status != XST_SUCCESS) {
	//  xil_printf("ERR: Unable to register custom timing table\r\n\r\n");
	//}

    /* VPSS Configuration*/
	memset(&VprocInst0, 0, sizeof(XVprocSs));
//	memset(VprocCfgPtr, 0, sizeof(XVprocSs_Config));
    VprocCfgPtr = XVprocSs_LookupConfig(XPAR_XVPROCSS_0_DEVICE_ID);

//    switch (VprocCfgPtr->Topology)
//    {
//        case XVPROCSS_TOPOLOGY_FULL_FLEDGED:
//        case XVPROCSS_TOPOLOGY_DEINTERLACE_ONLY:
//			XVprocSs_SetFrameBufBaseaddr(&VprocInst0, USR_FRAME_BUF_BASEADDR);
//			break;
//
//        default:
//            break;
//    }

    //XVprocSs_LogReset(&VprocInst0);

    Status = XVprocSs_CfgInitialize(&VprocInst0, VprocCfgPtr, VprocCfgPtr->BaseAddress);
    if(Status != XST_SUCCESS)
	{
		while(1)
		{
			NOP();
		}
	}
    //Get the resolution details
    resId = XVidC_GetVideoModeId(1600, 2560, fpsIn, 0);
    TimingPtr = XVidC_GetTimingInfo(resId);

//    TimingPtr->HActive = 1280;
//    TimingPtr->HFrontPorch = 110;
//	TimingPtr->HSyncWidth = 40;
//	TimingPtr->HBackPorch = 220;
//	TimingPtr->HTotal = 1650;
//	TimingPtr->HSyncPolarity = 1;
//	TimingPtr->VActive = 3840;
//	TimingPtr->F0PVFrontPorch = 5;
//	TimingPtr->F0PVSyncWidth = 5;
//	TimingPtr->F0PVBackPorch = 20;
//	TimingPtr->F0PVTotal = 3870;
    //	TimingPtr->F1VFrontPorch = 0;
//	TimingPtr->F1VSyncWidth = 0;
//	TimingPtr->F1VBackPorch = 0;
//	TimingPtr->F1VTotal = 0;
//	TimingPtr->VSyncPolarity = 1;

    //Set the input stream
    StreamIn.VmId           = resId;
    StreamIn.Timing         = *TimingPtr;
    StreamIn.ColorFormatId  = colorFmtIn0;
    StreamIn.ColorDepth     = VprocCfgPtr->ColorDepth;
    StreamIn.PixPerClk      = VprocCfgPtr->PixPerClock;
    StreamIn.FrameRate      = fpsIn;
    StreamIn.IsInterlaced   = 0;
    XVprocSs_SetVidStreamIn(&VprocInst0, &StreamIn);


//    TimingPtr->HActive = 1280;
//    TimingPtr->HFrontPorch = 110;
//	TimingPtr->HSyncWidth = 40;
//	TimingPtr->HBackPorch = 220;
//	TimingPtr->HTotal = 1650;
//	TimingPtr->HSyncPolarity = 1;
//	TimingPtr->VActive = 3840;
//	TimingPtr->F0PVFrontPorch = 5;
//	TimingPtr->F0PVSyncWidth = 5;
//	TimingPtr->F0PVBackPorch = 20;
//	TimingPtr->F0PVTotal = 3870;
//	TimingPtr->F1VFrontPorch = 0;
//	TimingPtr->F1VSyncWidth = 0;
//	TimingPtr->F1VBackPorch = 0;
//	TimingPtr->F1VTotal = 0;
//	TimingPtr->VSyncPolarity = 1;

    resId = XVidC_GetVideoModeId(670, 2160, fpsOut, 0);
    TimingPtr = XVidC_GetTimingInfo(resId);
    //Set the output stream
    StreamOut.VmId           = resId;
    StreamOut.Timing         = *TimingPtr;
    StreamOut.ColorFormatId  = colorFmtOut0;
    StreamOut.ColorDepth     = VprocCfgPtr->ColorDepth;
    StreamOut.PixPerClk      = VprocCfgPtr->PixPerClock;
    StreamOut.FrameRate      = fpsOut;
    StreamOut.IsInterlaced   = 0;
    XVprocSs_SetVidStreamOut(&VprocInst0, &StreamOut);

    Status = XVprocSs_SetSubsystemConfig(&VprocInst0);
    if(Status!= XST_SUCCESS)
    {
            xil_printf("VPSS0 failed\r\n");
//          return(XST_FAILURE);
    }
    //xil_printf("VPSS0 Started\r\n");
}


void vpss_config_tpg_4k(void)
{
    u32 Status;

    /* User registers custom timing table */
	//xil_printf("INFO> Registering Custom Timing Table with %d entries \r\n", (XVIDC_CM_NUM_SUPPORTED - (XVIDC_VM_CUSTOM + 1)));
	Status = XVidC_RegisterCustomTimingModes(XVidC_MyVideoTimingMode, (XVIDC_CM_NUM_SUPPORTED - (XVIDC_VM_CUSTOM + 1)));
	//if (Status != XST_SUCCESS) {
	//  xil_printf("ERR: Unable to register custom timing table\r\n\r\n");
	//}

    /* VPSS Configuration*/
	memset(&VprocInst1, 0, sizeof(XVprocSs));
//	memset(VprocCfgPtr, 0, sizeof(XVprocSs_Config));
    VprocCfgPtr = XVprocSs_LookupConfig(XPAR_XVPROCSS_1_DEVICE_ID);

//    switch (VprocCfgPtr->Topology)
//    {
//        case XVPROCSS_TOPOLOGY_FULL_FLEDGED:
//        case XVPROCSS_TOPOLOGY_DEINTERLACE_ONLY:
//			XVprocSs_SetFrameBufBaseaddr(&VprocInst0, USR_FRAME_BUF_BASEADDR);
//			break;
//
//        default:
//            break;
//    }

    //XVprocSs_LogReset(&VprocInst0);
    Status = XVprocSs_CfgInitialize(&VprocInst1, VprocCfgPtr, VprocCfgPtr->BaseAddress);
    if(Status != XST_SUCCESS)
	{
		while(1)
		{
			NOP();
			xil_printf("failed\r\n");
		}

	}
    //Get the resolution details
    resId = XVidC_GetVideoModeId(3840,2160, fpsIn, 0);
    TimingPtr = XVidC_GetTimingInfo(resId);

//    TimingPtr->HActive = 1280;
//    TimingPtr->HFrontPorch = 110;
//	TimingPtr->HSyncWidth = 40;
//	TimingPtr->HBackPorch = 220;
//	TimingPtr->HTotal = 1650;
//	TimingPtr->HSyncPolarity = 1;
//	TimingPtr->VActive = 3840;
//	TimingPtr->F0PVFrontPorch = 5;
//	TimingPtr->F0PVSyncWidth = 5;
//	TimingPtr->F0PVBackPorch = 20;
//	TimingPtr->F0PVTotal = 3870;
    //	TimingPtr->F1VFrontPorch = 0;
//	TimingPtr->F1VSyncWidth = 0;
//	TimingPtr->F1VBackPorch = 0;
//	TimingPtr->F1VTotal = 0;
//	TimingPtr->VSyncPolarity = 1;

    //Set the input stream
    StreamIn.VmId           = resId;
    StreamIn.Timing         = *TimingPtr;
    StreamIn.ColorFormatId  = colorFmtIn0;
    StreamIn.ColorDepth     = VprocCfgPtr->ColorDepth;
    StreamIn.PixPerClk      = VprocCfgPtr->PixPerClock;
    StreamIn.FrameRate      = fpsIn;
    StreamIn.IsInterlaced   = 0;
    XVprocSs_SetVidStreamIn(&VprocInst1, &StreamIn);

//    TimingPtr->HActive = 1280;
//    TimingPtr->HFrontPorch = 110;
//	TimingPtr->HSyncWidth = 40;
//	TimingPtr->HBackPorch = 220;
//	TimingPtr->HTotal = 1650;
//	TimingPtr->HSyncPolarity = 1;
//	TimingPtr->VActive = 3840;
//	TimingPtr->F0PVFrontPorch = 5;
//	TimingPtr->F0PVSyncWidth = 5;
//	TimingPtr->F0PVBackPorch = 20;
//	TimingPtr->F0PVTotal = 3870;
//	TimingPtr->F1VFrontPorch = 0;
//	TimingPtr->F1VSyncWidth = 0;
//	TimingPtr->F1VBackPorch = 0;
//	TimingPtr->F1VTotal = 0;
//	TimingPtr->VSyncPolarity = 1;

    resId = XVidC_GetVideoModeId(1920, 1080, fpsOut, 0);
    TimingPtr = XVidC_GetTimingInfo(resId);
    //Set the output stream
    StreamOut.VmId           = resId;
    StreamOut.Timing         = *TimingPtr;
    StreamOut.ColorFormatId  = colorFmtOut0;
    StreamOut.ColorDepth     = VprocCfgPtr->ColorDepth;
    StreamOut.PixPerClk      = VprocCfgPtr->PixPerClock;
    StreamOut.FrameRate      = fpsOut;
    StreamOut.IsInterlaced   = 0;
    XVprocSs_SetVidStreamOut(&VprocInst1, &StreamOut);
    Status = XVprocSs_SetSubsystemConfig(&VprocInst1);
    if(Status!= XST_SUCCESS)
    {
            xil_printf("VPSS1 failed\r\n");
//          return(XST_FAILURE);
    }
    //xil_printf("VPSS0 Started\r\n");
}

void clear_display(void)
{
    //print("Initialize frame buffer data...");
	//Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x00, 0x8A);
    //Disable the data cache to initialize the frame buffer with a blue color
//    Xil_DCacheDisable();
    UINTPTR Addr1=FRAME_BUFFER_1,Addr2=FRAME_BUFFER_2,Addr3=FRAME_BUFFER_3;
//    UINTPTR Addr4=FRAME_BUFFER_4,Addr5=FRAME_BUFFER_5,Addr6=FRAME_BUFFER_6;
    int line, column;
	for(line=0; line < 2160; line++)
	{
		for(column=0; column < 3840*3/4; column++)
		{
			Xil_Out32(Addr1, 0xFFFFFFFF);
			Xil_Out32(Addr2, 0xFFFFFFFF);
			Xil_Out32(Addr3, 0xFFFFFFFF);

			Addr1+=4;
			Addr2+=4;
			Addr3+=4;
		}
	}
//	memset((void*)Addr4, 0, 6220800);
//	memset((void*)Addr5, 0, 6220800);
//	memset((void*)Addr6, 0, 6220800);
//	print_no_link_notify();
	//vdma_config_blank();
    //Re-enable the data cache
//	Xil_DCacheEnable();
	Xil_DCacheFlush();
//	print("Done\n\r");
}


int main()
{
    init_platform();
    xgpio_init();
    xil_printf("2CG_Ima_ACQ_V3: 4K@30_rgb_out\n\r");

//    // for SiI9134
//    xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x08, 0x35, STRETCH_OFF);
//    xgpio_i2c_reg8_write(I2C_NO_0, 0x7a>>1, 0x2f, 0x00, STRETCH_OFF);

	// for sil1136/sil9136-3 @    /闂傚倸鍊风粈渚�骞夐敓鐘冲仭闁靛鏅涚壕鐟懊归悩宸剱闁哄懏鎮傞弻锕�螣娓氼垱笑闂佸搫妫庨崹鑽ゆ閹烘挾鐟归柛銉戝倸鏋堥柣搴㈩問閸ｎ噣宕滈悢鐓庢瀬闁瑰墽绮弲鎼佹煥閻曞倹瀚�/spjk闂傚倷娴囧畷鐢稿窗閹扮増鍋￠柕澶堝剻濞戞ǚ鏀介悗锝庣厜閹风兘宕掗悙鏉戞疂闂佸壊鐓堥崑鍡欑矓閸洘鈷戦悷娆忓閸斻倝鏌涢悢椋庝虎鐞氭瑥顭块懜闈涘闁绘挻鐟╅弻鐔碱敇閻旈鐟ㄩ梺鍛婃⒐濮樸劑骞夐幖浣瑰亱闁割偀鎳囧銊モ攽閻愮鎷″ù婊庝邯閵嗕礁螖娴ｇ懓顎撶紓浣割儓濞夋洟宕敓锟�
	//                       @    /闂備浇顕х�涒晠顢欓弽顓為棷妞ゆ牜鍋涚粻顖炴煟閹达絽袚闁绘挻娲樼换娑㈠幢濡搫顫岄梺璇查獜闂勫嫮鎹㈠┑瀣潊闁宠棄妫涢崝鐑芥⒑閸︻収娼掗柛銉戝拋妲规俊鐐�栫敮濠囨嚄閼哥數顩查柨鐕傛嫹/lattice/Embedded_Vision_DevelopmentKit/EmbeddedVisionDevelopmentKit/UserManual/HDMI-VIP-Output-Bridge-Board
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0xC7, 0x00, STRETCH_OFF); // HDMI configuration
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x1E, 0x00, STRETCH_OFF); // Power Up Transmitter
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x08, 0x60, STRETCH_OFF); // Input Bus/Pixel Repetition (default)
#if defined (HDMI_OUT_1080P60)
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x00, 0x02, STRETCH_OFF); // Pixel Clock
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x01, 0x3A, STRETCH_OFF); // Pixel Clock
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x02, 0x70, STRETCH_OFF); // Frame Rate * 100
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x03, 0x17, STRETCH_OFF); // Frame Rate * 100
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x04, 0x98, STRETCH_OFF); // Pixels
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x05, 0x08, STRETCH_OFF); // Pixels
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x06, 0x65, STRETCH_OFF); // Lines
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x07, 0x04, STRETCH_OFF); // Lines
#else
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x00, 0x04, STRETCH_OFF); // Pixel Clock
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x01, 0x74, STRETCH_OFF); // Pixel Clock
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x02, 0xB8, STRETCH_OFF); // Frame Rate * 100
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x03, 0x0B, STRETCH_OFF); // Frame Rate * 100
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x04, 0x30, STRETCH_OFF); // Pixels
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x05, 0x11, STRETCH_OFF); // Pixels
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x06, 0xCA, STRETCH_OFF); // Lines
	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x07, 0x08, STRETCH_OFF); // Lines
#endif
	//xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x09, 0x00, STRETCH_OFF); //
	//xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x0A, 0x00, STRETCH_OFF); //

	xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x1A, 0x00, STRETCH_OFF); //


    memset(FRAME_BUFFER_BASE_ADDR,0,FRAME_BUFFER_SIZE0*3);//background
    Xil_DCacheFlush();
//    clear_display();

    clkwiz_vtc_cfg();
    tpg_config();
    vdma_config();
#if TEST_3_MIX
    vpss_config_tpg_4k();//3 channel mix, channel_2 is 4K, need scale for 1080p
#endif

#if  TEST_channel_4_scaler
    vpss_config();
#endif

    while(1)
    {
    	NOP();
/********************************/

    sleep(2);

#if 1
    xil_printf("- tpg3 after vpss: %dx%d -\r\n", AXI_LITE_REG_mReadReg(XPAR_TPG_AXIS_PASSTHROUGH_MON_0_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG0_OFFSET),AXI_LITE_REG_mReadReg(XPAR_TPG_AXIS_PASSTHROUGH_MON_0_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG1_OFFSET));
    xil_printf("- tpg3 output: %dx%d -\r\n", AXI_LITE_REG_mReadReg(XPAR_TPG_AXIS_PASSTHROUGH_MON_1_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG0_OFFSET),AXI_LITE_REG_mReadReg(XPAR_TPG_AXIS_PASSTHROUGH_MON_1_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG1_OFFSET));
    xil_printf("- tpg3 after v_vsample: %dx%d -\r\n", AXI_LITE_REG_mReadReg(XPAR_TPG_AXIS_PASSTHROUGH_MON_2_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG0_OFFSET),AXI_LITE_REG_mReadReg(XPAR_TPG_AXIS_PASSTHROUGH_MON_2_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG1_OFFSET));
    xil_printf("- tpg1 output: %dx%d -\r\n", AXI_LITE_REG_mReadReg(XPAR_TPG_AXIS_PASSTHROUGH_MON_3_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG0_OFFSET),AXI_LITE_REG_mReadReg(XPAR_TPG_AXIS_PASSTHROUGH_MON_3_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG1_OFFSET));
#endif

#if 1
    xil_printf("-write channel_0 freq: %d -\r\n", AXI_LITE_REG_mReadReg(XPAR_VDMA_WRITE_AXIS_PASSTHROUGH_MON_0_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG2_OFFSET));
    xil_printf("- res: %dx%d -\r\n", AXI_LITE_REG_mReadReg(XPAR_VDMA_WRITE_AXIS_PASSTHROUGH_MON_0_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG0_OFFSET),AXI_LITE_REG_mReadReg(XPAR_VDMA_WRITE_AXIS_PASSTHROUGH_MON_0_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG1_OFFSET));

    xil_printf("-write channel_1 freq: %d -\r\n", AXI_LITE_REG_mReadReg(XPAR_VDMA_WRITE_AXIS_PASSTHROUGH_MON_1_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG2_OFFSET));
    xil_printf("- res: %dx%d -\r\n", AXI_LITE_REG_mReadReg(XPAR_VDMA_WRITE_AXIS_PASSTHROUGH_MON_1_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG0_OFFSET),AXI_LITE_REG_mReadReg(XPAR_VDMA_WRITE_AXIS_PASSTHROUGH_MON_1_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG1_OFFSET));

    xil_printf("-write channel_2 freq: %d -\r\n", AXI_LITE_REG_mReadReg(XPAR_VDMA_WRITE_AXIS_PASSTHROUGH_MON_2_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG2_OFFSET));
    xil_printf("- res: %dx%d -\r\n", AXI_LITE_REG_mReadReg(XPAR_VDMA_WRITE_AXIS_PASSTHROUGH_MON_2_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG0_OFFSET),AXI_LITE_REG_mReadReg(XPAR_VDMA_WRITE_AXIS_PASSTHROUGH_MON_2_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG1_OFFSET));

    xil_printf("-write channel_3 freq: %d -\r\n", AXI_LITE_REG_mReadReg(XPAR_VDMA_WRITE_AXIS_PASSTHROUGH_MON_3_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG2_OFFSET));
    xil_printf("- res: %dx%d -\r\n", AXI_LITE_REG_mReadReg(XPAR_VDMA_WRITE_AXIS_PASSTHROUGH_MON_3_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG0_OFFSET),AXI_LITE_REG_mReadReg(XPAR_VDMA_WRITE_AXIS_PASSTHROUGH_MON_3_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG1_OFFSET));

    xil_printf("-read freq: %d -\r\n", AXI_LITE_REG_mReadReg(XPAR_VDMA_SS_AXIS_PASSTHROUGH_MON_4_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG2_OFFSET));
    xil_printf("- res: %dx%d -\r\n", AXI_LITE_REG_mReadReg(XPAR_VDMA_SS_AXIS_PASSTHROUGH_MON_4_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG0_OFFSET),AXI_LITE_REG_mReadReg(XPAR_VDMA_SS_AXIS_PASSTHROUGH_MON_4_S00_AXI_BASEADDR, \
    	   	    AXI_LITE_REG_S00_AXI_SLV_REG1_OFFSET));
#endif

//

    xil_printf("/********************************/\r\n");

/**********************************/
    }

    cleanup_platform();
    return 0;
}

