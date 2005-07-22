// GridPropertyPage2.cpp : implementation file
//

#include "stdafx.h"
#include "WPhast.h"
#include "GridPropertyPage2.h"

#include "Global.h" // UM_DDX_FAILURE
#include "GridSetAction.h"
#include "ModelessPropertySheet.h"
#include "SeriesDialog.h"

// CGridPropertyPage2 dialog

IMPLEMENT_DYNAMIC(CGridPropertyPage2, CPropertyPage)
CGridPropertyPage2::CGridPropertyPage2()
: CPropertyPage(CGridPropertyPage2::IDD)
, m_nIndex(0)
, m_bNeedAction(false)
, m_pDoc(0)
, m_pActor(0)
, m_pAction(0)
{
}

CGridPropertyPage2::~CGridPropertyPage2()
{
	ASSERT(this->m_pAction == 0); // should have been reset in PostNcDestroy
}

void CGridPropertyPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_COOR, this->m_tabCtrl);
	DDX_Control(pDX, IDC_SPIN_U_NODES, this->m_spinUnified);
	DDX_Control(pDX, IDC_SPIN_N_NODES, this->m_spinNonunified);

	DDX_GridControl(pDX, IDC_GRID_NONUNIFORM, this->m_wndNonuniformGrid);

	if (this->m_tabCtrl.GetItemCount() == 0)
	{
		this->m_tabCtrl.InsertItem(0, "X");
		this->m_tabCtrl.InsertItem(1, "Y");
		this->m_tabCtrl.InsertItem(2, "Z");
		this->m_tabCtrl.SetCurSel(this->m_nIndex);
	}

	if (!pDX->m_bSaveAndValidate && this->m_wndNonuniformGrid.GetColumnCount() == 0)
	{
		const int MIN_ROWS = 20;
		TRY
		{
			this->m_wndNonuniformGrid.SetRowCount(MIN_ROWS);
			this->m_wndNonuniformGrid.SetColumnCount(2);
			this->m_wndNonuniformGrid.SetFixedRowCount(1);
			this->m_wndNonuniformGrid.SetFixedColumnCount(1);
			this->m_wndNonuniformGrid.EnableTitleTips(FALSE);
			this->m_wndNonuniformGrid.SetCurrentFocusCell(1, 1);
			this->m_wndNonuniformGrid.SetHighLight(GV_HIGHLIGHT_ALWAYS);
		}
		CATCH (CMemoryException, e)
		{
			e->ReportError();
			e->Delete();
		}
		END_CATCH

		this->m_wndNonuniformGrid.SetColumnWidth(0, 44);
		this->m_wndNonuniformGrid.SetColumnWidth(1, 116);
	}

	if (this->m_nIndex == 2)
	{
		DDX_Text(pDX, IDC_STATIC_MIN_UNITS, this->m_strVerticalUnits);
		DDX_Text(pDX, IDC_STATIC_MAX_UNITS, this->m_strVerticalUnits);
		DDX_Text(pDX, IDC_STATIC_SNAP_UNITS, this->m_strVerticalUnits);
	}
	else
	{
		DDX_Text(pDX, IDC_STATIC_MIN_UNITS, this->m_strHorizontalUnits);
		DDX_Text(pDX, IDC_STATIC_MAX_UNITS, this->m_strHorizontalUnits);
		DDX_Text(pDX, IDC_STATIC_SNAP_UNITS, this->m_strHorizontalUnits);
	}

	if (pDX->m_bSaveAndValidate)
	{
		ASSERT(this->m_tabCtrl.GetCurSel() == this->m_nIndex);

		// Uniform / Nonuniform radio
		//
		if (this->GetCheckedRadioButton(IDC_RADIO_UNIFORM, IDC_RADIO_NONUNIFORM) == IDC_RADIO_UNIFORM)
		{
			this->m_gridKeyword.m_grid[this->m_nIndex].uniform          = TRUE;
			this->m_gridKeyword.m_grid[this->m_nIndex].uniform_expanded = FALSE;

			// Minimum
			DDX_Text(pDX, IDC_EDIT_MIN, this->m_gridKeyword.m_grid[this->m_nIndex].coord[0]);

			// Maximum
			DDX_Text(pDX, IDC_EDIT_MAX, this->m_gridKeyword.m_grid[this->m_nIndex].coord[1]);

			// Number of nodes
			DDX_Text(pDX, IDC_EDIT_U_NODES, this->m_gridKeyword.m_grid[this->m_nIndex].count_coord);
		}
		else
		{
			this->m_gridKeyword.m_grid[this->m_nIndex].uniform = FALSE;

			int size;
			DDX_Text(pDX, IDC_EDIT_N_NODES, size);
			this->m_gridKeyword.m_grid[this->m_nIndex].Resize(size);

			for (int row = 1; row < this->m_wndNonuniformGrid.GetRowCount(); ++row)
			{
				DDX_TextGridControl(pDX, IDC_GRID_NONUNIFORM, row, 1, this->m_gridKeyword.m_grid[this->m_nIndex].At(row - 1));
				if (row > 1)
				{
					if (this->m_gridKeyword.m_grid[this->m_nIndex].At(row - 2) >= this->m_gridKeyword.m_grid[this->m_nIndex].At(row - 1))
					{
						CString str("Coordinate values must be in ascending order.");
						::DDX_GridControlFail(pDX, IDC_GRID_NONUNIFORM, row, 1, str);
					}
				}
			}
		}
	}
	else
	{
		// Uniform / Nonuniform radio
		//
		if (this->m_gridKeyword.m_grid[this->m_nIndex].uniform)
		{
			// Uniform
			//
			this->CheckRadioButton(IDC_RADIO_UNIFORM, IDC_RADIO_NONUNIFORM, IDC_RADIO_UNIFORM);

			// Minimum
			DDX_Text(pDX, IDC_EDIT_MIN, this->m_gridKeyword.m_grid[this->m_nIndex].coord[0]);

			// Maximum
			DDX_Text(pDX, IDC_EDIT_MAX, this->m_gridKeyword.m_grid[this->m_nIndex].coord[1]);

			// Number of nodes
			DDX_Text(pDX, IDC_EDIT_U_NODES, this->m_gridKeyword.m_grid[this->m_nIndex].count_coord);
			DDX_Text(pDX, IDC_EDIT_N_NODES, this->m_gridKeyword.m_grid[this->m_nIndex].count_coord);


			// Nonuniform grid
			//
			CGrid expandedGrid = this->m_gridKeyword.m_grid[this->m_nIndex];
			expandedGrid.Setup();
			ASSERT(expandedGrid.uniform_expanded);

			CString str;
			this->m_wndNonuniformGrid.SetRowCount(this->m_gridKeyword.m_grid[this->m_nIndex].count_coord + 1);
			for (int row = 0; row < this->m_wndNonuniformGrid.GetRowCount(); ++row)
			{
				for (int col = 0; col < this->m_wndNonuniformGrid.GetColumnCount(); ++col)
				{ 
					// set default format
					VERIFY(this->m_wndNonuniformGrid.SetItemFormat(row, col, DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS));
				}
				if (row)
				{
					str.Format(_T("%d"), row);
					VERIFY(this->m_wndNonuniformGrid.SetItemText(row, 0, str));
					VERIFY(this->m_wndNonuniformGrid.SetItemFormat(row, 0, DT_CENTER|DT_BOTTOM|DT_END_ELLIPSIS));
					str.Format(_T("%g"), expandedGrid.coord[row - 1]);
					VERIFY(this->m_wndNonuniformGrid.SetItemText(row, 1, str));
					VERIFY(this->m_wndNonuniformGrid.SetItemFormat(row, 1, DT_RIGHT|DT_BOTTOM|DT_END_ELLIPSIS));
				}
				else
				{
					str.Format(_T("Coordinate %s"), (this->m_nIndex == 2) ? this->m_strVerticalUnits : this->m_strHorizontalUnits);
					VERIFY(this->m_wndNonuniformGrid.SetItemText(0, 1, str));
					VERIFY(this->m_wndNonuniformGrid.SetItemFormat(0, 1, DT_CENTER|DT_BOTTOM|DT_END_ELLIPSIS));
				}
			}

			// enable / disable
			//
			this->EnableUniform(TRUE);
			this->EnableNonuniform(FALSE);
		}
		else
		{
			// Nonuniform
			//
			this->CheckRadioButton(IDC_RADIO_UNIFORM, IDC_RADIO_NONUNIFORM, IDC_RADIO_NONUNIFORM);

			// Minimum
			DDX_Text(pDX, IDC_EDIT_MIN, this->m_gridKeyword.m_grid[this->m_nIndex].coord[0]);

			// Maximum
			DDX_Text(pDX, IDC_EDIT_MAX, this->m_gridKeyword.m_grid[this->m_nIndex].coord[this->m_gridKeyword.m_grid[this->m_nIndex].count_coord - 1]);

			// Number of nodes
			DDX_Text(pDX, IDC_EDIT_U_NODES, this->m_gridKeyword.m_grid[this->m_nIndex].count_coord);
			DDX_Text(pDX, IDC_EDIT_N_NODES, this->m_gridKeyword.m_grid[this->m_nIndex].count_coord);

			// Nonuniform grid
			//
			CString str;
			this->m_wndNonuniformGrid.SetRowCount(this->m_gridKeyword.m_grid[this->m_nIndex].count_coord + 1);
			for (int row = 0; row < this->m_wndNonuniformGrid.GetRowCount(); ++row)
			{
				for (int col = 0; col < this->m_wndNonuniformGrid.GetColumnCount(); ++col)
				{ 
					// set default format
					VERIFY(this->m_wndNonuniformGrid.SetItemFormat(row, col, DT_LEFT|DT_BOTTOM|DT_END_ELLIPSIS));
				}
				if (row)
				{
					str.Format(_T("%d"), row);
					VERIFY(this->m_wndNonuniformGrid.SetItemText(row, 0, str));
					VERIFY(this->m_wndNonuniformGrid.SetItemFormat(row, 0, DT_CENTER|DT_BOTTOM|DT_END_ELLIPSIS));
					str.Format(_T("%g"), this->m_gridKeyword.m_grid[this->m_nIndex].coord[row - 1]);
					VERIFY(this->m_wndNonuniformGrid.SetItemText(row, 1, str));
					VERIFY(this->m_wndNonuniformGrid.SetItemFormat(row, 1, DT_RIGHT|DT_BOTTOM|DT_END_ELLIPSIS));
				}
				else
				{
					str.Format(_T("Coordinate %s"), (this->m_nIndex == 2) ? this->m_strVerticalUnits : this->m_strHorizontalUnits);
					VERIFY(this->m_wndNonuniformGrid.SetItemText(0, 1, str));
					VERIFY(this->m_wndNonuniformGrid.SetItemFormat(0, 1, DT_CENTER|DT_BOTTOM|DT_END_ELLIPSIS));
				}
			}

			// enable / disable
			//
			this->EnableNonuniform(TRUE);
			this->EnableUniform(FALSE);
		}
	}

	// snap
	//
	DDX_Text(pDX, IDC_EDIT_SNAP, this->m_gridKeyword.m_snap[this->m_nIndex]);

	// print_orientation
	//
	if (pDX->m_bSaveAndValidate)
	{
		this->m_gridKeyword.m_print_input_xy = true;
		if (this->GetCheckedRadioButton(IDC_RADIO_XY, IDC_RADIO_XZ) == IDC_RADIO_XZ)
		{
			this->m_gridKeyword.m_print_input_xy = false;
		}
	}
	else
	{
		if (this->m_gridKeyword.m_print_input_xy)
		{
			this->CheckRadioButton(IDC_RADIO_XY, IDC_RADIO_XZ, IDC_RADIO_XY);
		}
		else
		{
			this->CheckRadioButton(IDC_RADIO_XY, IDC_RADIO_XZ, IDC_RADIO_XZ);			 
		}
	}

	// chemistry_dimensions
	//
	DDX_Check(pDX, IDC_CHECK_X, this->m_gridKeyword.m_axes[0]);
	DDX_Check(pDX, IDC_CHECK_Y, this->m_gridKeyword.m_axes[1]);
	DDX_Check(pDX, IDC_CHECK_Z, this->m_gridKeyword.m_axes[2]);
	if (pDX->m_bSaveAndValidate)
	{
		if (this->m_gridKeyword.m_axes[0] == 0 && this->m_gridKeyword.m_axes[1] == 0 && this->m_gridKeyword.m_axes[2] == 0)
		{
			DDX_Check(pDX, IDC_CHECK_X, this->m_gridKeyword.m_axes[0]);
			::AfxMessageBox("At least one chemistry dimension must be selected.");
			pDX->Fail();
		}
	}
}


