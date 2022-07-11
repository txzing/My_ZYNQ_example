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

#include "netif/xadapter.h"
#include "lwip/udp.h"

#include "bitmanip.h"
#include "trace_zzg_debug.h"
#include "config.h"

#if LWIP_IPV6==1
#include "lwip/ip.h"
#else
#if LWIP_DHCP==1
#include "lwip/dhcp.h"
#endif
#endif

#include "qspi_ctrl/qspi.h"

#include "axis_passthrough_monitor.h"

#define MAX_FLASH_LEN   32*1024*1024

static struct udp_pcb *udp8080_pcb = NULL;
ip_addr_t target_addr;
unsigned char ip_export[4];
unsigned char mac_export[6];

char FlashRxBuffer[MAX_FLASH_LEN] ;
unsigned int ReceivedCount = 0 ;

extern XQspiPsu QspiInstance;

int StartUpdate = 0 ;

int FrameLengthCurr = 0 ;

/* defined by each RAW mode application */
int start_udp(unsigned int port);
int transfer_data(const char *pData, int len, const ip_addr_t *addr) ;
int send_data(const char *frame, int data_len);

/* missing declaration in lwIP */
void lwip_init();

#if LWIP_IPV6==0
#if LWIP_DHCP==1
extern volatile int dhcp_timoutcntr;
err_t dhcp_start(struct netif *netif);
#endif
#endif

static struct netif server_netif;
struct netif *echo_netif;

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

XAxis_Switch AxisSwitch0;
XAxis_Switch AxisSwitch1;
XAxis_Switch AxisSwitch2;
XAxis_Switch AxisSwitch3;
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
u32 bckgndId0=XTPG_BKGND_COLOR_BARS;
u32 bckgndId1=XTPG_BKGND_H_RAMP;
u32 bckgndId2=XTPG_BKGND_V_RAMP;
u32 bckgndId3=XTPG_BKGND_V_RAMP;

XVidC_VideoMode resId;
XVidC_VideoStream StreamIn, StreamOut;
XVidC_VideoTiming const *TimingPtr;

XVprocSs VprocInst0;
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
				2560, 5, 5, 10, 2580, 0, 0, 0, 0, 1} }
};

struct reginfo
{
	u8	addr;
    u16 reg;
    u8 	val;
};
#define SEQUENCE_PROPERTY    0xFFFD
#define SEQUENCE_WAIT_MS     0xFFFE
#define SEQUENCE_END	     0xFFFF
struct reginfo cfg_gmsl_no1[] =
{
//	{0x50, SEQUENCE_WAIT_MS, 0x80},
//	{0x50, SEQUENCE_END, 0x00},
};

int AxisSwitch(u16 DeviceId, XAxis_Switch * pAxisSwitch, u8 MiIndex, u8 SiIndex)
{
    XAxis_Switch_Config *Config;
    int Status;

    u8 num;
    if(DeviceId == XPAR_AXIS_SWITCH_0_DEVICE_ID)
    {
        num = 0;
    }

    /* Initialize the AXI4-Stream Switch driver so that it's ready to
     * use look up configuration in the config table, then
     * initialize it.
     */
    Config = XAxisScr_LookupConfig(DeviceId);
    if (NULL == Config) {
        return XST_FAILURE;
    }

    Status = XAxisScr_CfgInitialize(pAxisSwitch, Config,
                        Config->BaseAddress);
    if (Status != XST_SUCCESS) {
        xil_printf("AXI4-Stream initialization failed.\r\n");
        return XST_FAILURE;
    }

    /* Disable register update */
    XAxisScr_RegUpdateDisable(pAxisSwitch);

    /* Disable all MI ports */
    XAxisScr_MiPortDisableAll(pAxisSwitch);

    /* Source SI[1] to MI[0] */
    XAxisScr_MiPortEnable(pAxisSwitch, MiIndex, SiIndex);

    /* Enable register update */
    XAxisScr_RegUpdateEnable(pAxisSwitch);

    /* Check for MI port enable */
    Status = XAxisScr_IsMiPortEnabled(pAxisSwitch, MiIndex, SiIndex);
    if (Status) {
        //xil_printf("Switch %d: MI[%d] is sourced from SI[%d].\r\n", num, MiIndex, SiIndex);
    }

    return XST_SUCCESS;
}

