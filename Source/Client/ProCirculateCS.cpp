#include "StdAfx.h"
#include "procirculate.h"
#include "GameApp.h"
#include "PacketCmd.h"
#include "Character.h"
#include "ChaAttr.h"
#include "GameConfig.h"
#include "util2.h"
#include "LoginScene.h"
#include "netprotocol.h"

_DBC_USING

// Э��C->S : �����ж���Ϣ
void CProCirculateCS::BeginAction(CCharacter* pCha, DWORD type, void* param, CActionState* pState)
{
  WPacket pk = pCNetIf->GetWPacket();
  pk.WriteCmd(CMD_CM_BEGINACTION);
  pk.WriteLong(pCha->getAttachID());

  //  char	szLogName[1024] = { "BeginAction" };

  //  if( pCha )
  //  {
  //      //strcpy(szLogName, pCha->getLogName() );
  // 		strncpy_s(szLogName,sizeof(szLogName), pCha->getLogName(),_TRUNCATE );
  //  }

  try
  {
	//		LG(szLogName, "$$$PacketID:\t%d\n", pCNetIf->m_ulPacketCount);

#ifdef defPROTOCOL_HAVE_PACKETID
	pk.WriteLong(pCNetIf->m_ulPacketCount++);
#endif
	pk.WriteChar((dbc::uChar)type);
	switch(type)
	{
	case enumACTION_MOVE:
	{
	  stNetMoveInfo* pMove = (stNetMoveInfo*)param;
	  pk.WriteSequence((cChar*)pMove->pos_buf, uShort(sizeof(Point) * pMove->pos_num));
	  pCNetIf->SendPacketMessage(pk);

	  // 				LG(szLogName, "###Send(Move):\tTick:[%d]\n", GetTickCount());
	  // 				LG(szLogName, "Ping:\t%3d\n", pMove->dwAveragePing);
	  // 				LG(szLogName, "Point:\t%3d\n", pMove->pos_num);
	  // 				for (DWORD i = 0; i < pMove->pos_num; i++)
	  // 				{
	  // 					LG(szLogName, "\t%d, \t%d\n", pMove->pos_buf[i].x, pMove->pos_buf[i].y);
	  // 				}
	  // 				LG(szLogName, "\n");
	  break;
	}
	case enumACTION_SKILL:
	{
	  stNetSkillInfo* pSkill = (stNetSkillInfo*)param;
	  pk.WriteChar(pSkill->chMove);
	  pk.WriteChar(pSkill->byFightID);
	  if(pSkill->chMove == 2)
	  {
		pk.WriteSequence((cChar*)pSkill->SMove.pos_buf, uShort(sizeof(POINT) * pSkill->SMove.pos_num));
	  }
	  pk.WriteLong(pSkill->lSkillID);
	  pk.WriteLong(pSkill->lTarInfo1);
	  pk.WriteLong(pSkill->lTarInfo2);

	  //for (int n = 0; n < 200; ++n)
	  //{
	  pCNetIf->SendPacketMessage(pk);
	  //}

	  // 				LG(szLogName, "###Send(Skill):\tTick:[%d]\n", GetTickCount());
	  // 				if (pSkill->chMove == 2)
	  // 				{
	  // 					LG(szLogName, "Ping:\t%3d\n", pSkill->SMove.dwAveragePing);
	  // 					LG(szLogName, "Point:\t%3d\n", pSkill->SMove.pos_num);
	  // 					for (DWORD i = 0; i < pSkill->SMove.pos_num; i++)
	  // 					{
	  // 						LG(szLogName, "\t%d, \t%d\n", pSkill->SMove.pos_buf[i].x, pSkill->SMove.pos_buf[i].y);
	  // 					}
	  // 				}
	  // 				LG(szLogName, "Skill:\t%3d, FightID:%d\n", pSkill->lSkillID, pSkill->byFightID);
	  // 				LG(szLogName, "Target:\t%u, \t%u\n", pSkill->lTarInfo1, pSkill->lTarInfo2);
	  // 				LG(szLogName, "\n");
	  break;
	}
	case enumACTION_STOP_STATE:
	{
	  pk.WriteShort(*((short*)param));
	  pCNetIf->SendPacketMessage(pk);

	  // log
	  // 				LG(szLogName, "###Send(Stop Skill State %d):\tTick:[%d]\n", *((short *)param), GetTickCount());
	  // 				LG(szLogName, "\n");
	  //
	  break;
	}
	case enumACTION_LEAN: // �п�
	{
	  stNetLeanInfo* pSLean = (stNetLeanInfo*)param;
	  pk.WriteLong(pSLean->lPose);
	  pk.WriteLong(pSLean->lAngle);
	  pk.WriteLong(pSLean->lPosX);
	  pk.WriteLong(pSLean->lPosY);
	  pk.WriteLong(pSLean->lHeight);
	  pCNetIf->SendPacketMessage(pk);

	  // log
	  // 				LG(szLogName, "###Send(Lean):\tTick:[%d]\n", GetTickCount());
	  // 				LG(szLogName, "\n");
	  //
	  break;
	}
	case enumACTION_ITEM_PICK: // �����
	{
	  stNetItemPick* pPick = (stNetItemPick*)param;
	  pk.WriteLong(pPick->lWorldID);
	  pk.WriteLong(pPick->lHandle);
	  pCNetIf->SendPacketMessage(pk);

	  //              LG(szLogName, "###Send(Pick):\tTick:[%d]\n", GetTickCount());
	  //              LG(szLogName, "\n");
	  break;
	}
	case enumACTION_TOTAL_ITEM_PICK:
	{
	  vector<stNetItemPick>*		  pPickinfo = (vector<stNetItemPick>*)param;
	  vector<stNetItemPick>::iterator iter		= pPickinfo->begin();
	  pk.WriteShort((uShort)(*pPickinfo).size());
	  for(iter; iter != pPickinfo->end(); ++iter)
	  {
		pk.WriteLong(iter->lWorldID);
		pk.WriteLong(iter->lHandle);
	  }
	  pCNetIf->SendPacketMessage(pk);
	  break;
	}
	case enumACTION_ITEM_THROW: // ������
	{
	  stNetItemThrow* pThrow = (stNetItemThrow*)param;
	  pk.WriteShort(pThrow->sGridID);
	  pk.WriteShort((short)pThrow->lNum);
	  pk.WriteLong(pThrow->lPosX);
	  pk.WriteLong(pThrow->lPosY);
	  pCNetIf->SendPacketMessage(pk);

	  //              LG(szLogName, "###Send(Throw):\tTick:[%d]\n", GetTickCount());
	  //              LG(szLogName, "\n");
	  break;
	}
	case enumACTION_ITEM_USE:
	{
	  stNetUseItem* pUseItem = (stNetUseItem*)param;
	  pk.WriteShort(pUseItem->sGridID);
	  pk.WriteShort(pUseItem->sTarGridID);
	  pCNetIf->SendPacketMessage(pk);

	  //              LG(szLogName, "###Send(Use Item):\tTick:[%d]\n", GetTickCount());
	  // 				LG(szLogName, RES_STRING(CL_LANGUAGE_MATCH_322), pUseItem->sGridID, pUseItem->sTarGridID);
	  //              LG(szLogName, "\n");
	  break;
	}
	case enumACTION_ITEM_UNFIX: // жװ����
	{
	  stNetItemUnfix* pUnfix = (stNetItemUnfix*)param;
	  pk.WriteChar(pUnfix->chLinkID);
	  pk.WriteShort(pUnfix->sGridID);
	  if(pUnfix->sGridID < 0) // ��������
	  {
		pk.WriteLong(pUnfix->lPosX);
		pk.WriteLong(pUnfix->lPosY);
	  }
	  pCNetIf->SendPacketMessage(pk);

	  //              LG(szLogName, "###Send(Unfix):\tTick:[%d]\n", GetTickCount());
	  //              LG(szLogName, "\n");
	  break;
	}
	case enumACTION_ITEM_POS:
	{
	  stNetItemPos* pChangePos = (stNetItemPos*)param;
	  pk.WriteShort(pChangePos->sSrcGridID);
	  pk.WriteShort(pChangePos->sSrcNum);
	  pk.WriteShort(pChangePos->sTarGridID);
	  pCNetIf->SendPacketMessage(pk);

	  //              LG(szLogName, "###Send(Item pos):\tTick:[%d]\n", GetTickCount());
	  //              LG(szLogName, "\n");
	  break;
	}
	case enumACTION_ITEM_DELETE:
	{
	  stNetDelItem* pDelItem = (stNetDelItem*)param;
	  pk.WriteShort(pDelItem->sGridID);
	  pCNetIf->SendPacketMessage(pk);

	  //              LG(szLogName, "###Send(Del Item):\tTick:[%d]\n", GetTickCount());
	  // 				LG(szLogName, RES_STRING(CL_LANGUAGE_MATCH_323), pDelItem->sGridID);
	  //              LG(szLogName, "\n");
	  break;
	}
	case enumACTION_ITEM_INFO:
	{
	  stNetItemInfo* pItemInfo = (stNetItemInfo*)param;
	  pk.WriteChar(pItemInfo->chType);
	  pk.WriteShort(pItemInfo->sGridID);
	  pCNetIf->SendPacketMessage(pk);

	  //              LG(szLogName, "###Send(Item Info):\tTick:[%d]\n", GetTickCount());
	  // 				LG(szLogName, RES_STRING(CL_LANGUAGE_MATCH_324), pItemInfo->chType, pItemInfo->sGridID);
	  //              LG(szLogName, "\n");
	  break;
	}
	case enumACTION_SHORTCUT: // ���¿����
	{
	  stNetShortCutChange* pShortcutChange = (stNetShortCutChange*)param;
	  pk.WriteChar(pShortcutChange->chIndex);
	  pk.WriteChar(pShortcutChange->chType);
	  pk.WriteShort(pShortcutChange->shyGrid);
	  pCNetIf->SendPacketMessage(pk);
	  break;
	}
	case enumACTION_LOOK: // ������ۣ��紬�Ļ�װ��
	{
	  stNetChangeChaPart* pSChaPart = (stNetChangeChaPart*)param;
	  pk.WriteShort(pSChaPart->sTypeID);
	  for(int i = 0; i < enumEQUIP_NUM; i++)
	  {
		pk.WriteShort(pSChaPart->SLink[i].sID);
	  }
	  pCNetIf->SendPacketMessage(pk);

	  //              LG(szLogName, "###Send(Look):\tTick:[%d]\n", GetTickCount());
	  //              LG(szLogName, "\n");
	  break;
	}
	case enumACTION_TEMP: //��ʱ��װЭ��
	{
	  stTempChangeChaPart* pSTempChaPart = (stTempChangeChaPart*)param;
	  pk.WriteLong(pSTempChaPart->dwItemID);
	  pk.WriteLong(pSTempChaPart->dwPartID);
	  pCNetIf->SendPacketMessage(pk);

	  //              LG(szLogName, "###Send(Temp):\tTick:[%d]\n", GetTickCount());
	  //              LG(szLogName, "\n");
	  break;
	}
	case enumACTION_EVENT: // �����¼�
	{
	  stNetActivateEvent* pEvent = (stNetActivateEvent*)param;
	  pk.WriteLong(pEvent->lTargetID);
	  pk.WriteLong(pEvent->lHandle);
	  pk.WriteShort(pEvent->sEventID);
	  pCNetIf->SendPacketMessage(pk);

	  // log
	  // 				LG(szLogName, "###Send(Event):\tTick:[%d]\n", GetTickCount());
	  // 				LG(szLogName, "\n");
	  //
	  break;
	}
	case enumACTION_FACE:
	{
	  stNetFace* pNetFace = (stNetFace*)param;
	  pk.WriteShort(pNetFace->sAngle);
	  pk.WriteShort(pNetFace->sPose);
	  pCNetIf->SendPacketMessage(pk);

	  //              LG(szLogName, "###Send(Face):\tTick:[%d]\n", GetTickCount());
	  //              LG(szLogName, "\n");
	  break;
	}
	case enumACTION_SKILL_POSE:
	{
	  stNetFace* pNetFace = (stNetFace*)param;
	  pk.WriteShort(pNetFace->sAngle);
	  pk.WriteShort(pNetFace->sPose);
	  pCNetIf->SendPacketMessage(pk);

	  //              LG(szLogName, "###Send(Skill Pos):\tTick:[%d]\n", GetTickCount());
	  // 				LG(szLogName, "Angle:%d, Pose:%d\n", pNetFace->sAngle, pNetFace->sPose );
	  break;
	}
	case enumACTION_BANK:
	{
	  stNetBank* pNetBank = (stNetBank*)param;
	  pk.WriteChar(pNetBank->chSrcType);
	  pk.WriteShort(pNetBank->sSrcID);
	  pk.WriteShort(pNetBank->sSrcNum);
	  pk.WriteChar(pNetBank->chTarType);
	  pk.WriteShort(pNetBank->sTarID);
	  pCNetIf->SendPacketMessage(pk);

	  //              LG(szLogName, "###Send(Bank Req):\tTick:[%d]\n", GetTickCount());
	  //              LG(szLogName, "\n");
	  break;
	}
	case enumACTION_CLOSE_BANK:
	{
	  pCNetIf->SendPacketMessage(pk);

	  //              LG(szLogName, "###Send(Bank Close):\tTick:[%d]\n", GetTickCount());
	  //              LG(szLogName, "\n");
	  break;
	}
	case enumACTION_KITBAGTMP_DRAG: // �϶���ʱ����
	{
	  stNetTempKitbag* pNetTempKitbag = (stNetTempKitbag*)param;

	  pk.WriteShort(pNetTempKitbag->sSrcGridID);
	  pk.WriteShort(pNetTempKitbag->sSrcNum);
	  pk.WriteShort(pNetTempKitbag->sTarGridID);

	  pCNetIf->SendPacketMessage(pk);
	  break;
	}

	default:
	  break;
	}
  } catch(...)
  {
	MessageBox(0, "!!!!!!!!!!!!!!!!!!!!exception: Begin Action", "error", 0);
  }
}

