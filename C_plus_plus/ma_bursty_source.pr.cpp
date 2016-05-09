/* Process model C++ form file: ma_bursty_source.pr.cpp */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char ma_bursty_source_pr_cpp [] = "MIL_3_Tfile_Hdr_ 145A 30A modeler 7 572FC638 572FC638 1 Loren Loren 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1e80 8                                                                                                                                                                                                                                                                                                                                                                                                              ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

/* Include files. */

#include <math.h>
#include <stdio.h> 
#include <stdlib.h> 


#include "opencv2\core\core.hpp"
#include "opencv2\contrib\contrib.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\objdetect\objdetect.hpp"

//#include "cv.h"
//#include "highgui.h"

#include "oms_dist_support.h"
#include "oms_dist_support_base.h"


#include "myRTPJPEGheader.h"
#include "myffmpegheader.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <string>

#define __STDC_CONSTANT_MACROS
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/pixfmt.h"
}

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avutil.lib")

/* Define constants used in the process model.			*/
#define	OFF_TO_ON			10
#define	ON_TO_ON			20
#define	ON_TO_OFF			30

/* Define a small value (= 1 psec), which will be used	*/
/* to recover from double arithmetic precision losts	*/
/* while doing time related	precision sensitive			*/
/* computations.										*/
#define	PRECISION_RECOVERY	0.000000000001

/* State transition macro definitions.					*/
#define	INACTIVE_TO_ACTIVE	(intrpt_type == OPC_INTRPT_SELF && intrpt_code == OFF_TO_ON)
#define	REMAIN_ACTIVE		(intrpt_type == OPC_INTRPT_SELF && intrpt_code == ON_TO_ON || intrpt_type == OPC_INTRPT_STAT)//mohammad
#define	ACTIVE_TO_INACTIVE	(intrpt_type == OPC_INTRPT_SELF && intrpt_code == ON_TO_OFF)

/* Function Declarations.	*/
static void			bursty_source_sv_init ();
void				startFFMPEG(FFMPEGData &vidData, int node);

/*
OmsT_Dist_Handle           on_state_dist_handle;
OmsT_Dist_Handle           off_state_dist_handle;
OmsT_Dist_Handle           intarrvl_time_dist_handle;
OmsT_Distribution *           packet_size_dist_handle;*/  
//char				intarrvl_rate_string [128];
//double frameRate, frameSize;



char 				 myString[200];
double				 newValue;
int					 flag = 0;
int 				 appOpencvDebugFlag = 1;
int 				 Node_LorenDebugFlag = 0;
int 				 EAestimationTimeApp = 20;	//should match EAestimationTime in mac
int 				 transitionTimeApp = 20; 	// used to be 460
int 				 frameSizeAverageCalculationPeriod = 0.5;

char 			 	 curveInApp[100]="";
char				 methodInApp[100]="";
int 				 nodes_no_app;
char	             fmt_name[20];
int					 pk_size = 0;
int					 byte_load = 0;
//int 				 outFileFlag = 0;




//std::vector<FFMPEGData> vidData;
FFMPEGData* vidData;


//opencv declarations.
cv::Size 			 s;
int 				 rows = 0, cols = 0;

// Assorted declarations.
int 				 ffmpeg_flag = 0;
char 				 parentName[60];
int 				 nodeNumber = 0;
int 				 allocateFlag = 0;
int					 numOff = 5;
double				 prevAppRate = 0;
int					 createPipeFlag = 0;

FILE * appRateOutputFile;
char myAppRateTraceName[100];
int frameCount = 0;
FILE * frameTime;

HANDLE hPipe;
DWORD dwWritten = 0;
char message[20];


/* End of Header Block */

#if !defined (VOSD_NO_FIN)
#undef	BIN
#undef	BOUT
#define	BIN		FIN_LOCAL_FIELD(_op_last_line_passed) = __LINE__ - _op_block_origin;
#define	BOUT	BIN
#define	BINIT	FIN_LOCAL_FIELD(_op_last_line_passed) = 0; _op_block_origin = __LINE__;
#else
#define	BINIT
#endif /* #if !defined (VOSD_NO_FIN) */



/* State variable definitions */
class ma_bursty_source_state
	{
	private:
		/* Internal state tracking for FSM */
		FSM_SYS_STATE

	public:
		ma_bursty_source_state (void);

		/* Destructor contains Termination Block */
		~ma_bursty_source_state (void);

		/* State Variables */
		char	                   		pid_string [64]                                 ;	/* Process ID display string */
		Boolean	                		debug_mode                                      ;	/* Determines whether the simulation is in debug mode */
		double	                 		stop_time                                       ;	/* Stop time for traffic generation */
		double	                 		off_state_start_time                            ;	/* Time at which the process will enter the OFF state */
		Stathandle	             		pksize_stathandle                               ;	/* Statistic handle to the "Packet Generation Status" statistic */
		OmsT_Dist_Handle	       		on_state_dist_handle                            ;	/* On state distribution handle */
		OmsT_Dist_Handle	       		off_state_dist_handle                           ;	/* Off state distribution handle */
		OmsT_Dist_Handle	       		intarrvl_time_dist_handle                       ;	/* Interarrival time distribution handle */
		OmsT_Dist_Handle	       		packet_size_dist_handle_                        ;	/* Packet size distribution handle */
		double	                 		start_time                                      ;	/* Start time for traffic generation */
		Stathandle	             		bits_sent_stathandle                            ;
		Stathandle	             		bitssec_sent_stathandle                         ;
		Stathandle	             		pkts_sent_stathandle                            ;
		Stathandle	             		pktssec_sent_stathandle                         ;
		Stathandle	             		bits_sent_gstathandle                           ;
		Stathandle	             		bitssec_sent_gstathandle                        ;
		Stathandle	             		pkts_sent_gstathandle                           ;
		Stathandle	             		pktssec_sent_gstathandle                        ;
		int	                    		segmentation_size                               ;	/* Size using which segmentation will occur. */
		Sbhandle	               		segmentation_buf_handle                         ;
		Stathandle	             		app_appRate_stat                                ;
		double	                 		appRate                                         ;
		Stathandle	             		interarrival_stat                               ;
		Stathandle	             		frameRate_stat                                  ;
		Stathandle	             		average_packet_size_stat                        ;
		double	                 		frameRate                                       ;
		double	                 		frameSize                                       ;
		OmsT_Dist_Handle	       		packet_size_dist_handle                         ;
		Stathandle	             		my_packet_size_stat                             ;
		double	                 		total_bits_sent                                 ;
		Stathandle	             		my_packet_data_size_stat                        ;
		double	                 		next_frame_arrival_time                         ;
		double	                 		alreadySent                                     ;
		int	                    		FrameCounter                                    ;
		int	                    		imageLineNo                                     ;
		char	                   		myName[20]                                      ;
		Stathandle	             		frameSizeStat                                   ;
		double	                 		inputPacketSize                                 ;
		double	                 		averageFrameSizeInPackets                       ;
		int	                    		overallPacketCounter                            ;
		int	                    		RTPoverhead                                     ;
		int	                    		RTPoverheadResetFlag                            ;
		Stathandle	             		RTPoverheadStat                                 ;
		Stathandle	             		av_interarrival_stat                            ;
		int **	                 		sizeInfo                                        ;
		char**	                 		imageName                                       ;
		char**	                 		directoryName                                   ;
		int	                    		imageNo                                         ;
		double	                 		absDeviationSum                                 ;
		int	                    		DeviationCounter                                ;
		Stathandle	             		absDeviationStat                                ;
		Stathandle	             		signedDeviationStat                             ;
		double	                 		signedDeviationSum                              ;
		double	                 		lastFrameSize                                   ;
		double	                 		lastSmoothedFrameSize                           ;
		double	                 		lastFrameSizeAverageCalculationTime             ;
		int	                    		frameCounter                                    ;
		double	                 		frameSizeSum                                    ;
		double	                 		averageFrameSize                                ;
		Stathandle	             		averageAppFromAverageFrameSize_stat             ;

		/* FSM code */
		void ma_bursty_source (OP_SIM_CONTEXT_ARG_OPT);
		/* Diagnostic Block */
		void _op_ma_bursty_source_diag (OP_SIM_CONTEXT_ARG_OPT);

#if defined (VOSD_NEW_BAD_ALLOC)
		void * operator new (size_t) throw (VOSD_BAD_ALLOC);
#else
		void * operator new (size_t);
#endif
		void operator delete (void *);

		/* Memory management */
		static VosT_Obtype obtype;
	};

VosT_Obtype ma_bursty_source_state::obtype = (VosT_Obtype)OPC_NIL;

