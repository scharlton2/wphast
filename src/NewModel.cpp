#include "StdAfx.h"
#include "NewModel.h"

CNewModel::CNewModel(void)
: m_bHaveSiteMap2(false)
// COMMENT: {7/18/2005 6:39:21 PM}, m_print_input_xy(true)
{
// COMMENT: {7/18/2005 6:39:19 PM}	for(int i = 0; i < 3; ++i)
// COMMENT: {7/18/2005 6:39:19 PM}	{	
// COMMENT: {7/18/2005 6:39:19 PM}		this->m_axes[i] = TRUE;
// COMMENT: {7/18/2005 6:39:19 PM}	}
}

CNewModel::~CNewModel(void)
{
}

CNewModel CNewModel::Default(void)
{
	CNewModel model;
	model.m_media     = CGridElt::NewDefaults();
	model.m_headIC    = CHeadIC::NewDefaults();
	model.m_chemIC    = CChemIC::NewDefaults();
	model.m_printFreq = CPrintFreq::NewDefaults();

// COMMENT: {7/18/2005 6:39:30 PM}	for(int i = 0; i < 3; ++i)
// COMMENT: {7/18/2005 6:39:30 PM}	{	
// COMMENT: {7/18/2005 6:39:30 PM}		model.m_grid[i].count_coord = 2;
// COMMENT: {7/18/2005 6:39:30 PM}		model.m_grid[i].uniform     = TRUE;
// COMMENT: {7/18/2005 6:39:30 PM}		model.m_grid[i].coord[0]    = 0;
// COMMENT: {7/18/2005 6:39:30 PM}		model.m_grid[i].coord[1]    = 1;
// COMMENT: {7/18/2005 6:39:30 PM}	}
// COMMENT: {7/18/2005 6:39:30 PM}	model.m_grid[0].c = 'x';
// COMMENT: {7/18/2005 6:39:30 PM}	model.m_grid[1].c = 'y';
// COMMENT: {7/18/2005 6:39:30 PM}	model.m_grid[2].c = 'z';

	model.m_timeControl2 = CTimeControl2::NewDefaults();

	return model;
}

bool CNewModel::HasSiteMap2(void)const
{
	return this->m_bHaveSiteMap2;
}

void CNewModel::SetSiteMap2(const CSiteMap2 &siteMap2)
{
	this->m_siteMap2 = siteMap2;
	this->m_bHaveSiteMap2 = true;
}

CSiteMap2 CNewModel::GetSiteMap2(void)const
{
	return this->m_siteMap2;
}
