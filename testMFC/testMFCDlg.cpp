// testMFCDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "testMFC.h"
#include "testMFCDlg.h"
#include "afxdialogex.h"
#include <thread>
#include <random>
#include <mutex>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CtestMFCDlg 대화 상자



CtestMFCDlg::CtestMFCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TESTMFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtestMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CtestMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDOK, &CtestMFCDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RANDOMBUTTON, &CtestMFCDlg::OnBnClickedRandombutton)
	ON_MESSAGE(WM_USER + 1, &CtestMFCDlg::OnThreadUpdate)
END_MESSAGE_MAP()


// CtestMFCDlg 메시지 처리기

BOOL CtestMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	// 이미지 영역 좌표 설정 (예: 대화 상자 내부의 특정 위치)
	// 이 영역은 대화 상자 클라이언트 좌표 기준입니다.
	m_imageRect = CRect(10, 10, 730, 370);

	// 이미지 초기화
	// 8비트(그레이스케일) 이미지로 생성
	m_image.Create(m_imageRect.Width(), m_imageRect.Height(), 8);

	// 팔레트 설정 (그레이스케일: 0은 검정, 255는 흰색)
	static RGBQUAD rgb[256];
	for (int i = 0; i < 256; i++)
	{
		rgb[i].rgbRed = i;
		rgb[i].rgbGreen = i;
		rgb[i].rgbBlue = i;
	}
	m_image.SetColorTable(0, 256, rgb);

	// 이미지 초기화 (흰색)
	ClearImage();

	// 원 두께 설정 대화상자 표시
	CThicknessDlg dlg;
	if (dlg.DoModal() == IDOK) {
		// 입력값 범위 확인 (1~10)
		if (dlg.m_thickness < 1 || dlg.m_thickness > 10) {
			MessageBox(_T("원의 두께는 1에서 10 사이여야 합니다."), _T("입력 오류"), MB_OK | MB_ICONERROR);
			EndDialog(IDCANCEL);
			return FALSE;
		}
		m_circleThickness = dlg.m_thickness;
	}
	else {
		// 취소 버튼을 누른 경우 기본값 사용
		m_circleThickness = 1;
	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 이미지 영역을 흰색으로 지우는 헬퍼 함수
void CtestMFCDlg::ClearImage()
{
	if (m_image.IsNull()) return;

	unsigned char* pImage = (unsigned char*)m_image.GetBits();
	int width = m_image.GetWidth();
	int height = m_image.GetHeight();
	int pitch = m_image.GetPitch();

	for (int i = 0; i < height; i++)
	{
		// pitch만큼 반복하며 흰색(255)으로 채움
		memset(pImage + i * pitch, 255, width);
	}
}

// 이미지 좌표에 픽셀을 설정하는 헬퍼 함수
// ix, iy는 m_imageRect 내부의 좌표, 즉 0 ~ Width-1, 0 ~ Height-1 범위
void CtestMFCDlg::SetPixelInImage(int ix, int iy, unsigned char color)
{
	if (m_image.IsNull()) return;

	int width = m_image.GetWidth();
	int height = m_image.GetHeight();
	int pitch = m_image.GetPitch();

	// 이미지 경계 내에 있는지 확인
	if (ix >= 0 && ix < width && iy >= 0 && iy < height)
	{
		unsigned char* pImage = (unsigned char*)m_image.GetBits();
		pImage[iy * pitch + ix] = color;
	}
}


// 세 점을 지나는 원의 중심과 반지름을 계산하는 함수
// 세 점이 일직선상에 있으면 false 반환
bool CtestMFCDlg::CalculateCircumcircle(const CPoint& p1, const CPoint& p2, const CPoint& p3, CPoint& center, int& radius)
{
	// 세 점 (x1, y1), (x2, y2), (x3, y3)
	double x1 = p1.x, y1 = p1.y;
	double x2 = p2.x, y2 = p2.y;
	double x3 = p3.x, y3 = p3.y;

	// 점이 겹치는지 확인 (매우 근접한 경우도 처리 필요하면 epsilon 사용)
	if ((x1 == x2 && y1 == y2) || (x2 == x3 && y2 == y3) || (x1 == x3 && y1 == y3)) {
		return false; // 두 점 이상이 같음
	}

	// 세 점이 일직선상에 있는지 확인 (기울기 비교 또는 면적 계산)
	// (y2-y1)*(x3-x2) == (y3-y2)*(x2-x1) 이면 일직선
	double area = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
	if (abs(area) < 1e-6) { // 면적이 0에 가깝다면 일직선상
		return false;
	}

	// 세 점을 지나는 원의 중심 (h, k) 및 반지름 (r) 계산
	// 두 변의 수직 이등분선의 교점이 원의 중심
	// 변 (x1, y1) - (x2, y2) 의 수직 이등분선: 2(x2-x1)h + 2(y2-y1)k = (x2^2 + y2^2) - (x1^2 + y1^2)
	// 변 (x2, y2) - (x3, y3) 의 수직 이등분선: 2(x3-x2)h + 2(y3-y2)k = (x3^2 + y3^2) - (x2^2 + y2^2)

	double a = 2 * (x2 - x1);
	double b = 2 * (y2 - y1);
	double c = (x2 * x2 + y2 * y2) - (x1 * x1 + y1 * y1);

	double d = 2 * (x3 - x2);
	double e = 2 * (y3 - y2);
	double f = (x3 * x3 + y3 * y3) - (x2 * x2 + y2 * y2);

	// 연립 방정식 해결: ah + bk = c, dh + ek = f
	// Cramer's rule 사용
	double determinant = a * e - b * d;

	// determinant가 0이면 해가 없거나 무수히 많음 (일직선상 또는 점 겹침)
	if (abs(determinant) < 1e-6) {
		return false; // 일직선상 또는 점 겹침 (이미 위에서 확인했지만 안전하게 한번 더)
	}

	double h = (c * e - b * f) / determinant;
	double k = (a * f - c * d) / determinant;

	// 반지름 계산
	double r_sq = (x1 - h) * (x1 - h) + (y1 - k) * (y1 - k);
	double r = sqrt(r_sq);

	// 결과를 정수형으로 저장
	center.x = static_cast<int>(round(h));
	center.y = static_cast<int>(round(k));
	radius = static_cast<int>(round(r));

	// 반지름이 너무 작거나 음수인 경우 무시
	if (radius <= 0) return false;

	return true; // 계산 성공
}


// CImage에 직접 원의 테두리를 그리는 함수 (Midpoint Circle Algorithm)
void CtestMFCDlg::DrawCircleOnImage()
{
	// 이미지 지우기
	ClearImage();

	// 점이 3개 미만이면 원을 그리지 않음
	if (m_points.size() < 3) {
		m_circleCenter = CPoint(0, 0);
		m_circleRadius = 0;
		return;
	}

	// 세 점을 가져옴
	const CPoint& p1 = m_points[0].position;
	const CPoint& p2 = m_points[1].position;
	const CPoint& p3 = m_points[2].position;

	// 원의 중심과 반지름 계산
	if (!CalculateCircumcircle(p1, p2, p3, m_circleCenter, m_circleRadius))
	{
		m_circleCenter = CPoint(0, 0);
		m_circleRadius = 0;
		return;
	}

	// 원의 중심 좌표를 이미지의 로컬 좌표로 변환
	int image_centerX = m_circleCenter.x - m_imageRect.left;
	int image_centerY = m_circleCenter.y - m_imageRect.top;
	int radius = m_circleRadius;

	// 두 개의 원을 그리기 위한 반지름 계산
	int outer_radius = radius;
	int inner_radius = radius - m_circleThickness;

	// 원을 그리는 함수
	auto draw_circle = [&](int r) {
		int x = 0;
		int y = r;
		int p = 1 - r;

		auto plot_points = [&](int cx, int cy, int px, int py) {
			SetPixelInImage(cx + px, cy + py, 0);
			SetPixelInImage(cx - px, cy + py, 0);
			SetPixelInImage(cx + px, cy - py, 0);
			SetPixelInImage(cx - px, cy - py, 0);
			SetPixelInImage(cx + py, cy + px, 0);
			SetPixelInImage(cx - py, cy + px, 0);
			SetPixelInImage(cx + py, cy - px, 0);
			SetPixelInImage(cx - py, cy - px, 0);
		};

		plot_points(image_centerX, image_centerY, x, y);

		while (x < y) {
			x++;
			if (p < 0) {
				p += 2 * x + 1;
			}
			else {
				y--;
				p += 2 * (x - y) + 1;
			}
			plot_points(image_centerX, image_centerY, x, y);
		}
	};

	// 두 원 사이를 채우는 함수
	auto fill_between_circles = [&]() {
		for (int y = -outer_radius; y <= outer_radius; y++) {
			for (int x = -outer_radius; x <= outer_radius; x++) {
				int distance_squared = x * x + y * y;
				if (distance_squared <= outer_radius * outer_radius && 
					distance_squared >= inner_radius * inner_radius) {
					SetPixelInImage(image_centerX + x, image_centerY + y, 0);
				}
			}
		}
	};

	// 두께가 1이면 단일 원을 그림
	if (m_circleThickness == 1) {
		draw_circle(radius);
	}
	else {
		// 두 개의 원을 그리고 그 사이를 채움
		fill_between_circles();
	}
}


void CtestMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크가 이 작업을 자동으로 수행합니다.

void CtestMFCDlg::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

	// 대화 상자에 최소화 단추를 추가할 경우...
	if (!m_image.IsNull())
	{
		// 이미지 영역 (대화 상자 클라이언트 좌표)
		CRect destRect = m_imageRect;
		// 이미지 소스 영역 (CImage 객체의 전체 크기)
		CRect srcRect(0, 0, m_image.GetWidth(), m_image.GetHeight());

		// BitBlt 또는 StretchBlt 대신 CImage::Draw 사용
		m_image.Draw(dc.GetSafeHdc(), destRect, srcRect);
	}


	for (const auto& p : m_points) {
		COLORREF dotColor = RGB(0, 0, 0); // 기본 검정색

		CBrush brush(dotColor);
		CBrush* pOldBrush = dc.SelectObject(&brush);
		int dotSize = 10; // 점의 반지름
		dc.Ellipse(p.position.x - dotSize, p.position.y - dotSize, p.position.x + dotSize, p.position.y + dotSize);
		dc.SelectObject(pOldBrush);
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CtestMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CtestMFCDlg::OnBnClickedOk()
{
	// 랜덤 스레드가 실행 중이면 스레드를 종료하고 초기화
	if (m_isRandomThreadRunning) {
		m_isRandomThreadRunning = false;
		
		// 스레드가 완전히 종료될 때까지 잠시 대기
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		
		// 초기화 수행
		m_points.clear();
		m_selectedPoint = nullptr;
		m_circleCenter = CPoint(0, 0);
		m_circleRadius = 0;
		ClearImage();
		Invalidate();
		return;
	}

	// 일반적인 초기화 동작
	m_points.clear();
	m_selectedPoint = nullptr;
	m_circleCenter = CPoint(0, 0);
	m_circleRadius = 0;
	ClearImage();
	Invalidate();
}

void CtestMFCDlg::OnBnClickedRandombutton()
{
	// If thread is already running, don't start a new one
	if (m_isRandomThreadRunning) {
		return;
	}

	// Clear existing points and image
	m_points.clear();
	ClearImage();
	InvalidateRect(m_imageRect, FALSE);
	UpdateWindow();

	// Start the random thread
	m_isRandomThreadRunning = true;
	m_randomThread = std::thread(&CtestMFCDlg::RandomThreadFunction, this);
	m_randomThread.detach();  // Detach the thread to run independently
}

CPoint CtestMFCDlg::GenerateRandomPoint()
{
	// Get the image dimensions
	int width = m_imageRect.Width();
	int height = m_imageRect.Height();

	// Create random number generator
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> disX(0, width - 1);
	std::uniform_int_distribution<> disY(0, height - 1);

	// Generate random point within image bounds
	return CPoint(disX(gen), disY(gen));
}

LRESULT CtestMFCDlg::OnThreadUpdate(WPARAM wParam, LPARAM lParam)
{
	// This function is called in the main thread
	if (m_isRandomThreadRunning) {
		InvalidateRect(m_imageRect, FALSE);
		UpdateWindow();
	}
	return 0;
}

void CtestMFCDlg::RandomThreadFunction()
{
	try {
		// Run for 5 iterations
		for (int i = 0; i < 5 && m_isRandomThreadRunning; i++) {
			{
				// Lock mutex before accessing shared resources
				std::lock_guard<std::mutex> lock(m_mutex);
				
				// Clear existing points
				m_points.clear();
				
				// Generate 3 random points
				for (int j = 0; j < 3; j++) {
					CPoint randomPos = GenerateRandomPoint();
					m_points.push_back(Point(randomPos));
				}

				// Calculate and draw circle
				CPoint center;
				int radius;
				if (CalculateCircumcircle(m_points[0].position, 
										m_points[1].position, 
										m_points[2].position, 
										center, radius)) {
					// Clear and draw new points and circle
					ClearImage();
					DrawCircleOnImage();
				}
			} // mutex is automatically released here

			// Send message to main thread to update UI
			if (m_isRandomThreadRunning) {
				PostMessage(WM_USER + 1, 0, 0);
			}

			// Wait for 2 seconds
			for (int j = 0; j < 20 && m_isRandomThreadRunning; j++) {
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
	}
	catch (...) {
		// Handle any exceptions
	}
	
	m_isRandomThreadRunning = false;
}

// Add destructor to clean up thread
CtestMFCDlg::~CtestMFCDlg()
{
	m_isRandomThreadRunning = false;
	if (m_randomThread.joinable()) {
		m_randomThread.join();
	}
}

void CtestMFCDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// 랜덤 스레드가 실행 중이면 모든 마우스 상호작용 무시
	if (m_isRandomThreadRunning) {
		return;
	}

	// 이미지 영역 내에서만 처리
	if (!m_imageRect.PtInRect(point)) {
		CDialogEx::OnLButtonDown(nFlags, point);
		return;
	}

	// 기존 점 중 클릭한 점이 있는지 확인
	m_selectedPoint = nullptr;
	for (auto& p : m_points) {
		if (p.IsNear(point)) {
			p.isSelected = true;
			m_selectedPoint = &p;
			goto EndLButtonDown;
		}
	}

	// 새로운 점 추가 (최대 3개)
	if (m_points.size() < 3) {
		m_points.emplace_back(point);
	}

EndLButtonDown:
	// 점이 3개이면 원을 계산하고 그립니다.
	if (m_points.size() == 3) {
		DrawCircleOnImage();
	}

	Invalidate();
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CtestMFCDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// 랜덤 스레드가 실행 중이면 모든 마우스 상호작용 무시
	if (m_isRandomThreadRunning) {
		return;
	}

	// 이미지 영역 내에서만 드래그 처리
	if (!m_imageRect.PtInRect(point)) {
		// 선택된 점이 있다면 드래그 종료 처리
		if (m_selectedPoint) {
			m_selectedPoint->isSelected = false;
			m_selectedPoint = nullptr;
			// 점 이동이 끝났으므로 원 업데이트 및 화면 갱신
			if (m_points.size() == 3) {
				DrawCircleOnImage();
			}
			Invalidate();
		}
		CDialogEx::OnMouseMove(nFlags, point);
		return;
	}

	// 왼쪽 버튼이 눌린 상태이고 점이 선택되어 있으면 드래그
	if (m_selectedPoint && (nFlags & MK_LBUTTON)) {
		m_selectedPoint->Move(point);

		// 점이 이동했으므로 원을 다시 계산하고 그립니다.
		if (m_points.size() == 3) {
			DrawCircleOnImage();
		}
		Invalidate();
	}
	CDialogEx::OnMouseMove(nFlags, point);
}

void CtestMFCDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// 랜덤 스레드가 실행 중이면 모든 마우스 상호작용 무시
	if (m_isRandomThreadRunning) {
		return;
	}

	if (m_selectedPoint) {
		m_selectedPoint->isSelected = false;
		m_selectedPoint = nullptr;

		// 점 이동이 끝났으므로 원 업데이트 및 화면 갱신
		if (m_points.size() == 3) {
			DrawCircleOnImage();
		}
		Invalidate();
	}

	CDialogEx::OnLButtonUp(nFlags, point);
}

// CThicknessDlg 구현
IMPLEMENT_DYNAMIC(CThicknessDlg, CDialogEx)

CThicknessDlg::CThicknessDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_THICKNESS_DIALOG, pParent)
	, m_thickness(1)
{
}

CThicknessDlg::~CThicknessDlg()
{
}

void CThicknessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_THICKNESS_EDIT, m_thickness);
	
	// 입력값 검증
	if (pDX->m_bSaveAndValidate) {
		CString strValue;
		GetDlgItemText(IDC_THICKNESS_EDIT, strValue);
		
		// 숫자가 아닌 문자가 있는지 확인
		for (int i = 0; i < strValue.GetLength(); i++) {
			if (!isdigit(strValue[i])) {
				MessageBox(_T("숫자만 입력 가능합니다."), _T("입력 오류"), MB_OK | MB_ICONERROR);
				pDX->Fail();
				return;
			}
		}
		
		// 범위 검사 (1-10)
		if (m_thickness < 1 || m_thickness > 10) {
			MessageBox(_T("원의 두께는 1에서 10 사이여야 합니다."), _T("입력 오류"), MB_OK | MB_ICONERROR);
			pDX->Fail();
			return;
		}
	}
}

BEGIN_MESSAGE_MAP(CThicknessDlg, CDialogEx)
	ON_BN_CLICKED(IDC_DEFAULT_BUTTON, &CThicknessDlg::OnBnClickedDefaultButton)
END_MESSAGE_MAP()

void CThicknessDlg::OnBnClickedDefaultButton()
{
	m_thickness = 1;  // 기본값 설정
	UpdateData(FALSE);  // UI 업데이트
}