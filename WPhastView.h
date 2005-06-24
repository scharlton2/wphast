// WPhastView.h : interface of the CWPhastView class
//

#pragma once

#include "IObserver.h"

class vtkObject;
class vtkWin32OpenGLRenderWindow;
class vtkRenderer;
class vtkWin32RenderWindowInteractor;
class vtkBoxWidget;
class vtkPointWidget;
class vtkPointWidget2;

class CViewVTKCommand;

class vtkInteractorStyle;

class vtkCursor3D;
class vtkPolyDataMapper;
class vtkActor;

class vtkCubeSource;
class vtkCylinderSource;
class CWellActor;

class CWPhastView : public CView, public IObserver
{
protected: // create from serialization only
	CWPhastView();
	DECLARE_DYNCREATE(CWPhastView)

// Attributes
public:
	CWPhastDoc* GetDocument() const;

// Operations
public:
	void HighlightProp(vtkProp *pProp);
	void HighlightProp3D(vtkProp3D *pProp3D);

	// IObserver
	virtual void Update(IObserver* pSender = 0, LPARAM lHint = 0L, CObject* pHint = 0, vtkObject* pObject = 0);

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
private:
	void Pipeline( void );

public:
	virtual ~CWPhastView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	// Renderer
	vtkWin32OpenGLRenderWindow     *m_RenderWindow;
	vtkRenderer                    *m_Renderer;
	vtkWin32RenderWindowInteractor *m_RenderWindowInteractor;

	vtkBoxWidget                   *m_pBoxWidget;
	vtkPointWidget2                *m_pPointWidget2;

	CViewVTKCommand                *m_pViewVTKCommand;

	vtkInteractorStyle             *m_pInteractorStyle;

	bool                            m_bResetCamera;

	// 3D Cursor
	vtkCursor3D                    *m_pCursor3D;
	vtkPolyDataMapper              *m_pCursor3DMapper;
	vtkActor                       *m_pCursor3DActor;

	// new zone
	vtkCubeSource                  *m_pNewCube;
	vtkPolyDataMapper              *m_pNewCubeMapper;
	vtkActor                       *m_pNewCubeActor;

	int                             m_ViewFromDirection;

	// new well
	CWellActor                     *m_pWellActor;

	// new rier
	CRiverActor                    *m_pRiverActor;

	friend class CViewVTKCommand;


// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	vtkRenderer* GetRenderer(void) const {return this->m_Renderer;}
	vtkWin32RenderWindowInteractor* GetRenderWindowInteractor(void) const {return this->m_RenderWindowInteractor;}
	vtkBoxWidget *GetBoxWidget (void)const {return this->m_pBoxWidget;}

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
public:
	void ResetCamera(void);
	virtual void OnInitialUpdate(void);
	void DeleteContents(void);
	void ClearSelection(void);
	void Select(vtkProp *pProp);


	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnUpdateToolsNewZone(CCmdUI *pCmdUI);
	afx_msg void OnToolsNewZone();

	void CancelNewZone(void);
	bool CreatingNewZone(void)const;
	void StartNewZone(void);
	void EndNewZone(void);

public:
	afx_msg void OnToolsNewWell();
	afx_msg void OnUpdateToolsNewWell(CCmdUI *pCmdUI);

	bool CreatingNewWell(void)const;
	void StartNewWell(void);
	void CancelNewWell(void);
	void EndNewWell(void);

public:

	afx_msg void OnUpdateViewFromNx(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewFromNy(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewFromNz(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewFromPx(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewFromPy(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewFromPz(CCmdUI *pCmdUI);

	afx_msg void OnViewFromNx();
	afx_msg void OnViewFromNy();
	afx_msg void OnViewFromNz();
	afx_msg void OnViewFromPx();
	afx_msg void OnViewFromPy();
	afx_msg void OnViewFromPz();
	afx_msg void OnViewFromNextDirection();

	void ParallelProjectionOff(void);
	void ParallelProjectionOn(void);

protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

public:
	afx_msg void OnUpdateToolsNewRiver(CCmdUI *pCmdUI);
	afx_msg void OnToolsNewRiver();

	// Rivers
	//
	vtkCallbackCommand    *RiverCallbackCommand;
	static void RiverListener(vtkObject* caller, unsigned long eid, void* clientdata, void *calldata);
	void OnEndNewRiver(void);
	bool CreatingNewRiver(void)const;
	void StartNewRiver(void);
	void CancelNewRiver(void);
	void EndNewRiver(void);

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
};

#ifndef _DEBUG  // debug version in WPhastView.cpp
inline CWPhastDoc* CWPhastView::GetDocument() const
   { return reinterpret_cast<CWPhastDoc*>(m_pDocument); }
#endif