BEGIN_MESSAGE_MAP(CGridPropertyPage2, CPropertyPage)
	ON_BN_CLICKED(IDC_RADIO_UNIFORM, OnBnClickedRadioUniform)
	ON_BN_CLICKED(IDC_RADIO_NONUNIFORM, OnBnClickedRadioNonuniform)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB_COOR, OnTcnSelchangingTabCoor)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_COOR, OnTcnSelchangeTabCoor)
	ON_EN_CHANGE(IDC_EDIT_MIN, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_MAX, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_U_NODES, OnEnChange)
	ON_EN_CHANGE(IDC_EDIT_N_NODES, OnEnChangeEditNNodes)
	ON_BN_CLICKED(IDC_BUTTON_INS, OnBnClickedButtonIns)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_SERIES, OnBnClickedButtonSeries)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_NONUNIFORM, OnEndLabelEditGrid)
END_MESSAGE_MAP()


// CGridPropertyPage2 message handlers

BOOL CGridPropertyPage2::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// Add extra initialization here
	this->m_spinUnified.SetRange32(2, INT_MAX);
	this->m_spinNonunified.SetRange32(2, INT_MAX);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CGridPropertyPage2::SetUnits(const CUnits& units)
{
	this->m_strHorizontalUnits.Format("(%s)", units.horizontal.defined ? units.horizontal.input : units.horizontal.si);
	CGlobal::MinimizeLengthUnits(this->m_strHorizontalUnits);

	this->m_strVerticalUnits.Format("(%s)", units.vertical.defined ? units.vertical.input : units.vertical.si);
	CGlobal::MinimizeLengthUnits(this->m_strVerticalUnits);
}