// Э��C->S : ����ֹͣ�ж���Ϣ
void CProCirculateCS::EndAction(CActionState* pState)
{
  WPacket pk = pCNetIf->GetWPacket();

  pk.WriteCmd(CMD_CM_ENDACTION); //�����ж�
  pCNetIf->SendPacketMessage(pk);

  // log
  //  CCharacter *pMainCha = CGameApp::GetCurScene()->GetMainCha();
  // 	if (pMainCha)
  // 	{
  // 		LG(pMainCha->getLogName(), "###Send(EndAction):\tTick:[%d]\n", GetTickCount());
  // 	}
  //
}

bool CProCirculate::Connect(const char* hostname, unsigned short port, unsigned long timeout)
{
  return pCNetIf->m_connect.Connect(hostname, port, timeout);
}
void CProCirculate::Disconnect(int reason)
{
  pCNetIf->m_connect.Disconnect(reason);
}

void CProCirculate::Login(const char* accounts, const char* password, const char* passport)
{
  strncpy_s(g_NetIF->m_accounts, sizeof(g_NetIF->m_accounts), accounts, _TRUNCATE);
  strncpy_s(g_NetIF->m_passwd, sizeof(g_NetIF->m_passwd), password, _TRUNCATE);

  extern short g_sClientVer;
  WPacket	   pk = pCNetIf->GetWPacket();

  pk.WriteCmd(CMD_CM_LOGIN);
  //Add by sunny.sun 20081223

  WritePacketSequenceEncrypted(pk, g_NetIF->m_AESKey, (uint8_t*)g_NetIF->m_passwd, strlen(g_NetIF->m_passwd));

  string szLocale = CResourceBundleManage::Instance()->GetLocale().getName();
  pk.WriteString(szLocale.c_str());

  pk.WriteString(passport);
  pk.WriteString(accounts);

  //char szBuf[33]={ 0 };
  //md5string(password, szBuf);
  //pk.WriteString(szBuf);
  string strMac = GetMacString();
  if(strMac.empty()) strMac = "Unknown";
  pk.WriteString(strMac.c_str());
  pk.WriteShort(g_sClientVer);


  pCNetIf->SendPacketMessage(pk);
}

