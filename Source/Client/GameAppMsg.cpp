#include "Stdafx.h"
#include "Character.h"
#include "SceneObj.h"
#include "SceneObjFile.h"
#ifdef __EDITOR__
#include "MPEditor.h"
#endif
#include "GameApp.h"
#include "GameConfig.h"
#include "UIFormMgr.h"
#include "GameAppMsg.h"
#include "GlobalVar.h"
#include "PacketCmd.h"
#include "Lua_Platform.h"
#include "DrawPointList.h"
#include "UIHeadSay.h"
#include "UIEditor.h"
#include "teaminviteformmgr.h"
#include "InputBox.h"
#include "uistartform.h"
#include "ui3dcompent.h"
#include "UIsystemform.h"
#include "EffectObj.h"
#include "chastate.h"
#include "cameractrl.h"
#include "Smallmap.h"
#include "UITeam.h"
#include "UICommand.h"
#include "uiitemcommand.h"
#include "ItemRefineEffectSet.h"
#include "SceneObjSet.h"
#include "effectset.h"
#include "stattack.h"
#include "CameraCtrl.h"

#ifdef _KOSDEMO_
#include "uiequipform.h"
#endif
#include "stmove.h"

#include "uiwebbrowser.h"

// obj文件里摆放的物件类型定义  16位物件类型字段的最后2位
#define OBJ_MASK_SCENEOBJ 0		// 场景物件
#define OBJ_MASK_EFFECT	  16384 //  = 01 000000 000000 00
#define OBJ_MASK_CHA	  32768 //  = 10 000000 000000 00

short GetObjTypeID(short sValue)
{
  return sValue & 16383; // 返回有效的14位
}

short GetMaskType(short sValue)
{
  return sValue & 49152;
}

// 地图读盘回调函数, 此函数得到通知后, 生成地图上的物件
long CALLBACK TerrainNotice(int nFlag, int nSectionX, int nSectionY, unsigned long dwParam, LETerrain* pThis)
{
  long			nSectionObjCnt = 0;
  SSceneObjInfo infoex[MAX_MAP_SECTION_OBJ];

  // 这四个常量应该从.obj或.ifl文件头中获取
  const long clSectionWidth	 = 8;
  const long clSectionHeight = 8;
  const long clTileWidth	 = 2;
  const long clTileHeight	 = 2;
  //
  const long lSectionUnitNum = clSectionWidth * clTileWidth * clSectionHeight * clTileHeight;

  // SUnitBlockInfo	SUnitBlock[lSectionUnitNum] = {0};
  // SUnitHeightInfo	SUnitHeight[lSectionUnitNum] = {0};

  long		  lSectionTileNum = clSectionWidth * clSectionHeight;
  CGameScene* pScene;
  CSceneObj * pObj, *pObjList[MAX_MAP_SECTION_OBJ];
  CEffectObj *pEffObj, *pEffObjList[MAX_MAP_SECTION_OBJ];
  // CCharacter      *ChaList[MAX_MAP_SECTION_OBJ];
  bool	 bObjChange = false;
  _TCHAR tcsPrint[256];
  int	 nSectionNO;
  long	 returnValue = 1;

  if(g_Config.m_bNoObj) return 0;

  nSectionNO = nSectionY * pThis->GetSectionCntX() + nSectionX;

  if(nFlag == 0) // 读入Section数据
  {
	int nSceneObj = 0;

	if(g_ObjFile.ReadSectionObjInfo(nSectionNO, infoex, &nSectionObjCnt))
	{
	  for(int i = 0; i < nSectionObjCnt; i++)
	  {
		pScene = CGameApp::GetCurScene();

		if(infoex[i].GetType() == 0) // 场景物件
		{
		  // 修改特大场景物件的显示剔除bug。//by clp
		  CSceneObjInfo* pInfo = GetSceneObjInfo(infoex[i].GetID());
		  if(pInfo && pInfo->bIsReallyBig)
		  {
			// 特大物件用其他方法管理。
			continue;
		  }

		  nSceneObj++;

		  pObj = pScene->AddSceneObj(infoex[i].GetID());
		  if(pObj)
		  {
			pObj->setHeightOff(infoex[i].sHeightOff);
			pObj->setPos(infoex[i].nX, infoex[i].nY);
			pObj->setYaw(infoex[i].sYawAngle);
		  } else if(g_Config.m_bEnableLGMsg) // 如果物件添加错误，则咨询将其删除
		  {
			/*_stprintf(tcsPrint,
							_TEXT(RES_STRING(CL_LANGUAGE_MATCH_101)),
							infoex[i].GetID());*/
			_snprintf_s(tcsPrint, sizeof(tcsPrint), _TRUNCATE,
						_TEXT(RES_STRING(CL_LANGUAGE_MATCH_101)),
						infoex[i].GetID());
			if(IDYES == MessageBox(NULL, tcsPrint, _TEXT(RES_STRING(CL_LANGUAGE_MATCH_25)), MB_YESNO))
			{
			  for(int j = i; j < nSectionObjCnt - 1; j++)
				infoex[j] = infoex[j + 1];
			  i--;
			  nSectionObjCnt--;

			  bObjChange = true;
			}
		  }
		} else if(infoex[i].GetType() == 1) // 特效物件
		{
		  pEffObj = pScene->AddSceneEffect(infoex[i].GetID());
		  if(pEffObj)
		  {
			pEffObj->SetValid(TRUE);
			pEffObj->setHeightOff(infoex[i].sHeightOff);
			pEffObj->setPos(infoex[i].nX, infoex[i].nY);
			pEffObj->setYaw(infoex[i].sYawAngle);
		  } else if(g_Config.m_bEnableLGMsg) // 如果物件添加错误，则咨询将其删除
		  {
			/*_stprintf(tcsPrint,
							_TEXT("添加 ID 为：%d 的特效物件时发生错误，是否将此物件从.obj文件中删除"),
							infoex[i].GetID());*/
			_snprintf_s(tcsPrint, sizeof(tcsPrint), _TRUNCATE,
						_TEXT("Adf ID %d Effect failed ? Delete the .obj file?"),
						infoex[i].GetID());
			if(IDYES == MessageBox(NULL, tcsPrint, _TEXT("Failed"), MB_YESNO))
			{
			  for(int j = i; j < nSectionObjCnt - 1; j++)
				infoex[j] = infoex[j + 1];
			  i--;
			  nSectionObjCnt--;

			  bObjChange = true;
			}
		  }
		}
	  }
	  if(bObjChange)
		g_ObjFile.WriteSectionObjInfo(nSectionNO, infoex, nSectionObjCnt);

	  //          LG("load", "Total: %d, Scene: %d\n", nSectionObjCnt, nSceneObj);
	}

	LETile* SLETile;
	int		iTileX, iTileY;
	long	lPos[4];
	for(int i = 0; i < clSectionHeight; i++)
	{
	  iTileY = nSectionY * clSectionHeight + i;
	  for(int j = 0; j < clSectionWidth; j++)
	  {
		iTileX	= nSectionX * clSectionWidth + j;
		SLETile = pThis->GetTile(iTileX, iTileY);
		lPos[0] = i * clTileHeight * clSectionWidth * clTileWidth + j * clTileWidth;
		lPos[1] = lPos[0] + 1;
		lPos[2] = lPos[0] + clSectionWidth * clTileWidth;
		lPos[3] = lPos[2] + 1;
	  }
	}
  } else if(nFlag == 1) // 保存并释放Section数据
  {
	pScene = CGameApp::GetCurScene();
	int n  = 0;
	int iCount;
	int nStartX = nSectionX * 8 * 100;
	int nEndX	= (nSectionX + 1) * 8 * 100;
	int nStartY = nSectionY * 8 * 100;
	int nEndY	= (nSectionY + 1) * 8 * 100;
	int nPosX, nPosY;

	// 场景物件
	for(int i = 0; i < pScene->GetSceneObjCnt(); i++)
	{
	  pObj = pScene->GetSceneObj(i);

	  // 修改特大场景物件的显示剔除bug。//by clp
	  if(pScene->IsInRBOList(pObj))
	  {
		// 特大物件用其他方法管理。
		continue;
	  }

	  if(pObj->IsValid())
	  {
		nPosX = pObj->GetCurX();
		nPosY = pObj->GetCurY();
		if(nPosX >= nStartX && nPosX < nEndX &&
		   nPosY >= nStartY && nPosY < nEndY)
		{
		  //pScene->HandleSceneMsg(SCENEMSG_SCENEOBJ_DESTROY, pObj->getEffectID(),pObj->getID());
		  if(pThis->CanEdit())
		  {
			infoex[n].sTypeID	 = short(pObj->getTypeID());
			infoex[n].nX		 = nPosX;
			infoex[n].nY		 = nPosY;
			infoex[n].sYawAngle	 = pObj->getYaw();
			infoex[n].sHeightOff = pObj->getHeightOff();
		  }

		  pObjList[n] = pObj;
		  if(n >= MAX_MAP_SECTION_OBJ) break;
		  n++;
		}
	  }
	}
	iCount = n;

	// 特效物件
	for(int i = 0; i < pScene->GetSceneEffCnt(); i++)
	{
	  pEffObj = pScene->GetEffect(i);
	  if(pEffObj->IsValid())
	  {
		nPosX = pEffObj->GetCurX();
		nPosY = pEffObj->GetCurY();
		if(nPosX >= nStartX && nPosX < nEndX &&
		   nPosY >= nStartY && nPosY < nEndY)
		{
		  if(pEffObj->IsSceneEffect() == TRUE)
		  {
			if(pThis->CanEdit())
			{
			  infoex[n].sTypeID = pEffObj->getIdxID();
			  infoex[n].sTypeID |= 1 << (sizeof(infoex[i].sTypeID) * 8 - 2);
			  infoex[n].nX = nPosX;
			  infoex[n].nY = nPosY;
			  //if(pEffObj->getIdxID() == 1)
			  //	infoex[n].sYawAngle	= pEffObj->getYaw() +  D3DX_PI * 100;
			  //else
			  infoex[n].sYawAngle  = pEffObj->getYaw();
			  infoex[n].sHeightOff = pEffObj->getHeightOff();
			}

			pEffObjList[n] = pEffObj;
			if(n >= MAX_MAP_SECTION_OBJ) break;
			n++;
		  }
		}
	  }
	}

	for(int i = 0; i < iCount; i++)
	  pObjList[i]->SetValid(FALSE);
	for(int i = iCount; i < n; i++)
	  pEffObjList[i]->SetValid(FALSE);

	if(pThis->CanEdit())
	  if(!g_ObjFile.WriteSectionObjInfo(nSectionNO, infoex, n))
		returnValue = 0;
  }

  // 修改特大场景物件的显示剔除bug。//by clp
  std::set<CSceneObj*>::iterator itr = g_pGameApp->GetCurScene()->Begin_RBO();
  std::set<CSceneObj*>::iterator end = g_pGameApp->GetCurScene()->End_RBO();
  while(itr != end)
  {
	if(g_pGameApp->IsEnableSpSmMap())
	{
	  if((*itr)->IsHide())
	  {
		(*itr)->SetHide(FALSE);
	  }
	  ++itr;
	  continue;
	}

	(*itr)->GetObject()->CullPrimitive();
	bool nonVisible = (*itr)->GetObject()->GetCullingPrimitiveNum() == (*itr)->GetObject()->GetPrimitiveNum();
	if(nonVisible)
	{
	  if(!(*itr)->IsHide())
	  {
		(*itr)->SetHide(TRUE);
	  }
	} else
	{
	  if((*itr)->IsHide())
	  {
		(*itr)->SetHide(FALSE);
	  }
	}
	++itr;
  }
  // Added by clp

  return returnValue;
}

