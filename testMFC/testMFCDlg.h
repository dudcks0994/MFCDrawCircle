// testMFCDlg.h: 헤더 파일
//

#pragma once
#include <vector>
#include <thread>
#include <random>
#include <chrono>
#include <mutex>
#include <atomic>

// 원 두께 설정을 위한 대화상자
class CThicknessDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CThicknessDlg)

public:
	CThicknessDlg(CWnd* pParent = nullptr);
	virtual ~CThicknessDlg();

	enum { IDD = IDD_THICKNESS_DIALOG };

	int m_thickness;  // 원의 두께

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedDefaultButton();
};

class CPointObject {
public:
	CPoint position; // 점의 위치
	bool isSelected; // 점이 선택되었는지 여부

	CPointObject(CPoint pos) : position(pos), isSelected(false) {}

	// 점이 특정 위치와 가까운지 확인
	bool IsNear(CPoint point, int radius = 10) {
		int dx = position.x - point.x;
		int dy = position.y - point.y;
		return (dx * dx + dy * dy <= radius * radius);
	}

	// 점 이동
	void Move(CPoint newPos) {
		position = newPos;
	}
};


// CtestMFCDlg 대화 상자
class CtestMFCDlg : public CDialogEx
{
// 생성입니다.
public:
	CtestMFCDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	~CtestMFCDlg();  // 소멸자 추가

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTMFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;
	std::thread m_randomThread;  // Random thread
	std::atomic<bool> m_isRandomThreadRunning;  // Thread running flag
	std::mutex m_mutex;  // Mutex for thread safety
	int m_circleThickness;  // 원의 두께

	struct Point {
		CPoint position;
		bool isSelected;

		Point(CPoint p) : position(p), isSelected(false) {}

		bool IsNear(CPoint p, int tolerance = 20) const { // 클릭 허용 범위
			return abs(p.x - position.x) <= tolerance && abs(p.y - position.y) <= tolerance;
		}

		void Move(CPoint p) {
			position = p;
		}
	};
	std::vector<Point> m_points; // 사용자가 찍은 점들
	Point* m_selectedPoint = nullptr; // 드래그 중인 점

	CImage m_image; // 그림을 그릴 이미지 객체
	CRect m_imageRect; // 이미지가 그려질 영역 (대화 상자 클라이언트 영역 기준)

	// 원 정보를 저장
	CPoint m_circleCenter; // 원의 중심 (대화 상자 클라이언트 좌표)
	int m_circleRadius = 0; // 원의 반지름

	// 세 점을 지나는 원의 중심과 반지름을 계산하는 함수
	bool CalculateCircumcircle(const CPoint& p1, const CPoint& p2, const CPoint& p3, CPoint& center, int& radius);

	// CImage에 직접 원의 테두리를 그리는 함수
	void DrawCircleOnImage();

	// CImage를 흰색으로 지우는 함수
	void ClearImage();

	// 이미지 좌표에 점을 그리는 함수 (헬퍼)
	void SetPixelInImage(int x, int y, unsigned char color);

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	// Thread-safe message handling
	afx_msg LRESULT OnThreadUpdate(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedRandombutton();

	// Random thread function
	void RandomThreadFunction();

	// Helper function to generate random points
	CPoint GenerateRandomPoint();
};
