/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2015 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

/**********************************************************************
   Copyright [2014] [Cisco Systems, Inc.]
 
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
 
       http://www.apache.org/licenses/LICENSE-2.0
 
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**********************************************************************/


// ssp_main.c : Defines the entry point for the console application.
//

#ifdef __GNUC__
#if (!defined _BUILD_ANDROID) && (!defined _COSA_VEN501_) && (!defined _COSA_SIM_) && (!defined _NO_EXECINFO_H_)
#include <execinfo.h>
#endif
#endif

#include <sys/types.h>
#include <sys/ipc.h>
#ifdef _BUILD_ANDROID
#include <linux/msg.h>
#else
#include <sys/msg.h>
#endif
#include <string.h>

#include "ssp_global.h"

#ifdef FEATURE_SUPPORT_RDKLOG
#include "rdk_debug.h"
#endif

#ifdef INCLUDE_BREAKPAD
#include "breakpad_wrapper.h"
#endif
#define DEBUG_INI_NAME  "/etc/debug.ini"
#include "syscfg/syscfg.h"
#include "cap.h"
#include "telemetry_busmessage_sender.h"
static cap_user appcaps;

#ifndef DISABLE_LOGAGENT
extern int GetLogInfo(ANSC_HANDLE bus_handle, char *Subsytem, char *pParameterName);
#endif

PCCSP_CWMP_CPE_CONTROLLER_OBJECT    g_pCcspCwmpCpeController    = NULL;
BOOL                                bEngaged                = FALSE;

static char                         g_Tr069PaName[256]      = {0};
static char                         g_CrName[256]           = {0};
char                                g_Subsystem[32]         = {0};
static char                         g_PaMapperXmlFile[256]  = {0};
static char                         g_PaCustMapperFile[256] = {0};

char*                               g_Tr069_PA_Name         = g_Tr069PaName;
extern char*                        pComponentName;

extern  PDSLH_DATAMODEL_AGENT_OBJECT    g_DslhDataModelAgent;

#ifdef   _ANSC_USE_OPENSSL_
extern char* openssl_client_ca_certificate_files; //  = OPENSSL_CLIENT_CA_CERT_FILES;  // defined in user_openssl.c
char* openssl_client_dev_certificate_file = NULL;
char* openssl_client_private_key_file = NULL;
#endif

char* g_Tr069PaOutboundIfName = NULL;
char* g_Tr069PaAcsDefAddr = NULL;

#define  CCSP_TR069PA_CFG_FILE                      "/usr/ccsp/tr069pa/ccsp_tr069_pa_cfg.xml"
#define  CCSP_TR069PA_DEF_MAPPER_XML_FILE           "/usr/ccsp/tr069pa/ccsp_tr069_pa_mapper.xml"

CCSP_CWMP_CFG_INTERFACE             ccspCwmpCfgIf;
WEB_ACM_INTERFACE                   webAcmIf;

static char                         g_SdmXmlFile[256]       = {0};

#ifdef   _DEBUG
ULONG                               ulMemTraceFromBlockId   = 0;
extern void
AnscTraceMemoryTableFromBlockId 
    (
        ULONG                       ulBlockIdFrom
    );

extern ULONG
AnscGetNextMemoryBlockId
    (
    );
#endif

#define  CCSP_COMMON_COMPONENT_HEALTH_Red                   1
#define  CCSP_COMMON_COMPONENT_HEALTH_Yellow                2
#define  CCSP_COMMON_COMPONENT_HEALTH_Green                 3
int g_tr069Health = CCSP_COMMON_COMPONENT_HEALTH_Red;