void CGridPropertyPage2::OnBnClickedRadioUniform()
{
// COMMENT: {7/21/2005 8:52:36 PM}	// Uniform
// COMMENT: {7/21/2005 8:52:36 PM}	//
// COMMENT: {7/21/2005 8:52:36 PM}	int arrUniform[] =
// COMMENT: {7/21/2005 8:52:36 PM}	{ 
// COMMENT: {7/21/2005 8:52:36 PM}		IDC_STATIC_MIN,
// COMMENT: {7/21/2005 8:52:36 PM}		IDC_EDIT_MIN,
// COMMENT: {7/21/2005 8:52:36 PM}		IDC_STATIC_MAX,
// COMMENT: {7/21/2005 8:52:36 PM}		IDC_EDIT_MAX,
// COMMENT: {7/21/2005 8:52:36 PM}		IDC_STATIC_U_NODES,
// COMMENT: {7/21/2005 8:52:36 PM}		IDC_EDIT_U_NODES,
// COMMENT: {7/21/2005 8:52:36 PM}		IDC_SPIN_U_NODES,
// COMMENT: {7/21/2005 8:52:36 PM}		IDC_STATIC_MIN_UNITS,
// COMMENT: {7/21/2005 8:52:36 PM}		IDC_STATIC_MAX_UNITS,
// COMMENT: {7/21/2005 8:52:36 PM}	};
// COMMENT: {7/21/2005 8:52:36 PM}	for (int i = 0; i < sizeof(arrUniform)/sizeof(arrUniform[0]); ++i)
// COMMENT: {7/21/2005 8:52:36 PM}	{
// COMMENT: {7/21/2005 8:52:36 PM}		if (CWnd* pWnd = this->GetDlgItem(arrUniform[i]))
// COMMENT: {7/21/2005 8:52:36 PM}		{
// COMMENT: {7/21/2005 8:52:36 PM}			pWnd->EnableWindow(TRUE);
// COMMENT: {7/21/2005 8:52:36 PM}		}
// COMMENT: {7/21/2005 8:52:36 PM}	}
// COMMENT: {7/21/2005 8:52:36 PM}
// COMMENT: {7/21/2005 8:52:36 PM}	//
// COMMENT: {7/21/2005 8:52:36 PM}	// Nonuniform
// COMMENT: {7/21/2005 8:52:36 PM}	int arrNonuniform[] =
// COMMENT: {7/21/2005 8:52:36 PM}	{ 
// COMMENT: {7/21/2005 8:52:36 PM}		///IDC_GRID_NONUNIFORM,
// COMMENT: {7/21/2005 8:52:36 PM}		IDC_EDIT_N_NODES,
// COMMENT: {7/21/2005 8:52:36 PM}		IDC_STATIC_N_NODES,
// COMMENT: {7/21/2005 8:52:36 PM}		IDC_SPIN_N_NODES,
// COMMENT: {7/21/2005 8:52:36 PM}		IDC_BUTTON_INS,
// COMMENT: {7/21/2005 8:52:36 PM}		IDC_BUTTON_DEL,
// COMMENT: {7/21/2005 8:52:36 PM}		IDC_BUTTON_SERIES,
// COMMENT: {7/21/2005 8:52:36 PM}	};
// COMMENT: {7/21/2005 8:52:36 PM}	for (int i = 0; i < sizeof(arrNonuniform)/sizeof(arrNonuniform[0]); ++i)
// COMMENT: {7/21/2005 8:52:36 PM}	{
// COMMENT: {7/21/2005 8:52:36 PM}		if (CWnd* pWnd = this->GetDlgItem(arrNonuniform[i]))
// COMMENT: {7/21/2005 8:52:36 PM}		{
// COMMENT: {7/21/2005 8:52:36 PM}			pWnd->EnableWindow(FALSE);
// COMMENT: {7/21/2005 8:52:36 PM}		}
// COMMENT: {7/21/2005 8:52:36 PM}	}
// COMMENT: {7/21/2005 8:52:36 PM}	for (int row = 0; row < this->m_wndNonuniformGrid.GetRowCount(); ++row)
// COMMENT: {7/21/2005 8:52:36 PM}	{
// COMMENT: {7/21/2005 8:52:36 PM}		for (int col = 0; col < this->m_wndNonuniformGrid.GetColumnCount(); ++col)
// COMMENT: {7/21/2005 8:52:36 PM}		{ 
// COMMENT: {7/21/2005 8:52:36 PM}			VERIFY(this->m_wndNonuniformGrid.DisableCell(row, col));
// COMMENT: {7/21/2005 8:52:36 PM}		}
// COMMENT: {7/21/2005 8:52:36 PM}	}
// COMMENT: {7/21/2005 8:52:36 PM}	this->m_wndNonuniformGrid.SetFixedTextColor(::GetSysColor(COLOR_GRAYTEXT));
// COMMENT: {7/21/2005 8:52:36 PM}	this->m_wndNonuniformGrid.RedrawWindow();
	this->EnableUniform(TRUE);
	this->EnableNonuniform(FALSE);

	this->SetModified(TRUE);
}

