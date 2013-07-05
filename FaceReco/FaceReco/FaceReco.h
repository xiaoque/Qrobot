
// FaceReco.h : main header file for the PROJECT_NAME application
//

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CFaceRecoApp:
// See FaceReco.cpp for the implementation of this class
//

class CFaceRecoApp : public CWinApp
{
public:
	CFaceRecoApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CFaceRecoApp theApp;