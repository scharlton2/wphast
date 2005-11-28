#include "StdAfx.h"
#include "resource.h"
#include "ICZoneActor.h"
#include "PropertyTreeControlBar.h"
#include "ICHeadPropertyPage.h"
#include "ChemICPropertyPage.h"
#include "ChemICSpreadPropertyPage.h"
#include "ICHeadSpreadPropertyPage.h"

#include "property.h"
#include "Global.h"

#include "WPhastDoc.h"
#include "SetHeadICAction.h"
#include "SetChemICAction.h"
#include "ZoneCreateAction.h"
#include "FlowOnly.h"

#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro
#include <vtkPropAssembly.h>

vtkCxxRevisionMacro(CICZoneActor, "$Revision$");

CICZoneActor::CICZoneActor(void)
{
	this->m_pZone = new CZone();
}

CICZoneActor::~CICZoneActor(void)
{
	delete this->m_pZone;
}

void CICZoneActor::InsertAt(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	CString str;
	str.Format("%s", this->GetName());

	this->m_hti = pTreeCtrl->InsertItem(str, hParent, hInsertAfter);
	pTreeCtrl->SetItemData(this->m_hti, (DWORD_PTR)this);
	pTreeCtrl->SelectItem(this->m_hti); // might want to move this
	this->Update(pTreeCtrl, this->m_hti);
}

HTREEITEM CICZoneActor::GetHTreeItem(void)const
{
	return this->m_hti;
}