void CGridPropertyPage2::EnableUniform(BOOL bEnable)
{
	// Uniform
	//
	int arrUniform[] =
	{ 
		IDC_STATIC_MIN,
		IDC_EDIT_MIN,
		IDC_STATIC_MAX,
		IDC_EDIT_MAX,
		IDC_STATIC_U_NODES,
		IDC_EDIT_U_NODES,
		IDC_SPIN_U_NODES,
		IDC_STATIC_MIN_UNITS,
		IDC_STATIC_MAX_UNITS,
	};
	for (int i = 0; i < sizeof(arrUniform)/sizeof(arrUniform[0]); ++i)
	{
		if (CWnd* pWnd = this->GetDlgItem(arrUniform[i]))
		{
			pWnd->EnableWindow(bEnable);
		}
	}
}

void CGridPropertyPage2::EnableNonuniform(BOOL bEnable)
{
	//
	// Nonuniform
	int arrNonuniform[] =
	{ 
		IDC_GRID_NONUNIFORM,
		IDC_EDIT_N_NODES,
		IDC_STATIC_N_NODES,
		IDC_SPIN_N_NODES,
		IDC_BUTTON_INS,
		IDC_BUTTON_DEL,
		IDC_BUTTON_SERIES,
	};
	for (int i = 0; i < sizeof(arrNonuniform)/sizeof(arrNonuniform[0]); ++i)
	{
		if (CWnd* pWnd = this->GetDlgItem(arrNonuniform[i]))
		{
			pWnd->EnableWindow(bEnable);
		}
	}
	if (bEnable)
	{
		for (int row = 0; row < this->m_wndNonuniformGrid.GetRowCount(); ++row)
		{
			for (int col = 0; col < this->m_wndNonuniformGrid.GetColumnCount(); ++col)
			{ 
				VERIFY(this->m_wndNonuniformGrid.EnableCell(row, col));
			}
		}
		this->m_wndNonuniformGrid.SetFixedTextColor(CLR_DEFAULT);
	}
	else
	{
		for (int row = 0; row < this->m_wndNonuniformGrid.GetRowCount(); ++row)
		{
			for (int col = 0; col < this->m_wndNonuniformGrid.GetColumnCount(); ++col)
			{ 
				VERIFY(this->m_wndNonuniformGrid.DisableCell(row, col));
			}
		}
		this->m_wndNonuniformGrid.SetFixedTextColor(::GetSysColor(COLOR_GRAYTEXT));
	}
	this->m_wndNonuniformGrid.RedrawWindow();
}