#define TPG

void axis_switch_cfg(void)
{
    u32 Status;
#if (defined TPG)
    AxisSwitch(XPAR_CH_0_AXIS_SWITCH_0_DEVICE_ID, &AxisSwitch0, 0, 1); // tpg
    AxisSwitch(XPAR_CH_1_AXIS_SWITCH_1_DEVICE_ID, &AxisSwitch1, 0, 1); // tpg
    AxisSwitch(XPAR_CH_2_AXIS_SWITCH_2_DEVICE_ID, &AxisSwitch2, 0, 1); // tpg
    AxisSwitch(XPAR_CH_3_AXIS_SWITCH_3_DEVICE_ID, &AxisSwitch3, 0, 1); // tpg
#else
    AxisSwitch(XPAR_CH_0_AXIS_SWITCH_0_DEVICE_ID, &AxisSwitch0, 0, 0); // csi-rx
	AxisSwitch(XPAR_CH_1_AXIS_SWITCH_1_DEVICE_ID, &AxisSwitch1, 0, 0); // csi-rx
	AxisSwitch(XPAR_CH_2_AXIS_SWITCH_2_DEVICE_ID, &AxisSwitch2, 0, 0); // csi-rx
	AxisSwitch(XPAR_CH_3_AXIS_SWITCH_3_DEVICE_ID, &AxisSwitch3, 0, 0); // csi-rx
#endif

//	AxisSwitch(XPAR_AXIS_SWITCH_0_DEVICE_ID, &AxisSwitch4, 0, 0); // tpg
}

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

    // tpg0
    //xil_printf("TPG0 Initializing\n\r");

    Status = XV_tpg_Initialize(&tpg_inst0, XPAR_CH_0_V_TPG_0_DEVICE_ID);
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

    Status = XV_tpg_Initialize(&tpg_inst1, XPAR_CH_1_V_TPG_1_DEVICE_ID);
    if(Status!= XST_SUCCESS)
    {
        xil_printf("TPG1 configuration failed\r\n");
//      return(XST_FAILURE);
    }

    //Configure the TPG1
    tpg_cfg(&tpg_inst1, 1080, 1920, colorFmtIn1, bckgndId1);

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

    Status = XV_tpg_Initialize(&tpg_inst2, XPAR_CH_2_V_TPG_2_DEVICE_ID);
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

//////////////////////////////////////////////////
	// tpg3
	//xil_printf("TPG2 Initializing\n\r");

	Status = XV_tpg_Initialize(&tpg_inst3, XPAR_CH_3_V_TPG_3_DEVICE_ID);
	if(Status!= XST_SUCCESS)
	{
		xil_printf("TPG3 configuration failed\r\n");
//      return(XST_FAILURE);
	}

	//Configure the TPG3
	tpg_cfg(&tpg_inst3, 5120, 1920, colorFmtIn3, bckgndId3);

	//Configure the moving box of the TPG3
	tpg_box(&tpg_inst3, 50, 2);
	XV_tpg_Set_boxColorR(&tpg_inst3,0xff);
	XV_tpg_Set_boxColorG(&tpg_inst3,0xff);
	XV_tpg_Set_boxColorB(&tpg_inst3,0xff);

	//Start the TPG3
	XV_tpg_EnableAutoRestart(&tpg_inst3);
	XV_tpg_Start(&tpg_inst3);
	//xil_printf("TPG3 started!\r\n");
}

