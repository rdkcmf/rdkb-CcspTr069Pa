/*********************************************************************
 * Copyright 2018-2019 ARRIS Enterprises, LLC.
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
 **********************************************************************/

#include "ccsp_alias_mgr.h"
#include "ansc_platform.h"
#include "ansc_xml_dom_parser_interface.h"
#include "ansc_debug.h"

// TR-106 defines maximum object or parameter name length as 256
// It appears to me that the full path can actually be longer than 256
// but TR069 PA mapper uses 256, so setting it to the same value
#define CCSP_Alias_Mgr_Name_Length 256

// Alias XML node names
#define CCSP_ALIAS_MNGR_XML_NODE_NAME_EXTERNAL "External"
#define CCSP_ALIAS_MNGR_XML_NODE_NAME_INTERNAL "Internal"
#define CCSP_ALIAS_MNGR_XML_NODE_ATTR_STRICT   "strict"

typedef struct
_CCSP_ALIAS_MNGR_ALIAS_ENTRY
{
    struct _CCSP_ALIAS_MNGR_ALIAS_ENTRY   *Parent;
    struct _CCSP_ALIAS_MNGR_ALIAS_ENTRY   *Child;
    struct _CCSP_ALIAS_MNGR_ALIAS_ENTRY   *Sibling;
    char*                                 Name;
    SLIST_HEADER                          Aliases;
    SLIST_HEADER                          StrictAliases;
    BOOL                                  PartialName;
}
CCSP_ALIAS_MNGR_ALIAS_ENTRY, *PCCSP_ALIAS_MNGR_ALIAS_ENTRY;

typedef
struct
_CCSP_ALIAS_MNGR_OBJECT
{
    PCCSP_ALIAS_MNGR_ALIAS_ENTRY AliasExtTree;
    PCCSP_ALIAS_MNGR_ALIAS_ENTRY AliasIntTree;
}
CCSP_ALIAS_MNGR_OBJECT, *PCCSP_ALIAS_MNGR_OBJECT;

// Single linked list to keep the instance numbers during mapping
typedef
struct
_CCSP_STRING_SINGLE_LINK_LIST_ENTRY {
    SINGLE_LINK_ENTRY               Linkage;
    char*                           Value;
}
CCSP_STRING_SINGLE_LINK_LIST_ENTRY, *PCCSP_STRING_SINGLE_LINK_LIST_ENTRY;

#define ACCESS_STRING_SINGLE_LIST_ENTRY(p) ACCESS_CONTAINER(p, CCSP_STRING_SINGLE_LINK_LIST_ENTRY, Linkage)

static
BOOL
CcspAliasMgrAliasTreeAddItem
    (
        PCCSP_ALIAS_MNGR_ALIAS_ENTRY     pRoot,
        char*                            Name,
        char*                            Alias,
        BOOL                             bStrict
    );


/**
 * Initialize the queue
 */
static BOOL QueueInit(PQUEUE_HEADER pQueueHeader)
{
    if ( pQueueHeader == NULL )
    {
        CcspTraceError(("Queue header is NULL on initialization.\n"));
        return FALSE;
    }

    AnscQueueInitializeHeader(pQueueHeader);

    return TRUE;
}

/**
 * Add integer entry into the end of the queue
 */
static BOOL QueuePush(PQUEUE_HEADER pQueueHeader, char* Value)
{
    if ( pQueueHeader == NULL )
    {
        CcspTraceError(("Queue header is NULL.\n"));
        return FALSE;
    }

    PCCSP_STRING_SINGLE_LINK_LIST_ENTRY pListEntry = (PCCSP_STRING_SINGLE_LINK_LIST_ENTRY) AnscAllocateMemory( sizeof(CCSP_STRING_SINGLE_LINK_LIST_ENTRY) );

    if ( pListEntry == NULL )
    {
        CcspTraceError(("Could not allocate memory for the queue entry.\n"));
        return FALSE;
    }

    AnscZeroMemory( pListEntry, sizeof(CCSP_STRING_SINGLE_LINK_LIST_ENTRY) );

    pListEntry->Value = Value;

    AnscQueuePushEntry(pQueueHeader, &pListEntry->Linkage);

    return TRUE;
}

/**
 * Pop integer entry from the beginning of the queue
 */
#if 0 // not currently used, remove to avoid compiler error
static BOOL QueuePop(PQUEUE_HEADER pQueueHeader, char** pValue)
{
    PSINGLE_LINK_ENTRY pLink                       = NULL;
    PCCSP_STRING_SINGLE_LINK_LIST_ENTRY pListEntry = NULL;

    if ( pQueueHeader == NULL )
    {
        CcspTraceError(("Queue header is NULL.\n"));
        return FALSE;
    }

    pLink = AnscQueuePopEntry(pQueueHeader);

    if ( pLink )
    {
        pListEntry = ACCESS_STRING_SINGLE_LIST_ENTRY(pLink);
        *pValue = pListEntry->Value;

        AnscFreeMemory(pListEntry);

        return TRUE;
    }
    else
    {
        CcspTraceWarning(("Queue is empty.\n"));
        return FALSE;
    }
}
#endif

/**
 * Get next entry the queue
 */
static BOOL QueueGetNext(PQUEUE_HEADER pQueueHeader, PSINGLE_LINK_ENTRY pLink, char** pValue, PSINGLE_LINK_ENTRY* ppNextLink)
{
    PSINGLE_LINK_ENTRY pNextLink                       = NULL;
    PCCSP_STRING_SINGLE_LINK_LIST_ENTRY pListEntry = NULL;

    if ( pQueueHeader == NULL )
    {
        CcspTraceError(("Queue header is NULL.\n"));
        return FALSE;
    }

    if ( pLink == NULL )
    {
        pNextLink = AnscQueueGetFirstEntry(pQueueHeader);
    }
    else
    {
        pNextLink = AnscQueueGetNextEntry(pLink);
    }

    if ( pNextLink )
    {
        pListEntry = ACCESS_STRING_SINGLE_LIST_ENTRY(pNextLink);
        *pValue = pListEntry->Value;
        *ppNextLink = pNextLink;

        return TRUE;
    }
    else
    {
        CcspTraceWarning(("Queue is empty.\n"));
        return FALSE;
    }
}