void CProCirculate::Logout()
{
  WPacket pk = pCNetIf->GetWPacket();
  pk.WriteCmd(CMD_CM_LOGOUT);
  pCNetIf->SyncSendPacketMessage(pk, 3000);
}

void CProCirculate::BeginPlay(const char* cha)
{
  WPacket pk = pCNetIf->GetWPacket();
  pk.WriteCmd(CMD_CM_BGNPLAY);
  pk.WriteString(cha);

  pCNetIf->SendPacketMessage(pk);
}

void CProCirculate::EndPlay()
{
  WPacket pk = pCNetIf->GetWPacket();
  pk.WriteCmd(CMD_CM_ENDPLAY);
  pCNetIf->SendPacketMessage(pk);
}
void CProCirculate::NewCha(const char* chaname, const char* birth, LOOK& look)
{
  WPacket pk = pCNetIf->GetWPacket();
  pk.WriteCmd(CMD_CM_NEWCHA);
  pk.WriteString(chaname);
  pk.WriteString(birth);
  pk.WriteSequence(reinterpret_cast<char*>(&look), sizeof(LOOK));

  pCNetIf->SendPacketMessage(pk);
}
void CProCirculate::DelCha(const char* cha, const char szPassword2[])
{
  char szMD5[33] = {0};
  md5string(szPassword2, szMD5);

  WPacket pk = pCNetIf->GetWPacket();
  pk.WriteCmd(CMD_CM_DELCHA);
  pk.WriteString(cha);
  WritePacketSequenceEncrypted(pk, pCNetIf->m_AESKey, (uint8_t*)szPassword2, strlen(szMD5) + 1);
  pCNetIf->SendPacketMessage(pk);
}

