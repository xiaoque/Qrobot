
// FaceRecoDlg.h : header file
//

#pragma once
#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"




// CFaceRecoDlg dialog
class CFaceRecoDlg : public CDialogEx
{
// Construction
public:
	CFaceRecoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FACERECO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	IplImage* image;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEnChangeName();
};