/**
 * Check whether the queue is empty
 */
static BOOL IsQueueEmpty(PQUEUE_HEADER pQueueHeader)
{
    return AnscQueueQueryDepth(pQueueHeader) == 0 ;
}

/**
 * Remove all entries from the queue
 */
static VOID QueueRemoveAll(PQUEUE_HEADER pQueueHeader)
{
    PSINGLE_LINK_ENTRY pLink                       = NULL;
    PCCSP_STRING_SINGLE_LINK_LIST_ENTRY pListEntry = NULL;

    while ( (pLink = AnscQueuePopEntry(pQueueHeader)) )
    {
        pListEntry = ACCESS_STRING_SINGLE_LIST_ENTRY(pLink);

        if (pListEntry->Value)
        {
            AnscFreeMemory(pListEntry->Value);
        }

        AnscFreeMemory(pListEntry);
    }
}

/**
 * Push entry to string list
 */
static BOOL StringSListPushEntry(PSLIST_HEADER pList, char* value)
{
    PCCSP_STRING_SINGLE_LINK_LIST_ENTRY pListEntry = (PCCSP_STRING_SINGLE_LINK_LIST_ENTRY) AnscAllocateMemory( sizeof(CCSP_STRING_SINGLE_LINK_LIST_ENTRY) );

    if ( pListEntry == NULL )
    {
        CcspTraceError(("Could not allocate memory for the queue entry.\n"));
        return FALSE;
    }

    AnscZeroMemory( pListEntry, sizeof(CCSP_STRING_SINGLE_LINK_LIST_ENTRY) );

    pListEntry->Value = value;

    AnscSListPushEntry(pList, &pListEntry->Linkage);

    return TRUE;
}

/**
 * Remove all entries from string list
 */
static VOID StringSListRemoveAll(PSLIST_HEADER pList)
{
    PSINGLE_LINK_ENTRY              pLink;
    PCCSP_STRING_SINGLE_LINK_LIST_ENTRY pListEntry;

    if (pList == NULL)
    {
        CcspTraceWarning(("List header is NULL\n"));
        return;
    }

    while ( (pLink = AnscSListPopEntry(pList)) )
    {
        pListEntry = ACCESS_STRING_SINGLE_LIST_ENTRY(pLink);

        if (pListEntry->Value != NULL)
        {
            AnscFreeMemory(pListEntry->Value);
        }

        AnscFreeMemory(pListEntry);
    }
}

/**
 * Find an entry in string list
 */
static BOOL StringSListFind(PSLIST_HEADER pList, char* s)
{
    PSINGLE_LINK_ENTRY                  pLink      = NULL;
    PCCSP_STRING_SINGLE_LINK_LIST_ENTRY pListEntry = NULL;

    if (pList == NULL)
    {
        CcspTraceWarning(("List header is NULL\n"));
        return FALSE;
    }

    pLink = AnscSListGetFirstEntry(pList);

    while ( pLink )
    {
        pListEntry = ACCESS_STRING_SINGLE_LIST_ENTRY(pLink);

        if (pListEntry != NULL && AnscEqualString(pListEntry->Value, s, TRUE))
        {
            return TRUE;
        }

        pLink = AnscSListGetNextEntry(pLink);
    }

    return FALSE;
}

/**
 * Check is the list is empty.
 * Also returns TRUE if the list header is NULL.
 */
static BOOL StringSListIsEmpty(PSLIST_HEADER pList)
{
    if (pList == NULL)
    {
        CcspTraceWarning(("List header is NULL\n"));
        return TRUE;
    }

    return AnscSListQueryDepth(pList) == 0;
}
/**********************************************************************

    prototype:

        BOOL
        CcspAliasMgrEnsureTree
        (
            PCCSP_ALIAS_MNGR_ALIAS_ENTRY *ppTree
        );

    description:

        Checks the alias tree exists, otherwise creates one.

    argument:   PCCSP_ALIAS_MNGR_ALIAS_ENTRY *ppTree
                pointer to alias tree pointer;

    return:     true  if the tree exists or it's creation succeeded
                false if there was a failure while create the tree

**********************************************************************/
static inline
BOOL
CcspAliasMgrEnsureTree
    (
        PCCSP_ALIAS_MNGR_ALIAS_ENTRY *ppTree
    )
{
    if (*ppTree == NULL)
    {
        *ppTree =
            (PCCSP_ALIAS_MNGR_ALIAS_ENTRY) AnscAllocateMemory( sizeof(CCSP_ALIAS_MNGR_ALIAS_ENTRY) );

        if ( *ppTree == NULL )
        {
            CcspTraceError(("Failed to allocate Alias Manager alias tree.\n"));
            return  FALSE;
        }
        else
        {
            /* this root node is a virtual node - without node name */
            AnscZeroMemory( *ppTree, sizeof(CCSP_ALIAS_MNGR_ALIAS_ENTRY) );
            ((PCCSP_ALIAS_MNGR_ALIAS_ENTRY) *ppTree)->PartialName = TRUE;
            return TRUE;
        }
    }
    else
    {
        return TRUE;
    }
}