//------------------------
// Э��C->S : ����������Ϣ
//------------------------
void CProCirculate::Say(const char* content)
{
  WPacket pk = pCNetIf->GetWPacket();
  pk.WriteCmd(CMD_CM_SAY); //����
  pk.WriteSequence(content, uShort(strlen(content)) + 1);

  pCNetIf->SendPacketMessage(pk);
}

//----------------------------
// Э��C->S : ���ͷ������Ե�����
//----------------------------
void CProCirculate::SynBaseAttribute(CChaAttr* pCAttr)
{
  char chAttrNum = 0;
  for(int i = ATTR_STR; i <= ATTR_LUK; i++)
	if(pCAttr->GetChangeBitFlag(i))
	  chAttrNum++;

  if(chAttrNum == 0)
	return;

  WPacket pk = pCNetIf->GetWPacket();
  pk.WriteCmd(CMD_CM_SYNATTR); //ͬ����������
  pk.WriteChar(chAttrNum);
  for(int i = ATTR_STR; i <= ATTR_LUK; i++)
  {
	if(pCAttr->GetChangeBitFlag(i))
	{
	  pk.WriteShort(i);
	  pk.WriteLong((uLong)pCAttr->GetAttr(i));
	}
  }

  //// log
  //char szReqChangeAttr[256] = {0};
  ////strcpy(szReqChangeAttr, RES_STRING(CL_LANGUAGE_MATCH_325));
  //strncpy_s(szReqChangeAttr,sizeof(szReqChangeAttr), RES_STRING(CL_LANGUAGE_MATCH_325),_TRUNCATE);
  //
  //for (int i = ATTR_STR; i <= ATTR_LUK; i++)
  //{
  //	if (pCAttr->GetChangeBitFlag(i))
  //		LG(szReqChangeAttr, RES_STRING(CL_LANGUAGE_MATCH_326), i, pCAttr->GetAttr(i));
  //}
  //LG(szReqChangeAttr, "\n");

  pCNetIf->SendPacketMessage(pk);
}

