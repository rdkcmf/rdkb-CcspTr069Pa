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


/**********************************************************************

    module: ccsp_cwmp_acsco_base.c

        For CCSP CWMP protocol implementation

    ---------------------------------------------------------------

    description:

        This module implements the basic container object functions
        of the CCSP CWMP ACS Connection Object.

        *   CcspCwmpAcscoCreate
        *   CcspCwmpAcscoRemove
        *   CcspCwmpAcscoEnrollObjects
        *   CcspCwmpAcscoInitialize

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Bin Zhu
        Kang Quan

    ---------------------------------------------------------------

    revision:

        09/21/05    initial revision.
        10/13/11    resolve name conflict with DM library

**********************************************************************/


#include "ccsp_cwmp_acsco_global.h"
#include <stdbool.h>

#define TR069_HOSTS_CFG		"/usr/ccsp/tr069pa/tr69Hosts.cfg"
char **hostNames = NULL;
int numHosts = 0;

/**********************************************************************

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CcspCwmpAcscoCreate
            (
                ANSC_HANDLE                 hContainerContext,
                ANSC_HANDLE                 hOwnerContext,
                ANSC_HANDLE                 hAnscReserved
            );

    description:

        This function constructs the ACS Connection Object and
        initializes the member variables and functions.

    argument:   ANSC_HANDLE                 hContainerContext
                This handle is used by the container object to interact
                with the outside world. It could be the real container
                or an target object.

                ANSC_HANDLE                 hOwnerContext
                This handle is passed in by the owner of this object.

                ANSC_HANDLE                 hAnscReserved
                This handle is passed in by the owner of this object.

    return:     newly created container object.

**********************************************************************/

ANSC_HANDLE
CcspCwmpAcscoCreate
    (
        ANSC_HANDLE                 hContainerContext,
        ANSC_HANDLE                 hOwnerContext,
        ANSC_HANDLE                 hAnscReserved
    )
{
    UNREFERENCED_PARAMETER(hAnscReserved);
    PANSC_COMPONENT_OBJECT          pBaseObject  = NULL;
    PCCSP_CWMP_ACS_CONNECTION_OBJECT     pMyObject    = NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)AnscAllocateMemory(sizeof(CCSP_CWMP_ACS_CONNECTION_OBJECT));

    if ( !pMyObject )
    {
        return  (ANSC_HANDLE)NULL;
    }
    else
    {
        pBaseObject = (PANSC_COMPONENT_OBJECT)pMyObject;
    }

    /*
     * Initialize the common variables and functions for a container object.
     */
    /* AnscCopyString(pBaseObject->Name, CCSP_CWMP_ACS_CONNECTION_NAME); */

    pBaseObject->hContainerContext = hContainerContext;
    pBaseObject->hOwnerContext     = hOwnerContext;
    pBaseObject->Oid               = CCSP_CWMP_ACS_CONNECTION_OID;
    pBaseObject->Create            = CcspCwmpAcscoCreate;
    pBaseObject->Remove            = CcspCwmpAcscoRemove;
    pBaseObject->EnrollObjects     = CcspCwmpAcscoEnrollObjects;
    pBaseObject->Initialize        = CcspCwmpAcscoInitialize;

    pBaseObject->EnrollObjects((ANSC_HANDLE)pBaseObject);
    pBaseObject->Initialize   ((ANSC_HANDLE)pBaseObject);

    return  (ANSC_HANDLE)pMyObject;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcscoRemove
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function destroys the object.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpAcscoRemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT     pMyObject       = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;
    PHTTP_BSP_INTERFACE             pHttpBspIf      = (PHTTP_BSP_INTERFACE    )pMyObject->hHttpBspIf;
    PHTTP_ACM_INTERFACE             pHttpAcmIf      = (PHTTP_ACM_INTERFACE    )pMyObject->hHttpAcmIf;
    PHTTP_SIMPLE_CLIENT_OBJECT      pHttpClient     = (PHTTP_SIMPLE_CLIENT_OBJECT)pMyObject->hHttpSimpleClient;

    pMyObject->Reset((ANSC_HANDLE)pMyObject);

    pMyObject->RemoveCookies(hThisObject);

    if( pMyObject->AcsUrl != NULL)
    {
        AnscFreeMemory(pMyObject->AcsUrl);

        pMyObject->AcsUrl = NULL;
    }

    if( pMyObject->Username != NULL)
    {
        AnscFreeMemory(pMyObject->Username);

        pMyObject->Username = NULL;
    }

    if( pMyObject->Password != NULL)
    {
        AnscFreeMemory(pMyObject->Password);

        pMyObject->Password = NULL;
    }

    if ( pMyObject->AuthHeaderValue )
    {
        AnscFreeMemory(pMyObject->AuthHeaderValue);
        pMyObject->AuthHeaderValue = NULL;
    }

    if ( pHttpBspIf )
    {
        AnscFreeMemory(pHttpBspIf);

        pMyObject->hHttpBspIf = (ANSC_HANDLE)NULL;
    }

    if ( pHttpAcmIf )
    {
        AnscFreeMemory(pHttpAcmIf);

        pMyObject->hHttpAcmIf = (ANSC_HANDLE)NULL;
    }

    if( pHttpClient != NULL)
    {
        pHttpClient->Remove(pHttpClient);
    }

    AnscCoRemove((ANSC_HANDLE)pMyObject);

    return  ANSC_STATUS_SUCCESS;
}