int  display_menu()
{
    CcspTr069PaTraceInfo(("\n\n"));
    CcspTr069PaTraceInfo(("        *** 'e' - engage PA SSP                                                        ***\n"));
    CcspTr069PaTraceInfo(("        *** 'c' - cancel PA SSP                                                        ***\n"));
    CcspTr069PaTraceInfo(("        *** 'l' - set log level                                                        ***\n"));
    CcspTr069PaTraceInfo(("        *** 's' - simulate receiving systemReady signal from CR                        ***\n"));
    CcspTr069PaTraceInfo(("        *** 'd' - dump PA parameter tree                                               ***\n"));
    CcspTr069PaTraceInfo(("        *** 'g' - sending diagComplete event over D-Bus                                ***\n"));
    CcspTr069PaTraceInfo(("        *** '1' - test PA parameter filter function                                    ***\n"));
    CcspTr069PaTraceInfo(("        *** '2' - test GPN                                                             ***\n"));
    CcspTr069PaTraceInfo(("        *** '3' - test GPV                                                             ***\n"));
    CcspTr069PaTraceInfo(("        *** '4' - test SPV                                                             ***\n"));
    CcspTr069PaTraceInfo(("        *** '5' - test GPA                                                             ***\n"));
    CcspTr069PaTraceInfo(("        *** '6' - test SPA                                                             ***\n"));
    CcspTr069PaTraceInfo(("        *** '7' - test CDS                                                             ***\n"));
    CcspTr069PaTraceInfo(("        *** '8' - test firmware download                                               ***\n"));
    CcspTr069PaTraceInfo(("        *** 'a' - test AddObject                                                       ***\n"));
    CcspTr069PaTraceInfo(("        *** 'x' - test DeleteObject                                                    ***\n"));
    CcspTr069PaTraceInfo(("        *** 'v' - simulate SPV through other means than ACS                            ***\n"));
    CcspTr069PaTraceInfo(("        *** 'k' - check parameter notification in PA's cache                           ***\n"));
    CcspTr069PaTraceInfo(("        *** 'f' - flip invisible namespace check flag                                  ***\n"));
    CcspTr069PaTraceInfo(("\n\n"));

    return  0;
}


int  display_info()
{
    CcspTr069PaTraceInfo(("\n\n"));
    CcspTr069PaTraceInfo(("        ***************************************************************\n"));
    CcspTr069PaTraceInfo(("        ***                                                         ***\n"));
    CcspTr069PaTraceInfo(("        ***   CCSP TR-069 PA Program                                ***\n"));
    CcspTr069PaTraceInfo(("        ***      Copyright 2014  Cisco Systems, Inc.                ***\n"));
    CcspTr069PaTraceInfo(("        ***      Licensed under the Apache License, Version 2.0     ***\n"));
    CcspTr069PaTraceInfo(("        ***                                                         ***\n"));
    CcspTr069PaTraceInfo(("        ***************************************************************\n"));
    CcspTr069PaTraceInfo(("\n\n"));

    display_menu();

    return  0;
}


static void _print_stack_backtrace(void)
{
#ifdef __GNUC__
#if (!defined _BUILD_ANDROID) && (!defined _COSA_VEN501_) && (!defined _COSA_SIM_) && (!defined _NO_EXECINFO_H_)
    void* tracePtrs[100];
    char** funcNames = NULL;
    int i, count = 0;

    int fd;
    const char* path = "/nvram/tr069passp_backtrace";
    fd = open(path, O_RDWR | O_CREAT);
    if (fd < 0)
    {
        fprintf(stderr, "failed to open backtrace file: %s", path);
        return;
    }
    
    count = backtrace( tracePtrs, 100 );
    backtrace_symbols_fd( tracePtrs, count, fd );
    close(fd);

	funcNames = backtrace_symbols( tracePtrs, count );

	if ( funcNames ) {
		// Print the stack trace
		for( i = 0; i < count; i++ )
            fprintf(stderr, "%s\n", funcNames[i] );

		// Free the string pointers
		free( funcNames );
	}
#endif
#endif
}

static void daemonize(void) {
	switch (fork()) {
	case 0:
		break;
	case -1:
		// Error
        CcspTr069PaTraceError(("Error daemonizing (fork)! %d - %s\n", errno, strerror(errno)));
		exit(0);
		break;
	default:
		_exit(0);
	}

	if (setsid() < 	0) {
		CcspTr069PaTraceError(("Error demonizing (setsid)! %d - %s\n", errno, strerror(errno)));
		exit(0);
	}

    /* This causes mapper file loading failure due to the file is not located at root directory.
	chdir("/");
    */

#ifndef  _DEBUG
	int fd;

	fd = open("/dev/null", O_RDONLY);
	if (fd != 0) {
		dup2(fd, 0);
		close(fd);
	}
	fd = open("/dev/null", O_WRONLY);
	if (fd != 1) {
		dup2(fd, 1);
		close(fd);
	}
	fd = open("/dev/null", O_WRONLY);
	if (fd != 2) {
		dup2(fd, 2);
		close(fd);
	}
#endif
}