/**********************************************************************

    prototype:

        CcspAliasMgrFindMakeNode
            (
                PCCSP_ALIAS_MNGR_ALIAS_ENTRY pRoot,
                char*                        Name,
                char*                        Alias,
                BOOL                         bStrict,
            )

    description:

        Find or create a matching node

    argument: PCCSP_ALIAS_MNGR_ALIAS_ENTRY     pRoot
              Pointer to the alias tree;

              char*                            Name
              Parameter name (path) for which the alias is being added;

              char*                            Alias
              Alias name (path);

              BOOL                              bStrict
              Whether a strict name compare should be used with this name

    return:   found or created mathing node.
              All parent nodes will be created, if needed

**********************************************************************/
static inline
PCCSP_ALIAS_MNGR_ALIAS_ENTRY
CcspAliasMgrFindMakeNode
    (
        PCCSP_ALIAS_MNGR_ALIAS_ENTRY pRoot,
        char*                        Name,
        char*                        Alias,
        BOOL                         bStrict
    )
{
    PCCSP_ALIAS_MNGR_ALIAS_ENTRY      pNode               = pRoot;
    PCCSP_ALIAS_MNGR_ALIAS_ENTRY      pChildNode          = NULL;
    PCCSP_ALIAS_MNGR_ALIAS_ENTRY      pPrevSibling        = NULL;
    PANSC_TOKEN_CHAIN                 pNsTokenChain       = NULL;
    PANSC_TOKEN_CHAIN                 pAliasTokenChain    = NULL;
    PANSC_STRING_TOKEN                pStringToken        = NULL;
    PANSC_STRING_TOKEN                pAliasStringToken   = NULL;
    char*                             pAliasNodeName      = NULL;
    char                              pPartialAlias[CCSP_Alias_Mgr_Name_Length + 1]     = {0};
    char                              pPartialName[CCSP_Alias_Mgr_Name_Length + 1]     = {0};
    char*                             pNodeName           = NULL;
    BOOL                              bAddAux             = FALSE;

    pNsTokenChain =
        AnscTcAllocate
            (
                Name,
                "."
            );

    if(!pNsTokenChain)
    {
        CcspTraceError(("TR-069 alias mapper file Ns Token creation failed: %s\n", Name));
        return FALSE;
    }

    if ( bStrict == FALSE )
    {
        pAliasTokenChain =
            AnscTcAllocate
                (
                    Alias,
                    "."
                );

        if ( !pAliasTokenChain )
        {
            CcspTraceError(("TR-069 alias mapper file Alias Token creation failed: %s\n", Name));

            AnscTcFree((ANSC_HANDLE)pNsTokenChain);

            return FALSE;
        }
    }

    /* find the last matching node on the tree */
    while ( pNode && (pStringToken = AnscTcUnlinkToken(pNsTokenChain)) )
    {
        pNodeName = pStringToken->Name;

        /**
         *  In case we need to add explicit aliases up in the tree:
         *  1. Tokenize the alias
         *  2. Create bAddAux flag - it will indicate if Strict alias has to be added
         *  3. If next token is available - compare AliasName to NodeName
         *  4. If does not match - set bAddAux flag
         *  5. If bAddAux is set - add Strict alias to the tree
         */
        if ( pAliasTokenChain != NULL )
        {
            pAliasStringToken = AnscTcUnlinkToken(pAliasTokenChain);

            if ( pAliasStringToken != NULL )
            {
                pAliasNodeName = pAliasStringToken->Name;

                // check if the names fit within the strings, including the possible "."
                // exit the loop in the improbable case it doesn't fit
                if ( AnscSizeOfString(pPartialName) + 1 + AnscSizeOfString(pNodeName) > CCSP_Alias_Mgr_Name_Length
                     || AnscSizeOfString(pPartialAlias) + 1 + AnscSizeOfString(pAliasNodeName) > CCSP_Alias_Mgr_Name_Length)
                {
                    CcspTraceError(("Tokenized strings are too long: %s + %s; %s + %s\n", pPartialName, pNodeName, pPartialAlias, pAliasNodeName));

                    AnscFreeMemory(pStringToken);
                    pStringToken = NULL;

                    break;
                }

                if ( AnscSizeOfString(pPartialName) > 0 )
                {
                    AnscCatString(pPartialName, ".");
                }

                AnscCatString(pPartialName, pNodeName);

                if ( AnscSizeOfString(pPartialAlias) > 0 )
                {
                    AnscCatString(pPartialAlias, ".");
                }
                AnscCatString(pPartialAlias, pAliasNodeName);

                if ( bAddAux == FALSE )
                {
                    /* Start adding Strict aliases when encountering different node name */
                    if ( AnscEqualString(pNodeName, pAliasNodeName, TRUE) == FALSE)
                    {
                        bAddAux = TRUE;
                    }
                }

                if ( bAddAux && AnscEqualString(pPartialName, Name, TRUE) == FALSE )
                {
                    /* Add Strict alias - will only match the name fully */
                    CcspAliasMgrAliasTreeAddItem
                        (
                            pRoot,
                            pPartialName,
                            pPartialAlias,
                            TRUE
                        );
                }
            }
            else
            {
                CcspTraceError(("TR-69 alias mapper file contains unsupported alias %s %s - should be the same nesting level\n", Name, Alias));
            }
        }

        pPrevSibling = NULL;
        pChildNode = pNode->Child;

        /* find if any child node's name matches the token */
        while ( pChildNode )
        {
            if ( AnscEqualString(pNodeName, pChildNode->Name, TRUE) )
            {
                pNode        = pChildNode;
                break;
            }
            else
            {
                pPrevSibling = pChildNode;
                pChildNode   = pChildNode->Sibling;
            }
        }

        if ( !pChildNode )
        {
            /* no match - we found the last "matching" node */
            break;
        }

        AnscFreeMemory(pStringToken);
        pStringToken = NULL;

        if ( pAliasStringToken )
        {
            AnscFreeMemory(pAliasStringToken);
            pAliasStringToken = NULL;
        }

        /* go one level deeper */
        pNode        = pChildNode;
        pPrevSibling = NULL;
    }

    /*
       if pStringToken == NULL we have found an existing node.
       otherwise create a new one
    */
    if ( pStringToken != NULL )
    {
        /* create nodes on the tree for remaining tokens */
        do
        {
            pNodeName = pStringToken->Name;

            pChildNode =
                (PCCSP_ALIAS_MNGR_ALIAS_ENTRY) AnscAllocateMemory( sizeof(CCSP_ALIAS_MNGR_ALIAS_ENTRY) );

            if ( !pChildNode )
            {
                AnscTcFree((ANSC_HANDLE)pNsTokenChain);

                CcspTraceError(("TR-069 alias mapper - failed to allocate child node %s\n", Name));

                break;
            }
            else
            {
                AnscZeroMemory( pChildNode, sizeof(CCSP_ALIAS_MNGR_ALIAS_ENTRY) );
            }

            pChildNode->Name        = AnscCloneString(pNodeName);

            if ( pPrevSibling )
            {
                pPrevSibling->Sibling = pChildNode;
                pChildNode->Parent    = pPrevSibling->Parent;
                pPrevSibling          = NULL;
                pNode                 = pChildNode;
            }
            else
            {
                pNode->Child          = pChildNode;
                pChildNode->Parent    = pNode;
                pNode                 = pChildNode;
            }

            AnscInitializeSList(&pChildNode->Aliases);
            AnscInitializeSList(&pChildNode->StrictAliases);

            AnscFreeMemory(pStringToken);
            pStringToken = NULL;
        }
        while ( (pStringToken = AnscTcUnlinkToken(pNsTokenChain)) );

        if ( pStringToken )
        {
            AnscFreeMemory(pStringToken);
            pStringToken = NULL;
        }
    }

    if ( pAliasStringToken )
    {
        AnscFreeMemory(pAliasStringToken);
    }

    if ( pAliasTokenChain )
    {
        AnscTcFree((ANSC_HANDLE)pAliasTokenChain);
    }

    if ( pNsTokenChain )
    {
        AnscTcFree((ANSC_HANDLE)pNsTokenChain);
    }

    return pChildNode;
}

