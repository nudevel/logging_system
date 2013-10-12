/*=============================================================================

Copyright (c) 2013, Naoto Uegaki
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#include "hls.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

int iFd;

void test(void *vpArg)
{
    while(1){
        iHlsPrintf(iFd, "test %08x\n", (int)vpArg);
        usleep(300*1000);
    }
}

int main()
{
    int i = 0;
    pthread_t  tThread;
    char ch;
    
    iFd = iHlsOpen(4*1024);

    for(i=0; i<3; ++i)
      pthread_create(&tThread, NULL, (void*)test, (void*)(i+1));

    while( (ch = getchar()) != 'x' ){
        if( ch == 'd' ){
          iHlsDisplay(iFd, NULL);
        } else if( ch == 's' ){
            dHlsRequestStdout_t tReq = { 0 };
            iHlsControl(iFd, HlsRequestStdout, (void*)&tReq);
        } else if( ch == 'S' ){
            dHlsRequestStdout_t tReq = { 1 };
            iHlsControl(iFd, HlsRequestStdout, (void*)&tReq);
        }
    }

    vHlsClose(iFd);
    
    return 0;
}

