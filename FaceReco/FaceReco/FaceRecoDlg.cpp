
// FaceRecoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FaceReco.h"
#include "FaceRecoDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CvCapture* capture;
CRect vedio_rect,face1_rect,face2_rect;
CDC *pDC;
HDC vedio_DC,face1_DC,face2_DC;
CWnd *pWnd;

//CvVideoWriter* writer = 0;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFaceRecoDlg dialog




CFaceRecoDlg::CFaceRecoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFaceRecoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFaceRecoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFaceRecoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CFaceRecoDlg message handlers

BOOL CFaceRecoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here


	//pWnd = GetDlgItem(IDC_VEDIO);
	////pwnd->MoveWindow(35,30,352,288);
 //   pDC =pWnd->GetDC();
	////pDC =GetDC();
 //   hDC= pDC->GetSafeHdc();
	//pWnd->GetClientRect(&vedio_rect);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFaceRecoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFaceRecoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//CRect rect(0,0,810,540);
		CWnd::SetWindowPos(NULL,0,0,810,540,SWP_NOZORDER|SWP_NOMOVE);

		pWnd = GetDlgItem(IDC_VEDIO);
		pWnd->SetWindowPos(NULL,0,0,640,480,SWP_NOZORDER | SWP_NOMOVE);	
		pDC =pWnd->GetDC();
		vedio_DC= pDC->GetSafeHdc();
		pWnd->GetClientRect(&vedio_rect);

		pWnd = GetDlgItem(IDC_FACE1);
		pWnd->SetWindowPos(NULL,670,20,120,120,SWP_NOZORDER);	
		pDC =pWnd->GetDC();
		face1_DC= pDC->GetSafeHdc();
		pWnd->GetClientRect(&face1_rect);

		pWnd = GetDlgItem(IDC_FACE2);
		pWnd->SetWindowPos(NULL,670,160,120,120,SWP_NOZORDER);	
		pDC =pWnd->GetDC();
		face2_DC= pDC->GetSafeHdc();
		pWnd->GetClientRect(&face2_rect);

		pWnd = GetDlgItem(IDC_NAME);
		pWnd->SetWindowPos(NULL,700,300,70,20,SWP_NOZORDER);	

		pWnd = GetDlgItem(IDOK);
		pWnd->SetWindowPos(NULL,690,420,0,0,SWP_NOZORDER | SWP_NOSIZE);	

		pWnd = GetDlgItem(IDCANCEL);
		pWnd->SetWindowPos(NULL,690,460,0,0,SWP_NOZORDER | SWP_NOSIZE);	

		if(!capture)
	{
		capture = cvCaptureFromCAM(0);
		//AfxMessageBox("OK");
	}

	if (!capture)
	{
		AfxMessageBox(_T("无法打开摄像头"));
		return;
	}
	//writer=cvCreateVideoWriter("MyVideo.avi",CV_FOURCC('x','v','I','D'),25,cvSize(640,480));
	// 测试
	IplImage* m_Frame;
	m_Frame=cvQueryFrame(capture);
	CvvImage m_CvvImage;
	m_CvvImage.CopyOf(m_Frame,1);	
	if (true)
	{
		m_CvvImage.DrawToHDC(vedio_DC, &vedio_rect);
		//cvWaitKey(10);
	}

	// 设置计时器,每10ms触发一次事件
	SetTimer(1,10,NULL);
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFaceRecoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFaceRecoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	/************************************************************************/
	/* 显示摄像头                                                           */
	/************************************************************************/
	IplImage* m_Frame;
	m_Frame=cvQueryFrame(capture);
	CvvImage m_CvvImage;
	m_CvvImage.CopyOf(m_Frame,1);	
	if (true)
	{
		m_CvvImage.DrawToHDC(vedio_DC, &vedio_rect);
		//cvWriteFrame(writer,m_Frame);
		//cvWaitKey(10);
	}

	CDialogEx::OnTimer(nIDEvent);
}