/**********************************************************************

    prototype:

        CcspAliasMgrAliasTreeAddItem
        (
            PCCSP_ALIAS_MNGR_ALIAS_ENTRY     pRoot,
            char*                            Name,
            char*                            Alias,
            BOOL                             bStrict
        );

    description:

        Adds a new alias item to the provided alias tree.

    argument: PCCSP_ALIAS_MNGR_ALIAS_ENTRY     pRoot
              Pointer to the alias tree;

              char*                            Name
              Parameter name (path) for which the alias is being added;

              char*                            Alias
              Alias name (path);

              BOOL                              bStrict
              Whether a strict name compare should be used with this name

    return:     true  if the operation succeeded
                false if there was a failure

**********************************************************************/
static
BOOL
CcspAliasMgrAliasTreeAddItem
    (
        PCCSP_ALIAS_MNGR_ALIAS_ENTRY     pRoot,
        char*                            Name,
        char*                            Alias,
        BOOL                             bStrict
    )
{
    PCCSP_ALIAS_MNGR_ALIAS_ENTRY      pChildNode          = NULL;
    BOOL                              bPartialNs          = FALSE;
    int                               NameLen             = 0;

    if ( !Name || *Name == '\0' || *Name == '.' || strstr(Name, "..") )
    {
        CcspTraceError(("TR-69 alias mapper file contains invalid name: %s\n", Name ? Name : "NULL"));
        return FALSE;
    }
    else if ( !Alias || *Alias == '\0' || *Alias == '.' || strstr(Alias, "..") )
    {
        CcspTraceError(("TR-069 alias mapper file contains invalid alias: %s\n", Alias ? Alias : "NULL"));
        return FALSE;
    }

    NameLen = AnscSizeOfString(Name);
    bPartialNs = ( Name [ NameLen - 1 ] == '.' );

    if ( !(pChildNode = CcspAliasMgrFindMakeNode(pRoot, Name, Alias, bStrict)) )
    {
        /* This is error indication */
        return  FALSE;
    }

    /* fill in parameter (or object possibly allowed) */
    pChildNode->PartialName = bPartialNs;

    if ( bStrict)
    {
        if (!StringSListFind(&pChildNode->StrictAliases, Alias))
        {
            StringSListPushEntry(&pChildNode->StrictAliases, AnscCloneString(Alias));
        }
    }
    else
    {
        if (!StringSListFind(&pChildNode->Aliases, Alias))
        {
            StringSListPushEntry(&pChildNode->Aliases, AnscCloneString(Alias));
        }
    }

    return  TRUE;
}

/**********************************************************************

    prototype:

        CcspAliasMgrFreeAliasTree
        (
            PCCSP_ALIAS_MNGR_ALIAS_ENTRY pRoot
        );

    description:

        Release memory allocated for AliasTree

    argument: PCCSP_ALIAS_MNGR_ALIAS_ENTRY pRoot
              Pointer to alias tree

    return:

**********************************************************************/
static
VOID
CcspAliasMgrFreeAliasTree
    (
        PCCSP_ALIAS_MNGR_ALIAS_ENTRY      pRoot
    )
{
    PCCSP_ALIAS_MNGR_ALIAS_ENTRY          pChild, pSibling;

    if ( !pRoot ) return;

    if ( pRoot->Name ) AnscFreeMemory(pRoot->Name);

    StringSListRemoveAll(&pRoot->Aliases);
    StringSListRemoveAll(&pRoot->StrictAliases);

    pChild      = pRoot->Child;
    pSibling    = pRoot->Sibling;

    CcspAliasMgrFreeAliasTree(pSibling);
    CcspAliasMgrFreeAliasTree(pChild);

    AnscFreeMemory(pRoot);
}