void sig_handler(int sig)
{

    CcspTr069PaTraceDebug(("<%s> Signal %d received\n", __FUNCTION__, sig));
    
    if ( sig == SIGINT ) {
    	signal(SIGINT, sig_handler); /* reset it to this function */
    	CcspTr069PaTraceInfo(("SIGINT received!\n"));
        _print_stack_backtrace();
	    exit(0);
    }
    else if ( sig == SIGUSR1 ) {
    	signal(SIGUSR1, sig_handler); /* reset it to this function */
    	CcspTr069PaTraceInfo(("SIGUSR1 received!\n"));
    }
    else if ( sig == SIGUSR2 ) {
    	CcspTr069PaTraceInfo(("SIGUSR2 received!\n"));
        if ( bEngaged )
        {
            cancel_tr069pa();
            bEngaged = FALSE;
            exit(0);
        }
    }
    else if ( sig == SIGCHLD ) {
    	signal(SIGCHLD, sig_handler); /* reset it to this function */
    	CcspTr069PaTraceInfo(("SIGCHLD received!\n"));
    }
    else if ( sig == SIGPIPE ) {
    	signal(SIGPIPE, sig_handler); /* reset it to this function */
    	CcspTr069PaTraceInfo(("SIGPIPE received!\n"));
    }
	else if ( sig == SIGALRM ) {

    	signal(SIGALRM, sig_handler); /* reset it to this function */
    	CcspTr069PaTraceInfo(("SIGALRM received!\n"));		
    }
    else {
    	/* get stack trace first */
    	_print_stack_backtrace();
    	CcspTr069PaTraceInfo(("Signal %d received, exiting!\n", sig));
    	exit(0);
    }
}

#ifndef INCLUDE_BREAKPAD
static int is_core_dump_opened(void)
{
    FILE *fp;
    char path[256];
    char line[1024];
    char *start, *tok, *sp;
#define TITLE   "Max core file size"

    snprintf(path, sizeof(path), "/proc/%d/limits", getpid());
    if ((fp = fopen(path, "rb")) == NULL)
        return 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        if ((start = strstr(line, TITLE)) == NULL)
            continue;

        start += strlen(TITLE);
        if ((tok = strtok_r(start, " \t\r\n", &sp)) == NULL)
            break;

        fclose(fp);

        if (strcmp(tok, "0") == 0)
            return 0;
        else
            return 1;
    }

    fclose(fp);
    return 0;
}
#endif //ifndef INCLUDE_BREAKPAD

static void drop_root()
{
  appcaps.caps = NULL;
  appcaps.user_name = NULL;
  bool blocklist_ret = false;
  blocklist_ret = isBlocklisted();
  if(blocklist_ret)
  {
    CcspTraceInfo(("NonRoot feature is disabled\n"));
  }
  else
  {
    CcspTraceInfo(("NonRoot feature is enabled, dropping root privileges for CcspTr069Pa process\n"));
    init_capability();
    drop_root_caps(&appcaps);
    update_process_caps(&appcaps);
    read_capability(&appcaps);
  }
}

BOOL is_customer_data_model()
{
    char sysbuf[8] = {0};

    syscfg_init();

    syscfg_get( NULL, "custom_data_model_enabled", sysbuf, sizeof(sysbuf));

    if (sysbuf[0] != 0)
    {
        if ( AnscEqualString(sysbuf, "1", FALSE) )
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        CcspTr069PaTraceDebug(("syscfg_get returned an empty string for custom_data_model_enabled\n"));
        return FALSE;
    }
}

char* get_customer_data_model_file_name()
{
    char* sysbuf = AnscAllocateMemory(256);
    AnscZeroMemory(sysbuf, 256);

    syscfg_init();

    syscfg_get( NULL, "custom_data_model_file_name", sysbuf, 256);

    if ( sysbuf[0] != 0 )
    {
        return sysbuf;
    }
    else
    {
        CcspTr069PaTraceDebug(("syscfg_get returned an empty string for custom_data_model_file_name\n"));
        AnscFreeMemory(sysbuf);
        return NULL;
    }
}