//----------------------------
// Э��C->S : ����ˢ�½�ɫ���Ե�����
//----------------------------
void CProCirculate::RefreshChaData(long lWorldID, long lHandle)
{
  WPacket pk = pCNetIf->GetWPacket();
  pk.WriteCmd(CMD_CM_REFRESH_DATA); //ͬ����������
  pk.WriteLong(lWorldID);
  pk.WriteLong(lHandle);

  pCNetIf->SendPacketMessage(pk);
}

void CProCirculate::SkillUpgrade(short sSkillID, char chAddLv)
{
  WPacket pk = pCNetIf->GetWPacket();
  pk.WriteCmd(CMD_CM_SKILLUPGRADE); //ͬ����������
  pk.WriteShort(sSkillID);
  pk.WriteChar(chAddLv);

  //// log
  //char szReqChangeAttr[256] = {0};
  ////strcpy(szReqChangeAttr, RES_STRING(CL_LANGUAGE_MATCH_327));
  //strncpy_s(szReqChangeAttr,sizeof(szReqChangeAttr), RES_STRING(CL_LANGUAGE_MATCH_327),_TRUNCATE);

  //LG(szReqChangeAttr, RES_STRING(CL_LANGUAGE_MATCH_328), sSkillID, chAddLv);

  pCNetIf->SendPacketMessage(pk);
}