/**********************************************************************

    prototype:

        CcspAliasMgrIsNameInsIdentifier
        (
            char*                 Name
        );

    description:

        Check whether the given parameter name is an instance identifier,
        that is - instance number or instance alias.

    argument: char*                            Name
              Parameter name to check for being an instance number;

    return:     true  if the name is an instance number
                false if the name is not an instanse number

**********************************************************************/
static
BOOL
CcspAliasMgrIsNameInsIdentifier
    (
        char*                 Name
    )
{
    int                        len;
    unsigned int               InsNumber;
    char                       buf[32]         = {0};

    len = AnscSizeOfString(Name);
    if ( *Name == '0' )
    {
        return FALSE;
    }

    // Check for instance number
    InsNumber = (unsigned int) atol(Name);

    if ( InsNumber == 0 )
    {
        // Check for alias identifier
        if ( Name[0] == '[' && Name[len - 1] == ']' )
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    sprintf(buf, "%u", InsNumber);

    return AnscEqualString(buf, Name, TRUE);
}

/**********************************************************************

    prototype:

        CcspAliasMgrReplaceInstanceIds
            (
                char*         Alias,
                char*         Result,
                PQUEUE_HEADER pInsIdQueue
            )

    description:

        Replace instance id placeholders ({i}) in the alias
        with queued instance ids from the original object

    argument: char*                     Alias
              Alias string

              char*                     Result
              Result string

              PQUEUE_HEADER             pInsIdQueue
              Previously filled in instance id queue.
              The caller is expected to release the queue's memory.

    return:   TRUE if operation succeeded
              FALSE in cases of error

**********************************************************************/
static
BOOL
CcspAliasMgrReplaceInstanceIds
    (
        char*         Alias,
        char*         Result,
        PQUEUE_HEADER pInsIdQueue
    )
{
    PANSC_TOKEN_CHAIN                    pNsTokenChain       = NULL;
    PANSC_STRING_TOKEN                   pStringToken        = NULL;
    char*                                InsId               = NULL;
    BOOL                                 bRet                = TRUE;
    PSINGLE_LINK_ENTRY                   pLink               = NULL;

    pNsTokenChain =
        AnscTcAllocate
            (
                Alias,
                "."
            );

    if ( !pNsTokenChain )
    {
        CcspTraceError(("Token creation failed for instance mapping alias %s\n", Alias));

        return FALSE;
    }

    while ( (pStringToken = AnscTcUnlinkToken(pNsTokenChain)) )
    {
        if ( !IsQueueEmpty(pInsIdQueue) && AnscEqualString(pStringToken->Name, "{i}", TRUE) )
        {
            InsId = NULL;
            BOOL status = QueueGetNext(pInsIdQueue, pLink, &InsId, &pLink);
            if ( !status || InsId == NULL )
            {
                CcspTraceError(("Failed to pop instance number, alias mapping failed\n"));

                bRet = FALSE;

                break;
            }
            else
            {
                sprintf( &Result [ AnscSizeOfString(Result) ], ".%s", InsId );
            }
        }
        else
        {
            if ( AnscSizeOfString(Result) == 0 )
            {
                sprintf( Result + AnscSizeOfString(Result), "%s", pStringToken->Name );
            }
            else
            {
                sprintf( Result + AnscSizeOfString(Result), ".%s", pStringToken->Name );
            }
        }

        AnscFreeMemory(pStringToken);
        pStringToken = NULL;
    }

    if (pStringToken)
    {
        AnscFreeMemory(pStringToken);
    }

    if ( pNsTokenChain )
    {
        AnscTcFree((ANSC_HANDLE)pNsTokenChain);
    }

    return bRet;
}

/**********************************************************************

    prototype:

        PCCSP_ALIAS_MNGR_ALIAS_ENTRY
        CcspAliasMgrLookupAliasNode
            (
                PCCSP_ALIAS_MNGR_ALIAS_ENTRY         pRoot,
                char*                                Name,
                PQUEUE_HEADER                        pOutInsIdQueue,
                char*                                OutRemaining,
                unsigned int                         RemainingSize
             )

    description:

        Look up alias node for the given parameter name.

    argument: PCCSP_ALIAS_MNGR_ALIAS_ENTRY pRoot
              Alias tree root;

              char*                        Name
              Parameter name to find alias for

              PQUEUE_HEADER                pOutInstIdQueue
              [Output] Instance id queue - should be allocated by the caller

              char*                        OutRemaining
              [Output] Remaining name string after finding an alias
              Example: "Device.X_LGI-COM_CM" aliased to "Device.X_ARRIS_COM_CM"
              then for "Device.X_LGI-COM_CM.Status" the "remaining" will be "Status"
              Should be allocated by the caller

              unsigned int                 RemainingSize
              Remaining string size

    return:   found alias name node
              NULL if no alias has been found

**********************************************************************/
static
PCCSP_ALIAS_MNGR_ALIAS_ENTRY
CcspAliasMgrLookupAliasNode
    (
        PCCSP_ALIAS_MNGR_ALIAS_ENTRY         pRoot,
        char*                                Name,
        PQUEUE_HEADER                        pOutInsIdQueue,
        char*                                OutRemaining,
        unsigned int                         RemainingSize
    )
{
    PCCSP_ALIAS_MNGR_ALIAS_ENTRY         pNode               = pRoot;
    PANSC_TOKEN_CHAIN                    pNsTokenChain       = NULL;
    PANSC_STRING_TOKEN                   pStringToken        = NULL;
    PCCSP_ALIAS_MNGR_ALIAS_ENTRY         pChildNode          = NULL;
    PCCSP_ALIAS_MNGR_ALIAS_ENTRY         pAliasNode          = NULL;
    PCCSP_ALIAS_MNGR_ALIAS_ENTRY         pInsNode            = NULL;
    BOOL                                 bInsId              = FALSE;
    BOOL                                 bInsMatched         = FALSE;
    char*                                InsId               = NULL;

    pNsTokenChain =
        AnscTcAllocate
            (
                Name,
                "."
            );

    if (!pNsTokenChain)
    {
        CcspTraceError(("Memory allocation failure when processing for: %s\n", Name));

        return FALSE;
    }

    /*
        Several possible cases:
        + When Strict is FALSE:
          - If a partial match found - stop the search, use the parent object alias (full object aliasing)
          - If the search runs to the end - full alias may be present
        + When Strict is TRUE:
          - Only full alias matching is acceptable, partial can't be used
    */
    while ( pNode && (pStringToken = AnscTcUnlinkToken(pNsTokenChain)) )
    {
        pChildNode = pNode->Child;
        bInsId  = CcspAliasMgrIsNameInsIdentifier(pStringToken->Name);
        bInsMatched = FALSE;

        if (bInsId)
        {
            /* Remember the instance identifier to restore it in the aliased name */
            InsId = AnscAllocateMemory(AnscSizeOfString(pStringToken->Name) + 1);
            AnscCopyString(InsId, pStringToken->Name);

            QueuePush(pOutInsIdQueue, InsId);
        }


        /* find if any child node's name matches the token */
        while ( pChildNode )
        {
            /* unlike TR-069 PA specific instance mapping is not allowed here */
            if ( !bInsId && AnscEqualString(pStringToken->Name, pChildNode->Name, TRUE) )
            {
                pNode        = pChildNode;

                break;
            }
            else if ( bInsId && AnscEqualString(pChildNode->Name, "{i}", TRUE) )
            {
                bInsMatched  = TRUE;
                pInsNode     = pChildNode;

                break;
            }
            else
            {
                pChildNode   = pChildNode->Sibling;
            }
        }

        if ( !pChildNode )
        {
            if ( bInsMatched )
            {
                pChildNode = pInsNode;
                pInsNode   = NULL;
            }
        }
        else if ( !StringSListIsEmpty(&pChildNode->Aliases))
        {
            pAliasNode = pChildNode;

            /* Stop the search if the alias has been found */
            break;
        }
        /* For Strict we only stop if the name token is the last one - full match */
        else if ( AnscTcGetLength(pNsTokenChain, ".") == 0 && !StringSListIsEmpty(&pChildNode->StrictAliases))
        {
            pAliasNode = pChildNode;

            /* Stop the search if the alias has been found */
            break;
        }

        AnscFreeMemory(pStringToken);
        pStringToken = NULL;

        /* go one level deeper */
        pNode        = pChildNode;
    }

    if ( pStringToken )
    {
        AnscFreeMemory(pStringToken);
    }


    if ( pAliasNode != NULL )
    {
        while ( (pStringToken = AnscTcUnlinkToken(pNsTokenChain)) )
        {
            if ( AnscSizeOfString(OutRemaining) + AnscSizeOfString(pStringToken->Name) <= (RemainingSize - 1) )
            {
                AnscCatString(OutRemaining, ".");
                AnscCatString(OutRemaining, pStringToken->Name);
            }
            else
            {
                CcspTraceError(("Resulting string is too long %s %s\n", OutRemaining, pStringToken->Name));
                pAliasNode = NULL;

                break;
            }

            AnscFreeMemory(pStringToken);
            pStringToken = NULL;
        }

        if ( pStringToken )
        {
            AnscFreeMemory(pStringToken);
        }
    }

    if ( pNsTokenChain )
    {
        AnscTcFree((ANSC_HANDLE)pNsTokenChain);
        pNsTokenChain = NULL;
    }

    return pAliasNode;
}

/**********************************************************************

    prototype:

        CcspAliasMgrFindAliases
        (
            PCCSP_ALIAS_MNGR_ALIAS_ENTRY pRoot,
            char*                        Name
        );

    description:

        Find alias for the given parameter name.

    argument: PCCSP_ALIAS_MNGR_ALIAS_ENTRY pRoot
              Alias tree root;

              char*                        Name
              Parameter name to find alias for

    return:   found alias names as a single linked list
              NULL if no alias has been found

**********************************************************************/
static
PSLIST_HEADER
CcspAliasMgrFindAliases
    (
        PCCSP_ALIAS_MNGR_ALIAS_ENTRY pRoot,
        char*                        Name
    )
{
    PSLIST_HEADER                        pListResult         = NULL;
    PSLIST_HEADER                        pAliasList          = NULL;
    PCCSP_ALIAS_MNGR_ALIAS_ENTRY         pAliasNode          = NULL;
    QUEUE_HEADER                         InsIdQueue;
    char                                 remaining[CCSP_Alias_Mgr_Name_Length + 1] = {0};
    char*                                result              = NULL;

    BOOL isPartialName = Name [ AnscSizeOfString(Name) - 1 ] == '.';

    QueueInit(&InsIdQueue);

    if ( (pAliasNode = CcspAliasMgrLookupAliasNode(pRoot, Name, &InsIdQueue, remaining, CCSP_Alias_Mgr_Name_Length)) != NULL)
    {
        if (!StringSListIsEmpty(&pAliasNode->Aliases))
        {
            pAliasList = &pAliasNode->Aliases;
        }
        else
        {
            pAliasList = &pAliasNode->StrictAliases;
        }

        if (StringSListIsEmpty(pAliasList))
        {
            CcspTraceError(("Looked up node does not have aliases for name %s\n", Name));
        }
        else
        {
            PSINGLE_LINK_ENTRY                   pLink               = NULL;
            PCCSP_STRING_SINGLE_LINK_LIST_ENTRY  pListEntry          = NULL;
            PCCSP_STRING_SINGLE_LINK_LIST_ENTRY  pResListEntry       = NULL;

            result = AnscAllocateMemory(CCSP_Alias_Mgr_Name_Length + 1);

            pListResult = (PSLIST_HEADER) AnscAllocateMemory( sizeof (SLIST_HEADER) );
            AnscInitializeSList(pListResult);

            pLink = AnscSListGetFirstEntry(pAliasList);

            while (pLink)
            {
                pListEntry = ACCESS_STRING_SINGLE_LIST_ENTRY(pLink);

                if (pListEntry->Value == NULL)
                {
                    CcspTraceWarning(("Found empty alias in the list for %s\n", Name));
                }
                else
                {
                    AnscZeroMemory(result, CCSP_Alias_Mgr_Name_Length + 1);

                    if ( CcspAliasMgrReplaceInstanceIds(pListEntry->Value, result, &InsIdQueue) )
                    {
                        if ( AnscSizeOfString(remaining) > 0 )
                        {
                            if ( (AnscSizeOfString(result) + AnscSizeOfString(remaining)) <= CCSP_Alias_Mgr_Name_Length)
                            {
                                AnscCatString(result, remaining);
                            }
                            else
                            {
                                CcspTraceError(("Remaining name is too long, result =  %s, remaining = %s\n", result, remaining));

                                StringSListRemoveAll(pListResult);
                                AnscFreeMemory(pListResult);

                                pListResult = NULL;

                                break;
                            }
                        }

                        if ( isPartialName )
                        {
                            AnscCatString(result, ".");
                        }

                        pResListEntry = (PCCSP_STRING_SINGLE_LINK_LIST_ENTRY) AnscAllocateMemory( sizeof(CCSP_STRING_SINGLE_LINK_LIST_ENTRY) );

                        if (pResListEntry)
                        {
                            pResListEntry->Value = AnscAllocateMemory(AnscSizeOfString(result) + 1);
                        }

                        if (!pResListEntry || !pResListEntry->Value)
                        {
                            CcspTraceError(("Result list entry allocation failed for %s\n", Name));

                            if (pResListEntry)
                            {
                                AnscFreeMemory(pResListEntry);
                            }

                            StringSListRemoveAll(pListResult);
                            AnscFreeMemory(pListResult);

                            pListResult = NULL;

                            break;
                        }


                        AnscCopyString(pResListEntry->Value, result);

                        AnscSListPushEntry(pListResult, &pResListEntry->Linkage);
                    }
                    else
                    {
                        CcspTraceError(("Replacing instance numbers failed while looking up alias for %s\n", Name));
                    }
                }

                pLink = AnscSListGetNextEntry(pLink);
            }
        }
    }

    QueueRemoveAll(&InsIdQueue);

    if (result)
    {
        AnscFreeMemory(result);
    }

    return pListResult;
}

/**********************************************************************

    prototype:

        ANSC_HANDLE
        CcspAliasMgrInitialize();

    description:

        Initialize alias menagaer.

    argument:

    return:   Alias Manager handle to be used in further calls

**********************************************************************/
ANSC_HANDLE
CcspAliasMgrInitialize()
{
    PCCSP_ALIAS_MNGR_OBJECT am = AnscAllocateMemory( sizeof(CCSP_ALIAS_MNGR_OBJECT) );

    AnscZeroMemory( am, sizeof(CCSP_ALIAS_MNGR_OBJECT) );

    return am;
}

/**********************************************************************

    prototype:

        VOID
        CcspAliasMgrFree
        (
            ANSC_HANDLE hAliasMgr
        );

    description:

        Free memory allocated by this alias manager.
        The handle becomes invalid after making this call.

    argument: ANSC_HANDLE hAliasMgr
              Alias manager handle

    return:

**********************************************************************/
VOID
CcspAliasMgrFree
    (
        ANSC_HANDLE hAliasMgr
    )
{
    PCCSP_ALIAS_MNGR_OBJECT pam = (PCCSP_ALIAS_MNGR_OBJECT) hAliasMgr;

    if ( pam != NULL )
    {
        CcspAliasMgrFreeAliasTree(pam->AliasExtTree);
        CcspAliasMgrFreeAliasTree(pam->AliasIntTree);

        AnscFreeMemory(pam);
    }
}

/**********************************************************************

    prototype:

        BOOL
        CcspAliasMgrLoadAliasInfo
        (
            ANSC_HANDLE               hAliasMgr,
            PANSC_XML_DOM_NODE_OBJECT pPiNode
        );

    description:

        Load alias information from the provided alias xml node.

    argument: ANSC_HANDLE               hAliasMgr
              Alias manager handle;

              PANSC_XML_DOM_NODE_OBJECT pPiNode
              Alias XML node

    return:   true is the node has been successfully added
              false if there was a problem

**********************************************************************/
BOOL
CcspAliasMgrLoadAliasInfo
    (
        ANSC_HANDLE               hAliasMgr,
        PANSC_XML_DOM_NODE_OBJECT pPiNode
    )
{
    PANSC_XML_DOM_NODE_OBJECT         pChildNode     = NULL;
    ULONG                             ulSize;
    char                              internal[CCSP_Alias_Mgr_Name_Length + 1]     = {0};
    char                              external[CCSP_Alias_Mgr_Name_Length + 1]    = {0};
    BOOL                              bSucc          = FALSE;
    PCCSP_ALIAS_MNGR_OBJECT           pam = (PCCSP_ALIAS_MNGR_OBJECT) hAliasMgr;
    BOOL                              bStrict        = FALSE;

    if (pPiNode == NULL || pam == NULL)
    {
        return FALSE;
    }

    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)pPiNode->GetChildByName((ANSC_HANDLE)pPiNode, CCSP_ALIAS_MNGR_XML_NODE_NAME_INTERNAL);
    if ( pChildNode )
    {
        ulSize = CCSP_Alias_Mgr_Name_Length;
        pChildNode->GetDataString(pChildNode, NULL, internal, &ulSize);
    }

    pChildNode = (PANSC_XML_DOM_NODE_OBJECT)pPiNode->GetChildByName((ANSC_HANDLE)pPiNode, CCSP_ALIAS_MNGR_XML_NODE_NAME_EXTERNAL);
    if ( pChildNode )
    {
        ulSize = CCSP_Alias_Mgr_Name_Length;
        pChildNode->GetDataString(pChildNode, NULL, external, &ulSize);
    }

    pPiNode->GetAttrBoolean
        (
            (ANSC_HANDLE)pPiNode,
            CCSP_ALIAS_MNGR_XML_NODE_ATTR_STRICT,
            &bStrict
        );

    if ( CcspAliasMgrEnsureTree(&pam->AliasExtTree)
         && CcspAliasMgrEnsureTree(&pam->AliasIntTree))
    {

        bSucc =
            CcspAliasMgrAliasTreeAddItem
             (
                 pam->AliasExtTree,
                 external,
                 internal,
                 bStrict
             );

        // We only add to external tree if the name differs.
        // same name can be passed as Strict for cases when object has both
        // natively implemented parameters and mapped parameters
        if (bSucc && strcmp(internal, external))
        {
            CcspAliasMgrAliasTreeAddItem
             (
                 pam->AliasIntTree,
                 internal,
                 external,
                 bStrict
             );
        }
    }

    return bSucc;
}