int main(int argc, char* argv[])
{
	int                             cmdChar = 0;
	int                             idx = 0;
    BOOL                            bRunAsDaemon = TRUE;
    BOOL                            bShowDebugLog = FALSE;
    char                            cmd[1024]          = {0};
    FILE                           *fd                 = NULL;

#ifdef   _DEBUG
    /*AnscSetTraceLevel(CCSP_TRACE_LEVEL_DEBUG);*/
#endif
    memset(&ccspCwmpCfgIf, 0, sizeof(CCSP_CWMP_CFG_INTERFACE));
    memset(&webAcmIf, 0, sizeof(WEB_ACM_INTERFACE));
 
	for (idx = 1; idx < argc; idx++)
	{
		if ( (strcmp(argv[idx], "-pa") == 0) )
		{
            AnscCopyString(g_Tr069PaName, argv[idx+1]);
		}
        else if ( (strcmp(argv[idx], "-cr") == 0) )
        {
            AnscCopyString(g_CrName, argv[idx+1]);
        }
        else if ( (strcmp(argv[idx], "-subsys") == 0) )
        {
            AnscCopyString(g_Subsystem, argv[idx+1]);
        }
        else if ( (strcmp(argv[idx], "-mapper") == 0) )
        {
            AnscCopyString(g_PaMapperXmlFile, argv[idx+1]);
        }
        else if ( (strcmp(argv[idx], "-custom_mapper") == 0) )
        {
            AnscCopyString(g_PaCustMapperFile, argv[idx+1]);
        }
        else if ( (strcmp(argv[idx], "-sdm") == 0) )
        {
            AnscCopyString(g_SdmXmlFile, argv[idx+1]);
        }
        else if ( strcmp(argv[idx], "-c") == 0 )
        {
            bRunAsDaemon = FALSE;
        }
        else if ( strcmp(argv[idx], "-dbg") == 0 )
        {
            bShowDebugLog = TRUE;
        }
	}

    if ( g_Tr069PaName[0] == 0 )
    {
        AnscCopyString(g_Tr069PaName, CCSP_TR069PA_DEF_NAME);
    }

    pComponentName = g_Tr069PaName;

    if ( g_CrName[0] == 0 )
    {
        AnscCopyString(g_CrName, CCSP_DBUS_INTERFACE_CR);
    }

    if ( g_PaMapperXmlFile[0] == 0 )
    {
        AnscCopyString(g_PaMapperXmlFile, CCSP_TR069PA_DEF_MAPPER_XML_FILE);
    }

    if ( g_PaCustMapperFile[0] == 0 && is_customer_data_model() )
    {
        char* customer_file_name = get_customer_data_model_file_name();

        if ( customer_file_name )
        {
            CcspTr069PaTraceDebug(("Customer data-model file name is %s\n", customer_file_name));
            AnscCopyString(g_PaCustMapperFile, customer_file_name);
            AnscFreeMemory(customer_file_name);
        }
    }
#ifdef FEATURE_SUPPORT_RDKLOG
	RDK_LOGGER_INIT();
#endif

    CcspTr069PaTraceDebug(("<%s>: PaName=%s, CrName=%s, Xml=%s, Subsys=%s\n", __FUNCTION__, g_Tr069PaName, g_CrName, g_PaMapperXmlFile, g_Subsystem));

    drop_root();
    if ( bRunAsDaemon )
    	daemonize();

#ifdef INCLUDE_BREAKPAD
    breakpad_ExceptionHandler();
    signal(SIGUSR1, sig_handler);
#else
        
    if (is_core_dump_opened())
    {
        signal(SIGUSR1, sig_handler);
        signal(SIGPIPE, SIG_IGN);
        CcspTr069PaTraceWarning(("Core dump is opened, do not catch signal\n"));
    }
    else
    {
        CcspTr069PaTraceWarning(("Core dump is NOT opened, backtrace if possible\n"));
        signal(SIGTERM, sig_handler);
        signal(SIGINT, sig_handler);
        /*signal(SIGCHLD, sig_handler);*/
        signal(SIGUSR1, sig_handler);
        signal(SIGUSR2, sig_handler);
        
        signal(SIGSEGV, sig_handler);
        signal(SIGBUS, sig_handler);
        signal(SIGKILL, sig_handler);
        signal(SIGFPE, sig_handler);
        signal(SIGILL, sig_handler);
        signal(SIGQUIT, sig_handler);
        signal(SIGHUP, sig_handler);
        signal(SIGPIPE, SIG_IGN);
		signal(SIGALRM, sig_handler);
    }

#endif
  
    t2_init("ccsp-tr069-pa"); 
  
    display_info();
    
    if ( bShowDebugLog == TRUE )
    {
        g_iTraceLevel = CCSP_TRACE_LEVEL_DEBUG;
    }

    cmd_dispatch('e');
    syscfg_init();

	/*This is used for ccsp recovery manager */
    
     printf(" ***************** Ccsp Tr69 process is up  Creatting PID file **** \n");
    fd = fopen("/var/tmp/CcspTr069PaSsp.pid", "w+");
    if ( !fd )
    {
        CcspTraceWarning(("Create /var/tmp/CcspTr069PaSsp.pid error. \n"));
    }
    else
    {
        sprintf(cmd, "%d", getpid());
        fputs(cmd, fd);
        fclose(fd);
    }

    if ( bRunAsDaemon ) 
    {
		while (1)
			sleep(30);
    }
    else {
		while ( cmdChar != 'q' )
		{
			cmdChar = getchar();
            if(EOF == cmdChar)
            {
                AnscSleep(500);
            }
            else
            {
			cmd_dispatch(cmdChar);
            }
		}
    }

    if ( bEngaged )
    {
        cancel_tr069pa();

        bEngaged = FALSE;
    }

    return 0;
}


