#ifndef __DISPLAY_ERROR_H__
#define __DISPLAY_ERROR_H__

#include "prj_type.h"
#include "error.h"

void DisplayError(ErrorId_T mu8ErrCode, U8 mu8Cmd );
U8  GetErrorDisplayId(ErrorId_T mErrId);

#endif /*__DISPLAY_ERROR_H__*/
