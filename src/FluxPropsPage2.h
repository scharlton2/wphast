#pragma once

#include "BC.h"
#include "gridctrl/ModGridCtrlEx.h"
#include "afxwin.h"

#include "GridTimeSeries.h"

class CUnits;

// CFluxPropsPage2 dialog

class CFluxPropsPage2 : public CPropertyPage
{
	DECLARE_DYNAMIC(CFluxPropsPage2)

public:
	CFluxPropsPage2();
	virtual ~CFluxPropsPage2();

	void SetProperties(const CBC& rBC);
	void GetProperties(CBC& rBC)const;

	void SetFlowOnly(bool flowOnly)      { this->FlowOnly = flowOnly; }
	bool GetFlowOnly(void)const          { return this->FlowOnly; }

	void SetDefault(bool bDefault)       { this->Default = Default; }
	bool GetDefault(void)const           { return Default; }

	void SetDesc(LPCTSTR desc)           { this->Description = desc; }
	LPCTSTR GetDesc()                    { return this->Description; }

	void SetUnits(const CUnits &u);

// Dialog Data
	enum { IDD = IDD_PROPS_FLUX_2 };

	// type enum
	enum ModeType
	{
		NONE     = 0, // also represents chosen property
		CONSTANT = 1,
		LINEAR   = 2,
		POINTS   = 3,
		XYZ      = 4,
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

 	afx_msg void OnTreeSelChanging(NMHDR *pNotifyStruct, LRESULT *plResult);
 	afx_msg void OnTreeSelChanged(NMHDR *pNotifyStruct, LRESULT *plResult);

	afx_msg void OnEndLabelEditFlux(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnEndLabelEditSolution(NMHDR *pNotifyStruct, LRESULT *result);

	afx_msg void OnSelChangedFlux(NMHDR *pNotifyStruct, LRESULT *result);
	afx_msg void OnSelChangedSolution(NMHDR *pNotifyStruct, LRESULT *result);

	afx_msg void OnBnClickedButtonXYZ();

	afx_msg LRESULT OnUM_DDXFailure(WPARAM wParam, LPARAM lParam);

	afx_msg void OnEnSetfocusDescEdit();

	CGridTimeSeries FluxSeries;
	CGridTimeSeries SolutionSeries;

	CModGridCtrlEx PointsGrid;

	CRichEditCtrl  RichEditCtrl;
	CTreeCtrl      TreeCtrl;

	HTREEITEM ItemDDX;

	std::string m_sDescriptionRTF;   // IDR_DESCRIPTION_RTF
	std::string m_sAssocSolutionRTF; // IDR_BC_FLUX_ASSOC_SOL_RTF
	std::string m_sFluxRTF;          // IDR_BC_FLUX_FLUX_RTF
	std::string m_sFaceRTF;          // IDR_BC_FLUX_FACE_RTF

	CString Description;
// COMMENT: {6/30/2009 11:16:25 PM}	CString DefaultUnits;

	bool FlowOnly;
	bool Default;

protected:
	CBC BC;
};