/**********************************************************************

    prototype:

        const char*
        CcspAliasMgrGetInternalName
        (
            ANSC_HANDLE      hAliasMgr,
            char*            ParamName
        );

    description:

        Get an internal name for the object/parameter if an alias exists.

    argument: ANSC_HANDLE               hAliasMgr
              Alias manager handle;

              char*                     ParamName
              Name of the parameter to find the alias for

    return:   Internal name if found
              NULL if the name was not found

**********************************************************************/
PSLIST_HEADER
CcspAliasMgrGetInternalNames
    (
        ANSC_HANDLE      hAliasMgr,
        char*            ParamName
    )
{
    return CcspAliasMgrFindAliases(((PCCSP_ALIAS_MNGR_OBJECT) hAliasMgr)->AliasExtTree, ParamName);
}

/**********************************************************************

    prototype:

        const char*
        CcspAliasMgrGetNextName
            (
                PSLIST_HEADER             pNamesList
            )

    description:

        Get an next name in the list returned from CcspAliasMgrGetInternalNames.
        The name is popped from the list - that is, the entry gets removed.

    argument: PSLIST_HEADER               pNamesList
              Names list returned from CcspAliasMgrGetInternalNames

    return:   Internal name if present
              NULL if the list is empty

**********************************************************************/
const char*
CcspAliasMgrGetNextName
    (
        PSLIST_HEADER             pNamesList
    )
{
    char* res = NULL;

    if ( pNamesList )
    {
        PSINGLE_LINK_ENTRY pLink = AnscSListPopEntry(pNamesList);

        if ( pLink )
        {
            PCCSP_STRING_SINGLE_LINK_LIST_ENTRY pListEntry = ACCESS_STRING_SINGLE_LIST_ENTRY(pLink);
            res = pListEntry->Value;

            AnscFreeMemory(pListEntry);
        }
    }

    return res;

}