void CGridPropertyPage2::OnBnClickedRadioNonuniform()
{
	this->EnableNonuniform(TRUE);
	this->EnableUniform(FALSE);

// COMMENT: {7/21/2005 8:43:48 PM}	// Uniform
// COMMENT: {7/21/2005 8:43:48 PM}	//
// COMMENT: {7/21/2005 8:43:48 PM}	int arrUniform[] =
// COMMENT: {7/21/2005 8:43:48 PM}	{ 
// COMMENT: {7/21/2005 8:43:48 PM}		IDC_STATIC_MIN,
// COMMENT: {7/21/2005 8:43:48 PM}		IDC_EDIT_MIN,
// COMMENT: {7/21/2005 8:43:48 PM}		IDC_STATIC_MAX,
// COMMENT: {7/21/2005 8:43:48 PM}		IDC_EDIT_MAX,
// COMMENT: {7/21/2005 8:43:48 PM}		IDC_STATIC_U_NODES,
// COMMENT: {7/21/2005 8:43:48 PM}		IDC_EDIT_U_NODES,
// COMMENT: {7/21/2005 8:43:48 PM}		IDC_SPIN_U_NODES,
// COMMENT: {7/21/2005 8:43:48 PM}		IDC_STATIC_MIN_UNITS,
// COMMENT: {7/21/2005 8:43:48 PM}		IDC_STATIC_MAX_UNITS,
// COMMENT: {7/21/2005 8:43:48 PM}	};
// COMMENT: {7/21/2005 8:43:48 PM}	for (int i = 0; i < sizeof(arrUniform)/sizeof(arrUniform[0]); ++i)
// COMMENT: {7/21/2005 8:43:48 PM}	{
// COMMENT: {7/21/2005 8:43:48 PM}		if (CWnd* pWnd = this->GetDlgItem(arrUniform[i]))
// COMMENT: {7/21/2005 8:43:48 PM}		{
// COMMENT: {7/21/2005 8:43:48 PM}			pWnd->EnableWindow(FALSE);
// COMMENT: {7/21/2005 8:43:48 PM}		}
// COMMENT: {7/21/2005 8:43:48 PM}	}

	this->SetModified(TRUE);
}