void CGameApp::HandleKeyDown(DWORD dwKey)
{
#ifdef _LUA_GAME
  lua_platform_keydown(dwKey);
#endif

  if(!_IsSceneOk()) return;

  if(_pConsole->IsVisible()) return;

  //if(IsCtrlPress()&&IsShiftPress()&&IsKeyDown(DIK_M)/*&& !g_Config.m_bEditor*/)
  //{
  //	if(CGameScene::_pLargerMap)
  //		CGameScene::_pLargerMap->Show( TRUE );
  //}
  // 请注意此处填写的按键处理都必须是游戏里正式使用的
  // 如果为了调试使用或者特殊的选项, 请填写到HandleSuperKey()函数里
  if(IsKeyDown(DIK_F1) && IsCtrlPress()) // 激活|关闭SuperKey模式
  {
#if !defined(_DEBUG) && !defined(__FPS_DEBUG__)
	if(g_Config.IsPower() || (CGameScene::GetMainCha() && CGameScene::GetMainCha()->getGMLv()))
#endif
	{
#ifdef _DEBUG
	  g_nTemp = !g_nTemp;
#endif
	  EnableSuperKey(1 - _bEnableSuperKey);
	  TipI(_bEnableSuperKey, RES_STRING(CL_LANGUAGE_MATCH_102), RES_STRING(CL_LANGUAGE_MATCH_103));
	  // 			GetConsole()->Show( _bEnableSuperKey );
	}
  } else if(IsKeyDown(DIK_F12) && IsShiftPress()) // 激活LogView工具 add by cf
  {
	WinExec("system/Logvwr.exe", SW_SHOWNORMAL);
	EnableSprintSmMap(FALSE);
  } else if(IsKeyDown(DIK_HOME) && _pCurScene->GetMainCha())
  {
	GetMainCam()->SetFollowObj(_pCurScene->GetMainCha()->GetPos());
	ResetCamera();
  } else if(IsKeyDown((BYTE)DIKEYBOARD_SYSRQ))
  {
	g_Render.CaptureScreen();
  } else if(IsKeyDown(DIK_D) && IsCtrlPress())
  {
	if(g_Config.m_bEditor) ToggleScriptDebugWindow();
  } else if(IsKeyDown(DIK_L) && IsCtrlPress())
  {
	CCharacter* pMain = CGameScene::GetMainCha();
	if(pMain)
	{
	  char buf[128] = {0};
	  _snprintf_s(buf, _countof(buf), _TRUNCATE, "%d,%d", (pMain->GetCurX() + 50) / 100, (pMain->GetCurY() + 50) / 100);
	  //g_stUICoze.AddInput( buf );
	  CCozeForm::GetInstance()->AddToEdit(buf);
	}
  }

  if(IsEnableSuperKey())
  {
	HandleSuperKey();
  }

#ifdef __EDITOR__
  if(g_Config.m_bEditor && IsKeyDown(DIK_TAB)) // 激活|关闭编辑器
  {
	extern CEditor g_stUIEditor;
	g_stUIEditor.SetEnabled(!g_stUIEditor.frmEditor->GetIsShow());
  }
#endif

  if(IsKeyDown(DIK_RETURN) && IsAltPress())
  {
	// 暂时规避
	return;

	//设置全屏窗口切换
	g_stUISystem.m_sysProp.m_videoProp.bFullScreen =
		!g_stUISystem.m_sysProp.m_videoProp.bFullScreen;
	g_stUISystem.m_sysProp.ApplyVideo();
	return;
  }
#ifdef __EDITOR__
  g_Editor.HandleKeyDown();
#endif
}

void CGameApp::ChangeVideoStyle(int width, int height, D3DFORMAT format, bool bWindowed)
{
  // 	LG("video", RES_STRING(CL_LANGUAGE_MATCH_104), width, height, bWindowed);

  //MPInterfaceMgr* imgr = g_Render.GetInterfaceMgr();
  //MPIResourceMgr* res_mgr = imgr->res_mgr;
  //MPIDeviceObject* dev_obj = imgr->dev_obj;

  //HWND hwnd = g_pGameApp->GetHWND();

  //RECT wnd_rc = { 0, 0, width, height };
  //DWORD style = WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_VISIBLE;

  //if(bWindowed)
  //{
  //	//::AdjustWindowRectEx(&wnd_rc, style, 0, 0);
  //
  //	// if(height==768) height = 742;
  //
  ////	width = wnd_rc.right - wnd_rc.left;
  ////	height = wnd_rc.bottom - wnd_rc.top;
  //	LG("video", "窗口方式, 实际能创建窗口尺寸为width = %d, height = %d\n", width, height);
  //}
  //// else
  //{
  //	// wnd_rc.right = 1024;
  //	// wnd_rc.bottom = 768;
  //}

  //D3DDISPLAYMODE d3ddm;
  //dev_obj->GetDirect3D()->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

  //D3DPRESENT_PARAMETERS d3dpp;
  //memset(&d3dpp, 0, sizeof(d3dpp));

  //d3dpp.BackBufferHeight = height;
  //d3dpp.BackBufferWidth =  width;

  //d3dpp.Windowed = bWindowed;
  //d3dpp.hDeviceWindow = hwnd;
  //d3dpp.BackBufferCount = 1;
  //d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  //d3dpp.BackBufferFormat = d3ddm.Format;
  //d3dpp.EnableAutoDepthStencil = 1;
  //d3dpp.AutoDepthStencilFormat = format ;
  //d3dpp.FullScreen_RefreshRateInHz = d3ddm.RefreshRate;
  //d3dpp.Flags = 2;

  //#ifdef LW_USE_DX8
  //d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
  //#endif
  //#ifdef LW_USE_DX9
  //d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
  //#endif

  //lwWndInfo wnd_info;
  //wnd_info.hwnd = hwnd;
  //wnd_info.left = 0;
  //wnd_info.top = 0;
  //wnd_info.width =  wnd_rc.right - wnd_rc.left;
  //wnd_info.height = wnd_rc.bottom - wnd_rc.top;
  //wnd_info.windowed_style = style;

  //// 这个接口只是临时用来测试的，正式的版本中，d3dpp的参数事先就已经准备好，
  //// 无需外部传入，同时该接口将会包含在g_Render.ToggleFullScreen();中
  //LG("video", "传入给ToggleFullScreen wnd_info w = %d h = %d\n", wnd_info.width, wnd_info.height);
  //if(g_Render.ToggleFullScreen(&d3dpp, &wnd_info) == 0)
  //{
  //	LG("video", "msgToggleFullScreen error");
  //	return;
  //}

  if(g_Render.ToggleFullScreen(width, height, format, bWindowed) == 0)
  {
	//LG("video", "msgToggleFullScreen error");
	return;
  }

  // 	LG("video", RES_STRING(CL_LANGUAGE_MATCH_105));

  //SetIsFullScreen( !bWindowed);

  g_pGameApp->_nWindowWidth	 = width;
  g_pGameApp->_nWindowHeight = height;

  //	LG("video", "GetRender SetScreen w = %d, h = %d\n", width, height);

  GetRender().SetScreen(g_Render.GetScrWidth(), g_Render.GetScrHeight(), !bWindowed);

  ResetCaption();

  if(GetCurScene())
  {
	GetCurScene()->SetScreen(g_Render.GetScrWidth(), g_Render.GetScrHeight(), !bWindowed);
  }
}

#ifndef TESTDEMO

void CGameApp::HandleKeyContinue()
{
  if(_pConsole->IsVisible()) return;

  HandleContinueSuperKey();
}

void CGameApp::MouseButtonDown(int nButton)
{
#ifdef _LUA_GAME
  lua_platform_mousedown(nButton);
#endif

#ifdef APP_DEBUG
  try
  {
#endif
	if(!_IsSceneOk()) return;

#ifdef __EDITOR__
	if(g_Editor.IsEnable())
	{
	  if(CFormMgr::IsMouseInGui()) return;

	  g_Editor.MouseButtonDown(nButton);
	} else
#endif
	{
	  if(CFormMgr::IsMouseInGui()) return;

	  g_stUIStart.CheckMouseDown(GetMouseX(), GetMouseY());

	  if(!_MouseInScene) return;

	  GetCurScene()->_MouseButtonDown(nButton);
	  _stCursorMgr.MouseDown(nButton);
	}
#ifdef APP_DEBUG
  } catch(...)
  {
	MessageBox(0, "CGameApp MouseButtonDown exception!", "APP_DEBUG", 0);
  }
#endif
}

void CGameApp::MouseButtonUp(int nButton)
{
  _dwMouseDownTime[nButton] = 0;

  if(_IsSceneOk())
  {
	if(CFormMgr::IsMouseInGui()) return;

#ifdef __EDITOR__
	if(g_Editor.IsEnable())
	{
	  g_Editor.MouseButtonUp(nButton);
	} else
#endif
	{
	  GetCurScene()->_MouseButtonUp(nButton);

	  _stCursorMgr.MouseUp(nButton);
	}
  }
}

void CGameApp::MouseContinue(int nButton)
{
  if(g_Render.IsRealFPS())
  {
	if(_dwMouseDownTime[nButton] == 0)
	  _dwMouseDownTime[nButton] = g_dwCurFrameTick;
  } else
	_dwMouseDownTime[nButton]++;
}

void CGameApp::MouseMove(int nOffsetX, int nOffsetY)
{
#ifdef APP_DEBUG
  try
  {
#endif
	if(_IsSceneOk())
	{
	  if(g_Config.m_bEditor && IsCtrlPress())
	  {
		CCharacter* pMain = GetCurScene()->GetMainCha();
		if(pMain && pMain->getPatrolX() > 0)
		{
		  pMain->setPatrol((int)(GetCurScene()->GetMouseMapX() * 100.0f), (int)(GetCurScene()->GetMouseMapY() * 100.0f));
		}
	  }

	  if(IsMouseButtonPress(1))
	  {
		if(!IsCameraFollow())
		{
		  if(g_Config.m_bEditor && GetCurScene()->IsEnableCamDrag())
		  {
			float fSpeed = 0.1f;
			GetMainCam()->MoveRight(fSpeed * (float)nOffsetX, TRUE);
			GetMainCam()->MoveForward(fSpeed * (float)nOffsetY, TRUE);
		  }
		} else
		{
		  //lemon add
		  CCharacter* pCha = CGameScene::GetMainCha();
		  if(pCha)
		  {
			GetMainCam()->RotationCameraLR((float)nOffsetX / 300);
			GetMainCam()->MoveForwardBack((float)nOffsetY / 300);
			//Ninja::Camera *pCamera = GetNinjaCamera();
			//pCamera->Rotate_Vertical(-(float)nOffsetX / 300);	// 摄像机调整
		  }
		}
	  }

	  if(CFormMgr::IsMouseInGui()) return;

#ifdef __EDITOR__
	  if(g_Editor.IsEnable())
	  {
		g_Editor.MouseMove(nOffsetX, nOffsetY);
	  }
#endif
	  GetCurScene()->_MouseMove(nOffsetX, nOffsetY);
	}
#ifdef APP_DEBUG
  } catch(...)
  {
	MessageBox(0, "CGameApp MouseMove exception!", "APP_DEBUG", 0);
  }
#endif
}