/**********************************************************************

    prototype:

        VOID
        CcspAliasMgrFreeNamesList
            (
                PSLIST_HEADER             pNamesList
            )

    description:

        Remove all entries from the list returned from CcspAliasMgrGetInternalNames
        and release all memory allocations.

    argument: PSLIST_HEADER               pNamesList
              Names list returned from CcspAliasMgrGetInternalNames

**********************************************************************/
VOID
CcspAliasMgrFreeNamesList
    (
        PSLIST_HEADER             pNamesList
    )
{
    PSINGLE_LINK_ENTRY pLink;

    while ( (pLink = AnscSListPopEntry(pNamesList)) )
    {
        PCCSP_STRING_SINGLE_LINK_LIST_ENTRY pListEntry = ACCESS_STRING_SINGLE_LIST_ENTRY(pLink);

        if ( pListEntry )
        {
            AnscFreeMemory(pListEntry->Value);
            AnscFreeMemory(pListEntry);
        }
        else
        {
            CcspTraceWarning(("Could not access list entry in names linked list\n"));
        }
    }

    AnscFreeMemory(pNamesList);
}

/**********************************************************************

    prototype:

        const char*
        CcspAliasMgrGetFirstInternalName
        (
            ANSC_HANDLE      hAliasMgr,
            char*            ParamName
        );

    description:

        Get the first internal name for the object/parameter if an alias exists.

    argument: ANSC_HANDLE               hAliasMgr
              Alias manager handle;

              char*                     ParamName
              Name of the parameter to find the alias for

    return:   First found internal name
              NULL if the name was not found

**********************************************************************/
const char*
CcspAliasMgrGetFirstInternalName
    (
        ANSC_HANDLE               hAliasMgr,
        char*                     ParamName
    )
{
    const char* res = NULL;
    PSLIST_HEADER pNamesList = CcspAliasMgrGetInternalNames(hAliasMgr, ParamName);

    if ( pNamesList )
    {
        res = CcspAliasMgrGetNextName(pNamesList);
        CcspAliasMgrFreeNamesList(pNamesList);
    }

    return res;
}

/**********************************************************************

    prototype:

        const char*
        CcspAliasMgrGetExternalName
        (
            ANSC_HANDLE      hAliasMgr,
            char*            ParamName
        );

    description:

        Get an external name for the object/parameter if an alias exists.

    argument: ANSC_HANDLE               hAliasMgr
              Alias manager handle;

              char*                     ParamName
              Name of the parameter to find the alias for

    return:   External name if found
              NULL if the name was not found

**********************************************************************/
const char*
CcspAliasMgrGetExternalName
    (
        ANSC_HANDLE      hAliasMgr,
        char*            ParamName
    )
{
    const char* res = NULL;

    PSLIST_HEADER pNamesList = CcspAliasMgrFindAliases(((PCCSP_ALIAS_MNGR_OBJECT) hAliasMgr)->AliasIntTree, ParamName);

    if ( pNamesList )
    {
        res = CcspAliasMgrGetNextName(pNamesList);

        CcspAliasMgrFreeNamesList(pNamesList);
    }

    return res;
}