//-----------------------------------
//	Э��C->S : ��������һ����������
//-----------------------------------
void CProCirculate::RequestLeaveExp(char IsGet, int LeaveTime, int Position, int putNum)
{
  WPacket pk = pCNetIf->GetWPacket();
  pk.WriteCmd(CMD_CM_SELECT_TIME_ASK);
  pk.WriteChar(IsGet);
  pk.WriteShort(LeaveTime);
  pk.WriteShort(Position);
  pk.WriteShort(putNum);
  pCNetIf->SendPacketMessage(pk);
}

void CProCirculate::SendCoinInfo(int Position, int sNum)
{
  WPacket pk = pCNetIf->GetWPacket();
  pk.WriteCmd(CMD_CM_SELECT_COIN_ASK);
  pk.WriteShort(Position);
  pk.WriteShort(sNum);
  pCNetIf->SendPacketMessage(pk);
}
void CProCirculate::TradeWithYourSelf(BYTE byPageID, BYTE byIndex)
{
  WPacket packet = g_NetIF->GetWPacket();
  packet.WriteCmd(CMD_CM_REQUEST_ANYTIMETRADE);
  packet.WriteShort(CMD_CM_FUNCITEM);
  packet.WriteChar(byPageID);
  packet.WriteChar(byIndex);

  g_NetIF->SendPacketMessage(packet);
}

//	Rain.
void CProCirculate::InviteInGuild(const char* InviteName)
{
  WPacket packet = g_NetIF->GetWPacket();
  packet.WriteCmd(CMD_CP_GUILD_INVITE);
  packet.WriteString(InviteName);

  g_NetIF->SendPacketMessage(packet);
}

void CProCirculate::AcceptGuildInviteConfirm(unsigned long chaid)
{
  WPacket l_wpk = g_NetIF->GetWPacket();
  l_wpk.WriteCmd(CMD_CP_GUILD_ACCEPT);
  l_wpk.WriteLong(chaid);
  g_NetIF->SendPacketMessage(l_wpk);
}

void CProCirculate::RefuseGuildInviteConfirm(unsigned long chaid)
{
  WPacket l_wpk = g_NetIF->GetWPacket();
  l_wpk.WriteCmd(CMD_CP_GUILD_REFUSE);
  l_wpk.WriteLong(chaid);
  g_NetIF->SendPacketMessage(l_wpk);
}