void CGridPropertyPage2::OnTcnSelchangingTabCoor(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (!this->UpdateData(TRUE))
	{
		// notify which control caused failure
		//
		CWnd* pFocus = CWnd::GetFocus();
		::PostMessage(this->GetSafeHwnd(), UM_DDX_FAILURE, (WPARAM)pFocus, (LPARAM)0);

		// disallow change
		//
		*pResult = TRUE;
		return;
	}
	*pResult = 0;
}

void CGridPropertyPage2::OnTcnSelchangeTabCoor(NMHDR *pNMHDR, LRESULT *pResult)
{
	this->m_nIndex = this->m_tabCtrl.GetCurSel();
	this->UpdateData(FALSE);
	*pResult = 0;
}

void CGridPropertyPage2::SetCurSel(int index)
{
	this->m_nIndex = index;
}

int CGridPropertyPage2::GetCurSel(void)const
{
	return this->m_nIndex;
}

void CGridPropertyPage2::SetProperties(const CGridKeyword& gridKeyword)
{
	this->m_gridKeyword = gridKeyword;
}

void CGridPropertyPage2::GetProperties(CGridKeyword& gridKeyword)const
{
	gridKeyword = this->m_gridKeyword;
}

BOOL CGridPropertyPage2::OnApply()
{
	ASSERT(this->m_pDoc);
	ASSERT(this->m_pActor);
	CWaitCursor wait;
	if (this->m_bNeedAction && this->UpdateData(TRUE))
	{
		if (this->m_pAction == 0)
		{
			this->m_pAction = new CGridSetAction(this->m_pDoc, this->m_pActor, this->m_gridKeyword);
			if (this->m_pAction == 0) ::AfxMessageBox("Out of Memory");
			if (CModelessPropertySheet *pSheet = reinterpret_cast<CModelessPropertySheet*>(this->GetParent()))
			{
				if (this->m_pAction)
				{
					this->m_pAction->Apply(this->m_gridKeyword);
					pSheet->AddAction(this->m_pAction);
				}
			}
		}
		else
		{
			this->m_pAction->Apply(this->m_gridKeyword);
		}
		this->m_bNeedAction = false;
	}
	return CPropertyPage::OnApply();
}

