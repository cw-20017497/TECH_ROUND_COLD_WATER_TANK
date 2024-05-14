#ifndef __ERR_COMP_BLDC_H__
#define __ERR_COMP_BLDC_H__

#include "prj_type.h"
#include "comp_bldc.h"


void InitErrCompBldc(void);

#define ERR_COMP_PRT    0
#define ERR_COMP_ERR    1
U8 GetErrCompBldcCount(U8 mType, BldcErrorId_T mId);

// ERROR
U8 CheckErrCompBldc(U8 mu8Error);

// PROTECT
U8 CheckProtectCompBldc(U8 mu8Error);
U8 ReleaseProtectCompBldc(U8 mu8Error);

#endif /* __ERR_COMP_BLDC_H__ */