void CGameApp::MouseButtonDB(int nButton)
{
#ifdef APP_DEBUG
  try
  {
#endif
	//IP("ok");
	if(_IsSceneOk())
	{
	  if(CFormMgr::IsMouseInGui()) return;

	  GetCurScene()->_MouseButtonDB(nButton);

	  //在游戏模式中加入双击恢复默认视角
	  if((nButton == 1) && (!g_Config.m_bEditor) && (!_pMainCam->IsDefaultView()))
	  {
		//_pMainCam->ResetCamera();
		ResetCamera();
	  }

	  //float f = sqrt((float)35);
	  //LG("LOG","msgok%f",f);
	  //CCameraCtrl *pCam = g_pGameApp->GetMainCam();
	  //CCharacter *pCha = GetMainCha();
	  //if(pCha)
	  //{
	  //	D3DXVECTOR3 vecCha = pCha->GetPos();

	  //	g_pGameApp->Get ->Reset(vecCha.x,vecCha.y,vecCha.z);
	  //	pCam->SetFollowObj(vecCha);
	  //	pCam->FrameMove(0);
	  //	g_Render.SetWorldViewFOV(Angle2Radian(pCam->m_ffov));
	  //	g_Render.LookAt(pCam->m_EyePos, pCam->m_RefPos);
	  //	g_Render.SetCurrentView(LERender::VIEW_WORLD);
	  //}
	}
#ifdef APP_DEBUG
  } catch(...)
  {
	MessageBox(0, "CGameApp MouseButtonDB exception!", "APP_DEBUG", 0);
  }
#endif
}

void CGameApp::MouseScroll(int nScroll)
{
#ifdef APP_DEBUG
  try
  {
#endif
	if(_IsSceneOk())
	{
	  ihei += nScroll > 0 ? 1 : -1;
	  if(CFormMgr::IsMouseInGui())
	  {
		CFormMgr::s_Mgr.MouseScroll(nScroll);
		return;
	  }

#ifdef __EDITOR__
	  if(g_Editor.IsEnable())
	  {
		g_Editor.MouseScroll(nScroll);
	  } else
#endif
	  {
		GetCurScene()->_MouseScroll(nScroll);
	  }

#ifdef __EDITOR__
	  if(!(g_Editor.IsEnable() && g_Editor.m_nSelTypeID))
#endif
	  {
		CCameraCtrl* pCam = g_pGameApp->GetMainCam();
		int			 f	  = nScroll < 0 ? 30 : -30;
		//pCam->MoveForwardBack((float)f * 0.001f);
		pCam->Scale((float)f * 0.005f);
		//int f = nScroll < 0 ? 100 : -100;
		//Ninja::Camera* pCamera = GetNinjaCamera();	// 摄像机调整
		//pCamera->Range((float)f * 0.02f);
	  }
	}
#ifdef APP_DEBUG
  } catch(...)
  {
	MessageBox(0, "CGameApp MouseScroll exception!", "APP_DEBUG", 0);
  }
#endif
}

void CGameApp::HandleSuperKey()
{
  if(!IsEnableSuperKey()) return;

#ifdef APP_DEBUG
  try
  {
#endif

	if(IsKeyDown(DIK_C))
	{
#ifdef __EDITOR__
	  if(IsCtrlPress()) // Ctrl+C 切换 Pick 模式， 激活状态时，能显示当前鼠标指到的物体
	  {
		g_Editor.SwitchPickMode();
	  } else
#endif
	  {
		//GetCurScene()->SetMainCha(GetCurScene()->GetMainCha()->getID());

		if(CGameScene::GetMainCha())
		{
		  _bCameraFollow = !_bCameraFollow;
		  SetCameraPos(CGameScene::GetMainCha()->GetPos());
		}
		//if(_bCameraFollow)
		//{
		//	CCameraCtrl *pCam = GetMainCam();
		//	CCharacter *pCha = GetCurScene()->GetMainCha();
		//	D3DXVECTOR3 vecCha = pCha->GetPos();

		//	pCam->InitBuf(vecCha.x,vecCha.y,vecCha.z);
		//	pCam->SetBufVel( pCha->getMoveSpeed() ,pCha->getID());

		//	ResetGameCamera( pCha->IsBoat() ? 1 : 0 );
		//	pCam->FrameMove(0);

		//	g_Render.SetWorldViewFOV(Angle2Radian(pCam->m_ffov));
		//	g_Render.LookAt(pCam->m_EyePos, pCam->m_RefPos);
		//	g_Render.SetCurrentView(LERender::VIEW_WORLD);
		//	//ResetGameCamera( GetCurScene()->GetMainCha()->IsBoat() ? 1 : 0 );
		//}

		TipI(_bCameraFollow, RES_STRING(CL_LANGUAGE_MATCH_106), RES_STRING(CL_LANGUAGE_MATCH_107));
	  }
	} else if(IsKeyDown(DIK_INSERT) && IsCtrlPress())
	{
	  g_Render.EnableCaptureAVI(1 - g_Render.IsEnableCaptureAVI());
	} else if(IsKeyDown(DIK_F3) && g_Config.m_bEditor && IsCtrlPress())
	{
	  // 切换场景 by lh test
	  static int type = 0;
	  type			  = GetCurScene()->GetSceneTypeID();
	  type++;
	  if(type >= enumSceneEnd) type = 0;

	  LoadScriptScene((eSceneType)type);
	} else if(g_Config.m_bEditor && IsCtrlPress() && IsKeyDown(DIK_P))
	{
	  CCharacter* pMain = GetCurScene()->GetMainCha();
	  if(pMain)
	  {
		if(pMain->getPatrolX() > 0)
		{
		  pMain->setPatrol(0, 0);
		} else
		{
		  pMain->setPatrol(1, 0);
		}
	  }
	}
//	else if( IsCtrlPress() && IsKeyDown(DIK_P) )
//	{
//		if( IsShiftPress() )
//			g_fSpeed += 0.01f;
//		else
//			g_fSpeed += 0.001f;
//	}
//	else if( IsCtrlPress() && IsKeyDown(DIK_O) )
//	{
//		if( IsShiftPress() )
//			g_fSpeed -= 0.01f;
//		else
//			g_fSpeed -= 0.001f;
//	}
#ifdef _DEBUG
	else if(IsKeyDown(DIK_F4) && IsCtrlPress())
	{
	  CFormMgr::s_Mgr.SetEnabled(!CFormMgr::s_Mgr.GetEnabled());
	} else if(IsKeyDown(DIK_K) && IsCtrlPress())
	{
	  GetCurScene()->ShowTerrain(false);
	  GetCurScene()->ShowSceneObj(FALSE);
	  btest = true;
	} else if(IsKeyDown(DIK_J) && IsCtrlPress())
	{
	  GetCurScene()->ShowTerrain(true);
	  GetCurScene()->ShowSceneObj(FALSE);
	  btest = true;
	} else if(IsKeyDown(DIK_H) && IsCtrlPress())
	{
	  GetCurScene()->ShowTerrain(true);
	  GetCurScene()->ShowSceneObj(true);
	  btest = false;
	}
#endif
#ifdef _LOG_NAME_
	else if(IsCtrlPress())
	{
	  if(IsKeyDown(DIK_Y))
	  {
		_pDrawPoints->Clear();
	  } else if(IsKeyDown(DIK_N))
	  {
		CCharacter::IsShowLogName = !CCharacter::IsShowLogName;
	  } else if(IsKeyDown(DIK_L))
	  {
		GetDrawPoints()->SetIsEnabled(!GetDrawPoints()->GetIsEnabled());
		AddTipText(RES_STRING(CL_LANGUAGE_MATCH_108));
	  } else if(IsKeyDown(DIK_T))
	  {
		SetIsRenderTipText(!GetIsRenderTipText());
		AddTipText(RES_STRING(CL_LANGUAGE_MATCH_109));
	  }
	}
#endif

	if(_IsSceneOk()) GetCurScene()->_HandleSuperKey();
#ifdef APP_DEBUG
  } catch(...)
  {
	MessageBox(0, "CGameApp HandleSuperKey exception!", "APP_DEBUG", 0);
  }
#endif
}
//pTerr->GetWidth()
void CGameApp::HandleContinueSuperKey()
{
  CCameraCtrl*	 pCam	 = g_pGameApp->GetMainCam();
  Ninja::Camera* pCamera = GetNinjaCamera();

#ifdef _SKYBOX_
  // 通过bRot确定是否处于旋转(AD)状态，若处于旋转状态，则不允许前进与后退动作(WS)
  static bool bRot = FALSE;

  if(!IsAltPress() && !(CCompent::GetActive()))
  {
	if(IsKeyContinue(DIK_W))
	{
	  //向前走

	  CCharacter* cha	  = GetCurScene()->GetMainCha();
	  D3DXVECTOR3 vCurDir = GetMainCam()->m_vDir;
	  D3DXVECTOR3 vMoveTo = (cha->GetPos() - vCurDir * 2) * 100.0f;
	  //CWorldScene* pScene = dynamic_cast<CWorldScene*>(GetCurScene());
	  //pScene->GetMouseDown().ActMove( GetCurScene()->GetMainCha(), int( vMoveTo.x ), int( vMoveTo.y ), false, true, false );
	  //GetCurScene()->GetMainCha()->PlayPose(6, PLAY_LOOP);
#if 0
			COneMoveState * move = new COneMoveState(cha->GetActor());
			move->SetWalkLine( true );
			move->SetMoveTo( vMoveTo.x, vMoveTo.y );
			cha->GetActor()->SwitchState(move);
#else
	  stNetMoveInfo stPathInfo;
	  stPathInfo.dwAveragePing = 0;
	  stPathInfo.pos_buf[0].x  = int(cha->GetPos().x * 100);
	  stPathInfo.pos_buf[0].y  = int(cha->GetPos().y * 100);
	  stPathInfo.pos_buf[1].x  = int(vMoveTo.x);
	  stPathInfo.pos_buf[1].y  = int(vMoveTo.y);
	  stPathInfo.pos_num	   = 2;
	  CS_BeginAction(GetCurScene()->GetMainCha(), enumACTION_MOVE, (void*)&stPathInfo, NULL);

	  //FreeMove( stPathInfo );
	  cha->SetServerPos(int(vMoveTo.x), int(vMoveTo.y));
	  //cha->setPos( int( vMoveTo.x ), int( vMoveTo.y ) );
	  cha->MoveTo(int(vMoveTo.x), int(vMoveTo.y));
#endif
	}
	if(IsKeyContinue(DIK_S) && !bRot)
	{
	  //向后走
	  D3DXVECTOR3  vCurDir = GetMainCam()->m_vDir;
	  D3DXVECTOR3  vMoveTo = (g_pGameApp->GetCurScene()->GetMainCha()->GetPos() + vCurDir * 2) * 100.0f;
	  CWorldScene* pScene  = dynamic_cast<CWorldScene*>(GetCurScene());
	  pScene->GetMouseDown().ActMove(g_pGameApp->GetCurScene()->GetMainCha(), int(vMoveTo.x), int(vMoveTo.y), true, false, false);
	}
	if(IsKeyContinue(DIK_A))
	{
	  //向左旋转镜头
	  CCharacter* pCha = g_pGameApp->GetCurScene()->GetMainCha();
	  pCha->StopMove();

	  GetMainCam()->ScroolLR(-0.1f);
	  float pi	  = 3.1416;
	  float pi2	  = 6.2832;
	  float angle = GetMainCam()->m_fAngle;
	  angle -= int(angle / pi2) * pi2;
	  if(angle < 0)
	  {
		angle = pi2 + angle;
	  }
	  _pCurScene->GetMainCha()->FaceTo((180 * angle) / pi);
	  bRot = TRUE;
	} else if(IsKeyContinue(DIK_D))
	{
	  //向右旋转镜头
	  CCharacter* pCha = g_pGameApp->GetCurScene()->GetMainCha();
	  pCha->StopMove();

	  GetMainCam()->ScroolLR(0.1f);
	  float pi	  = 3.1416;
	  float pi2	  = 6.2832;
	  float angle = GetMainCam()->m_fAngle;
	  angle -= int(angle / pi2) * pi2;
	  if(angle < 0)
	  {
		angle = pi2 + angle;
	  }
	  _pCurScene->GetMainCha()->FaceTo((180 * angle) / pi);
	  bRot = TRUE;
	} else
	{
	  bRot = FALSE;
	}
  }
#endif

#ifdef _KOSDEMO_
  if(!IsAltPress() && !(CCompent::GetActive()))
  {
	if(IsKeyDown(DIK_SPACE))
	{
	  // 跳跃处理
	  //g_pGameApp->GetCurScene()->GetMainCha()->PlayPose(20);
	}

	if(IsKeyDown(DIK_G))
	{
	  // 若包裹里有麋鹿宝宝卷轴，则使用
	  short ItemPos = g_stUIEquip.GetItemPos(2889);
	  if(ItemPos >= 0)
	  {
		stNetUseItem param;
		param.sGridID	 = ItemPos;
		param.sTarGridID = 0;
		CS_BeginAction(GetCurScene()->GetMainCha(), enumACTION_ITEM_USE, (void*)&param);
	  }
	}

	if(IsKeyDown(DIK_H))
	{
	  // 隐藏(显示)幻兽
	  // 脱离铠化状态
	  CCharacter* pPetCha = NULL;
	  if(PetWorldID)
	  {
		pPetCha = GetCurScene()->SearchByID(PetWorldID);
	  }

	  if(pPetCha)
	  {
		pPetCha->SetHide(!(pPetCha->IsHide()));
		GetCurScene()->GetMainCha()->UpdataItem(1, 3);
		if(!pPetCha->IsHide())
		{
		  pPetCha->PlayPose(1);
		}
	  }

	  CMagicEff* eff = GetCurScene()->GetMainCha()->curEff;
	  if(eff)
	  {
		eff->SetHide(true);
	  }
	}

	if(IsKeyDown(DIK_T))
	{
	  // 开始铠化
	  CCharacter* pPetCha = NULL;
	  if(PetWorldID)
	  {
		pPetCha = GetCurScene()->SearchByID(PetWorldID);
		if(pPetCha)
		{
		  pPetCha->SetHide(FALSE);
		  pPetCha->FaceTo(GetCurScene()->GetMainCha()->getYaw());
		  pPetCha->PlayPose(10);
		  pPetCha->SelfEffect(949);
		  pPetCha->SetPetHideCount(52);

		  GetCurScene()->GetMainCha()->SetMainChaChangeItemCount(50);
		  CMagicEff* tEff = GetCurScene()->GetMainCha()->SelfEffect(950);
		  tEff->SetDailTime(1500);
		}
	  }
	}

	if(IsKeyDown(DIK_P))
	{
	}
  }
#endif

  if(IsKeyContinue(DIK_LSHIFT) || IsKeyContinue(DIK_RSHIFT))
  {
	if(IsKeyContinue(DIK_UP))
	  pCam->MoveForwardBack((float)30 * 0.001f);
	if(IsKeyContinue(DIK_DOWN))
	  pCam->MoveForwardBack((float)-30 * 0.001f);

	if(IsKeyContinue(DIK_LEFT))
	{
	  pCam->RotationCameraLR(0.03f);
	  pCamera->Rotate_Vertical(-0.03f);
	}
	if(IsKeyContinue(DIK_RIGHT))
	{
	  pCam->RotationCameraLR(-0.03f);
	  pCamera->Rotate_Vertical(+0.03f);
	}
  }

  if(!IsEnableSuperKey())
  {
	return;
  }
  // Continue Key Events Handle Routines...

  if(IsKeyContinue(DIK_E))
  {
	GetMainCam()->MoveForward(0.3f, TRUE);
  }

  if(IsKeyContinue(DIK_D))
  {
	GetMainCam()->MoveForward(-0.3f, TRUE);
  }

  if(IsKeyContinue(DIK_S))
  {
	float fSpeed = 0.20f;
	GetMainCam()->MoveRight(fSpeed, TRUE);
  }

  if(IsKeyContinue(DIK_F))
  {
	float fSpeed = -0.20f;
	GetMainCam()->MoveRight(fSpeed, TRUE);
  }
  if(IsKeyContinue(DIK_Q))
  {
	//btest = !btest;
  }

  if(IsKeyContinue(DIK_LEFT))
  {
	if(IsCameraFollow())
	{
	  CCharacter* pCha = _pCurScene->GetMainCha();
	  if(pCha)
	  {
		float fX = (float)pCha->GetCurX() / 100.0f;
		float fY = (float)pCha->GetCurY() / 100.0f;

		VECTOR3 vecPos = pCha->GetPos(); //  VECTOR3(fX, fY, );
		GetMainCam()->Turn(0.4f, &vecPos);
	  }
	} else
	{
	  VECTOR3 vecPos = GetMainCam()->m_RefPos;
	  GetMainCam()->Turn(0.4f, &vecPos);
	}
  }

  if(IsKeyContinue(DIK_RIGHT))
  {
	if(IsCameraFollow())
	{
	  CCharacter* pCha = _pCurScene->GetMainCha();
	  if(pCha)
	  {
		float fX = (float)pCha->GetCurX() / 100.0f;
		float fY = (float)pCha->GetCurY() / 100.0f;
		// VECTOR3 vecPos = VECTOR3(fX, fY, 0.0f);
		VECTOR3 vecPos = pCha->GetPos(); //  VECTOR3(fX, fY, );
		GetMainCam()->Turn(-0.4f, &vecPos);
	  }
	} else
	{
	  VECTOR3 vecPos = GetMainCam()->m_RefPos;
	  GetMainCam()->Turn(-0.4f, &vecPos);
	}
  }

  // if(IsKeyContinue(DIK_UP))   GetMainCam()->MoveForward(0.2f, FALSE);
  // if(IsKeyContinue(DIK_DOWN)) GetMainCam()->MoveForward(-0.2f,FALSE);

  //
  //if(IsKeyContinue(DIK_A)) GetMainCam()->Move(MOVE_UP);
  //if(IsKeyContinue(DIK_Z)) GetMainCam()->Move(MOVE_DOWN);

  if(IsKeyContinue(DIK_O))
  {
	// 应美术消除广角,增加拉伸方式 modify by rock090824
#ifdef __EDITOR__
	if(g_Editor.IsPitchWatch())
	{
	  GetMainCam()->m_EyePos.z++;
	} else
	{
	  g_Render.SetWorldViewFOV(g_Render.GetWorldViewFOV() - D3DX_PI / 180.0f);
	}
#else
	g_Render.SetWorldViewFOV(g_Render.GetWorldViewFOV() - D3DX_PI / 180.0f);
#endif
  } else if(IsKeyContinue(DIK_P))
  {
	// 应美术消除广角,增加拉伸方式 modify by rock090824
#ifdef __EDITOR__
	if(g_Editor.IsPitchWatch())
	{
	  GetMainCam()->m_EyePos.z--;
	} else
	{
	  g_Render.SetWorldViewFOV(g_Render.GetWorldViewFOV() + D3DX_PI / 180.0f);
	}
#else
	g_Render.SetWorldViewFOV(g_Render.GetWorldViewFOV() + D3DX_PI / 180.0f);
#endif
  }
#ifdef __EDITOR__
  g_Editor.HandleKeyContinue();
#endif
}
#endif //of !TESTDEMO