#define pid_string              		op_sv_ptr->pid_string
#define debug_mode              		op_sv_ptr->debug_mode
#define stop_time               		op_sv_ptr->stop_time
#define off_state_start_time    		op_sv_ptr->off_state_start_time
#define pksize_stathandle       		op_sv_ptr->pksize_stathandle
#define on_state_dist_handle    		op_sv_ptr->on_state_dist_handle
#define off_state_dist_handle   		op_sv_ptr->off_state_dist_handle
#define intarrvl_time_dist_handle		op_sv_ptr->intarrvl_time_dist_handle
#define packet_size_dist_handle_		op_sv_ptr->packet_size_dist_handle_
#define start_time              		op_sv_ptr->start_time
#define bits_sent_stathandle    		op_sv_ptr->bits_sent_stathandle
#define bitssec_sent_stathandle 		op_sv_ptr->bitssec_sent_stathandle
#define pkts_sent_stathandle    		op_sv_ptr->pkts_sent_stathandle
#define pktssec_sent_stathandle 		op_sv_ptr->pktssec_sent_stathandle
#define bits_sent_gstathandle   		op_sv_ptr->bits_sent_gstathandle
#define bitssec_sent_gstathandle		op_sv_ptr->bitssec_sent_gstathandle
#define pkts_sent_gstathandle   		op_sv_ptr->pkts_sent_gstathandle
#define pktssec_sent_gstathandle		op_sv_ptr->pktssec_sent_gstathandle
#define segmentation_size       		op_sv_ptr->segmentation_size
#define segmentation_buf_handle 		op_sv_ptr->segmentation_buf_handle
#define app_appRate_stat        		op_sv_ptr->app_appRate_stat
#define appRate                 		op_sv_ptr->appRate
#define interarrival_stat       		op_sv_ptr->interarrival_stat
#define frameRate_stat          		op_sv_ptr->frameRate_stat
#define average_packet_size_stat		op_sv_ptr->average_packet_size_stat
#define frameRate               		op_sv_ptr->frameRate
#define frameSize               		op_sv_ptr->frameSize
#define packet_size_dist_handle 		op_sv_ptr->packet_size_dist_handle
#define my_packet_size_stat     		op_sv_ptr->my_packet_size_stat
#define total_bits_sent         		op_sv_ptr->total_bits_sent
#define my_packet_data_size_stat		op_sv_ptr->my_packet_data_size_stat
#define next_frame_arrival_time 		op_sv_ptr->next_frame_arrival_time
#define alreadySent             		op_sv_ptr->alreadySent
#define FrameCounter            		op_sv_ptr->FrameCounter
#define imageLineNo             		op_sv_ptr->imageLineNo
#define myName                  		op_sv_ptr->myName
#define frameSizeStat           		op_sv_ptr->frameSizeStat
#define inputPacketSize         		op_sv_ptr->inputPacketSize
#define averageFrameSizeInPackets		op_sv_ptr->averageFrameSizeInPackets
#define overallPacketCounter    		op_sv_ptr->overallPacketCounter
#define RTPoverhead             		op_sv_ptr->RTPoverhead
#define RTPoverheadResetFlag    		op_sv_ptr->RTPoverheadResetFlag
#define RTPoverheadStat         		op_sv_ptr->RTPoverheadStat
#define av_interarrival_stat    		op_sv_ptr->av_interarrival_stat
#define sizeInfo                		op_sv_ptr->sizeInfo
#define imageName               		op_sv_ptr->imageName
#define directoryName           		op_sv_ptr->directoryName
#define imageNo                 		op_sv_ptr->imageNo
#define absDeviationSum         		op_sv_ptr->absDeviationSum
#define DeviationCounter        		op_sv_ptr->DeviationCounter
#define absDeviationStat        		op_sv_ptr->absDeviationStat
#define signedDeviationStat     		op_sv_ptr->signedDeviationStat
#define signedDeviationSum      		op_sv_ptr->signedDeviationSum
#define lastFrameSize           		op_sv_ptr->lastFrameSize
#define lastSmoothedFrameSize   		op_sv_ptr->lastSmoothedFrameSize
#define lastFrameSizeAverageCalculationTime		op_sv_ptr->lastFrameSizeAverageCalculationTime
#define frameCounter            		op_sv_ptr->frameCounter
#define frameSizeSum            		op_sv_ptr->frameSizeSum
#define averageFrameSize        		op_sv_ptr->averageFrameSize
#define averageAppFromAverageFrameSize_stat		op_sv_ptr->averageAppFromAverageFrameSize_stat

