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

    module: ccsp_types.h

        For CCSP library for FC

    ---------------------------------------------------------------

    description:

        This file defines data types used to define CCSP  
        library APIs.

    ---------------------------------------------------------------

    environment:

        platform independent

    ---------------------------------------------------------------

    author:

        Kang Quan 

    ---------------------------------------------------------------

    revision:

        04/04/2011    initial revision.

**********************************************************************/
 
#include <string.h>

#ifndef  _CCSP_TYPES_DEF_
#define  _CCSP_TYPES_DEF_

/* data type definitions */
typedef  unsigned char                              CCSP_BOOL;
typedef  int                                        CCSP_INT;
typedef  unsigned int                               CCSP_UINT;
typedef  long                                       CCSP_LONG;
typedef  unsigned long                              CCSP_ULONG;
typedef  char                                       CCSP_CHAR;
typedef  char*                                      CCSP_STRING;
typedef  void*                                      CCSP_HANDLE;
typedef  void                                       CCSP_VOID;
typedef  float                                      CCSP_FLOAT;
typedef  double                                     CCSP_DOUBLE;

#define  CCSP_TRUE                                  1
#define  CCSP_FALSE                                 0


/**************************************************************************************
 * base response 
 **************************************************************************************/

#define  CCSP_FAULT                                                                 \
    CCSP_ULONG                      Code;                                           \
    CCSP_STRING                     String;                                         \


#define  CCSP_VARIABLE_TYPE_Unspecified             0
#define  CCSP_VARIABLE_TYPE_Boolean                 1
#define  CCSP_VARIABLE_TYPE_Int                     2
#define  CCSP_VARIABLE_TYPE_Uint                    3
#define  CCSP_VARIABLE_TYPE_String                  4


typedef  struct 
_CCSP_VARIABLE
{
    CCSP_UINT                       Type;

    union 
    {
        CCSP_BOOL                   varBool;
        CCSP_INT                    varInt;
        CCSP_UINT                   varUint;
        CCSP_STRING                 varString;
        CCSP_LONG                   varLong;
        CCSP_ULONG                  varUlong;
        CCSP_FLOAT                  varFloat;
        CCSP_DOUBLE                 varDouble;
    } 
    v;
}
CCSP_VARIABLE, *PCCSP_VARIABLE;


#define  CcspVariableInitialize(ccspVar)                                    \
    do {                                                                    \
        (ccspVar)->Type     = CCSP_VARIABLE_TYPE_Unspecified;               \
        (ccspVar)->varInt   = 0;                                            \
    } while (0)


#define  CcspVariableAlloc(ma_ptr,ccspVar)                                  \
    do {                                                                    \
        ccspVar = (CCSP_VARIABLE*)(*(ma_ptr))(sizeof(CCSP_VARIABLE));       \
        if ( ccspVar )                                                      \
        {                                                                   \
            CcspVariableInitialize(ccspVar);                                \
        }                                                                   \
    } while (0)


#define  CcspVariableClean(mf_ptr,ccspVar)                                  \
    do {                                                                    \
        if ( (ccspVar) && (ccspVar)->Type == CCSP_VARIABLE_TYPE_String )    \
        {                                                                   \
            if ( (ccspVar)->varString )                                     \
            {                                                               \
                (*(mf_ptr))((ccspVar)->varString);                          \
            }                                                               \
        }                                                                   \
        CcspVariableInitialize(ccspVar);                                    \
    } while (0)


#define  CcspVariableFree(mf_ptr,ccspVar)                                   \
    do {                                                                    \
        if ( ccspVar )                                                      \
        {                                                                   \
            CcspVariableClean(ccspVar);                                     \
            (*(mf_ptr))(ccspVar);                                           \
        }                                                                   \
    } while (0)


static inline
void strip_line (char *str)
{
       if (0==str)
               return;
       int len = strlen(str);
       str[len-1] = 0;
}

#endif