#include "resource.h"
bool CGameApp::HandleWindowMsg(DWORD dwMsg, DWORD dwParam1, DWORD dwParam2)
{
  if(dwMsg > WM_USER)
	_HandleMsg(dwMsg, dwParam1, dwParam2);
  else if(!_IsSceneOk())
	return false;

  switch(dwMsg)
  {
  case WM_KEYDOWN:
  {
	if(!_IsUserEnabled) return false;

	if(g_Config.IsPower() || (CGameScene::GetMainCha() && CGameScene::GetMainCha()->getGMLv()))
	{
	  GetConsole()->OnKeyDownEvent(dwParam1);
	}
	//g_InputBox.HandleWindowMsg(dwMsg, dwParam1, dwParam2);
	//			LG( "key", "keydown:%d, %d\n", dwParam1, dwParam2 );

	CFormMgr::s_Mgr.OnKeyDown((int)dwParam1);

	GetCurScene()->_KeyDownEvent((int)dwParam1);
	break;
  }
  case WM_KEYUP:
  {
	//g_InputBox.HandleWindowMsg(dwMsg, dwParam1, dwParam2);
	break;
  }
  case WM_CHAR:
  {
	if(!_IsUserEnabled) return false;

	if(g_Config.IsPower() || (CGameScene::GetMainCha() && CGameScene::GetMainCha()->getGMLv()))
	{
	  if(GetConsole()->OnCharEvent((TCHAR)dwParam1, dwParam2)) return false;
	}
	//g_InputBox.HandleWindowMsg(dwMsg, dwParam1, dwParam2);
	//			LG( "key", "keychar:%d, %d\n", dwParam1, dwParam2 );

	if(CFormMgr::s_Mgr.OnKeyChar((char)dwParam1)) return false;

	break;
  }
  case WM_SYSKEYDOWN:
  {
	if(!_IsUserEnabled) return false;

	if(IsAltPress())
	{
	  if(dwParam1 == 'm' || dwParam1 == 'M')
		GetCurScene()->ShowMinimap(!GetCurScene()->GetIsShowMinimap());

	  if(CFormMgr::s_Mgr.GetEnableHotKey()) // 热键是否被屏蔽
		CFormMgr::s_Mgr.OnHotKey((char)dwParam1, 0);
	  else if(dwParam1 == 'd' || dwParam1 == 'D') // 临进背包特殊处理
		CFormMgr::s_Mgr.OnHotKey((char)dwParam1, 0);
	} else if(dwParam1 == VK_F10)
	{
	  CFormMgr::s_Mgr.OnKeyDown((int)dwParam1);
	  GetCurScene()->_KeyDownEvent((int)dwParam1);
	}
	//g_InputBox.HandleWindowMsg(dwMsg, dwParam1, dwParam2);
	break;
  }
  case WM_MOUSEMOVE:
	//CFormMgr::s_Mgr.MouseReset();
  case WM_LBUTTONDOWN:
  case WM_RBUTTONDBLCLK:
  case WM_RBUTTONDOWN:
  case WM_RBUTTONUP:
  {
	if(!_IsUserEnabled) return false;

	int xPos = LOWORD(dwParam2);
	int yPos = HIWORD(dwParam2);
	g_pGameApp->SetMouseXY(xPos, yPos);
	break;
  }
  case WM_TIMER:
	g_pGameApp->FrameMove(g_pGameApp->GetCurTick());
	break;
  case WM_MOVE:
	CWebBrowser::WindowMove();
	break;
  default:
  {
	// g_InputBox.HandleWindowMsg(dwMsg, dwParam1, dwParam2);
	if(CFormMgr::s_Mgr.HandleWindowMsg(dwMsg, dwParam1, dwParam2)) return false;
	break;
  }
  }
  return false;
}