void CGridPropertyPage2::OnCancel()
{
	if (this->m_pAction)
	{
		this->m_pAction->UnExecute();
	}
	CPropertyPage::OnCancel();
}

void CGridPropertyPage2::PostNcDestroy()
{
	this->m_pAction = 0;
	CPropertyPage::PostNcDestroy();
}

void CGridPropertyPage2::OnEnChange()
{
	this->SetModified(TRUE);
	this->m_bNeedAction = TRUE;
}

void CGridPropertyPage2::OnEnChangeEditNNodes()
{
	if (this->m_wndNonuniformGrid.GetSafeHwnd())
	{
		BOOL bTranslated;
		UINT count_coord =(int)this->GetDlgItemInt(IDC_EDIT_N_NODES, &bTranslated, FALSE);
		if (bTranslated)
		{
			int prev_rows = this->m_wndNonuniformGrid.GetRowCount();
			VERIFY(this->m_wndNonuniformGrid.SetRowCount(count_coord + 1));
			CString str;
			for (int row = prev_rows; row < this->m_wndNonuniformGrid.GetRowCount(); ++row)
			{
				str.Format(_T("%d"), row);
				VERIFY(this->m_wndNonuniformGrid.SetItemText(row, 0, str));
				VERIFY(this->m_wndNonuniformGrid.SetItemFormat(row, 0, DT_CENTER|DT_BOTTOM|DT_END_ELLIPSIS));
				VERIFY(this->m_wndNonuniformGrid.SetItemFormat(row, 1, DT_RIGHT|DT_BOTTOM|DT_END_ELLIPSIS));
			}
		}
	}
	this->SetModified(TRUE);
	this->m_bNeedAction = TRUE;
}

void CGridPropertyPage2::OnBnClickedButtonIns()
{
	CCellRange range = this->m_wndNonuniformGrid.GetSelectedCellRange();
	TRACE("range min row = %d\n", range.GetMinRow());
	TRACE("range max row = %d\n", range.GetMaxRow());
	if (!range.IsValid()) return;

	ASSERT(range.GetMinRow() <= range.GetMaxRow());
	ASSERT(0 < range.GetMinRow() && range.GetMinRow() <= this->m_wndNonuniformGrid.GetRowCount());
	ASSERT(0 < range.GetMaxRow() && range.GetMaxRow() <= this->m_wndNonuniformGrid.GetRowCount());

	CString str;
	CCellID cell = this->m_wndNonuniformGrid.GetFocusCell();
	for (int i = 0; i < range.GetRowSpan(); ++i)
	{
		str.Format(_T("%d"), range.GetMinRow() + i);
		this->m_wndNonuniformGrid.InsertRow(str, range.GetMinRow() + i);
		VERIFY(this->m_wndNonuniformGrid.SetItemFormat(range.GetMinRow() + i, 0, DT_CENTER|DT_BOTTOM|DT_END_ELLIPSIS));
		VERIFY(this->m_wndNonuniformGrid.SetItemFormat(range.GetMinRow() + i, 1, DT_RIGHT|DT_BOTTOM|DT_END_ELLIPSIS));
	}
	for (int row = range.GetMaxRow() + 1; row < this->m_wndNonuniformGrid.GetRowCount(); ++row)
	{
		if (row)
		{
			str.Format(_T("%d"), row);
			VERIFY(this->m_wndNonuniformGrid.SetItemText(row, 0, str));
		}
	}
	this->m_wndNonuniformGrid.SetCurrentFocusCell(cell.row, cell.col);
	this->m_wndNonuniformGrid.SetSelectedRange(range);
	this->SetDlgItemInt(IDC_EDIT_N_NODES, (UINT)this->m_wndNonuniformGrid.GetRowCount() - 1, FALSE);
	this->m_wndNonuniformGrid.RedrawWindow();
}

