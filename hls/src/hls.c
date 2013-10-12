/*=============================================================================

Copyright (c) 2013, Naoto Uegaki
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

  Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#include "hls.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct
{
    char acTmp[2048];
    char *pcBuff;
    int iLen;
    char *pcWritePointer;
    pthread_mutex_t tMutex;
    int iStdout;
    int iMemout;
} dHls_t;

int iHlsOpen(int iRingBufferLength)
{
    dHls_t *ptFd = calloc( 1, sizeof(dHls_t) );

    if( ptFd ){
        ptFd->pcBuff = calloc( 1, iRingBufferLength );
        if( !ptFd->pcBuff ){
            free(ptFd);
            ptFd = NULL;
        } else {
            ptFd->iLen = iRingBufferLength;
            ptFd->pcWritePointer = ptFd->pcBuff;
            ptFd->iStdout = 1;
            ptFd->iMemout = 1;
            pthread_mutex_init( &ptFd->tMutex, NULL); // always return 0
        }
    }

    return (int)ptFd;
}

void vHlsClose(int iFd)
{
     dHls_t *ptFd = calloc( 1, sizeof(dHls_t) );
    
    if( ptFd && ptFd->pcBuff ) free(ptFd->pcBuff);
    if( ptFd ){
        pthread_mutex_destroy( &ptFd->tMutex );
        free(ptFd);
    }
}

int iHlsPrintf(int iFd, const char *pcFormat, ... )
{
    dHls_t* ptFd = (dHls_t*)iFd;
    int iLen;
    va_list ap;

    if( !ptFd )
      return -1;

    if( pthread_mutex_lock( &ptFd->tMutex ) != 0 )
      return -1;

    iLen = sprintf( ptFd->acTmp, "<%08x>", (int)time(NULL) );
    
    va_start( ap, pcFormat );
    vsnprintf( ptFd->acTmp + iLen, 2048 - iLen - 1, pcFormat, ap);
	va_end( ap );

    iLen = strlen( ptFd->acTmp );

    if( ptFd->iMemout ){
        if( ptFd->pcWritePointer + iLen > ptFd->pcBuff + ptFd->iLen ){
            int iTmpLen = ptFd->pcBuff + ptFd->iLen - ptFd->pcWritePointer;
            memcpy( ptFd->pcWritePointer, ptFd->acTmp, iTmpLen );
            memcpy( ptFd->pcBuff, ptFd->acTmp + iTmpLen, iLen - iTmpLen );
            ptFd->pcWritePointer = ptFd->pcBuff + iLen - iTmpLen;
        } else {
            memcpy( ptFd->pcWritePointer, ptFd->acTmp, iLen );
            ptFd->pcWritePointer += iLen;
        }
    }

    if( pthread_mutex_unlock( &ptFd->tMutex ) != 0 )
      return -1;

    if( ptFd->iStdout )
      printf(ptFd->acTmp);

    return iLen;
}

int iHlsControl(int iFd, dHlsRequest_e eRequest, void* vpArg)
{
    int iRet = 0;
    dHls_t* ptFd = (dHls_t*)iFd;

    if( !ptFd )
      return -1;

    if( pthread_mutex_lock( &ptFd->tMutex ) != 0 )
      return -1;
    
    switch(eRequest){
      case HlsRequestStdout:
        ptFd->iStdout = ((dHlsRequestStdout_t*)vpArg)->iOn;
        break;
      case HlsRequestMemout:
        ptFd->iMemout = ((dHlsRequestMemout_t*)vpArg)->iOn;
        break;
      default:
        iRet = -1;
        break;
    }

    if( pthread_mutex_unlock( &ptFd->tMutex ) != 0 )
      return -1;
    
    return iRet;
}

int iHlsDisplay(int iFd, const char* pcFileName)
{
    FILE* ptFile = NULL;
    dHls_t* ptFd = (dHls_t*)iFd;
    char *pcReadPointer;
    
    if( !ptFd )
      return -1;

    if( pcFileName ){
        ptFile = fopen(pcFileName, "w");
        if( !ptFile )
          return -1;
    }

    if( pthread_mutex_lock( &ptFd->tMutex ) != 0 )
      goto _err_end_;

    for( pcReadPointer = ptFd->pcWritePointer; pcReadPointer < ptFd->pcBuff + ptFd->iLen; pcReadPointer++ ){
        if( *pcReadPointer != 0 )
          fprintf( ptFile==NULL ? stdout : ptFile, "%c", *pcReadPointer);
    }
    for( pcReadPointer = ptFd->pcBuff; pcReadPointer < ptFd->pcWritePointer; pcReadPointer++ ){
        if( *pcReadPointer != 0 )
          fprintf( ptFile==NULL ? stdout : ptFile, "%c", *pcReadPointer);
    }
    
    if( pthread_mutex_unlock( &ptFd->tMutex ) != 0 )
      goto _err_end_;

    if( ptFile ) fclose(ptFile);
    return 0;
    
  _err_end_:
    if( ptFile ) fclose(ptFile);
    return -1;
}