//-----------------
// 怪物摆放编辑功能
//-----------------
const char* HandleMonsterCommand(string& strCmd, string& p1, string& p2)
{
  if(g_Config.m_bEditor == FALSE) return "";

  CGameScene* pScene = g_pGameApp->GetCurScene();
  if(!pScene) return "";

  if(strCmd == "load")
  {
	if(p1 == "") return RES_STRING(CL_LANGUAGE_MATCH_110);
	string strFileName = p1 + ".lua";

	strFileName = "monster/" + strFileName;

	ifstream in;
	in.open(strFileName.c_str());
	if(in.is_open() == false)
	{
	  return RES_STRING(CL_LANGUAGE_MATCH_111);
	}
	char   szCha[255];
	string strList[10];
	while(!in.eof())
	{
	  in.getline(szCha, 255);
	  if(strlen(szCha) == 0) break;

	  Util_ResolveTextLine(szCha, strList, 2, '('); // 去掉左括号
	  string strRight = strList[1];
	  Util_ResolveTextLine(strRight.c_str(), strList, 2, ')');
	  string strValue = strList[0]; // 去掉右括号
	  int	 n		  = Util_ResolveTextLine(strValue.c_str(), strList, 8, ',');

	  int nChaID = Str2Int(strList[0]);
	  int x		 = Str2Int(strList[1]);
	  int y		 = Str2Int(strList[2]);
	  int angle	 = Str2Int(strList[3]);
	  int time	 = Str2Int(strList[4]);

	  int px = 0, py = 0;
	  if(n > 5) // 读入巡逻点参数
	  {
		px = Str2Int(strList[5]);
		py = Str2Int(strList[6]);
	  }

	  CCharacter* pCha = pScene->AddCharacter(nChaID);
	  if(pCha)
	  {
		pCha->setPos(x, y);
		pCha->setYaw(angle);
		pCha->setReliveTime(time);
		pCha->setPatrol(px, py);
	  } else
	  {
		LG(RES_STRING(CL_LANGUAGE_MATCH_112), RES_STRING(CL_LANGUAGE_MATCH_113), nChaID);
	  }
	}
	in.close();
	return RES_STRING(CL_LANGUAGE_MATCH_114);
  } else if(strCmd == "save") // 保存放怪记录
  {
	if(p1 == "") return RES_STRING(CL_LANGUAGE_MATCH_110);
	Util_MakeDir("monster");
	string strFileName = p1 + ".lua";

	strFileName = "monster/" + strFileName;

	DWORD dwTime = 0;

	if(p2 != "") dwTime = Str2Int(p2); // 重生时间

	// 摆放的角色全部写入到一个文本文件里
	FILE* fp;
	fopen_s(&fp, strFileName.c_str(), "wt");

	if(fp == NULL)
	  return RES_STRING(CL_LANGUAGE_MATCH_115);

	for(int i = 0; i < pScene->GetChaCnt(); i++)
	{
	  CCharacter* pCha = pScene->GetCha(i);
	  if(pCha->IsValid())
	  {
		if(pCha != pScene->GetMainCha() && pCha->getTypeID() > 4)
		{
		  DWORD dwTime1 = dwTime;
		  if(dwTime1 == 0) dwTime1 = pCha->getReliveTime();
		  if(pCha->getPatrolX() == 0) // 不带巡逻点的设置
		  {
			fprintf(fp, "CreateCha(%d, %d, %d, %d, %d)\n", pCha->getTypeID(), pCha->GetCurX(), pCha->GetCurY(), FixAngle(pCha->getYaw()), dwTime1);
		  } else // 带巡逻点的设置
		  {
			fprintf(fp, "CreatePatrolCha(%d, %d, %d, %d, %d, %d, %d)\n", pCha->getTypeID(), pCha->GetCurX(), pCha->GetCurY(), FixAngle(pCha->getYaw()), dwTime1, pCha->getPatrolX(), pCha->getPatrolY());
		  }
		}
	  }
	}
	fclose(fp);
	return RES_STRING(CL_LANGUAGE_MATCH_115);
  } else if(strCmd == "seek") // 按照怪物编号寻找一个怪
  {
	if(p1 == "") return RES_STRING(CL_LANGUAGE_MATCH_110);
	int nScriptID = Str2Int(p1);

	for(int i = 0; i < pScene->GetChaCnt(); i++)
	{
	  CCharacter* pCha = pScene->GetCha(i);
	  if(pCha->IsValid())
	  {
		if(pCha != pScene->GetMainCha() && pCha->getTypeID() > 4)
		{
		  CChaRecord* pInfo = GetChaRecordInfo(pCha->getTypeID());
		  if((pInfo && p1 == pInfo->szDataName) || nScriptID == pCha->getTypeID())
		  {
			// 当前怪物的坐标
			int x = pCha->GetCurX();
			int y = pCha->GetCurY();
			pScene->SetMainCha(pCha->getID());
			// g_pGameApp->EnableCameraFollow(FALSE);
			break;
		  }
		}
	  }
	}
	return "";
  } else if(strCmd == "clear") // 清除所有怪物
  {
	for(int i = 0; i < pScene->GetChaCnt(); i++)
	{
	  CCharacter* pCha = pScene->GetCha(i);
	  pCha->SetValid(FALSE);
	}
  }
  return RES_STRING(CL_LANGUAGE_MATCH_116);
}

void CheckSkillEffect(CSkillRecord* pSkill, int nEffectID)
{
  if(!pSkill) return;

  if(nEffectID <= 0) return;

  string name;
  if(nEffectID >= 1000 && nEffectID < 2000)
  {
	int		   n	= nEffectID % 1000;
	EFF_Param* pEFF = GetEFFParam(n);
	if(pEFF)
	{
	  name = pEFF->szName;
	}
  } else if(nEffectID >= 2000 && nEffectID < 3000)
  {
	int			 n		= nEffectID % 2000;
	Group_Param* pGroup = GetGroupParam(n);
	if(pGroup)
	{
	  name = pGroup->szName;
	}
  } else
  {
	CMagicInfo* pInfo = GetMagicInfo(nEffectID);
	if(pInfo)
	{
	  name = pInfo->szName;
	}
  }

  if(name.empty())
  {
	LG("skillinfoerror", RES_STRING(CL_LANGUAGE_MATCH_117), pSkill->nID, pSkill->szName, nEffectID);
	return;
  }

  int n = (int)name.find(RES_STRING(CL_LANGUAGE_MATCH_118));
  if(n >= 0)
  {
	LG("skillinfoerror", RES_STRING(CL_LANGUAGE_MATCH_119), pSkill->nID, pSkill->szName, nEffectID);
  }
}