#define DEVICE_PROPERTIES    "/etc/device.properties"
static int bIsComcastImage( void)
{
    static int isComcastImage = -1;
    if (isComcastImage == -1)
    {
        char PartnerId[255] = {'\0'};
        errno_t rc       = -1;
        int     ind      = -1;

        getPartnerId ( PartnerId ) ;
        rc = strcmp_s("comcast", strlen("comcast"), PartnerId, &ind);
        ERR_CHK(rc);
        if((rc == EOK) && (ind != 0))
            isComcastImage = 0;
        else
            isComcastImage = 1;
    }

    return isComcastImage;
}

char **getHostNames()
{
	static bool initialized = false;
	/* To load hostnames configuration file only once*/
	if(!initialized)
	{
		FILE *fp;
		fp = fopen(TR069_HOSTS_CFG, "r");
		if(fp != NULL)
		{
			char ch;
			char tempHost[128] = {'\0'};
			while(!feof(fp))
			{
				ch = fgetc(fp);
				if(ch == '\n')
					numHosts++;
			}
			if(numHosts > 0)
			{
				hostNames = (char **)malloc(sizeof(char *) * numHosts);
				int i = 0;
				fseek(fp, 0, SEEK_SET);
				while(fgets(tempHost, 128, fp) != NULL)
				{
					hostNames[i] = strndup(tempHost, strlen(tempHost)-1);
					i++;
				}
			}
			fclose(fp);
		}
		else
		{
			numHosts = 0;
			hostNames = NULL;
		}
		initialized = true;
	}
	return hostNames;
}
/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcscoEnrollObjects
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function enrolls all the objects required by this object.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpAcscoEnrollObjects
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT     pMyObject       = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;
    PHTTP_BSP_INTERFACE             pHttpBspIf      = (PHTTP_BSP_INTERFACE    )pMyObject->hHttpBspIf;
    PHTTP_ACM_INTERFACE             pHttpAcmIf      = (PHTTP_ACM_INTERFACE    )pMyObject->hHttpAcmIf;
    PHTTP_SIMPLE_CLIENT_OBJECT      pHttpClient     = (PHTTP_SIMPLE_CLIENT_OBJECT)NULL;
    errno_t rc = -1;
    HTTP_SCO_HOST_NAMES		    hosts;

    if ( !pHttpBspIf )
    {
        pHttpBspIf = (PHTTP_BSP_INTERFACE)AnscAllocateMemory(sizeof(HTTP_BSP_INTERFACE));

        if ( !pHttpBspIf )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hHttpBspIf = (ANSC_HANDLE)pHttpBspIf;
        }

        rc =  strcpy_s(pHttpBspIf->Name, sizeof(pHttpBspIf->Name), HTTP_BSP_INTERFACE_NAME);
        if(rc != EOK)
        {
            ERR_CHK(rc);
            return  ANSC_STATUS_FAILURE;
        }

        pHttpBspIf->InterfaceId   = HTTP_BSP_INTERFACE_ID;
        pHttpBspIf->hOwnerContext = (ANSC_HANDLE)pMyObject;
        pHttpBspIf->Size          = sizeof(HTTP_BSP_INTERFACE);

        pHttpBspIf->Polish        = CcspCwmpAcscoHttpBspPolish;
        pHttpBspIf->Browse        = CcspCwmpAcscoHttpBspBrowse;
        pHttpBspIf->Notify        = CcspCwmpAcscoHttpBspNotify;
    }

    if ( !pHttpAcmIf )
    {
        pHttpAcmIf = (PHTTP_ACM_INTERFACE)AnscAllocateMemory(sizeof(HTTP_ACM_INTERFACE));

        if ( !pHttpAcmIf )
        {
            return  ANSC_STATUS_RESOURCES;
        }
        else
        {
            pMyObject->hHttpAcmIf = (ANSC_HANDLE)pHttpAcmIf;
        }

        pHttpAcmIf->hOwnerContext = (ANSC_HANDLE)pMyObject;
        pHttpAcmIf->Size          = sizeof(HTTP_ACM_INTERFACE);

        pHttpAcmIf->GetCredential = CcspCwmpAcscoHttpGetCredential;
    }

    /*
     *  Create Http Simple Client
     */
    pHttpClient =
        (PHTTP_SIMPLE_CLIENT_OBJECT)HttpCreateSimpleClient
            (
                (ANSC_HANDLE)pMyObject->hHttpHelpContainer,
                (ANSC_HANDLE)pMyObject,
                (ANSC_HANDLE)NULL
            );


    if( pHttpClient == NULL)
    {
        return ANSC_STATUS_RESOURCES;
    }

    pHttpClient->SetProductName(pHttpClient, "HTTP Client V1.0");
    pHttpClient->SetBspIf((ANSC_HANDLE)pHttpClient, pMyObject->hHttpBspIf);
    pHttpClient->SetClientMode(pHttpClient, HTTP_SCO_MODE_XSOCKET | HTTP_SCO_MODE_COMPACT | HTTP_SCO_MODE_NOTIFY_ON_ALL_CONN_ONCE);

    memset(&hosts, 0, sizeof(HTTP_SCO_HOST_NAMES));
    if ( bIsComcastImage() && getHostNames() != NULL)
    {
		char **phostNames = NULL;
		int i = 0;
		phostNames = getHostNames();
		CcspTraceInfo(("Adding hostnames for validation\n"));
		hosts.peerVerify = TRUE;
		hosts.numHosts = numHosts;
                hosts.hostNames = (char **)malloc(sizeof(char *)*numHosts);
		for(i = 0; i<numHosts; i++)
		{
			if(phostNames[i] != NULL)
			{
				hosts.hostNames[i] = strdup(phostNames[i]);
			}
		}
    }
    pHttpClient->SetHostNames(pHttpClient, &hosts);

    pMyObject->hHttpSimpleClient = (ANSC_HANDLE)pHttpClient;

    AnscCoEnrollObjects((ANSC_HANDLE)pMyObject);

    return  ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_STATUS
        CcspCwmpAcscoInitialize
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function first calls the initialization member function
        of the base class object to set the common member fields
        inherited from the base class. It then initializes the member
        fields that are specific to this object.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     status of operation.