int  cmd_dispatch(int  command)
{
    switch ( command )
    {
        case    'e' :

                if ( !bEngaged )
                {
                    engage_tr069pa();

                    bEngaged = TRUE;
                }

                break;

        case    'c' :

                if ( bEngaged )
                {
                    CcspTr069PaTraceInfo(("CCSP TR-069 PA is being unloaded ...\n"));

                    if ( TRUE )
                    {
                        g_pCcspCwmpCpeController->StopCWMP((ANSC_HANDLE)g_pCcspCwmpCpeController);
                    }

                    AnscSleep(5000);

                    cancel_tr069pa();

                    bEngaged = FALSE;

                    CcspTr069PaTraceInfo(("CCSP TR-069 PA has been unloaded.\n"));
                }

                break;

#ifdef   _DEBUG
        case    'l' :

                ssp_setLogLevel();

                break;

        case    's' :

                ssp_testNsSyncWithCR();

                break;

        case    'g' :

                ssp_sendDiagCompleteSignal();

                break;

        case    'd' :

                ssp_dumpPiTree();                
                
                break;

        case    '1' :

                ssp_testNsAgainstMapper();

                break;

        case    '2' :

                ssp_testGPN();

                break;        

        case    '3' :
        
                ssp_testGPV();

                break;        

        case    '4' : 

                ssp_testSPV();

                break;

        case    '5' :

                ssp_testGPA();

                break;

        case    '6' :

                ssp_testSPA();

                break;

        case    '7' :

                ssp_testCDS();

                break;

        case    '8' :
    
                ssp_testFirmwareDownload();

                break;

        case    'a' :

                ssp_testAddObject();

                break;

        case    'x' :

                ssp_testDeleteObject();

                break;

        case    'v' :

                ssp_testSPV_notFromPA();

                break;

        case    'k' :

                ssp_checkCachedParamAttr();

                break;

        case    'f' :

                ssp_flipInvNamespaceChkFlag();

                break;

        case    'i' :

                ssp_sendValueChangeSignal();

                break;

#endif

        case    'm' :

                display_menu();
/*
                AnscTraceMemoryUsage();
*/
                break;
#ifdef _DEBUG
        case    't' :

                if ( ulMemTraceFromBlockId != 0 )
                {
                    printf("### Memory blocks whose AllocId is larger to equal to %u: ###\n", (unsigned int)ulMemTraceFromBlockId);
                    AnscTraceMemoryTableFromBlockId(ulMemTraceFromBlockId);
                    printf("\n\n");
                }

                printf("### Memory blocks not freed so far: ###\n");
                AnscTraceMemoryTable();

                if ( TRUE )
                {
                    ULONG           ulMemoryPeakSize = AnscGetProcessMaxMemorySize();

                    printf("### Peak memory size = %u ###\n", (unsigned int)ulMemoryPeakSize);
                }

                break;
        case    'n' :

                ulMemTraceFromBlockId = AnscGetNextMemoryBlockId();

                break;
#endif


        default :

                break;
    }

    return  0;
}