const char* ConsoleCallback(const char* pszCmd)
{
  string strInput = pszCmd;
  string strList[80];
  int	 n = Util_ResolveTextLine(pszCmd, strList, 80, ' ');

  string strCmd = strList[0];
  string strRes = RES_STRING(CL_LANGUAGE_MATCH_120);

  string p1 = strList[1];
  string p2 = strList[2];
  string p3 = strList[3];
  string p4 = strList[4];

  static char szConsoleHelp[][64] =
	  {
		  "command       param    detail",
		  //"指令        参数     说明",
		  //"loadmap     地图名   读取指定名称的地图",
		  //"savemap     地图名   保存地图为指定名称的文件",
		  //"brushheight 高度值   设置抚平地表的刷子的高度"
		  "brushheight   0.6",
		  ""};
  static bool UI_DEBUG_FLAG_ARCOL = false;
  if(strCmd == "?")
  {
	int n = 0;
	while(1)
	{
	  if(strlen(szConsoleHelp[n]) == 0) break;
	  g_pGameApp->GetConsole()->AddText(szConsoleHelp[n]);
	  n++;
	}
  }
  if(strCmd == "thanks")
  {
	g_pGameApp->GetConsole()->AddText(RES_STRING(CL_LANGUAGE_MATCH_121));
	g_pGameApp->GetConsole()->AddText(RES_STRING(CL_LANGUAGE_MATCH_122));
	g_pGameApp->GetConsole()->AddText("Designer:   Paco Koyo S.K Robin Baby Idle Sage Mars");
	g_pGameApp->GetConsole()->AddText("Programmer: Ryan Jack Adnor Lemon Jerry Jacky Claude Knight Arcol Michael");
	g_pGameApp->GetConsole()->AddText("Artist:     Thirteen Gsc xiaojinjin lmayaz sean tiger rondy potion omo");
	g_pGameApp->GetConsole()->AddText("            always milo AF redpig aoao ldc gooncoo RBMMax koala momo");
	g_pGameApp->GetConsole()->AddText("            ");
	g_pGameApp->GetConsole()->AddText(RES_STRING(CL_LANGUAGE_MATCH_123));
  } else if(strCmd == "reload")
  {
  } else if(strCmd == "loadmap")
  {
  } else if(strCmd == "savemap")
  {
  }
#ifdef __EDITOR__
  else if(strCmd == "brushheight")
  {
	g_Editor.m_nBrushHeight = Str2Int(strList[1]);
  }
#endif
  else if(strCmd == "delobj")
  {
  } else if(strCmd == "login")
  {
  } else if(strCmd == "cha_light")
  {
  } else if(strCmd == "cha_color")
  {
  } else if(strCmd == "light_dir")
  {
	float fX = Str2Float(p1);
	float fY = Str2Float(p2);
	float fZ = Str2Float(p3);
	g_Render.SetDirectLightDir(fX, fY, fZ);
  } else if(strCmd == "light_color")
  {
	float r = Str2Float(p1);
	float g = Str2Float(p2);
	float b = Str2Float(p3);
	g_Render.SetDirectLightColor(r, g, b, 1.0f);
  } else if(strCmd == "freefps")
  {
	int nFree = Str2Int(p1);
	if(nFree)
	  g_pGameApp->SetFPSInterval(nFree);
	else
	  g_pGameApp->SetFPSInterval(40);
  } else if(strCmd == "perf")
  {
	BOOL bPerf = Str2Int(p1);
	g_Render.EnablePrint(INFO_PERF, bPerf);
  } else if(strCmd == "ui") //显示UI信息，参数： 0关闭 1打开	-added by Arcol
  {
	if(p1 == "1" || p1 == "on" || p1 == "y" || p1 == "yes")
	{
	  UI_DEBUG_FLAG_ARCOL = true;
	} else
	{
	  UI_DEBUG_FLAG_ARCOL = false;
	}
	CFormMgr::SetDebugMode(UI_DEBUG_FLAG_ARCOL);
  } else if(UI_DEBUG_FLAG_ARCOL && strCmd == "show") //added by Arcol
  {
	if(p1.length())
	{
	  CForm* pForm = CFormMgr::s_Mgr.Find(p1.c_str());
	  if(pForm) pForm->Show();
	}
  } else if(UI_DEBUG_FLAG_ARCOL && strCmd == "hide") //added by Arcol
  {
	if(p1.length())
	{
	  CForm* pForm = CFormMgr::s_Mgr.Find(p1.c_str());
	  if(pForm) pForm->Hide();
	}
  } else if(UI_DEBUG_FLAG_ARCOL && strCmd == "frame") //added by Arcol
  {
	bool flag = false;
	if(p1 == "1" || p1 == "on" || p1 == "y" || p1 == "yes")
	{
	  flag = true;
	}
	CFormMgr::SetDrawFrameInDebugMode(flag);
  } else if(UI_DEBUG_FLAG_ARCOL && strCmd == "close")
  {
	CForm* p = CForm::GetActive();
	if(p)
	{
	  p->Close();
	}
  } else if(UI_DEBUG_FLAG_ARCOL && strCmd == "background") //added by Arcol
  {
	bool flag = false;
	if(p1 == "1" || p1 == "on" || p1 == "y" || p1 == "yes")
	{
	  flag = true;
	}
	CFormMgr::SetDrawBackGroundInDebugMode(flag);
  } else if(strCmd == "netlogin")
  {
	CS_Login(p1.c_str(), p2.c_str(), "nobill");
  } else if(strCmd == "invite")
  {
	g_stTeamInviteFormMgr.AddInviteForm(213, "abcdef");
	g_stTeamInviteFormMgr.AddInviteForm(214, "435223");
  } else if(strCmd == "refuse")
  {
	g_stTeamInviteFormMgr.RemoveInviteForm(214);
	g_stTeamInviteFormMgr.RemoveInviteForm(213);
  } else if(strCmd == "unhide") // 停止隐形
  {
	CCharacter* pCha = CGameScene::GetMainCha();
	if(pCha && pCha->GetStateMgr()->GetSkillStateNum() > 0)
	{
	  int nState = 43; //Str2Int( p1 );
	  CS_BeginAction(pCha, enumACTION_STOP_STATE, &nState);
	}
  } else if(strCmd == "unshield") // 停止魔法盾
  {
	CCharacter* pCha = CGameScene::GetMainCha();
	if(pCha && pCha->GetStateMgr()->GetSkillStateNum() > 0)
	{
	  int nState = 83; // Str2Int( p1 );
	  CS_BeginAction(pCha, enumACTION_STOP_STATE, &nState);
	}
  } else if(strCmd == "unstate" && g_pGameApp->IsEnableSuperKey())
  {
	CCharacter* pCha = CGameScene::GetMainCha();
	if(pCha && pCha->GetStateMgr()->GetSkillStateNum() > 0)
	{
	  int nState = Str2Int(p1);
	  CS_BeginAction(pCha, enumACTION_STOP_STATE, &nState);
	}
  } else if(strCmd == "up") // 站立
  {
	if(!g_pGameApp->GetCurScene())
	  return strRes.c_str();

	::SendMessage(g_pGameApp->GetHWND(), WM_KEYDOWN, VK_INSERT, 0);
  } else if(strCmd == "throw")
  {
	int* p = NULL;
	*p	   = 0;
  } else if(strCmd == "th1")
  {
	int a = 0;
	a	  = 3 / a;
  } else if(strCmd == "th2")
  {
	_asm int 3;
  } else if(strCmd == "th3")
  {
	throw std::logic_error("test\n");
  } else if(strCmd == "logno")
  {
	extern long g_nCurrentLogNo;
	g_pGameApp->SysInfo("LogNo:%d", g_nCurrentLogNo);
  } else if(strCmd == "checkfile")
  {
	g_pGameApp->HasLogFile("gui");
	g_pGameApp->HasLogFile("roadsay");
	g_pGameApp->HasLogFile("iteminfoerror");
  } else if(strCmd == "lg")
  {
	LGInfo* pInfo	  = g_pGameApp->GetLGConfig();
	pInfo->bEnableAll = Str2Int(p1) != 0;
	g_pGameApp->LG_Config(*pInfo);
  } else if(strCmd == "lgmsg")
  {
	LGInfo* pInfo  = g_pGameApp->GetLGConfig();
	pInfo->bMsgBox = Str2Int(p1) != 0;
	g_pGameApp->LG_Config(*pInfo);
  } else if(strCmd == "lgclear")
  {
	::LG_CloseAll();
  } else if(strCmd == "gate")
  {
	if(g_NetIF->IsConnected())
	{
	  g_pGameApp->SysInfo(g_NetIF->m_connect.GetDatasock()->GetPeerIP());
	} else
	{
	  g_pGameApp->SysInfo(RES_STRING(CL_LANGUAGE_MATCH_124));
	}
  } else if(strCmd == "teamleaderid")
  {
	LG("teamleaderid", "msg%u\n", CTeamMgr::GetTeamLeaderID());
  }
#ifdef __FPS_DEBUG__
  else if(strCmd == "state")
  {
	CGameScene* pScene = CGameApp::GetCurScene();
	if(pScene)
	{
	  g_pGameApp->SysInfo(RES_STRING(CL_LANGUAGE_MATCH_125), pScene->m_dwValidEffCnt, pScene->m_dwValidChaCnt, pScene->m_dwValidSceneObjCnt);
	}
  }
#endif
  else if(g_Config.m_bEditor && strCmd == "refine")
  {
	CCharacter* pMain = CGameScene::GetMainCha();
	if(pMain)
	{
	  CSceneItem* pItem	 = NULL;
	  CGameScene* pScene = CGameApp::GetCurScene();
	  if(p1 == "left")
	  {
		pItem = pMain->GetHandItem(enumEQUIP_LHAND);
	  } else if(p1 == "right")
	  {
		pItem = pMain->GetHandItem(enumEQUIP_RHAND);
	  }
	  if(pItem)
	  {
		int	  RefineID	  = Str2Int(p2);
		int	  Level		  = Str2Int(p3);
		float EffectScale = Str2Float(p4);
		if(RefineID == 0)
		{
		  pItem->LitUnresetTexture();

		  // 卸下
		  int		  nCount  = pItem->GetEffectNum();
		  CEffectObj* pEffect = NULL;
		  for(int i = 0; i < nCount; i++)
		  {
			pEffect = pScene->GetEffect(pItem->GetEffectID(i));
			if(pEffect)
			{
			  pEffect->SetValid(FALSE);
			}
		  }
		  return strRes.c_str();
		}

		int nCharID = pMain->getTypeID() - 1;
		if(nCharID < 0 || nCharID > 3)
		{
		  LG("error", RES_STRING(CL_LANGUAGE_MATCH_126), pMain->GetDefaultChaInfo()->szName, RefineID);
		  return strRes.c_str();
		}

		CItemRefineEffectInfo* pInfo = GetItemRefineEffectInfo(RefineID);
		if(!pInfo)
		{
		  LG("error", RES_STRING(CL_LANGUAGE_MATCH_127), RefineID);
		  return strRes.c_str();
		}

		pItem->lTag = RefineID * 10 + Level; // 用于copy时还原

		if(pInfo->nLightID != 0)
		  pItem->LitResetTexture(pInfo->nLightID, Level); // 加载流光
		else
		  pItem->LitUnresetTexture();

		// 先取消之前的特效
		int			nCount	= pItem->GetEffectNum();
		CEffectObj* pEffect = NULL;
		for(int i = 0; i < nCount; i++)
		{
		  pEffect = pScene->GetEffect(pItem->GetEffectID(i));
		  if(pEffect)
		  {
			pEffect->SetValid(FALSE);
		  }
		}

		int nEffectID = 0;
		for(int i = 0; i < pInfo->GetEffectNum(nCharID); i++)
		{
		  nEffectID = pInfo->sEffectID[nCharID][i] * 10 + Level;
		  pEffect	= pScene->GetFirstInvalidEffObj();

		  if(!pEffect) continue;

		  // 使用道具表里的dummy
		  if(!pEffect->Create(nEffectID))
		  {
			LG("ERROR", "msgcreate cha`s effect fail,ID %d", nEffectID);
			return strRes.c_str();
		  }
		  pEffect->setFollowObj((CSceneNode*)pItem, NODE_ITEM, pInfo->chDummy[i]);
		  pEffect->SetScale(EffectScale, EffectScale, EffectScale);
		  pEffect->SetAlpha(SItemForge::GetAlpha(Level));
		  pEffect->Emission(-1, NULL, NULL);
		  pEffect->SetValid(TRUE);
		  pItem->AddEffect(pEffect->getID());

		  //					LG( RES_STRING(CL_LANGUAGE_MATCH_128), "ID:%d, Dummy:%d\n", nEffectID, pInfo->chDummy[i] );
		}
	  }
	}
  } else if(strCmd == "forge")
  {
	CCharacter* pMain = CGameScene::GetMainCha();
	if(pMain)
	{
	  CSceneItem* pItem	 = NULL;
	  CGameScene* pScene = CGameApp::GetCurScene();
	  if(p1 == "left")
	  {
		pItem = pMain->GetHandItem(enumEQUIP_LHAND);
	  } else if(p1 == "right")
	  {
		pItem = pMain->GetHandItem(enumEQUIP_RHAND);
	  }

	  if(pItem)
	  {
		int nForge = Str2Int(p2);
		pItem->SetForgeEffect(nForge, pMain->getTypeID());
	  }
	}
  } else if(g_Config.m_bEditor && strCmd == "take")
  {
	CCharacter* pMain	= CGameScene::GetMainCha();
	int			nItemID = Str2Int(p2);
	if(pMain)
	{
	  if(p1 == "left")
	  {
		pMain->UpdataItem(nItemID, enumEQUIP_LHAND);
	  } else if(p1 == "right")
	  {
		pMain->UpdataItem(nItemID, enumEQUIP_RHAND);
	  }
	  pMain->RefreshItem(true);
	}
  } else if(g_Config.m_bEditor && strCmd == "ShowAllObjects")
  {
	CGameScene* pScene = g_pGameApp->GetCurScene();
	if(pScene)
	{
	  for(int i(1); i < CSceneObjSet::I()->GetLastID() + 1; i++)
	  {
		CSceneObj* pSceneObj = pScene->AddSceneObj(i);
		if(pSceneObj)
		  pSceneObj->SetValid(false);
	  }
	}

  } else if(strCmd == "write_hint")
  {
	if(!p1.empty())
	{
	  CItemCommand* pItem = dynamic_cast<CItemCommand*>(CGuiData::GetHintItem());
	  if(pItem)
	  {
		ofstream out(p1.c_str(), ios::app);
		out << "ID:" << pItem->GetItemInfo()->lID << endl;
	  }

	  int nLine = CCommandObj::GetHints().WriteText(p1.c_str());
	  if(nLine > 0)
	  {
		CTextHint::stHint* pHint = CCommandObj::GetHints().GetHint(0);
		g_pGameApp->SysInfo(RES_STRING(CL_LANGUAGE_MATCH_129), pHint->hint.c_str(), nLine);
	  }
	}
  } else if(strCmd == "repair")
  {
	extern void NetBeginRepairItem(void);
	NetBeginRepairItem();
  } else if(strCmd == "move")
  {
	extern int MOVE_LENGTH;
	MOVE_LENGTH = Str2Int(p1);
  } else if(strCmd == "luaGetStoneHint")
  {
	string hint = g_pGameApp->GetScriptMgr()->GetStoneHint(p1.c_str(), Str2Int(p2));
	g_pGameApp->SysInfo("Hint:%s", hint.c_str());
  } else if(strCmd == "luaFunc")
  {
	string str;
	if(g_pGameApp->GetScriptMgr()->DoString(p1.c_str(), "d-s", Str2Int(p2), &str))
	{
	  g_pGameApp->SysInfo("return:%s", str.c_str());
	}
  } else if(strCmd == "autotest")
  {
	g_pGameApp->GetConsole()->Show(FALSE);

	g_pGameApp->AutoTestInfo(RES_STRING(CL_LANGUAGE_MATCH_130));
	g_pGameApp->AutoTest();
	g_pGameApp->AutoTestInfo(RES_STRING(CL_LANGUAGE_MATCH_131));
  } else if(strCmd == "testeffect")
  {
	// 里边i乱用，看不懂了
	//g_pGameApp->GetConsole()->Show( FALSE );

	//CGameScene* pScene = g_pGameApp->GetCurScene();
	//if( !pScene ) return "";

	//D3DXVECTOR3 TestPos;
	//int nTestX = 0;
	//int nTestY = 0;
	//if( CGameScene::GetMainCha() )
	//{
	//	TestPos = CGameScene::GetMainCha()->GetPos();
	//	TestPos.x -= 3.0f;

	//	nTestX = CGameScene::GetMainCha()->GetCurX() - 300;
	//	nTestY = CGameScene::GetMainCha()->GetCurY();
	//}

	//int nStart = Str2Int( p1 );
	//int nEnd = Str2Int( p2 );
	//int nTestCount = Str2Int( p3 );
	//if( nTestCount<=0 ) nTestCount=1;
	//g_pGameApp->AutoTestInfo( RES_STRING(CL_LANGUAGE_MATCH_132), nStart, nEnd, nTestCount );

	//nEnd++;
	//for (int j(0); j<nTestCount; j++)
	//{
	//	//g_pGameApp->AutoTestInfo( "第%d轮测试...", j );

	//	// 正在测试所有特效
	//	{
	//		//g_pGameApp->AutoTestInfo( "正在测试所有特效" );
	//		int nCount = CEffectSet::I()->GetLastID() + 1;
	//		CEffectObj* pEffect = NULL;
	//		CMagicInfo* pInfo = NULL;

	//		string name;
	//		bool IsDel = false;
	//		for( int i=nStart; i<nEnd; i++ )
	//		{
	//			pInfo = GetMagicInfo(i);
	//			if(!pInfo) continue;

	//			IsDel = false;
	//			name = pInfo->szName;
	//			int n = (int)name.find( RES_STRING(CL_LANGUAGE_MATCH_118) );
	//			if( n >= 0 )
	//			{
	//				IsDel = true;
	//			}

	//			while( true )
	//			{
	//				//g_pGameApp->AutoTestUpdate();
	//				pEffect = pScene->GetFirstInvalidEffObj();
	//				if( pEffect )
	//				{
	//					break;
	//				}
	//				else
	//				{
	//					Sleep( 10 );
	//
	//					//g_pGameApp->AutoTestInfo( "特效空间已满，正在清除所有特效" );
	//					int nCount = pScene->GetInitParam()->nMaxEff;
	//					for( int i=0; i<nCount; i++ )
	//					{
	//						pEffect = pScene->GetEffect( i );
	//						if( pEffect )
	//						{
	//							pEffect->SetValid( FALSE );
	//						}
	//					}
	//				}
	//			}

	//			if( !pEffect->Create( i ) )
	//			{
	//				//g_pGameApp->AutoTestInfo( "Error, 特效不能创建:%d", i );
	//				continue;
	//			}

	//			D3DXVECTOR3 pos = TestPos;
	//			pos.x = pos.x + (float)(rand() % 10000) / 1000.0f - 5.0f;
	//			pos.y = pos.y + (float)(rand() % 10000) / 1000.0f - 5.0f;
	//			pEffect->Emission( -1, &pos, NULL );
	//			pEffect->SetValid( TRUE );

	//			//g_pGameApp->AutoTestUpdate();
	//			if( IsDel )
	//			{
	//				pEffect->SetValid( FALSE );
	//			}
	//		}
	//	}
	//}
	//g_pGameApp->AutoTestInfo( "开始测试结束" );
  } else if(strCmd == "testskilleffect")
  {
	// 检测技能表中的特效是否正常
	{
	  int			nCount		= CSkillRecordSet::I()->GetLastID() + 1;
	  CSkillRecord* _pSkillInfo = NULL;
	  for(int k = 0; k < nCount; k++)
	  {
		_pSkillInfo = GetSkillRecordInfo(k);
		if(!_pSkillInfo) continue;

		for(int i = 0; i < defSKILL_ACTION_EFFECT; i++)
		{
		  CheckSkillEffect(_pSkillInfo, _pSkillInfo->sActionEffect[i]);
		}

		CheckSkillEffect(_pSkillInfo, _pSkillInfo->sItemEffect1[0]);
		CheckSkillEffect(_pSkillInfo, _pSkillInfo->sItemEffect2[0]);

		CheckSkillEffect(_pSkillInfo, _pSkillInfo->sSkyEffect);

		CheckSkillEffect(_pSkillInfo, _pSkillInfo->sAgroundEffectID);
		CheckSkillEffect(_pSkillInfo, _pSkillInfo->sWaterEffectID);
		CheckSkillEffect(_pSkillInfo, _pSkillInfo->sTargetEffectID);
	  }

	  g_pGameApp->HasLogFile("skillinfoerror");
	}

	// 创建技能表中的特效
	g_pGameApp->GetConsole()->Show(FALSE);

	CGameScene* pScene = g_pGameApp->GetCurScene();
	if(!pScene) return "";

	D3DXVECTOR3 TestPos;
	int			nTestX = 0;
	int			nTestY = 0;
	if(CGameScene::GetMainCha())
	{
	  TestPos = CGameScene::GetMainCha()->GetPos();
	  TestPos.x -= 5.0f;

	  nTestX = CGameScene::GetMainCha()->GetCurX() - 300;
	  nTestY = CGameScene::GetMainCha()->GetCurY();
	} else
	{
	  return "";
	}

	int nStart	   = Str2Int(p1);
	int nEnd	   = Str2Int(p2);
	int nTestCount = Str2Int(p3);
	if(nTestCount <= 0) nTestCount = 1;
	g_pGameApp->AutoTestInfo(RES_STRING(CL_LANGUAGE_MATCH_133), nStart, nEnd, nTestCount);

	nEnd++;
	CSkillRecord* pInfo		  = NULL;
	CSkillRecord* _pSkillInfo = NULL;
	CSkillRecord* _pSkill	  = NULL;
	CCharacter*	  _pSelf	  = CGameScene::GetMainCha();
	CCharacter*	  _pTarget	  = _pSelf;
	CCharacter*	  _pAttack	  = _pSelf;
	drMatrix44	  mat;
	D3DXVECTOR3	  pos;
	int			  _nAttackX, _nAttackY;
	for(int k = nStart; k < nEnd; k++)
	{
	  pInfo = GetSkillRecordInfo(k);
	  if(!pInfo) continue;

	  _pSkillInfo = pInfo;

	  pos		= TestPos;
	  pos.x		= pos.x + (float)(rand() % 10000) / 1000.0f - 5.0f;
	  pos.y		= pos.y + (float)(rand() % 10000) / 1000.0f - 5.0f;
	  pos.z		= CGameApp::GetCurScene()->GetGridHeight(pos.x, pos.y);
	  _nAttackX = (int)(pos.x * 100.0f);
	  _nAttackY = (int)(pos.y * 100.0f);

	  for(int i = 0; i < defSKILL_ACTION_EFFECT; i++)
	  {
		if(_pSkillInfo->sActionEffect[i] > 0)
		{
		  if(_pSkillInfo->sApplyDistance == 0 && _pSkillInfo->sActionEffectType[i] == 1 && _pSkillInfo->GetShape() == enumRANGE_TYPE_CIRCLE)
		  {
			_pSelf->SelfEffect(_pSkillInfo->sActionEffect[i], _pSkillInfo->sActionDummyLink[i], false, _pSkillInfo->GetRange(), _pSelf->getYaw());
		  } else
		  {
			_pSelf->SelfEffect(_pSkillInfo->sActionEffect[i], _pSkillInfo->sActionDummyLink[i], false, -1, _pSelf->getYaw());
		  }
		} else
		{
		  break;
		}
	  }

	  if(_pSkillInfo->sItemEffect1[0] > 0) _pSelf->ItemEffect(_pSkillInfo->sItemEffect1[0], _pSkillInfo->sItemDummyLink, _pSelf->getYaw());
	  if(_pSkillInfo->sItemEffect2[0] > 0) _pSelf->ItemEffect(_pSkillInfo->sItemEffect2[0], _pSkillInfo->sItemDummyLink, _pSelf->getYaw());

	  if(_pSkillInfo->sSkyEffect != 0)
	  {
		int nTargetID = -1;

		if(_pTarget)
		{
		  nTargetID = _pTarget->getID();

		  if(_pSkillInfo->sTargetDummyLink >= 0 && _pTarget->GetObjDummyRunTimeMatrix(&mat, _pSkillInfo->sTargetDummyLink) >= 0)
		  {
			pos = *(D3DXVECTOR3*)&mat._41;
		  }
		}

		_pSelf->SkyEffect(_pSkillInfo->sSkyEffect, _pSkillInfo->sSkyEffectActionDummyLink, _pSkillInfo->sSkyEffectItemDummyLink, _pSkillInfo->sSkySpd, &pos, nTargetID, _pSkillInfo);
	  }

	  _pSkill = _pSkillInfo;
	  if(_pSkill->sAgroundEffectID > 0)
	  {
		CGameApp::GetCurScene()->CreateEffect(_pSkill->sAgroundEffectID, _nAttackX, _nAttackY);
	  }

	  if(_pSkill->sWaterEffectID > 0 && CGameApp::GetCurScene()->GetGridRegion(_nAttackX / 100, _nAttackY / 100))
	  {
		CGameApp::GetCurScene()->CreateEffect(_pSkill->sWaterEffectID, _nAttackX, _nAttackY);
	  }

	  int nAngle = 999;
	  if(_pAttack) nAngle = _pAttack->getYaw();
	  if(_pSkill->IsAttackArea())
	  {
		if(_pSkill->sTargetEffectID > 0) _pAttack->SelfEffect(_pSkill->sTargetEffectID, _pSkill->sTargetDummyLink, false, -1, nAngle);
	  } else if(_pTarget)
	  {
		_pTarget->HitEffect(nAngle);
		if(_pSkill->sTargetEffectID > 0) _pTarget->SelfEffect(_pSkill->sTargetEffectID, _pSkill->sTargetDummyLink, false, -1, nAngle);
	  }

	  g_pGameApp->AutoTestUpdate();
	}
  } else if(strCmd == "cha_effect")
  {
	CCharacter* pMain = CGameScene::GetMainCha();
	if(!pMain) return strRes.c_str();

	int nEffectID = Str2Int(p1);
	if(nEffectID <= 0) return strRes.c_str();

	int nDummy = Str2Int(p2);

	CEffectObj* pEffect = pMain->GetScene()->GetFirstInvalidEffObj();
	if(!pEffect) return strRes.c_str();
	;

	// 使用道具表里的dummy
	if(!pEffect->Create(nEffectID))
	{
	  LG("ERROR", "msgcreate cha`s effect fail,ID %d", nEffectID);
	  return strRes.c_str();
	}
	pEffect->setFollowObj((CSceneNode*)pMain, NODE_CHA, nDummy);
	pEffect->SetLoop(true);
	pEffect->Emission(-1, NULL, NULL);
	pEffect->SetValid(TRUE);
	pMain->AddEffect(pEffect->getID());
	return strRes.c_str();
  } else if(strCmd == "cha_clear")
  {
	CCharacter* pMain = CGameScene::GetMainCha();
	if(!pMain) return strRes.c_str();

	pMain->DieTime();
  } else if(strCmd == "copy")
  {
	if(!CGameScene::GetMainCha())
	  return strRes.c_str();

	CCharacter* pMain = CGameScene::GetMainCha();
	int			n	  = Str2Int(p1);
	if(n <= 0) n = 1;
	CGameScene* pScene = CGameApp::GetCurScene();

	int nCharID = pMain->getTypeID() - 1;

	CCharacter* pCha	 = NULL;
	CSceneItem* pItem	 = NULL;
	CSceneItem* pChaItem = NULL;
	for(int i = 0; i < n; i++)
	{
	  int nTestX = pMain->GetCurX() + rand() % 1000 - 500;
	  int nTestY = pMain->GetCurY() + rand() % 1000 - 500;

	  pCha = CGameApp::GetCurScene()->AddCharacter(pMain->getTypeID());
	  if(pCha)
	  {
		pItem = pMain->GetHandItem(enumEQUIP_LHAND);
		if(pItem)
		{
		  pCha->UpdataItem(pItem->GetItemInfo()->nID, LINK_ID_LEFTHAND);
		  pChaItem = pCha->GetHandItem(enumEQUIP_LHAND);
		  if(pChaItem)
		  {
			int RefineID				 = pItem->lTag / 10;
			int Level					 = pItem->lTag % 10;
			pChaItem->lTag				 = pItem->lTag;
			CItemRefineEffectInfo* pInfo = GetItemRefineEffectInfo(RefineID);
			if(pInfo)
			{
			  pItem = pChaItem;
			  if(pInfo->nLightID != 0)
				pItem->LitResetTexture(pInfo->nLightID, Level); // 加载流光
			  else
				pItem->LitUnresetTexture();

			  // 先取消之前的特效
			  int		  nCount  = pItem->GetEffectNum();
			  CEffectObj* pEffect = NULL;
			  for(int i = 0; i < nCount; i++)
			  {
				pEffect = pScene->GetEffect(pItem->GetEffectID(i));
				if(pEffect)
				{
				  pEffect->SetValid(FALSE);
				}
			  }

			  int nEffectID = 0;
			  for(int i = 0; i < pInfo->GetEffectNum(nCharID); i++)
			  {
				nEffectID = pInfo->sEffectID[nCharID][i] * 10 + Level;
				pEffect	  = pScene->GetFirstInvalidEffObj();

				if(!pEffect) continue;

				// 使用道具表里的dummy
				if(!pEffect->Create(nEffectID))
				{
				  LG("ERROR", "msgcreate cha`s effect fail,ID %d", nEffectID);
				  return strRes.c_str();
				}
				pEffect->setFollowObj((CSceneNode*)pItem, NODE_ITEM, pInfo->chDummy[i]);
				static float fScale[4] = {1.0f, 1.2f, 1.0f, 0.7f};
				pEffect->SetScale(fScale[nCharID], fScale[nCharID], fScale[nCharID]);
				pEffect->Emission(-1, NULL, NULL);
				pEffect->SetValid(TRUE);
				pItem->AddEffect(pEffect->getID());

				//								LG( RES_STRING(CL_LANGUAGE_MATCH_128), "ID:%d, Dummy:%d\n", nEffectID, pInfo->chDummy[i] );
			  }
			}
		  }
		}
		pItem = pMain->GetHandItem(enumEQUIP_RHAND);
		if(pItem)
		{
		  pCha->UpdataItem(pItem->GetItemInfo()->nID, LINK_ID_RIGHTHAND);
		  pChaItem = pCha->GetHandItem(LINK_ID_RIGHTHAND);
		  if(pChaItem)
		  {
			int RefineID				 = pItem->lTag / 10;
			int Level					 = pItem->lTag % 10;
			pChaItem->lTag				 = pItem->lTag;
			CItemRefineEffectInfo* pInfo = GetItemRefineEffectInfo(RefineID);
			if(pInfo)
			{
			  pItem = pChaItem;
			  if(pInfo->nLightID != 0)
				pItem->LitResetTexture(pInfo->nLightID, Level); // 加载流光
			  else
				pItem->LitUnresetTexture();

			  // 先取消之前的特效
			  int		  nCount  = pItem->GetEffectNum();
			  CEffectObj* pEffect = NULL;
			  for(int i = 0; i < nCount; i++)
			  {
				pEffect = pScene->GetEffect(pItem->GetEffectID(i));
				if(pEffect)
				{
				  pEffect->SetValid(FALSE);
				}
			  }

			  int nEffectID = 0;
			  for(int i = 0; i < pInfo->GetEffectNum(nCharID); i++)
			  {
				nEffectID = pInfo->sEffectID[nCharID][i] * 10 + Level;
				pEffect	  = pScene->GetFirstInvalidEffObj();

				if(!pEffect) continue;

				// 使用道具表里的dummy
				if(!pEffect->Create(nEffectID))
				{
				  LG("ERROR", "msgcreate cha`s effect fail,ID %d", nEffectID);
				  return strRes.c_str();
				}
				pEffect->setFollowObj((CSceneNode*)pItem, NODE_ITEM, pInfo->chDummy[i]);
				static float fScale[4] = {1.0f, 1.2f, 1.0f, 0.7f};
				pEffect->SetScale(fScale[nCharID], fScale[nCharID], fScale[nCharID]);
				pEffect->Emission(-1, NULL, NULL);
				pEffect->SetValid(TRUE);
				pItem->AddEffect(pEffect->getID());

				//								LG( RES_STRING(CL_LANGUAGE_MATCH_128), "ID:%d, Dummy:%d\n", nEffectID, pInfo->chDummy[i] );
			  }
			}
		  }
		}

		pCha->setYaw(rand());
		pCha->setPos(nTestX, nTestY);
		pCha->RefreshItem(true);
	  }
	}
  }else if(strCmd == "playpose")
  {
	CCharacter* pMain = CGameScene::GetMainCha();
	if(!pMain) return strRes.c_str();

	int	 nListID	= 1;
	int	 nPoseCount = Str2Int(strList[nListID++]);
	int	 pose[100]	= {0};
	char szBuf[50]	= {0};
	for(int i = 0; i < nPoseCount; i++)
	{
	  pose[i] = Str2Int(strList[nListID++]);
	}
	float fPoseSpeed = Str2Float(strList[nListID++]);

	CHitAttackState* pState = new CHitAttackState(pMain->GetActor());
	pState->SetPose(pose, nPoseCount);
	pState->SetSpeed(fPoseSpeed);
	pMain->GetActor()->AddState(pState);
  } else if(strCmd == "playallpose")
  {
	CCharacter* pMain = CGameScene::GetMainCha();
	if(!pMain) return strRes.c_str();

	int nStart = Str2Int(p1);
	int nEnd   = Str2Int(p2);

	CAllPoseState* pState = new CAllPoseState(pMain->GetActor());
	if(nStart == 0 || nEnd == 0)
	{
	  pState->SetChaRange(1, CChaRecordSet::I()->GetLastID() + 1);
	} else
	{
	  CCharacter* pCha = pMain->GetScene()->AddCharacter(nStart);
	  if(pCha)
	  {
		pCha->setPos(pMain->GetCurX(), pMain->GetCurY());
		pMain->SetValid(FALSE);
		pMain = pCha;
		pState->SetChaRange(nStart, nEnd);
	  }
	}
	pMain->GetActor()->AddState(pState);
  } else if(strCmd == "clear_state")
  {
	CCharacter* pMain = CGameScene::GetMainCha();
	if(!pMain) return strRes.c_str();

	if(pMain->GetActor()->GetCurState())
	  pMain->GetActor()->GetCurState()->PopState();
  } else if(strCmd == "exit")
  {
	exit(Str2Int(p1));
  } else if(strCmd == "showpath")
  {
	g_pGameApp->GetDrawPoints()->SetMaxCount(Str2Int(p1));
  } else if(strCmd == "client_move")
  {
	g_Config.m_IsMoveClient = Str2Int(p1) != 0;
  } else if(strCmd == "createcha")
  {
	int	  nStart  = Str2Int(p1);
	int	  nEnd	  = Str2Int(p2);
	DWORD dwStart = ::GetTickCount();
	int	  nTestX  = 0;
	int	  nTestY  = 0;
	if(CGameScene::GetMainCha())
	{
	  nTestX = CGameScene::GetMainCha()->GetCurX() - 300;
	  nTestY = CGameScene::GetMainCha()->GetCurY();
	}

	CGameScene* pScene = g_pGameApp->GetCurScene();
	CCharacter* pCha   = NULL;

	for(int i = nStart; i < nEnd; i++)
	{
	  pCha = pScene->AddCharacter(i);
	  if(pCha)
	  {
		pCha->setPos(nTestX, nTestY);
		nTestY -= 100;
	  }
	}
	g_pGameApp->SysInfo("Time:%d", ::GetTickCount() - dwStart);
	return "";

	CChaRecord* pInfo = NULL;
	int			nScriptID;
	BYTE		loadtex_flag;
	drIByteSet* res_bs = 0;
	for(int kkk = nStart; kkk < nEnd; kkk++)
	{
	  nScriptID = kkk;
	  pInfo		= GetChaRecordInfo(nScriptID);
	  if(!pInfo) continue;

	  pCha = pScene->GetCha(nScriptID);
	  if(!pCha) continue;

	  pCha->SetValid(FALSE);
	  pCha->Destroy();
	  pCha->setTypeID(nScriptID);
	  pCha->SetDefaultChaInfo(pInfo);
	  if(pInfo->chModalType == enumMODAL_MAIN_CHA)
	  {
		// save loading res mt flag
		res_bs		 = g_Render.GetInterfaceMgr()->res_mgr->GetByteSet();
		loadtex_flag = res_bs->GetValue(OPT_RESMGR_LOADTEXTURE_MT);
		res_bs->SetValue(OPT_RESMGR_LOADTEXTURE_MT, 0);

		// 读id的表格
		DWORD part_buf[5] =
			{
				pInfo->sSkinInfo[0],
				pInfo->sSkinInfo[1],
				pInfo->sSkinInfo[2],
				pInfo->sSkinInfo[3],
				pInfo->sSkinInfo[4],
			};

		if(((CCharacterModel*)pCha)->LoadCha(pInfo->chModalType, pInfo->sModel, part_buf) == 0)
		{
		  LG("error", RES_STRING(CL_LANGUAGE_MATCH_26), nScriptID, pInfo->szDataName);
		  continue;
		}
	  } else if(pInfo->chModalType == enumMODAL_BOAT)
	  {
		DWORD part_buf[3] =
			{
				pInfo->sSkinInfo[0],
				pInfo->sSkinInfo[1],
				pInfo->sSkinInfo[2],
			};

		if(((CCharacterModel*)pCha)->LoadShip(pInfo->chModalType, pInfo->sModel, part_buf) == 0)
		{
		  LG("error", RES_STRING(CL_LANGUAGE_MATCH_26), nScriptID, pInfo->szDataName);
		  continue;
		}
	  } else if(pInfo->chModalType == enumMODAL_EMPL)
	  {
		DWORD part_buf[5] =
			{
				pInfo->sSkinInfo[0],
				pInfo->sSkinInfo[1],
				pInfo->sSkinInfo[2],
				pInfo->sSkinInfo[3],
			};

		if(((CCharacterModel*)pCha)->LoadTower(pInfo->chModalType, part_buf) == 0)
		{
		  LG("error", RES_STRING(CL_LANGUAGE_MATCH_26), nScriptID, pInfo->szDataName);
		  continue;
		}
	  } else if(pInfo->chModalType == enumMODAL_OTHER)
	  {
		// 主角创建
		LEChaLoadInfo load_info;

		_snprintf_s(load_info.bone, _countof(load_info.bone), _TRUNCATE, "%04d.lab", pInfo->sModel);

		for(DWORD i = 0; i < 5; i++)
		{
		  if(pInfo->sSkinInfo[i] != 0)
		  {
			DWORD file_id = pInfo->sModel * 1000000 + pInfo->sSuitID * 10000 + i;
			_snprintf_s(load_info.part[i], _countof(load_info.part[i]), _TRUNCATE, "%010d.lgo", file_id);
		  }
		}

		if(((CCharacterModel*)pCha)->LoadCha(&load_info) == 0)
		{
		  LG("error", RES_STRING(CL_LANGUAGE_MATCH_26), nScriptID, pInfo->szDataName);
		  continue;
		}
	  }
	  if(((CCharacterModel*)pCha)->LoadPose(pInfo->sActionID) == 0)
	  {
		LG("error", RES_STRING(CL_LANGUAGE_MATCH_27), nScriptID, pInfo->szDataName);
		continue;
	  }
	  pCha->SetValid(TRUE);
	  pCha->setPos(nTestX, nTestY);
	  nTestY -= 100;
	}
	g_pGameApp->SysInfo("Time:%d", ::GetTickCount() - dwStart);
  } else if(strCmd == "cam1")
  {
	g_pGameApp->GetMainCam()->m_fxy = (float)atof(p1.c_str());
  } else if(strCmd == "cam2")
  {
	g_pGameApp->GetMainCam()->m_fz = (float)atof(p1.c_str());
  } else if(strCmd == "cam3")
  {
	g_pGameApp->GetMainCam()->m_ffov = (float)atof(p1.c_str());
  } else if(strCmd == "cam4")
  {
	g_pGameApp->GetMainCam()->m_fAngle = (float)atof(p1.c_str());
  } else if(strCmd == "cam5")
  {
	g_pGameApp->GetMainCam()->m_iShowWidth = atoi(p1.c_str());
  } else
  {
	return HandleMonsterCommand(strCmd, p1, p2);
  }
  //T_REPORT
  return strRes.c_str();
}

BOOL CGameApp::IsEnableSuperKey()
{
  return _bEnableSuperKey;
}