void CGridPropertyPage2::OnBnClickedButtonDel()
{
	CCellRange range = this->m_wndNonuniformGrid.GetSelectedCellRange();
	TRACE("range min row = %d\n", range.GetMinRow());
	TRACE("range max row = %d\n", range.GetMaxRow());
	if (!range.IsValid()) return;

	ASSERT(range.GetMinRow() <= range.GetMaxRow());
	ASSERT(0 < range.GetMinRow() && range.GetMinRow() <= this->m_wndNonuniformGrid.GetRowCount());
	ASSERT(0 < range.GetMaxRow() && range.GetMaxRow() <= this->m_wndNonuniformGrid.GetRowCount());

	CString str;
	CCellID cell = this->m_wndNonuniformGrid.GetFocusCell();
	for (int i = 0; i < range.GetRowSpan(); ++i)
	{
		this->m_wndNonuniformGrid.DeleteRow(range.GetMinRow());
	}
	for (int row = range.GetMinRow(); row < this->m_wndNonuniformGrid.GetRowCount(); ++row)
	{
		if (row)
		{
			str.Format(_T("%d"), row);
			VERIFY(this->m_wndNonuniformGrid.SetItemText(row, 0, str));
		}
	}
	this->m_wndNonuniformGrid.SetSelectedRange(-1, -1, -1, -1);
	this->SetDlgItemInt(IDC_EDIT_N_NODES, (UINT)this->m_wndNonuniformGrid.GetRowCount() - 1, FALSE);
	this->m_wndNonuniformGrid.RedrawWindow();
}

void CGridPropertyPage2::OnBnClickedButtonSeries()
{
	CCellRange range = this->m_wndNonuniformGrid.GetSelectedCellRange();
	CCellID cell = this->m_wndNonuniformGrid.GetFocusCell();

	CString str;
	str = this->m_wndNonuniformGrid.GetItemText(range.GetMinRow(), 1);
	double start = 1.0;
	if (!str.IsEmpty())
	{
		if (_stscanf(str, _T("%lf"), &start) != 1)
		{
			start = 1.0;
		}
	}

	CSeriesDialog dlg;
	dlg.m_dStartValue = start;
	if (dlg.DoModal() == IDOK)
	{
		if (dlg.m_nType == CSeriesDialog::SD_LINEAR)
		{
			double value;
			for (int row = range.GetMinRow(), i = 0; row <= range.GetMaxRow(); ++row, ++i)
			{
				if (row > 0)
				{
					value = dlg.m_dStartValue + i * dlg.m_dStepValue;
					if ( !dlg.m_bStopValueDefined || (value <= dlg.m_dStopValue) )
					{
						str.Format(_T("%g"), value);
						VERIFY(this->m_wndNonuniformGrid.SetItemText(row, 1, str));
					}
				}
			}

		}
		else if (dlg.m_nType == CSeriesDialog::SD_GROWTH)
		{
			double value;
			for (int row = range.GetMinRow(), i = 0; row <= range.GetMaxRow(); ++row, ++i)
			{
				if (row > 0)
				{
					value = dlg.m_dStartValue * ::pow(dlg.m_dStepValue, i);
					if ( !dlg.m_bStopValueDefined || (value <= dlg.m_dStopValue) )
					{
						// str.Format(_T("%g"), value);						
						str.Format(_T("%.*g"), DBL_DIG, value);
						VERIFY(this->m_wndNonuniformGrid.SetItemText(row, 1, str));
					}
				}
			}
		}
		else
		{
			ASSERT(FALSE);
		}

		this->m_wndNonuniformGrid.SetCurrentFocusCell(cell.row, cell.col);
		this->m_wndNonuniformGrid.SetSelectedRange(range);
		this->m_wndNonuniformGrid.RedrawWindow();
	}	
}


void CGridPropertyPage2::OnEndLabelEditGrid(NMHDR *pNotifyStruct, LRESULT *result)
{
	NM_GRIDVIEW *pnmgv = (NM_GRIDVIEW*)pNotifyStruct;
	TRACE("OnEndLabelEditGrid row=%d col=%d\n", pnmgv->iRow, pnmgv->iColumn);
	this->SetModified(TRUE);
}