**********************************************************************/

ANSC_STATUS
CcspCwmpAcscoInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCCSP_CWMP_ACS_CONNECTION_OBJECT     pMyObject    = (PCCSP_CWMP_ACS_CONNECTION_OBJECT)hThisObject;

    /*
     * Until you have to simulate C++ object-oriented programming style with standard C, you don't
     * appreciate all the nice little things come with C++ language and all the dirty works that
     * have been done by the C++ compilers. Member initialization is one of these things. While in
     * C++ you don't have to initialize all the member fields inherited from the base class since
     * the compiler will do it for you, such is not the case with C.
     */
    AnscCoInitialize((ANSC_HANDLE)pMyObject);

    /*
     * Although we have initialized some of the member fields in the "create" member function, we
     * repeat the work here for completeness. While this simulation approach is pretty stupid from
     * a C++/Java programmer perspective, it's the best we can get for universal embedded network
     * programming. Before we develop our own operating system (don't expect that to happen any
     * time soon), this is the way things gonna be.
     */
    pMyObject->Oid                            = CCSP_CWMP_ACS_CONNECTION_OID;
    pMyObject->Create                         = CcspCwmpAcscoCreate;
    pMyObject->Remove                         = CcspCwmpAcscoRemove;
    pMyObject->EnrollObjects                  = CcspCwmpAcscoEnrollObjects;
    pMyObject->Initialize                     = CcspCwmpAcscoInitialize;

    pMyObject->hCcspCwmpSession               = NULL;
    pMyObject->AcsUrl                         = NULL;
    pMyObject->Username                       = NULL;
    pMyObject->Password                       = NULL;
    pMyObject->AuthHeaderValue                = NULL;
    pMyObject->bActive                        = FALSE;

    AnscZeroMemory(pMyObject->Cookies, CCSP_CWMP_ACSCO_MAX_COOKIE * sizeof(char *));
    pMyObject->NumCookies                     = 0;

    pMyObject->GetHttpSimpleClient            = CcspCwmpAcscoGetHttpSimpleClient;
    pMyObject->GetCcspCwmpSession             = CcspCwmpAcscoGetCcspCwmpSession;
    pMyObject->SetCcspCwmpSession             = CcspCwmpAcscoSetCcspCwmpSession;
    pMyObject->GetAcsUrl                      = CcspCwmpAcscoGetAcsUrl;
    pMyObject->SetAcsUrl                      = CcspCwmpAcscoSetAcsUrl;
    pMyObject->GetUsername                    = CcspCwmpAcscoGetUsername;
    pMyObject->SetUsername                    = CcspCwmpAcscoSetUsername;
    pMyObject->GetPassword                    = CcspCwmpAcscoGetPassword;
    pMyObject->SetPassword                    = CcspCwmpAcscoSetPassword;
    pMyObject->Reset                          = CcspCwmpAcscoReset;

    pMyObject->Connect                        = CcspCwmpAcscoConnect;
    pMyObject->Request                        = CcspCwmpAcscoRequest;
    pMyObject->RequestOnly                    = CcspCwmpAcscoRequestOnly;
    pMyObject->Close                          = CcspCwmpAcscoClose;

    pMyObject->HttpBspPolish                  = CcspCwmpAcscoHttpBspPolish;
    pMyObject->HttpBspBrowse                  = CcspCwmpAcscoHttpBspBrowse;
    pMyObject->HttpBspNotify                  = CcspCwmpAcscoHttpBspNotify;

    pMyObject->HttpGetCredential              = CcspCwmpAcscoHttpGetCredential;

    pMyObject->AddCookie                      = CcspCwmpAcscoHttpAddCookie;
    pMyObject->RemoveCookies                  = CcspCwmpAcscoHttpRemoveCookies;
    pMyObject->FindCookie                     = CcspCwmpAcscoHttpFindCookie;
    pMyObject->DelCookie                      = CcspCwmpAcscoHttpDelCookie;

    return  ANSC_STATUS_SUCCESS;
}
