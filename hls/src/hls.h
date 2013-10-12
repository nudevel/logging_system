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

#ifndef _HLS_H_
#define _HLS_H_

/*-----------------------------------------------------------------------------
  HLS (Hybrid Logging System) to output message to the stdout and a ring buffer.
  ---------------------------------------------------------------------------*/

typedef enum
{
    HlsRequestStdout, /* dHlsRequestStdout_t */
    HlsRequestMemout, /* dHlsRequestMemout_t */
} dHlsRequest_e;

typedef struct
{
    int iOn;
} dHlsRequestStdout_t;

typedef struct
{
    int iOn;
} dHlsRequestMemout_t;

int iHlsOpen(int iRingBufferLength);
void vHlsClose(int iFd);
int iHlsPrintf(int iFd, const char *pcFormat, ... );
int iHlsControl(int iFd, dHlsRequest_e eRequest, void* vpArg);
int iHlsDisplay(int iFd, const char* pcFileName);

#endif