/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	ma_bursty_source_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((ma_bursty_source_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


/* Function Block */

#if !defined (VOSD_NO_FIN)
enum { _op_block_origin = __LINE__ + 2};
#endif

static void
bursty_source_sv_init ()
{
	Prohandle			my_prohandle;
	int					my_pro_id;
	Objid				my_id;
	Objid				parent_id;
	Objid				traf_gen_comp_attr_objid, traf_conf_objid;
	Objid				pkt_gen_comp_attr_objid, pkt_gen_args_objid;
	char				on_state_string [128], off_state_string [128];
	char				packet_size_string [128];
	char				intarrvl_rate_string[128],start_time_string [128];
	char				temp[5];
	
	OmsT_Dist_Handle	start_time_dist_handle;
	
	int lineNo, q, compare = 0;
	FILE	 * sizeInfoFile;
	char line[300];
	

	/**	Initializes state variables associated with		**/
	/**	this process model.								**/
	FIN (bursty_source_sv_init ());
	
	
	op_ima_sim_attr_get_str("curve",99, curveInApp);
	op_ima_sim_attr_get_str("Bnadwidth Allocation Method",99, methodInApp);
	op_ima_sim_attr_get_int32("Network Size",&nodes_no_app);
	
	
	//Start FFMPEG by first registering the use of all codecs and network streams.
	av_register_all();
	avformat_network_init();

	if(strcmp(methodInApp,"EDCA")==0 || strcmp(methodInApp,"dist_withoutAnyEnhancement")==0)
	{
		transitionTimeApp=10;
	}
	

	/*	Determine the prohandle of this process as well as	*/
	/*	the object IDs of the containing module and node.*/
	my_prohandle = op_pro_self ();
	my_pro_id = op_pro_id (my_prohandle);
	my_id = op_id_self ();
	parent_id = op_topo_parent(my_id);

	/*	Determine the process ID display string.	*/
	sprintf (pid_string, "bursty_source PID (%d)", my_pro_id);

	/*	Determine whether or not the simulation is in debug	*/
	/*	mode.  Trace statement are only enabled when the	*/
	/*	simulation is in debug mode.						*/
	debug_mode = op_sim_debug ();
	
	op_ima_obj_attr_get_str (my_id, "name",20, myName);
	op_ima_obj_attr_get_str (parent_id, "name", 60, parentName);

	/* Read the traffic generation parameters.	*/
	op_ima_obj_attr_get (my_id, "Traffic Generation Parameters", &traf_gen_comp_attr_objid);
	traf_conf_objid = op_topo_child (traf_gen_comp_attr_objid, OPC_OBJTYPE_GENERIC, 0);
	
	//read source video trafic from atributes
	op_ima_obj_attr_get (traf_conf_objid, "Frame Rate (frame/sec)", &frameRate);
	op_ima_obj_attr_get (traf_conf_objid, "Frame Size (KB)", &frameSize);
	
	//calculating application rate in bytes
	frameSize = frameSize*1024; //frame size in bytes
	appRate = frameRate * frameSize;
	
	frameRate_stat = op_stat_reg ("Frame Rate (frame/sec) from app",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);

	op_stat_write (frameRate_stat, (double) frameRate);
	
	
	/* Determine the start time for traffic generation.	*/
	op_ima_obj_attr_get (traf_conf_objid, "Start Time", start_time_string);
	if (strcmp (start_time_string, "Never") != 0)
	{
		start_time_dist_handle = oms_dist_load_from_string (start_time_string);
		start_time = oms_dist_outcome (start_time_dist_handle);
		start_time = start_time + 0.5;//added by me to allow mac to inicialize before app
	}
	else
	{
		start_time = -1.0;
	}

	/* Determine the stop time for traffic generation.	*/
	op_ima_obj_attr_get (traf_conf_objid, "Stop Time", &stop_time);
	
	if (stop_time == -1.0)
	{	
		stop_time = OPC_DBL_INFINITY;
	}
		
	/* If the start time is set to "Infinity", then there	*/
	/* is no need to schedule an interrupt as this node has	*/
	/* been set will not generate any traffic.				*/
	if ((start_time >= 0.0) && (stop_time > start_time))
	{
		/* Load the distribution used to determine the time	*/
		/* for which the process stays in the "ON" state.	*/
		op_ima_obj_attr_get (traf_conf_objid, "ON State Time", on_state_string);
		on_state_dist_handle = oms_dist_load_from_string (on_state_string); 

		/* Load the distribution used to determine the time	*/
		/* for which the process stays in the "OFF" state.	*/
		op_ima_obj_attr_get (traf_conf_objid, "OFF State Time", off_state_string);
		off_state_dist_handle = oms_dist_load_from_string (off_state_string);

		/* Load the distribution used to determine the packet	*/
		/* interarrivals.										*/
		op_ima_obj_attr_get (traf_conf_objid, "Packet Generation Arguments", &pkt_gen_comp_attr_objid);
		pkt_gen_args_objid = op_topo_child (pkt_gen_comp_attr_objid, OPC_OBJTYPE_GENERIC, 0);

		
		/* Load the distribution used to determine the size of	*/
		/* the packets being generated.							*/
		op_ima_obj_attr_get (pkt_gen_args_objid, "Packet Size", packet_size_string);//should be constant //will not be used
		packet_size_dist_handle = oms_dist_load_from_string (packet_size_string);
		
		inputPacketSize = (double)packet_size_dist_handle->dist_info.dist_params.dist_arg0;//will be used instead of packet_size_dist_handle
		
				
		//op_ima_obj_attr_get (pkt_gen_args_objid, "Interarrival Time", intarrvl_rate_string);
		//sprintf(intarrvl_rate_string,"exponential(%f)",packet_size_dist_handle->dist_info.dist_params.dist_arg0/appRate);//need to be changed to frame interarrival time
		sprintf(intarrvl_rate_string,"exponential(%f)",(double)frameSize/appRate);//need to be changed to frame interarrival time//not used
		intarrvl_time_dist_handle = oms_dist_load_from_string (intarrvl_rate_string);

		/*	Initilaize the packet generation status statistic	*/
		/*	to indicate that currently there are no packets 	*/
		/*	being generated.									*/
		pksize_stathandle = op_stat_reg ("Traffic Source.Packet Generation Status", OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
		op_stat_write (pksize_stathandle, (double) OPC_FALSE);
		
		/* Initilaize the statistic handles to keep	*/
		/* track of traffic Sourceed by this process.	*/
		bits_sent_stathandle 		= op_stat_reg ("Traffic Source.Traffic Sent (bits)",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
		bitssec_sent_stathandle 	= op_stat_reg ("Traffic Source.Traffic Sent (bits/sec)",	OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
		pkts_sent_stathandle 		= op_stat_reg ("Traffic Source.Traffic Sent (packets)",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
		pktssec_sent_stathandle 	= op_stat_reg ("Traffic Source.Traffic Sent (packets/sec)",	OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);

		
		
		bits_sent_gstathandle 		= op_stat_reg ("Traffic Source.Traffic Sent (bits)",		OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
		bitssec_sent_gstathandle 	= op_stat_reg ("Traffic Source.Traffic Sent (bits/sec)",	OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
		pkts_sent_gstathandle 		= op_stat_reg ("Traffic Source.Traffic Sent (packets)",		OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
		pktssec_sent_gstathandle 	= op_stat_reg ("Traffic Source.Traffic Sent (packets/sec)",	OPC_STAT_INDEX_NONE, OPC_STAT_GLOBAL);
		
		interarrival_stat			=op_stat_reg ("interarrival_stat",	OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
		
		av_interarrival_stat		=op_stat_reg ("av_interarrival_stat",	OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
		
		//mohammad
		app_appRate_stat = op_stat_reg ("application rate from source",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
		average_packet_size_stat = op_stat_reg ("average app packet size",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
		my_packet_size_stat = op_stat_reg ("My App Packet Size (bits)",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
		my_packet_data_size_stat = op_stat_reg ("My App Packet DATA Size (bits)",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
		RTPoverheadStat = op_stat_reg ("RTPoverheadStat",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);

		//appRate = on_state_dist_handle->dist_info.dist_params.dist_arg0 / (on_state_dist_handle->dist_info.dist_params.dist_arg0+off_state_dist_handle->dist_info.dist_params.dist_arg0)
		//	* packet_size_dist_handle->dist_info.dist_params.dist_arg0 /intarrvl_time_dist_handle->dist_info.dist_params.dist_arg0;

		op_stat_write (average_packet_size_stat,(double) packet_size_dist_handle->dist_info.dist_params.dist_arg0);
		op_stat_write (app_appRate_stat, (double) appRate*8);
		op_stat_write (interarrival_stat,(double) frameSize/appRate);
		/* Check if packet segmentation is modeled.	*/
		op_ima_obj_attr_get (pkt_gen_args_objid, "Segmentation Size", &segmentation_size);
		if (segmentation_size != -1)
		{
			segmentation_size *= 8;
			segmentation_buf_handle = op_sar_buf_create (OPC_SAR_BUF_TYPE_SEGMENT, OPC_SAR_BUF_OPT_PK_BNDRY);
		}
	}
	
	
	//loading size info
	if(strcmp(curveInApp,"accu_quality_GT50_withNI")==0 || strcmp(curveInApp,"accu_quality_GT50_withoutNI")==0)
	{	
		sizeInfoFile = fopen("C:\\facedatabase\\georgiaTech\\gt_db50\\sizeInfo50.txt","r");
	}
	
	else if(strcmp(curveInApp,"accu_quality_GT_withoutNI")==0)
	{
		sizeInfoFile = fopen("C:\\facedatabase\\georgiaTech\\sizeInfo.txt","r");
	}
	else if(strcmp(curveInApp,"accu_quality_GT75_withoutNI")==0)
	{
		sizeInfoFile = fopen("C:\\facedatabase\\georgiaTech\\gt_db75\\sizeInfo75.txt","r");
	}	
	else if(strcmp(curveInApp,"accu_quality_scfaced1_withNI")==0 || strcmp(curveInApp,"accu_quality_scfaced1_withoutNI")==0)
	{
		sizeInfoFile = fopen("C:\\facedatabase\\SCFace\\sizeInfo_distance1.txt","r");
	}
	else if(strcmp(curveInApp,"accu_quality_scfaced2_withNI")==0 || strcmp(curveInApp,"accu_quality_scfaced2_withoutNI")==0)
	{
		sizeInfoFile = fopen("C:\\facedatabase\\SCFace\\sizeInfo_distance2.txt","r");
	}
	else if(strcmp(curveInApp,"accu_quality_scfaced3_withNI")==0 || strcmp(curveInApp,"accu_quality_scfaced3_withoutNI")==0)
	{
		sizeInfoFile = fopen("C:\\facedatabase\\SCFace\\sizeInfo_distance3.txt","r");
	}
	else if(strcmp(curveInApp,"accu_quality_cmumit_withNI")==0||strcmp(curveInApp,"accu_quality_cmumit_withoutNI")==0)
	{			
		sizeInfoFile = fopen("C:\\facedatabase\\CMU_MIT\\sizeInfo.txt","r");
	}
	else if(strcmp(curveInApp,"accu_quality_NEWcmumit_withoutNI")==0)
	{			
		sizeInfoFile = fopen("C:\\facedatabase\\CMU_MIT\\anew\\NewCMUSizeInfo.txt","r");
	}
	else if(strcmp(curveInApp,"accu_quality_NEWcmumitSmall_withoutNI")==0)
	{			
		sizeInfoFile = fopen("C:\\facedatabase\\CMU_MIT\\anewSmall\\sizeInfo.txt","r");
	}
	
	if(!sizeInfoFile)
	{
		op_sim_end("size info file could not be opened"," ","","");
	}
	
	fgets(line, 1000,sizeInfoFile);
	if(appOpencvDebugFlag)
	{
		sprintf(myString,"first line is %s\n",line);
		op_prg_odb_print_major(myString,OPC_NIL);
	}
	
	printf("first line is %s\n",line);
		
	sscanf(line,"%d",&imageNo);//read image number from the size info file first line
	//imageLineNo = rand() % imageNo + 1; // the image line number randomly this is the line number that we want to read from the size info file
		
	sizeInfo = (int**)calloc (imageNo+1,sizeof(int*));
	directoryName = (char**) calloc (imageNo+1,sizeof(char*));
	imageName = (char**) calloc (imageNo+1,sizeof(char*));
		
	for(lineNo = 1 ; lineNo < imageNo+1;lineNo++)
	{		
		sizeInfo[lineNo]= (int*)calloc (101,sizeof(int));
		directoryName[lineNo] = (char*) calloc (20,sizeof(char));
		imageName[lineNo] = (char*) calloc (50,sizeof(char));
		fscanf(sizeInfoFile,"%s%s",imageName[lineNo], directoryName[lineNo]);
			
		for(q = 1 ; q < 101;q++)
		{
			fscanf(sizeInfoFile,"%d",&sizeInfo[lineNo][q]);
		}
		/*
		if(Node_LorenDebugFlag)
		{
			printf("%s%s\n",imageName[lineNo], directoryName[lineNo]);		
		}
		*/
	}
	
	fclose(sizeInfoFile);
	
	printf("allocateFlag = %d\n", allocateFlag);
	
	//Allocate the structs for each node. This only needs to be done once.
	if(allocateFlag == 0)
	{
		printf("allocateFlag == 0, allocating ffmpeg struct\n");
		op_ima_sim_attr_get_int32("Network Size",&nodeNumber);
		
		//Subtract 1 from the number of nodes since node 0 doesn't need a struct.
		vidData = new FFMPEGData[nodeNumber-1];
        allocateFlag = 1;
	}


	//First check if this is the right node for executing this code.
	printf("about to check parent name for node number.\n");
	
	compare = strcmp(parentName, "node_1");
	
	printf("compare = %d\n", compare);
	
	//int ID = parentName[numOff] - '0' - 1;
	snprintf(temp, 5, &parentName[numOff]);
	printf("temp string = %s\n", temp);
	int ID = atoi(temp) - 1;
	printf("ID = %d\n", ID);
	
	if(compare == 0)
	{
		if(vidData[ID].restart == 1)
		{
			startFFMPEG(vidData[ID], ID);
			printf("filepath = %s\n", vidData[ID].filepath);
			//vidData[ID].restart = 0;
		}
		printf("about to create pipe file.\n");
		hPipe = CreateFile(TEXT("\\\\.\\pipe\\Pipe"), 
						  GENERIC_READ | GENERIC_WRITE, 
	                      0,
	                      NULL,
	                      OPEN_EXISTING,
	                      0,
	                      NULL);

	}
	
	FOUT;
}


void startFFMPEG(FFMPEGData &vidData, int node)
{
	char sdp[100];
	FIN (startFFMPEG(vidData, node));
	
	/* This FFMPEG code is based off the Dranger Tutorials at http://dranger.com/ffmpeg/ */
	/* FFMPEG version 2.8 Zeranoe implementation used. 									 */

	
	printf("Calling ffmpeg code.\n");
	
	//translate the node number into the proper sdp file name.
	sprintf(sdp, "C:\\OPNET\\14.5.A\\models\\std\\before GT30\\traf_gen\\test%d.sdp", node + 1);
	printf("%s\n", sdp);
	
	strcpy(vidData.filepath, sdp);
	
	//Load ffmpeg stream
	vidData.pFormatCtx = avformat_alloc_context();
		
	//printf("ffmpeg_flag = %d\n", ffmpeg_flag);

	//Open the input stream using the filepath. In this case it is the external stream from my FFMPEG streaming program.
	if(avformat_open_input(&vidData.pFormatCtx, vidData.filepath,NULL,NULL)!=0)
	{
		printf("Couldn't open input stream.\n");
		ffmpeg_flag = 1;
	}
	

	if(avformat_find_stream_info(vidData.pFormatCtx,NULL)<0)
	{
		printf("Couldn't find stream information.\n");
		//return -1;
	}

	vidData.videoindex=-1;
	for(unsigned int i=0; i<vidData.pFormatCtx->nb_streams; i++)
	{
		if(vidData.pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
		{
			vidData.videoindex=i;
			break;
		}
	}

	if(vidData.videoindex==-1)
	{
		printf("Didn't find a video stream.\n");
		//return -1;
	}

	vidData.pCodecCtx = vidData.pFormatCtx->streams[vidData.videoindex]->codec;
	vidData.pCodec = avcodec_find_decoder(vidData.pCodecCtx->codec_id);

	if(vidData.pCodec==NULL)
	{
		printf("Codec not found.\n");
		//return -1;
	}

	if(avcodec_open2(vidData.pCodecCtx, vidData.pCodec,NULL)<0)
	{
		printf("Could not open codec.\n");
	    //return -1;
	}

	//Allocate memory for ffmpeg packet.
	vidData.ffmpeg_packet=(AVPacket *)av_malloc(sizeof(AVPacket));
	
	FOUT;
}

/* End of Function Block */

/* Undefine optional tracing in FIN/FOUT/FRET */
/* The FSM has its own tracing code and the other */
/* functions should not have any tracing.		  */
#undef FIN_TRACING
#define FIN_TRACING

#undef FOUTRET_TRACING
#define FOUTRET_TRACING

/* Undefine shortcuts to state variables because the */
/* following functions are part of the state class */
#undef pid_string
#undef debug_mode
#undef stop_time
#undef off_state_start_time
#undef pksize_stathandle
#undef on_state_dist_handle
#undef off_state_dist_handle
#undef intarrvl_time_dist_handle
#undef packet_size_dist_handle_
#undef start_time
#undef bits_sent_stathandle
#undef bitssec_sent_stathandle
#undef pkts_sent_stathandle
#undef pktssec_sent_stathandle
#undef bits_sent_gstathandle
#undef bitssec_sent_gstathandle
#undef pkts_sent_gstathandle
#undef pktssec_sent_gstathandle
#undef segmentation_size
#undef segmentation_buf_handle
#undef app_appRate_stat
#undef appRate
#undef interarrival_stat
#undef frameRate_stat
#undef average_packet_size_stat
#undef frameRate
#undef frameSize
#undef packet_size_dist_handle
#undef my_packet_size_stat
#undef total_bits_sent
#undef my_packet_data_size_stat
#undef next_frame_arrival_time
#undef alreadySent
#undef FrameCounter
#undef imageLineNo
#undef myName
#undef frameSizeStat
#undef inputPacketSize
#undef averageFrameSizeInPackets
#undef overallPacketCounter
#undef RTPoverhead
#undef RTPoverheadResetFlag
#undef RTPoverheadStat
#undef av_interarrival_stat
#undef sizeInfo
#undef imageName
#undef directoryName
#undef imageNo
#undef absDeviationSum
#undef DeviationCounter
#undef absDeviationStat
#undef signedDeviationStat
#undef signedDeviationSum
#undef lastFrameSize
#undef lastSmoothedFrameSize
#undef lastFrameSizeAverageCalculationTime
#undef frameCounter
#undef frameSizeSum
#undef averageFrameSize
#undef averageAppFromAverageFrameSize_stat

/* Access from C kernel using C linkage */
extern "C"
{
	VosT_Obtype _op_ma_bursty_source_init (int * init_block_ptr);
	VosT_Address _op_ma_bursty_source_alloc (VosT_Obtype, int);
	void ma_bursty_source (OP_SIM_CONTEXT_ARG_OPT)
		{
		((ma_bursty_source_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))->ma_bursty_source (OP_SIM_CONTEXT_PTR_OPT);
		}

	void _op_ma_bursty_source_svar (void *, const char *, void **);

	void _op_ma_bursty_source_diag (OP_SIM_CONTEXT_ARG_OPT)
		{
		((ma_bursty_source_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr))->_op_ma_bursty_source_diag (OP_SIM_CONTEXT_PTR_OPT);
		}

	void _op_ma_bursty_source_terminate (OP_SIM_CONTEXT_ARG_OPT)
		{
		/* The destructor is the Termination Block */
		delete (ma_bursty_source_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr);
		}


} /* end of 'extern "C"' */




/* Process model interrupt handling procedure */


void
ma_bursty_source_state::ma_bursty_source (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (ma_bursty_source_state::ma_bursty_source ());
	try
		{
		/* Temporary Variables */
		int					intrpt_type;
		int					intrpt_code;
		
		Packet*				pkptr;
		
		double				pksize;
		double				on_period;
		//double				off_period;
		
		char 				intarrvl_rate_string[128];
		/* End of Temporary Variables */


		FSM_ENTER ("ma_bursty_source")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (init) enter executives **/
			FSM_STATE_ENTER_UNFORCED_NOLABEL (0, "init", "ma_bursty_source [init enter execs]")
				FSM_PROFILE_SECTION_IN ("ma_bursty_source [init enter execs]", state0_enter_exec)
				{
				/* Initialize the traffic generation parameters.	*/
				bursty_source_sv_init ();
				
				frameSizeStat 		= op_stat_reg ("frameSizeStat",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
				absDeviationStat 		= op_stat_reg ("absDeviationStat",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
				signedDeviationStat 		= op_stat_reg ("signedDeviationStat",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
				averageAppFromAverageFrameSize_stat = op_stat_reg ("averageAppFromAverageFrameSize_stat",		OPC_STAT_INDEX_NONE, OPC_STAT_LOCAL);
				
				total_bits_sent = 0;
				alreadySent = 0;
				FrameCounter=0;
				averageFrameSizeInPackets = 0;
				overallPacketCounter = 0;
				
				RTPoverhead = 0;
				RTPoverheadResetFlag = 0;
				
				absDeviationSum = 0;
				signedDeviationSum = 0;
				DeviationCounter = 0;
				frameCounter = 0;
				frameSizeSum = 0;
				lastFrameSizeAverageCalculationTime = EAestimationTimeApp;
				
				/* Schedule the first OFF-period scheduling by setting	*/
				/* a self-interrupt for the start time. If the start	*/
				/* time is set to "Infinity", then there is no need to	*/
				/* to schedule an interrupt as this node has been set	*/
				/* will not generate any traffic.						*/
				if (start_time >= 0.0)
					{
					op_intrpt_schedule_self (op_sim_time () + start_time, 0);
					}
				
				}
				FSM_PROFILE_SECTION_OUT (state0_enter_exec)

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (1,"ma_bursty_source")


			/** state (init) exit executives **/
			FSM_STATE_EXIT_UNFORCED (0, "init", "ma_bursty_source [init exit execs]")


			/** state (init) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "init", "off", "tr_0", "ma_bursty_source [init -> off : default / ]")
				/*---------------------------------------------------------*/



			/** state (off) enter executives **/
			FSM_STATE_ENTER_UNFORCED (1, "off", state1_enter_exec, "ma_bursty_source [off enter execs]")
				FSM_PROFILE_SECTION_IN ("ma_bursty_source [off enter execs]", state1_enter_exec)
				{
				/*	Schedule a self-interrupt to transit to the "ON"	*/
				/*	when the "OFF" state duration expires.				*/
				//if (op_sim_time () + off_period < stop_time)
				//	op_intrpt_schedule_self (op_sim_time () + off_period, OFF_TO_ON);
				
				sprintf(myString,"OFF State\n");
				op_prg_odb_print_major(myString,OPC_NIL);
				printf("start time at %f\n",(float) start_time);
				
				
				//if a new value of the application rate arrived from the mac layer
				if(intrpt_type == OPC_INTRPT_STAT)
				{
					// Store the last apprate for later comparison.
					prevAppRate = appRate; 
					
					newValue = op_stat_local_read(0)/8.0;
					
					//if(newValue > 54000000.0/8.0/nodes_no_app)
					//	newValue = 54000000.0/8.0/nodes_no_app;
					
					appRate = newValue;
					frameSize = appRate/ frameRate;
					
					sprintf(intarrvl_rate_string,"exponential(%f)",packet_size_dist_handle->dist_info.dist_params.dist_arg0/(appRate));
					intarrvl_time_dist_handle = oms_dist_load_from_string (intarrvl_rate_string);
					op_stat_write (app_appRate_stat, (double) appRate);
				}
				
				//(mohammad) go to on state directlly
				if (start_time < stop_time && intrpt_type == OPC_INTRPT_SELF && intrpt_code == 0)
				{
					printf("on period will start at %f\n",(float) start_time);
					op_intrpt_schedule_self (start_time , OFF_TO_ON);
				}
					
				}
				FSM_PROFILE_SECTION_OUT (state1_enter_exec)

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (3,"ma_bursty_source")


			/** state (off) exit executives **/
			FSM_STATE_EXIT_UNFORCED (1, "off", "ma_bursty_source [off exit execs]")
				FSM_PROFILE_SECTION_IN ("ma_bursty_source [off exit execs]", state1_exit_exec)
				{
				/* Determine the type of interrupt.		*/
				intrpt_type = op_intrpt_type ();
				intrpt_code = op_intrpt_code ();
				}
				FSM_PROFILE_SECTION_OUT (state1_exit_exec)


			/** state (off) transition processing **/
			FSM_PROFILE_SECTION_IN ("ma_bursty_source [off trans conditions]", state1_trans_conds)
			FSM_INIT_COND (INACTIVE_TO_ACTIVE)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("off")
			FSM_PROFILE_SECTION_OUT (state1_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, ;, "INACTIVE_TO_ACTIVE", "", "off", "on", "tr_1", "ma_bursty_source [off -> on : INACTIVE_TO_ACTIVE / ]")
				FSM_CASE_TRANSIT (1, 1, state1_enter_exec, ;, "default", "", "off", "off", "tr_14", "ma_bursty_source [off -> off : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (on) enter executives **/
			FSM_STATE_ENTER_UNFORCED (2, "on", state2_enter_exec, "ma_bursty_source [on enter execs]")
				FSM_PROFILE_SECTION_IN ("ma_bursty_source [on enter execs]", state2_enter_exec)
				{
				if (op_intrpt_code () == OFF_TO_ON)
				{
					/*	Determine the time at which this process will	*/
					/*	enter the next	"OFF" state.					*/
					//on_period = oms_dist_positive_outcome_with_error_msg (on_state_dist_handle, 
					//	"This occurs for ON period distribution in bursty_source process model.");
					
					on_period=OPC_DBL_INFINITY; //(mohammad)keep it on
					off_state_start_time = op_sim_time () + on_period;
				
					/*	Schedule a self-interrupt to transit to "OFF"	*/
					/*	state when the "ON" state duration expires.		*/
					op_intrpt_schedule_self (off_state_start_time, ON_TO_OFF);//keep it on all the time
				}
				
				
				//if a new value of the application rate arrived from the mac layer
				if(intrpt_type == OPC_INTRPT_STAT )
				{
						
					newValue = op_stat_local_read(0)/8.0;//read apprate in bytes
					
					//if(newValue > 54000000.0/8.0/nodes_no_app)
					//	newValue = 54000000.0/8.0/nodes_no_app;	
					
					appRate = newValue;
					frameSize = appRate/frameRate;//frame size in byte
					printf("New appRate = %f, framesize = %f, framerate = %f\n", (float)appRate, (float)frameSize, (float)frameRate);
					flag = 1;
					
					sprintf(intarrvl_rate_string,"exponential(%f)",(double)frameSize/(appRate));//it is used
					intarrvl_time_dist_handle = oms_dist_load_from_string (intarrvl_rate_string);
				
				}
					
				
				
				else if( (intrpt_code==ON_TO_ON || intrpt_code==OFF_TO_ON) && intrpt_type ==OPC_INTRPT_SELF )//&&added by me
				{
					Objid				my_id;
					Objid				parent_id;
					char 				temp[5];
					printf("Start of ma_bursty_source on-on\n");
					
					
					my_id = op_id_self ();
					parent_id = op_topo_parent(my_id);
					op_ima_obj_attr_get_str (parent_id, "name", 60, parentName);
					
					// Get the ID of this node to use for the vidData array.
					//int ID = parentName[numOff] - '0' - 1;
					snprintf(temp, 5, &parentName[numOff]);
					printf("temp string = %s\n", temp);
					int ID = atoi(temp) - 1;
					printf("ID = %d\n", ID);
					
					//Added to determine how often the apprate changes.
					sprintf(myAppRateTraceName, "C:\\AppRateTraceFiles\\Apprate_%s.txt", parentName);
					appRateOutputFile = fopen(myAppRateTraceName, "a");
					fprintf(appRateOutputFile, "%f: %s apprate = %f\n", op_sim_time(), parentName, (float)appRate);
					fclose(appRateOutputFile);
					
					
					// Determine whether we need to notify the control program that the bitrate has changed.
					// Include some hystersis so that the control program doesn't constantly have to change the bitrate.
					if(ID == 0)
					{
					if((appRate >= (prevAppRate + 50)) || (appRate <= (prevAppRate - 50)))
					{
					
						printf("Node: %s, ID: %d\n", parentName, ID);
						// Create the string with the correct format to be handled by the control program.
						sprintf_s(message, "%d,%d", ID, (int)appRate);
						
						/* Named Pipe Stuff */
						
								
						printf("checking if pipe handle is valid.\n");
						if (hPipe != INVALID_HANDLE_VALUE)
						{
							printf("About to write to pipe.\n");
							WriteFile(hPipe,
									 message,				
								     sizeof(message),						 // = length of string + terminating '\0' !!!
								     &dwWritten,
								     NULL);
							
							printf("about to close pipe handle.\n");				
							//CloseHandle(hPipe);
						}
						
						// Prepare for restartj
						printf("preparing for restart\n");
						//av_free_packet(vidData[ID].ffmpeg_packet);
						printf("1\n");
						//sws_freeContext(vidData[ID].convert_ctx);
						printf("2\n");
						//av_frame_free(&vidData[ID].pFrame);
						printf("3\n");
						avcodec_close(vidData[ID].pCodecCtx);
						printf("4\n");
						avformat_close_input(&vidData[ID].pFormatCtx);
						
						printf("about to sleep\n");
						Sleep(1000);
						printf("done sleeping, calling startFFMPEG.\n");
						
						// Restart the stream.
						startFFMPEG(vidData[ID], ID);
						printf("done with startFFMPEG.\n");
						prevAppRate = appRate;
					}
					}
					
					if (op_sim_time () >= EAestimationTimeApp)//if EA estimation time is done
					{
				
						int q;
						//int sizes[100];//will hold the sizes of the image choosen for its 100 qualities
						char line[1001];//will be used in the skipping process and to hold the file path
						//char directoryName[10]="";
						//char imageName[50];
				
						myImageStructure *ids, *originalids;
						
						int		FrameSizeInPackets=0; 
						int 	PacketCounter=0;
						int 	frameDataPacketsSizeSum = 0;
						int     testValue = 0;
						double 	originalFrameSize = 0;
						
						int 	lastPacketSize = 0;
						double  currentSmoothedFrameSize;
						double 	alpha = 0.5;
						int size = 0;
						int compare = 0;
						
						
						//Loren, counter value
						int x = 0;
						
						printf("I am: %s\n", myName);
						printf("Parent is %s\n", parentName);
						
						if(RTPoverheadResetFlag == 0)
						{
							RTPoverheadResetFlag = 1;
							RTPoverhead = 0;
						}
						
							
						printf("Capturing frame from stream.\n");
				
						
				
						frameSize = appRate/frameRate;
						originalFrameSize = frameSize;
						alreadySent = 0;
						printf("Frame Size = %d, AppRate = %d\n",(int)frameSize,(int)appRate);	
						op_stat_write (app_appRate_stat, (double) appRate*8);
						
						printf("methodInApp is %s\n",methodInApp);
						//printf("transitionTimeApp=%d\n"	,	transitionTimeApp);
				
						imageLineNo = rand() % imageNo + 1; // the image line number randomly this is the line number that we want to read from the size info file
						
						if(appOpencvDebugFlag)
						{
							printf("image %d chosen from total of %d\n",imageLineNo,imageNo);			
							printf("image %s chosen from directory %s\n",imageName[imageLineNo], directoryName[imageLineNo]);
						}
						
						//this code will chose the closest frame size to the optimization output
						for(q =1; q<=100; q++)
						{
							
							if(sizeInfo[imageLineNo][q] >= frameSize)
							{
							
								if(q > 1 && frameSize - sizeInfo[imageLineNo][q-1] < sizeInfo[imageLineNo][q] - frameSize)
								{			
									frameSize = sizeInfo[imageLineNo][q-1];						//adjust frame size to a possible value
									q = q-1;
								}
								
								else if(q > 1 && frameSize - sizeInfo[imageLineNo][q-1] > sizeInfo[imageLineNo][q] - frameSize) 
								{
									frameSize = sizeInfo[imageLineNo][q];						//adjust frame size to a possible value
								}
							
								else
								{	
									frameSize = sizeInfo[imageLineNo][q];
								}
								
								break;
							}
						}
					
						if(q >= 101) //frame size is very large send just the full quality image
						{
							frameSize = sizeInfo[imageLineNo][100];
							q = 100;
						}
						
						printf("Frame Size recalculated = %d\n",(int)frameSize);
						
						frameCounter++;
						frameSizeSum += frameSize;
						
						if(op_sim_time()-lastFrameSizeAverageCalculationTime >= frameSizeAverageCalculationPeriod)
						{
							averageFrameSize = frameSizeSum / frameCounter;
							frameCounter = 0;
							frameSizeSum = 0;
							lastFrameSizeAverageCalculationTime = op_sim_time();
						}
				
						op_stat_write (averageAppFromAverageFrameSize_stat,averageFrameSize*20*8);
							
						 
						absDeviationSum += abs(originalFrameSize - frameSize);
						signedDeviationSum += originalFrameSize - frameSize;
						DeviationCounter ++;
						 
						 
						 
						if(DeviationCounter ==1)
						{
							currentSmoothedFrameSize = originalFrameSize;
						}
						else
						{
							currentSmoothedFrameSize = alpha * lastSmoothedFrameSize + (1-alpha) * frameSize;
						}
						
						lastFrameSize = frameSize;
						lastSmoothedFrameSize = currentSmoothedFrameSize;
						printf("Current Smoothed Frame Size = %d\n", (int)currentSmoothedFrameSize);	 
									
						// update opnet states.				
						op_stat_write (frameSizeStat, (double) frameSize);
						op_stat_write (absDeviationStat, (double) abs(originalFrameSize - frameSize)/originalFrameSize);
						op_stat_write (signedDeviationStat, (double) (originalFrameSize - frameSize)/originalFrameSize);		
				
						if(appOpencvDebugFlag)
						{
							printf("image %s chosen from directory %s with quality %d and size = %d, appRate = %f\n",imageName[imageLineNo], directoryName[imageLineNo],q, (int)frameSize, (float) appRate);
						}
						 
						op_stat_write (av_interarrival_stat,(double) originalFrameSize/appRate);
						
						sprintf(intarrvl_rate_string,"exponential(%f)",(double)originalFrameSize/(appRate));//use original frame size to keep trying to get the rate from the optimization
						
						intarrvl_time_dist_handle = oms_dist_load_from_string (intarrvl_rate_string);
						
						next_frame_arrival_time = op_sim_time () + oms_dist_outcome(intarrvl_time_dist_handle);//originalFrameSize / appRate;
						
						op_stat_write (interarrival_stat,(double)next_frame_arrival_time - op_sim_time ());
				
						if(frameSize != 0)
						{
					
							if(strcmp(curveInApp,"accu_quality_GT50_withNI")==0 || strcmp(curveInApp,"accu_quality_GT50_withoutNI")==0)
							{
								sprintf(line,"C:\\facedatabase\\georgiaTech\\gt_db50\\%03d\\%s\\%s",q,directoryName[imageLineNo],imageName[imageLineNo]);
							}
							else if(strcmp(curveInApp,"accu_quality_GT_withoutNI")==0)
							{
								sprintf(line,"C:\\facedatabase\\georgiaTech\\%03d\\%s\\%s",q,directoryName[imageLineNo],imageName[imageLineNo]);
							}
							else if(strcmp(curveInApp,"accu_quality_GT75_withoutNI")==0)
							{
								sprintf(line,"C:\\facedatabase\\georgiaTech\\gt_db75\\%03d\\%s\\%s",q,directoryName[imageLineNo],imageName[imageLineNo]);
							}
							else if(strcmp(curveInApp,"accu_quality_scfaced1_withNI")==0 || strcmp(curveInApp,"accu_quality_scfaced1_withoutNI")==0)
							{	
								sprintf(line,"C:\\facedatabase\\SCFace\\%03d\\distance1\\%s\\%s",q,directoryName[imageLineNo],imageName[imageLineNo]);
							}
							else if(strcmp(curveInApp,"accu_quality_scfaced2_withNI")==0 || strcmp(curveInApp,"accu_quality_scfaced2_withoutNI")==0)
							{
								sprintf(line,"C:\\facedatabase\\SCFace\\%03d\\distance2\\%s\\%s",q,directoryName[imageLineNo],imageName[imageLineNo]);	
							}
							else if(strcmp(curveInApp,"accu_quality_scfaced3_withNI")==0 || strcmp(curveInApp,"accu_quality_scfaced3_withoutNI")==0)
							{
								sprintf(line,"C:\\facedatabase\\SCFace\\%03d\\distance3\\%s\\%s",q,directoryName[imageLineNo],imageName[imageLineNo]);
							}		
							else if(strcmp(curveInApp,"accu_quality_cmumit_withNI")==0||strcmp(curveInApp,"accu_quality_cmumit_withoutNI")==0 || strcmp(curveInApp,"accu_quality_NEWcmumit_withoutNI")==0|| strcmp(curveInApp,"accu_quality_NEWcmumitSmall_withoutNI")==0)
							{	
								sprintf(line,"C:\\facedatabase\\CMU_MIT\\Q%03d\\%s\\%s.jpg",q,directoryName[imageLineNo],imageName[imageLineNo]);
							}
					
					
							if(appOpencvDebugFlag)
							{
								printf("after sprintf\n");
								printf("image file name with path is %s\n",line);
								printf("Input packet size = %f\n", (double)inputPacketSize);
							}
					
							ids = createImageDataStructure(line,inputPacketSize * 8);
							//originalids = createImageDataStructure(line,inputPacketSize * 8);
								
					
							if(!ids)
							{
								op_sim_end ("could not create image data structure in the application module", "", "", "");
							}
					
							lastPacketSize = decodeHuffman_PKT(ids,0);//packetize the image and return the last packet size in bits
					
							if(appOpencvDebugFlag)
							{
								printf("image data structure packetized\n");
							}
							
							
							// lorenModified to limit to 1 packet
						    FrameSizeInPackets = 1;
							//FrameSizeInPackets = ids->rtpPacketsNeeded;
						
							if(appOpencvDebugFlag)
							{
								printf("FrameSizeInPackets is %d\n",FrameSizeInPackets);
							}
						
							//while(ids->components[0][x].packetID <= FrameSizeInPackets)
							//{
								printf("packet ID = %d\n", ids->components[0][x].packetID);
								printf("Component Size = %d\n", sizeof(ids->components[0][x]));
							//	x++;
							//}
							//Loren: doing this here again
							printf("ids size = %d, image data structure size = %d\n\n", sizeof(ids), sizeof(myImageStructure));
							deleteImageDataStructure(ids);
							
							if(appOpencvDebugFlag)
							{
								printf("image data structure deleted\n");
							}
					
							
							if(appOpencvDebugFlag)
							{
								printf("sending image %s packets, number of packets is %d, lastPacketSize = %d  \n",line, FrameSizeInPackets,lastPacketSize);
							}
							
							RTPoverhead += (64+23)*8 + (FrameSizeInPackets-1)*23*8;
						
							op_stat_write (RTPoverheadStat, (double) RTPoverhead/(op_sim_time () - 20));
					
							PacketCounter = 0;
					
							frameDataPacketsSizeSum = 0;
							
							
							while(PacketCounter < FrameSizeInPackets)
							{
								int packetStatus = 0,tPS;
						
								pksize = floor ((double) oms_dist_positive_outcome_with_error_msg (packet_size_dist_handle, 
									"This occurs for packet size distribution in bursty_source process model."));
								
								pksize *= 8;//get packet size in bits.
								//Loren comment this if out if switching back to image
								if(PacketCounter == FrameSizeInPackets -1)//last packet
								{
									pksize = lastPacketSize;
									packetStatus = -1;
								}
								
								if(PacketCounter==0)
								{
									frameDataPacketsSizeSum += pksize -  (64+23)*8;
								}
								else
								{
									frameDataPacketsSizeSum += pksize -  23*8;
								}
								
								/* don't need this */
								alreadySent += pksize ;//- (PacketCounter ==0?(64+23)*8:23*8);
						
						
								if(appOpencvDebugFlag)
								{
									printf("sending image %s packets, sending packet number %d, pksize = %lf \n",line, PacketCounter,pksize);
								}
								
								total_bits_sent +=pksize;
								
								printf("total bits sent = %f\n", (double)total_bits_sent);
						
								//pkptr  = op_pk_create ((pksize>(7*32)?pksize-7*32:1)); //(pksize>7*32?pksize-7*32:1)
								pkptr  = op_pk_create_fmt("my_rtp_pkt");
								
								op_pk_format (pkptr, fmt_name);
								
								//printf("packet format at ma_bursty_source = %s\n", fmt_name);
								
								/* Compute total size of packet bytes 					*/
								pk_size = op_pk_total_size_get (pkptr);
								byte_load = (pk_size / 8.0);
								
								//printf("total initial packet size: %d. In bytes: %d\n", (int)pk_size, (int)byte_load);
								
								//marking the packets
								tPS = (PacketCounter==0 ? pksize -  (64+23)*8 : pksize -  23*8);
								printf("tPS = %d\n", (int)tPS);
								
								
								//Load ffmpeg stream
								printf("Calling ffmpeg code outside of init.\n");
					
								printf("about to check parent name for node number.\n");
								compare = strcmp(parentName, "node_1");
								printf("compare = %d\n", compare);
					
								if(compare == 0)
								{
									printf("filepath = %s\n", vidData[ID].filepath);
									//Output Info-----------------------------
									printf("--------------- File Information ----------------\n");
									av_dump_format(vidData[ID].pFormatCtx,0,vidData[ID].filepath,0);
									printf("-------------------------------------------------\n");
									
									if(av_read_frame(vidData[ID].pFormatCtx, vidData[ID].ffmpeg_packet) < 0)
									{
										printf("packet no good.\n");
									}
									else
									{
										frameCount++;
										time_t rawtime;
										struct tm * timeinfo;
										char buffer[80];
				
										time (&rawtime);
										timeinfo = localtime(&rawtime);
				
										strftime(buffer,80,"%d-%m-%Y %I:%M:%S",timeinfo);
												
										frameTime = fopen("C:\\AppRateTraceFiles\\frameTime.txt", "a");
										fprintf(frameTime, "%s: frameCount = %d\n",buffer, frameCount);
										fclose(frameTime);
									}
						
								}
								
								
								//Loren, for debug only
								//testValue = 42; 
								size = sizeof(originalids);
								printf("size of Image structure = %d\n", size);
								
								/*
								op_pk_fd_set (pkptr, 1, OPC_FIELD_TYPE_INTEGER, FrameCounter, 32);
								printf("pksize after filling in frame counter info  = %lf \n",(double) op_pk_total_size_get(pkptr));
								op_pk_fd_set (pkptr, 2, OPC_FIELD_TYPE_INTEGER, PacketCounter, 32);
								printf("pksize after filling in packet counter info  = %lf \n",(double) op_pk_total_size_get(pkptr));
								op_pk_fd_set (pkptr, 3, OPC_FIELD_TYPE_INTEGER, packetStatus, 32);
								//loren
								printf("pksize after filling in packet status info  = %lf \n",(double) op_pk_total_size_get(pkptr));
								op_pk_fd_set (pkptr, 4, OPC_FIELD_TYPE_INTEGER, FrameSizeInPackets, 32);
								printf("pksize after filling in frame size in packets info  = %lf \n",(double) op_pk_total_size_get(pkptr));
								op_pk_fd_set (pkptr, 5, OPC_FIELD_TYPE_INTEGER, imageLineNo, 32);
								printf("pksize after filling in image line number info  = %lf \n",(double) op_pk_total_size_get(pkptr));
								op_pk_fd_set (pkptr, 6, OPC_FIELD_TYPE_INTEGER, q, 32);
								printf("pksize after filling in quality info  = %lf \n",(double) op_pk_total_size_get(pkptr));
								op_pk_fd_set (pkptr, 7, OPC_FIELD_TYPE_INTEGER, tPS, 32);
								printf("pksize after filling in tps info  = %lf \n",(double) op_pk_total_size_get(pkptr));
								op_pk_fd_set (pkptr, 8, OPC_FIELD_TYPE_INTEGER, myArray, pksize);
								printf("pksize after filling in image data info  = %lf \n",(double) op_pk_total_size_get(pkptr));
								//op_pk_fd_set (pkptr, 8, OPC_FIELD_TYPE_INTEGER, testValue, 32);
								//op_pk_fd_set_ptr (pkptr, 7, originalids, frameSize, op_prg_mem_copy_create, op_prg_mem_free, sizeof(myImageStructure));
								printf("about to add image to packet.\n");
								//op_pk_fd_set (pkptr, 7, OPC_FIELD_TYPE_STRUCT, originalids, 512);//, op_prg_mem_copy_create, op_prg_mem_free);
								
								*/
								op_pk_nfd_set (pkptr, "frame_counter", FrameCounter);
								printf("pksize after filling in frame counter info  = %lf \n",(double) op_pk_total_size_get(pkptr));
								
								op_pk_nfd_set (pkptr, "packet_counter", PacketCounter);
								printf("pksize after filling in packet counter info  = %lf \n",(double) op_pk_total_size_get(pkptr));
								
								op_pk_nfd_set (pkptr, "packet_status", packetStatus);
								printf("pksize after filling in packet status info  = %lf \n",(double) op_pk_total_size_get(pkptr));
								
								op_pk_nfd_set (pkptr, "FrameSizeInPackets", FrameSizeInPackets);
								printf("pksize after filling in frame size in packets info  = %lf \n",(double) op_pk_total_size_get(pkptr));
								
								op_pk_nfd_set (pkptr, "image_line_number", imageLineNo);
								printf("pksize after filling in image line number info  = %lf \n",(double) op_pk_total_size_get(pkptr));
								
								op_pk_nfd_set (pkptr, "quality", q);
								printf("pksize after filling in quality info  = %lf \n",(double) op_pk_total_size_get(pkptr));
								
								op_pk_nfd_set (pkptr, "total_packet_size", tPS);
								printf("pksize after filling in tps info  = %lf \n",(double) op_pk_total_size_get(pkptr));
								
								
								if(compare == 0)
								{
								//char myarray[7400] = {0};
									op_pk_nfd_set (pkptr, "data", vidData[ID].ffmpeg_packet, op_prg_mem_copy_create, op_prg_mem_free, sizeof(AVPacket));
								
									
									//av_free_packet(ffmpeg_packet);
								}
								
								printf("pksize after filling in image data info  = %lf \n",(double) op_pk_total_size_get(pkptr));
								
								
								if(appOpencvDebugFlag)
								{
									printf("pksize after filling in info  = %lf \n",(double) op_pk_total_size_get(pkptr));
								}
								
								PacketCounter++;
								overallPacketCounter++;
					
								op_stat_write(my_packet_size_stat,op_pk_total_size_get(pkptr));
							
								//	Record statistics to indicate that a packet 	
								//	was generated at the current simulation time.	
								op_stat_write (pksize_stathandle, (double) OPC_TRUE);
							
								// Update local statistics.				
								op_stat_write (bits_sent_stathandle, 		pksize);
								op_stat_write (pkts_sent_stathandle, 		1.0);
							
								op_stat_write (bitssec_sent_stathandle, 	pksize);
								op_stat_write (bitssec_sent_stathandle, 	0.0);
								op_stat_write (pktssec_sent_stathandle, 	1.0);
								op_stat_write (pktssec_sent_stathandle, 	0.0);
							
								// Update global statistics.				
								op_stat_write (bits_sent_gstathandle, 		pksize);
								op_stat_write (pkts_sent_gstathandle, 		1.0);
								op_stat_write (bitssec_sent_gstathandle, 	pksize);
								op_stat_write (bitssec_sent_gstathandle, 	0.0);
								op_stat_write (pktssec_sent_gstathandle, 	1.0);
								op_stat_write (pktssec_sent_gstathandle, 	0.0);
							
								// Loop through to remove segments of the original application packet and   
								// send them out to the lower layer.  
								printf(" Segmentation Size compare, Segmentation Size = %d\n", (int)segmentation_size);
								if (segmentation_size > 0  && pksize > segmentation_size)
								{
									//Insert the packet into the segmentation buffer and pull out segments 
									// of size specified by the attribute segment size.                     
						
									op_sar_segbuf_pk_insert (segmentation_buf_handle, pkptr, 0);
						
									while (pksize > 0)
									{
										// Remove segments of size equal to the segment size and send them to   
										// transport layer. If the number of available bits in the buffer is    
										// is lesser than the segment size a packet will be created only with   				
										// remaining bits.                                                      
										pkptr = op_sar_srcbuf_seg_remove (segmentation_buf_handle, segmentation_size);
										
										// Update the pk_size remaining to reflect the reduced size.  
										pksize -= segmentation_size;
									
										// Send the packet to the lower layer.
										printf("Sending packet\n");
										op_pk_send (pkptr, 0);
									}
								}
								else
								{
									// Send the packet to the lower layer.	
									op_pk_send (pkptr, 0);
								}
								
							}
							
				
							if(appOpencvDebugFlag)
							{
								printf("image data structure deleted\n");
							}
							
							op_stat_write(my_packet_data_size_stat,frameDataPacketsSizeSum/FrameSizeInPackets);
						}
						
					
						FrameCounter++;
						PacketCounter = 0;
						
						/*	Check if the next packet arrival time is within the	*/
						/*	time in which the process remains in "ON" (active)	*/
						/*	state or not.										*/
						/*	Schedule the next packet arrival.					*/
						if ((next_frame_arrival_time + PRECISION_RECOVERY < off_state_start_time) &&
							(next_frame_arrival_time + PRECISION_RECOVERY < stop_time))
						{
							op_intrpt_schedule_self (next_frame_arrival_time, ON_TO_ON);
							//sprintf(myString,"next frame will be at %f",(float)next_frame_arrival_time);
							//op_prg_odb_print_major(myString,OPC_NIL);
						}
					}
					
					else //if in EA estimation time
					{
						int FrameSizeInPackets = 0;
						int PacketCounter = 0;
						int tPS;
						
						
						alreadySent = 0;
						
						
						frameSize = appRate/frameRate;
						
						printf("Frame Rate = %f, Frame Size = %f\n", (float)frameRate, (float)frameSize);
													
						pksize = inputPacketSize;//floor ((double) oms_dist_positive_outcome_with_error_msg (packet_size_dist_handle, 
						printf("Input packet size = %f\n", (double)inputPacketSize);
						
						//"This occurs for packet size distribution in bursty_source process model."));
						pksize *= 8;//get packet size in bits.
									
						next_frame_arrival_time = op_sim_time () + frameSize / appRate ;
						
						printf("next frame arrival time = %f\n", (float)next_frame_arrival_time);
									
						if(frameSize*8 - alreadySent < pksize)
						{
							pksize = frameSize*8 - alreadySent;
						}			
						
						averageFrameSize = frameSize; //initialization
											
						printf("appRate = %f, pksize = %f, frameSize = %f\n",(float)appRate,(float)pksize,(float)frameSize);
						//FrameSizeInPackets = ceil(frameSize*8.0/pksize);
						FrameSizeInPackets = 10;
									
						RTPoverhead += (64+23)*8 + (FrameSizeInPackets-1)*23*8;
						op_stat_write (RTPoverheadStat, (double) RTPoverhead/(op_sim_time () - 20));
									
						if(appOpencvDebugFlag)
						{
							printf("FrameSizeInPackets is %d\n",FrameSizeInPackets);
						}			
									
						while(alreadySent < frameSize*8)
						{
							int packetStatus = 0;
									
							alreadySent += pksize;
				
							if (alreadySent == frameSize*1024*8)
							{
								packetStatus = -1;
							}		
								
							total_bits_sent +=pksize; 
							
							pkptr  = op_pk_create ((pksize>5*32?pksize-6*32:1));
										
							if(appOpencvDebugFlag)
							{
								printf("sending frame %d packets, sending packet number %d, pksize = %lf \n",FrameCounter, PacketCounter,pksize);
							}			
									
							//marking the packets						
							tPS = (PacketCounter==0?pksize -  (64+23)*8:pksize -  23*8);
										
							op_pk_fd_set (pkptr, 1, OPC_FIELD_TYPE_INTEGER, FrameCounter, 32);
							op_pk_fd_set (pkptr, 2, OPC_FIELD_TYPE_INTEGER, PacketCounter, 32);
								
							op_pk_fd_set (pkptr, 3, OPC_FIELD_TYPE_INTEGER, packetStatus, 32);
							op_pk_fd_set (pkptr, 4, OPC_FIELD_TYPE_INTEGER, FrameSizeInPackets, 32);
							op_pk_fd_set (pkptr, 5, OPC_FIELD_TYPE_INTEGER, tPS, 32);
							op_pk_fd_set (pkptr, 6, OPC_FIELD_TYPE_DOUBLE, (double) start_time, 32);
											
							if(appOpencvDebugFlag)
							{
								printf("pksize after filling in info  = %lf \n",(double) op_pk_total_size_get(pkptr));
							}	
										
							PacketCounter++;
									
							op_stat_write(my_packet_size_stat,op_pk_total_size_get(pkptr));
							
							/*	Record statistics to indicate that a packet 	*/
							/*	was generated at the current simulation time.	*/
							//op_stat_write (pksize_stathandle, (double) OPC_TRUE);
									
							/* Update local statistics.				*/
							op_stat_write (bits_sent_stathandle, 		pksize);
							op_stat_write (pkts_sent_stathandle, 		1.0);
							
							op_stat_write (bitssec_sent_stathandle, 	pksize);
							op_stat_write (bitssec_sent_stathandle, 	0.0);
							op_stat_write (pktssec_sent_stathandle, 	1.0);
							op_stat_write (pktssec_sent_stathandle, 	0.0);
									
							/* Update global statistics.				*/
							op_stat_write (bits_sent_gstathandle, 		pksize);
							op_stat_write (pkts_sent_gstathandle, 		1.0);
							op_stat_write (bitssec_sent_gstathandle, 	pksize);
							op_stat_write (bitssec_sent_gstathandle, 	0.0);
							op_stat_write (pktssec_sent_gstathandle, 	1.0);
							op_stat_write (pktssec_sent_gstathandle, 	0.0);
									
							/* Loop through to remove segments of the original application packet and   */
							/* send them out to the lower layer.                                        */
							if (segmentation_size > 0  && pksize > segmentation_size)
							{
								/* Insert the packet into the segmentation buffer and pull out segments */
								/* of size specified by the attribute segment size.                     */
										
								op_sar_segbuf_pk_insert (segmentation_buf_handle, pkptr, 0);
										
								while (pksize > 0)
								{
									/* Remove segments of size equal to the segment size and send them to   */
									/* transport layer. If the number of available bits in the buffer is    */
									/* is lesser than the segment size a packet will be created only with   */
									/* remaining bits.                                                      */
									pkptr = op_sar_srcbuf_seg_remove (segmentation_buf_handle, segmentation_size);
											
									/* Update the pk_size remaining to reflect the reduced size.    */
									pksize -= segmentation_size;
											
									/* Send the packet to the lower layer.	*/
									op_pk_send (pkptr, 0);
								}
							}
							
							else
							{
								/* Send the packet to the lower layer.	*/
								op_pk_send (pkptr, 0);
							}
								
							pksize = floor ((double) oms_dist_positive_outcome_with_error_msg (packet_size_dist_handle, 
													"This occurs for packet size distribution in bursty_source process model."));
							
							pksize *= 8;//get packet size in bits.
						
							if(frameSize*1024*8 - alreadySent < pksize)
							{
								pksize = frameSize*1024*8 - alreadySent;
							}
							
							if(pksize == 0)
							{
								break;
							}
						}
						
						//Loren used to test framesizeinpacket output.
						//op_sim_end ("I wanted to end here.", "", "", "");
						
						FrameCounter++;
						PacketCounter = 0;
									
						/*	Check if the next packet arrival time is within the	*/
						/*	time in which the process remains in "ON" (active)	*/
						/*	state or not.										*/
						/*	Schedule the next packet arrival.					*/
						if ((next_frame_arrival_time + PRECISION_RECOVERY < off_state_start_time) &&
							(next_frame_arrival_time + PRECISION_RECOVERY < stop_time))
						{
							op_intrpt_schedule_self (next_frame_arrival_time, ON_TO_ON);
							sprintf(myString,"next frame will be at %f",next_frame_arrival_time);
							op_prg_odb_print_major(myString,OPC_NIL);
						}
					}
					printf("end of ma_bursty_source.\n");
				}
				}
				FSM_PROFILE_SECTION_OUT (state2_enter_exec)

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (5,"ma_bursty_source")


			/** state (on) exit executives **/
			FSM_STATE_EXIT_UNFORCED (2, "on", "ma_bursty_source [on exit execs]")
				FSM_PROFILE_SECTION_IN ("ma_bursty_source [on exit execs]", state2_exit_exec)
				{
				/* Determine the type of interrupt.		*/
				intrpt_type = op_intrpt_type ();
				intrpt_code = op_intrpt_code ();
				
				
				}
				FSM_PROFILE_SECTION_OUT (state2_exit_exec)


			/** state (on) transition processing **/
			FSM_PROFILE_SECTION_IN ("ma_bursty_source [on trans conditions]", state2_trans_conds)
			FSM_INIT_COND (ACTIVE_TO_INACTIVE)
			FSM_TEST_COND (REMAIN_ACTIVE)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("on")
			FSM_PROFILE_SECTION_OUT (state2_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 1, state1_enter_exec, ;, "ACTIVE_TO_INACTIVE", "", "on", "off", "tr_3", "ma_bursty_source [on -> off : ACTIVE_TO_INACTIVE / ]")
				FSM_CASE_TRANSIT (1, 2, state2_enter_exec, ;, "REMAIN_ACTIVE", "", "on", "on", "tr_4", "ma_bursty_source [on -> on : REMAIN_ACTIVE / ]")
				FSM_CASE_TRANSIT (2, 2, state2_enter_exec, ;, "default", "", "on", "on", "tr_13", "ma_bursty_source [on -> on : default / ]")
				}
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (0,"ma_bursty_source")
		}
	catch (...)
		{
		Vos_Error_Print (VOSC_ERROR_ABORT,
			(const char *)VOSC_NIL,
			"Unhandled C++ exception in process model (ma_bursty_source)",
			(const char *)VOSC_NIL, (const char *)VOSC_NIL);
		}
	}




void
ma_bursty_source_state::_op_ma_bursty_source_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}

void
ma_bursty_source_state::operator delete (void* ptr)
	{
	FIN (ma_bursty_source_state::operator delete (ptr));
	Vos_Poolmem_Dealloc (ptr);
	FOUT
	}

ma_bursty_source_state::~ma_bursty_source_state (void)
	{

	FIN (ma_bursty_source_state::~ma_bursty_source_state ())


	/* No Termination Block */


	FOUT
	}


#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

void *
ma_bursty_source_state::operator new (size_t)
#if defined (VOSD_NEW_BAD_ALLOC)
		throw (VOSD_BAD_ALLOC)
#endif
	{
	void * new_ptr;

	FIN_MT (ma_bursty_source_state::operator new ());

	new_ptr = Vos_Alloc_Object (ma_bursty_source_state::obtype);
#if defined (VOSD_NEW_BAD_ALLOC)
	if (new_ptr == VOSC_NIL) throw VOSD_BAD_ALLOC();
#endif
	FRET (new_ptr)
	}

/* State constructor initializes FSM handling */
/* by setting the initial state to the first */
/* block of code to enter. */

ma_bursty_source_state::ma_bursty_source_state (void) :
		_op_current_block (0)
	{
#if defined (OPD_ALLOW_ODB)
		_op_current_state = "ma_bursty_source [init enter execs]";
#endif
	}

VosT_Obtype
_op_ma_bursty_source_init (int * init_block_ptr)
	{
	FIN_MT (_op_ma_bursty_source_init (init_block_ptr))

	ma_bursty_source_state::obtype = Vos_Define_Object_Prstate ("proc state vars (ma_bursty_source)",
		sizeof (ma_bursty_source_state));
	*init_block_ptr = 0;

	FRET (ma_bursty_source_state::obtype)
	}

VosT_Address
_op_ma_bursty_source_alloc (VosT_Obtype, int)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	ma_bursty_source_state * ptr;
	FIN_MT (_op_ma_bursty_source_alloc ())

	/* New instance will have FSM handling initialized */
#if defined (VOSD_NEW_BAD_ALLOC)
	try {
		ptr = new ma_bursty_source_state;
	} catch (const VOSD_BAD_ALLOC &) {
		ptr = VOSC_NIL;
	}
#else
	ptr = new ma_bursty_source_state;
#endif
	FRET ((VosT_Address)ptr)
	}



void
_op_ma_bursty_source_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{
	ma_bursty_source_state		*prs_ptr;

	FIN_MT (_op_ma_bursty_source_svar (gen_ptr, var_name, var_p_ptr))

	if (var_name == OPC_NIL)
		{
		*var_p_ptr = (void *)OPC_NIL;
		FOUT
		}
	prs_ptr = (ma_bursty_source_state *)gen_ptr;

	if (strcmp ("pid_string" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->pid_string);
		FOUT
		}
	if (strcmp ("debug_mode" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->debug_mode);
		FOUT
		}
	if (strcmp ("stop_time" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->stop_time);
		FOUT
		}
	if (strcmp ("off_state_start_time" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->off_state_start_time);
		FOUT
		}
	if (strcmp ("pksize_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pksize_stathandle);
		FOUT
		}
	if (strcmp ("on_state_dist_handle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->on_state_dist_handle);
		FOUT
		}
	if (strcmp ("off_state_dist_handle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->off_state_dist_handle);
		FOUT
		}
	if (strcmp ("intarrvl_time_dist_handle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->intarrvl_time_dist_handle);
		FOUT
		}
	if (strcmp ("packet_size_dist_handle_" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->packet_size_dist_handle_);
		FOUT
		}
	if (strcmp ("start_time" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->start_time);
		FOUT
		}
	if (strcmp ("bits_sent_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bits_sent_stathandle);
		FOUT
		}
	if (strcmp ("bitssec_sent_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bitssec_sent_stathandle);
		FOUT
		}
	if (strcmp ("pkts_sent_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pkts_sent_stathandle);
		FOUT
		}
	if (strcmp ("pktssec_sent_stathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pktssec_sent_stathandle);
		FOUT
		}
	if (strcmp ("bits_sent_gstathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bits_sent_gstathandle);
		FOUT
		}
	if (strcmp ("bitssec_sent_gstathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bitssec_sent_gstathandle);
		FOUT
		}
	if (strcmp ("pkts_sent_gstathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pkts_sent_gstathandle);
		FOUT
		}
	if (strcmp ("pktssec_sent_gstathandle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pktssec_sent_gstathandle);
		FOUT
		}
	if (strcmp ("segmentation_size" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->segmentation_size);
		FOUT
		}
	if (strcmp ("segmentation_buf_handle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->segmentation_buf_handle);
		FOUT
		}
	if (strcmp ("app_appRate_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->app_appRate_stat);
		FOUT
		}
	if (strcmp ("appRate" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->appRate);
		FOUT
		}
	if (strcmp ("interarrival_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->interarrival_stat);
		FOUT
		}
	if (strcmp ("frameRate_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->frameRate_stat);
		FOUT
		}
	if (strcmp ("average_packet_size_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->average_packet_size_stat);
		FOUT
		}
	if (strcmp ("frameRate" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->frameRate);
		FOUT
		}
	if (strcmp ("frameSize" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->frameSize);
		FOUT
		}
	if (strcmp ("packet_size_dist_handle" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->packet_size_dist_handle);
		FOUT
		}
	if (strcmp ("my_packet_size_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->my_packet_size_stat);
		FOUT
		}
	if (strcmp ("total_bits_sent" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->total_bits_sent);
		FOUT
		}
	if (strcmp ("my_packet_data_size_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->my_packet_data_size_stat);
		FOUT
		}
	if (strcmp ("next_frame_arrival_time" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->next_frame_arrival_time);
		FOUT
		}
	if (strcmp ("alreadySent" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->alreadySent);
		FOUT
		}
	if (strcmp ("FrameCounter" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->FrameCounter);
		FOUT
		}
	if (strcmp ("imageLineNo" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->imageLineNo);
		FOUT
		}
	if (strcmp ("myName" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->myName);
		FOUT
		}
	if (strcmp ("frameSizeStat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->frameSizeStat);
		FOUT
		}
	if (strcmp ("inputPacketSize" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->inputPacketSize);
		FOUT
		}
	if (strcmp ("averageFrameSizeInPackets" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->averageFrameSizeInPackets);
		FOUT
		}
	if (strcmp ("overallPacketCounter" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->overallPacketCounter);
		FOUT
		}
	if (strcmp ("RTPoverhead" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->RTPoverhead);
		FOUT
		}
	if (strcmp ("RTPoverheadResetFlag" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->RTPoverheadResetFlag);
		FOUT
		}
	if (strcmp ("RTPoverheadStat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->RTPoverheadStat);
		FOUT
		}
	if (strcmp ("av_interarrival_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->av_interarrival_stat);
		FOUT
		}
	if (strcmp ("sizeInfo" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->sizeInfo);
		FOUT
		}
	if (strcmp ("imageName" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->imageName);
		FOUT
		}
	if (strcmp ("directoryName" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->directoryName);
		FOUT
		}
	if (strcmp ("imageNo" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->imageNo);
		FOUT
		}
	if (strcmp ("absDeviationSum" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->absDeviationSum);
		FOUT
		}
	if (strcmp ("DeviationCounter" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->DeviationCounter);
		FOUT
		}
	if (strcmp ("absDeviationStat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->absDeviationStat);
		FOUT
		}
	if (strcmp ("signedDeviationStat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->signedDeviationStat);
		FOUT
		}
	if (strcmp ("signedDeviationSum" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->signedDeviationSum);
		FOUT
		}
	if (strcmp ("lastFrameSize" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->lastFrameSize);
		FOUT
		}
	if (strcmp ("lastSmoothedFrameSize" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->lastSmoothedFrameSize);
		FOUT
		}
	if (strcmp ("lastFrameSizeAverageCalculationTime" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->lastFrameSizeAverageCalculationTime);
		FOUT
		}
	if (strcmp ("frameCounter" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->frameCounter);
		FOUT
		}
	if (strcmp ("frameSizeSum" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->frameSizeSum);
		FOUT
		}
	if (strcmp ("averageFrameSize" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->averageFrameSize);
		FOUT
		}
	if (strcmp ("averageAppFromAverageFrameSize_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->averageAppFromAverageFrameSize_stat);
		FOUT
		}
	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