void vdma_config(void)
{
    /* Start of VDMA Configuration */
    u32 bytePerPixels = 3;
    int offset0 = 0; // (y*w+x)*Bpp
    int offset1 = 0; // (y*w+x)*Bpp
    u32 stride0 = 3840;
    u32 width0 = 3840;
    u32 height0 = 2160;
    u32 stride1 = 3840;  // crop keeps write Stride
    u32 width1 = 3840;
    u32 height1 = 2160;

    /* Configure the Write interface (S2MM)*/
    // S2MM Control Register
    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0x30, 0x8B);
    //S2MM Start Address 1
    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0xAC, FRAME_BUFFER_1 + offset0);
    //S2MM Start Address 2
    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0xB0, FRAME_BUFFER_2 + offset0);
    //S2MM Start Address 3
    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0xB4, FRAME_BUFFER_3 + offset0);
    //S2MM Frame delay / Stride register
    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0xA8, stride0*bytePerPixels);
    // S2MM HSIZE register
    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0xA4, width0*bytePerPixels);
    // S2MM VSIZE register
    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0xA0, height0);

    /* Configure the Read interface (MM2S)*/
    // MM2S Control Register
    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0x00, 0x8B);
    // MM2S Start Address 1
    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0x5C, FRAME_BUFFER_1 + offset1);
    // MM2S Start Address 2
    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0x60, FRAME_BUFFER_2 + offset1);
    // MM2S Start Address 3
    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0x64, FRAME_BUFFER_3 + offset1);
    // MM2S Frame delay / Stride register
    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0x58, stride1*bytePerPixels);
    // MM2S HSIZE register
    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0x54, width1*bytePerPixels);
    // MM2S VSIZE register
    Xil_Out32(XPAR_VDMA_SS_AXI_VDMA_0_BASEADDR + 0x50, height1);
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
    VprocCfgPtr = XVprocSs_LookupConfig(XPAR_VID_OUT_SS_V_TC_0_DEVICE_ID);

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
			NOP();
	}
    //Get the resolution details
    resId = XVidC_GetVideoModeId(1920, 2560, fpsIn, 0);
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
    XVprocSs_SetVidStreamOut(&VprocInst0, &StreamOut);

    Status = XVprocSs_SetSubsystemConfig(&VprocInst0);
    if(Status!= XST_SUCCESS)
    {
            xil_printf("VPSS0 failed\r\n");
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

#if LWIP_IPV6==1
void print_ip6(char *msg, ip_addr_t *ip)
{
	print(msg);
	xil_printf(" %x:%x:%x:%x:%x:%x:%x:%x\n\r",
			IP6_ADDR_BLOCK1(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK2(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK3(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK4(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK5(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK6(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK7(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK8(&ip->u_addr.ip6));

}
#else
void
print_ip(char *msg, ip_addr_t *ip)
{
	print(msg);
	xil_printf("%d.%d.%d.%d\n\r", ip4_addr1(ip), ip4_addr2(ip),
			ip4_addr3(ip), ip4_addr4(ip));
}

void
print_ip_settings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw)
{

	print_ip("Board IP: ", ip);
//	print_ip("Netmask : ", mask);
//	print_ip("Gateway : ", gw);
}
#endif

void print_app_header() {
	xil_printf("\n\r\n\r-----LwIP UDP Remote Update------\n\r");
	xil_printf("UDP packets sent to port 8080\n\r");
}

#if defined (__arm__) && !defined (ARMR5)
#if XPAR_GIGE_PCS_PMA_SGMII_CORE_PRESENT == 1 || XPAR_GIGE_PCS_PMA_1000BASEX_CORE_PRESENT == 1
int ProgramSi5324(void);
int ProgramSfpPhy(void);
#endif
#endif

#ifdef XPS_BOARD_ZCU102
#ifdef XPAR_XIICPS_0_DEVICE_ID
int IicPhyReset(void);
#endif
#endif

int main()
{
    int Status;

#ifdef UDP_UPDATE
//    int Status;

#if LWIP_IPV6==0
	ip_addr_t ipaddr, netmask, gw;

#endif

	/* the mac address of the board. this should be unique per board */
	unsigned char mac_ethernet_address[] =
	{ 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

	echo_netif = &server_netif;
#if defined (__arm__) && !defined (ARMR5)
#if XPAR_GIGE_PCS_PMA_SGMII_CORE_PRESENT == 1 || XPAR_GIGE_PCS_PMA_1000BASEX_CORE_PRESENT == 1
	ProgramSi5324();
	ProgramSfpPhy();
#endif
#endif

	/* Define this board specific macro in order perform PHY reset on ZCU102 */
#ifdef XPS_BOARD_ZCU102
	if(IicPhyReset()) {
		xil_printf("Error performing PHY reset \n\r");
		return -1;
	}
#endif
#endif

    init_platform();
    xgpio_init();
    print("2CG_Ima_ACQ_V3: 4K@30_rgb_out\n\r");

//    // for SiI9134
//    xgpio_i2c_reg8_write(I2C_NO_0, 0x72>>1, 0x08, 0x35, STRETCH_OFF);
//    xgpio_i2c_reg8_write(I2C_NO_0, 0x7a>>1, 0x2f, 0x00, STRETCH_OFF);

	// for sil1136/sil9136-3 @    /鍙傝�冭祫鏂�/spjk瑙嗛杈撳嚭鎺ュ彛杞崲
	//                       @    /寮�鍙戞澘璧勬枡/lattice/Embedded_Vision_DevelopmentKit/EmbeddedVisionDevelopmentKit/UserManual/HDMI-VIP-Output-Bridge-Board
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

    axis_switch_cfg();
    clkwiz_vtc_cfg();
    tpg_config();
    vdma_config();
    vpss_config();

#ifdef UDP_UPDATE
#if LWIP_IPV6==0
#if LWIP_DHCP==1
	ipaddr.addr = 0;
	gw.addr = 0;
	netmask.addr = 0;
#else
	/* initliaze IP addresses to be used */
	IP4_ADDR(&ipaddr,  192, 168,   1, 10);
	IP4_ADDR(&netmask, 255, 255, 255,  0);
	IP4_ADDR(&gw,      192, 168,   1,  1);
#endif
#endif
	print_app_header();

	lwip_init();

#if (LWIP_IPV6 == 0)
	/* Add network interface to the netif_list, and set it as default */
	if (!xemac_add(echo_netif, &ipaddr, &netmask,
			&gw, mac_ethernet_address,
			PLATFORM_EMAC_BASEADDR)) {
		xil_printf("Error adding N/W interface\n\r");
		return -1;
	}
#else
	/* Add network interface to the netif_list, and set it as default */
	if (!xemac_add(echo_netif, NULL, NULL, NULL, mac_ethernet_address,
			PLATFORM_EMAC_BASEADDR)) {
		xil_printf("Error adding N/W interface\n\r");
		return -1;
	}
	echo_netif->ip6_autoconfig_enabled = 1;

	netif_create_ip6_linklocal_address(echo_netif, 1);
	netif_ip6_addr_set_state(echo_netif, 0, IP6_ADDR_VALID);

	print_ip6("\n\rBoard IPv6 address ", &echo_netif->ip6_addr[0].u_addr.ip6);

#endif
	netif_set_default(echo_netif);

	/* now enable interrupts */
	platform_enable_interrupts();

	/* specify that the network if is up */
	netif_set_up(echo_netif);

#if (LWIP_IPV6 == 0)
#if (LWIP_DHCP==1)
	/* Create a new DHCP client for this interface.
	 * Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
	 * the predefined regular intervals after starting the client.
	 */
	dhcp_start(echo_netif);
	dhcp_timoutcntr = 24;

	while(((echo_netif->ip_addr.addr) == 0) && (dhcp_timoutcntr > 0))
		xemacif_input(echo_netif);

	if (dhcp_timoutcntr <= 0) {
		if ((echo_netif->ip_addr.addr) == 0) {
			xil_printf("DHCP Timeout\r\n");
			xil_printf("Configuring default IP of 192.168.1.10\r\n");
			IP4_ADDR(&(echo_netif->ip_addr),  192, 168,   1, 10);
			IP4_ADDR(&(echo_netif->netmask), 255, 255, 255,  0);
			IP4_ADDR(&(echo_netif->gw),      192, 168,   1,  1);
		}
	}

	ipaddr.addr = echo_netif->ip_addr.addr;
	gw.addr = echo_netif->gw.addr;
	netmask.addr = echo_netif->netmask.addr;
#endif

	print_ip_settings(&ipaddr, &netmask, &gw);
	memcpy(ip_export, &ipaddr, 4);
	memcpy(mac_export, &mac_ethernet_address, 6);

#endif
	/* start the application (web server, rxtest, txtest, etc..) */
	start_udp(8080);
#endif

    while(1)
    {
    	NOP();
/********************************/

   	  sleep(2);
   	  xil_printf("- freq: %d -\r\n", AXI_LITE_REG_mReadReg(XPAR_VDMA_SS_AXIS_PASSTHROUGH_MON_4_S00_AXI_BASEADDR, \
   	            AXI_LITE_REG_S00_AXI_SLV_REG2_OFFSET));
   	  xil_printf("- res: %dx%d -\r\n", AXI_LITE_REG_mReadReg(XPAR_VDMA_SS_AXIS_PASSTHROUGH_MON_4_S00_AXI_BASEADDR, \
   	        AXI_LITE_REG_S00_AXI_SLV_REG0_OFFSET),AXI_LITE_REG_mReadReg(XPAR_VDMA_SS_AXIS_PASSTHROUGH_MON_4_S00_AXI_BASEADDR, \
   	        AXI_LITE_REG_S00_AXI_SLV_REG1_OFFSET));

/**********************************/

#ifdef UDP_UPDATE
		xemacif_input(echo_netif);
		if (StartUpdate)
		{
			Status = update_qspi(&QspiInstance, QSPIPSU_DEVICE_ID, ReceivedCount, FlashRxBuffer) ;
			if (Status != XST_SUCCESS)
				xil_printf("Update Flash Error!\r\n") ;

			StartUpdate = 0 ;
			ReceivedCount = 0;
		}
#endif
    }

    cleanup_platform();
    return 0;
}

/*
 * Call back funtion for udp receiver
 *
 * @param arg is argument
 * @param pcb is udp pcb pointer
 * @param p_rx is pbuf pointer
 * @param addr is IP address
 * @param port is udp port
 *
 */
void udp_recive(void *arg, struct udp_pcb *pcb, struct pbuf *p_rx,
		const ip_addr_t *addr, u16_t port) {

	char *pData;
	if (p_rx != NULL)
	{
		pData = (char *) p_rx->payload;

		int udp_len = p_rx->len ;

		if (!(memcmp("update", p_rx->payload + p_rx->len - 6, 6)))
		{
			memcpy(&FlashRxBuffer[ReceivedCount], pData, udp_len - 6);
			ReceivedCount += udp_len - 6 ;
			xil_printf("Received Size is %u Bytes\r\n", ReceivedCount) ;
			xil_printf("Initialization done, programming the memory\r\n") ;
			StartUpdate = 1 ;
		}
		else
		{
			memcpy(&FlashRxBuffer[ReceivedCount], pData, udp_len);
			ReceivedCount += udp_len ;
		}

		pbuf_free(p_rx);
	}
}
/*
 * Create new pcb, bind pcb and port, set call back function
 */
int start_udp(unsigned int port) {
	err_t err;
	udp8080_pcb = udp_new();
	if (!udp8080_pcb) {
		xil_printf("Error creating PCB. Out of Memory\n\r");
		return -1;
	}
	/* bind to specified @port */
	err = udp_bind(udp8080_pcb, IP_ADDR_ANY, port);
	if (err != ERR_OK) {
		xil_printf("Unable to bind to port %d: err = %d\n\r", port, err);
		return -2;
	}
	udp_recv(udp8080_pcb, udp_recive, 0);
	IP4_ADDR(&target_addr, 192,168,1,35);

	return 0;
}