int  engage_tr069pa()
{
    ANSC_STATUS                     status = ANSC_STATUS_SUCCESS;
    PCCSP_CWMP_TCPCR_HANDLER_OBJECT      pCcspCwmpTcpcrHandler = NULL;

    if ( bEngaged )
    {
        return  0;
    }

    CcspTr069PaSsp_LoadCfgFile(CCSP_TR069PA_CFG_FILE);

    CcspTr069PaSsp_InitCcspCwmpCfgIf();
    ssp_BbhmInitWebAcmIf();

    g_pCcspCwmpCpeController = 
        CcspCwmpCreateCpeController
            (
                g_Tr069PaName,
                g_CrName,
                (ANSC_HANDLE)NULL,
                (ANSC_HANDLE)NULL,
                (ANSC_HANDLE)NULL
            );

    if ( g_pCcspCwmpCpeController )
    {
        CcspTr069PaSsp_InitCcspCwmpCfgIf((ANSC_HANDLE)g_pCcspCwmpCpeController);
        g_pCcspCwmpCpeController->SetCcspCwmpCfgIf((ANSC_HANDLE)g_pCcspCwmpCpeController, (ANSC_HANDLE)&ccspCwmpCfgIf);

        if ( g_Subsystem[0] != 0 )
        {
            status = g_pCcspCwmpCpeController->SetSubsysName((ANSC_HANDLE)g_pCcspCwmpCpeController, g_Subsystem);
        }

        status = g_pCcspCwmpCpeController->SetPAMapperFile((ANSC_HANDLE)g_pCcspCwmpCpeController, g_PaMapperXmlFile);

        if ( g_PaCustMapperFile[0] != 0 )
        {
            status = g_pCcspCwmpCpeController->SetPACustomMapperFile((ANSC_HANDLE)g_pCcspCwmpCpeController, g_PaCustMapperFile);
        }

        if ( g_SdmXmlFile[0] )
        {
            g_pCcspCwmpCpeController->SetSDMXmlFilename((ANSC_HANDLE)g_pCcspCwmpCpeController, g_SdmXmlFile);
        }

        pCcspCwmpTcpcrHandler = (PCCSP_CWMP_TCPCR_HANDLER_OBJECT)g_pCcspCwmpCpeController->hCcspCwmpTcpConnReqHandler;
        if ( pCcspCwmpTcpcrHandler )
        {
            pCcspCwmpTcpcrHandler->SetWebAcmIf((ANSC_HANDLE)pCcspCwmpTcpcrHandler, (ANSC_HANDLE)&webAcmIf);
        }

        if ( g_Tr069PaOutboundIfName && g_Tr069PaOutboundIfName[0])
        {
            status = g_pCcspCwmpCpeController->SetOutboundIfName((ANSC_HANDLE)g_pCcspCwmpCpeController, g_Tr069PaOutboundIfName);
        }

        status = g_pCcspCwmpCpeController->Engage((ANSC_HANDLE)g_pCcspCwmpCpeController);

        if ( status != ANSC_STATUS_SUCCESS )
        {
            CcspTr069PaTraceError(("Failed to engage CCSP CWMP CPE Controller object!\n"));
            CcspTr069PaFreeMemory(g_pCcspCwmpCpeController);
            g_pCcspCwmpCpeController = NULL;
        }
        else
        {
            CcspTr069PaTraceInfo(("CCSP CWMP CPE Controller object has been engaged!\n"));
            bEngaged = TRUE;
        }
    }

	if( g_DslhDataModelAgent )
	{
		g_DslhDataModelAgent->pPrefix=g_Subsystem;
	}
	else
	{
		g_DslhDataModelAgent = (PDSLH_DATAMODEL_AGENT_OBJECT)AnscAllocateMemory(sizeof(DSLH_DATAMODEL_AGENT_OBJECT));
		g_DslhDataModelAgent->pPrefix = g_Subsystem;
	}
    g_tr069Health = CCSP_COMMON_COMPONENT_HEALTH_Green;

    return 0;
}


int  cancel_tr069pa()
{
    if ( g_pCcspCwmpCpeController )
    {
        g_pCcspCwmpCpeController->Cancel((ANSC_HANDLE)g_pCcspCwmpCpeController);

        g_pCcspCwmpCpeController->Remove((ANSC_HANDLE)g_pCcspCwmpCpeController);
        g_pCcspCwmpCpeController = NULL;
    }

    bEngaged = FALSE;

	if( g_DslhDataModelAgent )
	{
		AnscFreeMemory(g_DslhDataModelAgent);
	}
    return  0;
}

