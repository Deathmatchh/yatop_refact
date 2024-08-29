// CharTrade.cpp Created by knight-gongjian 2004.12.7.
//---------------------------------------------------------

#include "CharTrade.h"
#include "GameApp.h"
#include "GameAppNet.h"
#include "SubMap.h"
#include "Player.h"
#include "GameDB.h"
#include "lua_gamectrl.h"
//---------------------------------------------------------

mission::CTradeSystem g_TradeSystem;

mission::CStoreSystem g_StoreSystem;
extern BOOL Cmd_ChangeFlatForTradeFailed( DWORD iCslNum, DWORD dcslnum, int type, char* act_name, DWORD cha_id, char * actName, DWORD  chaID  );
namespace mission
{
	//	2008-8-27	yangyinyu	add	begin!
	void	lgtool_printTrade(	CCharacter*	pRequest,	CCharacter*	pAccept,	CTradeData*	pTradeData	)
	{
		//	�ַ�����
		static	char	sText[512];
		static	char	sTextEx[512];

		//	*
		//_snprintf(	sText,	512,	"@%s;%d;",	pRequest->GetName(),	pTradeData->ReqTradeData.dwMoney	);
		_snprintf_s(	sText,sizeof(sText),_TRUNCATE,	"@%s;%d;",	pRequest->GetName(), pTradeData->ReqTradeData.dwMoney );
		bool	once	=	true;
		for(	int	i	=	0;	i	<	ROLE_MAXNUM_TRADEDATA;	i	++	)
		{
			if(	pTradeData->ReqTradeData.ItemArray[i].sItemID	!=	0	)
			{
				CItemRecord*	cir	=	GetItemRecordInfo(	pTradeData->ReqTradeData.ItemArray[i].sItemID	);

				if(	cir	)
				{
					//
					if(	once	==	false	)
						//strncat(	sText,	",",	256	);	
						strncat_s(	sText,sizeof(sText)	,",", _TRUNCATE	);	
					else
						once	=	false;

					//
					//_snprintf(	sTextEx,	512,"%s-%d",	cir->szName,	pTradeData->ReqTradeData.ItemArray[i].byCount	);
					_snprintf_s(	sTextEx,sizeof(sTextEx),_TRUNCATE,"%s-%d",	cir->szName,	pTradeData->ReqTradeData.ItemArray[i].byCount	);
					//strncat(	sText,	sTextEx,	512	);
					strncat_s( sText,sizeof(sText), sTextEx,_TRUNCATE);
				};
			};
		};


		//
		once	=	true;
		//_snprintf(	sTextEx,	512,	";%s;%d;",	pAccept->GetName(),	pTradeData->AcpTradeData.dwMoney	);
		_snprintf_s(	sTextEx,sizeof(sTextEx),_TRUNCATE,";%s;%d;",	pAccept->GetName(),	pTradeData->AcpTradeData.dwMoney	);
		//strncat(	sText,	sTextEx,	512	);
		strncat_s( sText,sizeof(sText), sTextEx,_TRUNCATE);
		for(	int	i	=	0;	i	<	ROLE_MAXNUM_TRADEDATA;	i	++	)
		{
			if(	pTradeData->AcpTradeData.ItemArray[i].sItemID	!=	0	)
			{
				CItemRecord*	cir	=	GetItemRecordInfo(	pTradeData->AcpTradeData.ItemArray[i].sItemID	);

				if(	cir	)
				{
					//
					if(	once	==	false	)
						//strncat(	sText,	",",	512	);
						strncat_s( sText,sizeof(sText), ",",_TRUNCATE);
					else
						once	=	false;

					//
					//_snprintf(	sTextEx,	512,"%s-%d",	cir->szName,	pTradeData->AcpTradeData.ItemArray[i].byCount	);
					_snprintf_s(	sTextEx,sizeof(sTextEx),_TRUNCATE,"%s-%d",	cir->szName,	pTradeData->AcpTradeData.ItemArray[i].byCount	);
					//strncat(	sText,	sTextEx,	512	);
					strncat_s( sText,sizeof(sText), sTextEx,_TRUNCATE);
				};
			};
		};

		//strncat(sText,	"\n",	512	);
		strncat_s( sText,sizeof(sText), "\n",_TRUNCATE);

		//
		LG("query_trade",	sText	);
	};
	//	2008-8-27	yangyinyu	add	end!

	//----------------------------------------------------
	// CTradeData implemented

	CTradeData::CTradeData(dbc::uLong lSize)
	: PreAllocStru(1)
	{T_B

	T_E}

	CTradeData::~CTradeData()
	{T_B

	T_E}

	//----------------------------------------------------
	// CTradeSystem implemented

	CTradeSystem::CTradeSystem()
	{T_B

	T_E}

	CTradeSystem::~CTradeSystem()
	{T_B

	T_E}

	// ���ײ���
	BOOL CTradeSystem::Request( BYTE byType, CCharacter& character, DWORD dwAcceptID )
	{T_B
		if(character.GetPlyMainCha()->IsStoreEnable())
		{
			//character.SystemNotice("�޷�����!");
			character.SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00001));
			return FALSE;
		}

		if( character.GetBoat() )
		{
			character.SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00002) );
			return FALSE;
		}

		if( character.GetStallData() )
		{
			//character.SystemNotice( "���ڰ�̯,�����Խ���" );
			character.SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00003) );
			return FALSE;
		}

		//add by ALLEN 2007-10-16
		if(character.IsReadBook())
		{
			//character.SystemNotice("���ڶ���,�����Խ���");
			character.SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00004));
			return FALSE;
		}

		if( character.GetKitbag()->IsLock() || !character.GetActControl(enumACTCONTROL_ITEM_OPT) )
		{
			//character.SystemNotice( "�����ѱ�����,�����Խ��ף�" );
			character.SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00005) );
			return FALSE;
		}

		if( character.GetPlyMainCha() && character.GetPlyMainCha()->GetKitbag()->IsLock() )
		{
			//character.SystemNotice( "�����ѱ�����,�����Խ��ף�" );
			character.SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00005) );
			return FALSE;
		}

        if( character.GetPlyMainCha() && character.GetPlyMainCha()->GetKitbag()->IsPwdLocked() )
        {
            //character.SystemNotice( "�����ѱ���������,�����Խ��ף�" );
			character.SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00006) );
			return FALSE;
        }

		CCharacter* pMain = &character;
		CCharacter* pChar = pMain->GetSubMap()->FindCharacter( dwAcceptID, pMain->GetShape().centre );
		if( pChar == NULL ) 
		{
			//pMain->SystemNotice( "����������Ѿ��뿪!" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00007) );
			return FALSE;
		}

        if(pChar->GetPlayer()->GetBankNpc())
        {
            //pMain->SystemNotice( "�Է�����ʹ������,���Ժ����ԣ�" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00008)  );
            return FALSE;
        }

		if(pChar->GetPlyMainCha()->IsStoreEnable())
		{
			//character.SystemNotice("�޷�����!");
			character.SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00001));
			return FALSE;
		}

		if( !pMain->GetPlyMainCha() || !pChar->GetPlyMainCha() )
		{
			/*pMain->SystemNotice( "���׽�ɫ�����ڣ�" );
			pChar->SystemNotice( "���׽�ɫ�����ڣ�" );*/
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00010) );
			pChar->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00010) );
		}

		if(pMain->GetPlyMainCha()->GetLevel() < 6)
		{
			//pMain->SystemNotice("���ĵȼ�����,�޷�����!");
			pMain->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00011));
			return FALSE;
		}

		if( pChar->GetBoat() )
		{
			//character.SystemNotice( "��ɫ%s�����촬,�����Խ���", pChar->GetName() );
			character.SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00012), pChar->GetName() );
			return FALSE;
		}

		if( pChar->GetStallData() )
		{
			//character.SystemNotice( "��ɫ%s���ڰ�̯,�����Խ���", pChar->GetName() );
			character.SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00013), pChar->GetName() );
			return FALSE;
		}

		//add by ALLEN 2007-10-16
		if( pChar->IsReadBook() )
		{
			//character.SystemNotice( "��ɫ%s���ڶ���,�����Խ���", pChar->GetName() );
			character.SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00014), pChar->GetName() );
			return FALSE;
		}

		if( pChar->GetKitbag()->IsLock() || !pChar->GetActControl(enumACTCONTROL_ITEM_OPT) )
		{
			//character.SystemNotice( "��ɫ%s�����ѱ�����,�����Խ��ף�", pChar->GetName() );
			character.SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00015), pChar->GetName() );
			return FALSE;
		}

        if( pChar->GetPlyMainCha()->GetKitbag()->IsPwdLocked() )
        {
           // character.SystemNotice( "��ɫ%s�����ѱ���������,�����Խ��ף�", pChar->GetName() );
			 character.SystemNotice( "��ɫ%s�����ѱ���������,�����Խ��ף�", pChar->GetName() );
			return FALSE;
        }
		
		if( byType == mission::TRADE_CHAR )
		{
			pMain = pMain->GetPlyMainCha();
			pChar = pChar->GetPlyMainCha();
		}
		else
		{
			if( pChar == pChar->GetPlyMainCha() || pMain == pMain->GetPlyMainCha() )
			{
				/*pMain->SystemNotice( "���׽�ɫ���Ͳ�ƥ�䣡" );
				pChar->SystemNotice( "���׽�ɫ���Ͳ�ƥ�䣡" );*/
				pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00017) );
				pChar->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00017) );
				return FALSE;
			}
		}

		if( pMain->GetPlayer()->IsLuanchOut() || pChar->GetPlayer()->IsLuanchOut() )
		{
			//pMain->SystemNotice( "���Ͻ�ֹ���ף�" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00018) );
			return FALSE;
		}
		/*else if( pMain->GetPlayer()->IsLuanchOut() && !pChar->GetPlayer()->IsLuanchOut() )
		{
			pMain->SystemNotice( "���Ѿ�����,���ڲ����������������ף�" );
			return FALSE;
		}*/
		else if( pMain->GetPlayer()->IsInForge() )
		{
			//pMain->SystemNotice( "�����ڲ����������������˽��ף�" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00019) );
			return FALSE;
		}

		CTradeData* pTradeData1 = pChar->GetTradeData();
		if( pTradeData1 )
		{
			//pMain->SystemNotice( "%s���ڽ����У�", pChar->GetName() );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00020), pChar->GetName() );
			return FALSE;
		}

		CTradeData* pTradeData2 = pMain->GetTradeData();
		if( pTradeData2 )
		{			
			return FALSE;
		}

		// ���ͽ�������
		WPACKET packet = GETWPACKET();
        WRITE_CMD(packet, CMD_MC_CHARTRADE);
        WRITE_SHORT(packet, CMD_MC_CHARTRADE_REQUEST);
		WRITE_CHAR(packet, byType);
		WRITE_LONG(packet, character.GetID());

		pChar->ReflectINFof( pChar, packet );
		return TRUE;
	T_E}

	BOOL CTradeSystem::IsTradeDist( CCharacter& Char1, CCharacter& Char2, DWORD dwDist )
	{T_B
		DWORD dwxDist = (Char1.GetShape().centre.x - Char2.GetShape().centre.x) * 
			(Char1.GetShape().centre.x - Char2.GetShape().centre.x);
		DWORD dwyDist = (Char1.GetShape().centre.y - Char2.GetShape().centre.y) * 
			(Char1.GetShape().centre.y - Char2.GetShape().centre.y);
		return ( dwxDist + dwyDist < dwDist * 100 );
	T_E}

	BOOL CTradeSystem::Accept( BYTE byType, CCharacter& character, DWORD dwRequestID )
	{T_B
		if( character.GetBoat() )
		{
			//character.SystemNotice( "���ڽ��촬ֻ,�����Խ��ף�" );
			character.SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00002) );
			return FALSE;
		}

		if( character.GetStallData() )
		{
			//character.SystemNotice( "���ڰ�̯,�����Խ���" );
			character.SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00003) );
			return FALSE;
		}

		//add by ALLEN 2007-10-16
				if( character.IsReadBook() )
		{
			//character.SystemNotice("���ڶ���,�����Խ���");
			character.SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00004));
			return FALSE;
		}

		if( character.GetKitbag()->IsLock() || !character.GetActControl(enumACTCONTROL_ITEM_OPT) )
		{
			//character.SystemNotice( "�����ѱ�����,�����Խ��ף�" );
			character.SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00005) );
			return FALSE;
		}

		if( character.GetPlyMainCha() && character.GetPlyMainCha()->GetKitbag()->IsLock() )
		{
			//character.SystemNotice( "�����ѱ�����,�����Խ��ף�" );
			character.SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00005) );
			return FALSE;
		}

        if( character.GetPlyMainCha() && character.GetPlyMainCha()->GetKitbag()->IsPwdLocked() )
        {
           // character.SystemNotice( "�����ѱ���������,�����Խ��ף�" );
			 character.SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00006) );
			return FALSE;
        }

		CCharacter* pMain = &character;
		if( pMain->GetID() == dwRequestID )
		{
			//pMain->SystemNotice( "������������Լ����ף�" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00021) );
			return FALSE;
		}

		CCharacter* pChar = pMain->GetSubMap()->FindCharacter( dwRequestID, pMain->GetShape().centre );
		if( pChar == NULL ) 
		{
			//pMain->SystemNotice( "����֪ͨ�ý�ɫ�Ѿ��뿪!" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00022) );
			return FALSE;
		}

        if(character.GetPlyMainCha()->GetPlayer()->GetBankNpc())
        {
           // character.SystemNotice("������ʹ������,�����Խ���");
			character.SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00023));
           // pChar->SystemNotice( "�Է�����ʹ������,���Ժ����ԣ�" );
           pChar->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00008) );
			return FALSE;
        }

		if(character.GetPlyMainCha()->IsStoreEnable() || pChar->GetPlyMainCha()->IsStoreEnable())
		{
			/*character.SystemNotice("�޷�����!");
			pChar->SystemNotice("�޷�����!");*/
			character.SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00001));
			pChar->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00001));
			return FALSE;
		}

		if( !pChar->IsLiveing() )
		{
			//pMain->SystemNotice( "�����׷����������ɽ��ף�" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00025) );
			return FALSE;
		}

		if( !pMain->IsLiveing() )
		{
			//pMain->SystemNotice( "�����������ɽ��ף�" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00026) );
			return FALSE;
		}

		if( pChar->GetBoat() )
		{
			//pChar->SystemNotice( "���ڽ��촬ֻ,�����Խ��ף�" );
			pChar->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00002) );
			return FALSE;
		}

		if( pChar->GetStallData() )
		{
			//pChar->SystemNotice( "���ڰ�̯,�����Խ��ף�" );
			pChar->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00003) );
			return FALSE;
		}

		//add by ALLEN 2007-10-16
				if( pChar->IsReadBook() )
		{
			//pChar->SystemNotice( "���ڶ���,�����Խ��ף�" );
			pChar->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00004) );
			return FALSE;
		}

		if( pChar->GetKitbag()->IsLock() || !pChar->GetActControl(enumACTCONTROL_ITEM_OPT) )
		{
			//pChar->SystemNotice( "�����ѱ�����,�����Խ��ף�" );
			pChar->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00005) );
			return FALSE;
		}

        if( pChar->GetPlyMainCha()->GetKitbag()->IsPwdLocked() )
        {
            //pChar->SystemNotice( "�����ѱ���������,�����Խ��ף�" );
			pChar->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00006) );
			return FALSE;
        }

        if(pChar->GetPlayer()->GetBankNpc())
        {
           // pChar->SystemNotice("���д�ʱ,���������ף�");
			 pChar->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00027));
            return FALSE;
        }

		if( byType == mission::TRADE_CHAR )
		{
			pMain = pMain->GetPlyMainCha();
			pChar = pChar->GetPlyMainCha();
		}
		else
		{
			if( pChar == pChar->GetPlyMainCha() || pMain == pMain->GetPlyMainCha() )
			{
				/*pMain->SystemNotice( "���׽�ɫ���Ͳ�ƥ�䣡" );
				pChar->SystemNotice( "���׽�ɫ���Ͳ�ƥ�䣡" );*/
				pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00017) );
				pChar->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00017) );
				return FALSE;
			}
		}

		if( !pMain->GetPlayer()->IsLuanchOut() && pChar->GetPlayer()->IsLuanchOut() )
		{
			//pMain->SystemNotice( "�Է��Ѿ�����,�����ڲ����Խ��������������ף�" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00029) );
			return FALSE;
		}
		else if( pMain->GetPlayer()->IsLuanchOut() && !pChar->GetPlayer()->IsLuanchOut() )
		{
			//pMain->SystemNotice( "���Ѿ�����,���ڲ����Խ��������������ף�" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00024) );
			return FALSE;
		}
		else if( pMain->GetPlayer()->IsInForge() )
		{
			//pMain->SystemNotice( "�����ڲ������������������˽��ף�" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00030) );
			return FALSE;
		}

		//if( !IsTradeDist( *pMain, *pChar, ROLE_MAXSIZE_TRADEDIST - 400 ) )
		//{
		//	// ������ɫ���׾���,���ͽ�ɫ���뿪��Ϣ��
		//	return FALSE;
		//}

		CTradeData* pTradeData1 = pChar->GetTradeData();
		if( pTradeData1 )
		{
			//pMain->SystemNotice( "%s���ڽ����У�", pChar->GetName() );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00020), pChar->GetName() );
			return FALSE;
		}

		CTradeData* pTradeData2 = pMain->GetTradeData();
		if( pTradeData2 )
		{
			// �Լ��������������ҽ��н�����
			return FALSE;
		}

		// �������Դ�ɽ����������ͷ�
		CTradeData* pData = g_pGameApp->m_TradeDataHeap.Get();
		if( pData == NULL ) 
		{
			//pMain->SystemNotice( "���佻�����ݻ���ʧ�ܣ�" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00031) );
			return FALSE;
		}
		pData->Clear();
		pData->pRequest = pChar;
		pData->pAccept  = pMain;
		pData->dwTradeTime = GetTickCount();
		pData->bTradeStart = ROLE_TRADE_START;

		//// ������֤�ص�
		//pData->sxPos = (USHORT)pMain->GetShape().centre.x;
		//pData->syPos = (USHORT)pMain->GetShape().centre.y;

		// ���ý��׽�����Ϣ����
		pMain->SetTradeData( pData );
		pChar->SetTradeData( pData );
		
		// �������׽�ɫ״̬
		pMain->TradeAction( TRUE );
		pChar->TradeAction( TRUE );
		CKitbag& ReqBag = *pData->pRequest->GetKitbag();;
		CKitbag& AcpBag = *pData->pAccept->GetKitbag();;
		ReqBag.Lock();
		AcpBag.Lock();

		// ���ͽ�ɫ����ҳ����
		WPACKET packet = GETWPACKET();
        WRITE_CMD(packet, CMD_MC_CHARTRADE);
        WRITE_SHORT(packet, CMD_MC_CHARTRADE_PAGE);
		WRITE_CHAR(packet, byType);
        WRITE_LONG(packet, pMain->GetID());
        WRITE_LONG(packet, pChar->GetID());
		pChar->ReflectINFof( pMain, packet );
		pMain->ReflectINFof( pMain, packet );
		return TRUE;
	T_E}

	BOOL CTradeSystem::Cancel( BYTE byType, CCharacter& character, DWORD dwCharID )
	{T_B
		CCharacter* pMain = &character;
		if( !pMain->GetPlyMainCha() )
		{
			//pMain->SystemNotice( "���׽�ɫ�����ڣ�" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00010) );
		}

		if( byType == mission::TRADE_CHAR )
		{
			pMain = pMain->GetPlyMainCha();
		}
		else
		{
			if( pMain == pMain->GetPlyMainCha() )
			{
				//pMain->SystemNotice( "���׽�ɫ���Ͳ�ƥ�䣡" );
				pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00017) );
				return FALSE;
			}
		}

		CTradeData* pTradeData2 = pMain->GetTradeData();
		if( !pTradeData2 )
		{
			char szData[128];
			//sprintf( szData, "Cancel:�ý�ɫ%s����������!\n", pMain->GetName() );
			//sprintf( szData, RES_STRING(GM_CHARTRADE_CPP_00032), pMain->GetName() );
			_snprintf_s( szData,sizeof(szData),_TRUNCATE, "Cancel: Cha %s is't in trade\n", pMain->GetName() );
			LG( "trade_error", szData );
			return FALSE;
		}

		CCharacter* pChar;
		if( pMain->GetID() == dwCharID )
		{
			//printf( "������Ϣ����,����ȡ�����Լ�ID��ͬ�Ľ��ײ�����" );
			printf( RES_STRING(GM_CHARTRADE_CPP_00033) );
			return FALSE;
		}
		else if( pTradeData2->pRequest->GetID() == dwCharID )
		{			
			pChar = pTradeData2->pRequest;
		}
		else if( pTradeData2->pAccept->GetID() == dwCharID )
		{
			pChar = pTradeData2->pAccept;
		}
		else
		{
			//pMain->SystemNotice( "�ͻ�������Ľ��׶�����Ϣ����ID = 0x%x", dwCharID );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00034), dwCharID );
			return FALSE;
		}
		
		CTradeData* pTradeData1 = pChar->GetTradeData();
		if( pTradeData1 == NULL || pTradeData2 != pTradeData1 )
		{
			//pMain->SystemNotice( "����:���%sδ������н��ף�", pChar->GetName() );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00009), pChar->GetName() );
			return FALSE;
		}
		
		// ���������λ����״̬
		pTradeData1->pAccept->GetKitbag()->UnLock();
		pTradeData1->pRequest->GetKitbag()->UnLock();

		ResetItemState( *pTradeData1->pAccept, *pTradeData1 );
		ResetItemState( *pTradeData1->pRequest, *pTradeData1 );
		
		pTradeData1->pAccept->SetTradeData( NULL );
		pTradeData1->pRequest->SetTradeData( NULL );

		// ȡ����ɫ����
		WPACKET packet = GETWPACKET();
		WRITE_CMD(packet, CMD_MC_CHARTRADE );
		WRITE_SHORT(packet, CMD_MC_CHARTRADE_CANCEL );
		WRITE_LONG(packet, pMain->GetID() );

		pTradeData1->pAccept->ReflectINFof( pMain, packet );
		pTradeData1->pRequest->ReflectINFof( pMain, packet );

		// ȡ����ɫ����״̬
		pTradeData1->pAccept->TradeAction( FALSE );
		pTradeData1->pRequest->TradeAction( FALSE );

		pTradeData1->Free();

		return TRUE;
	T_E}

	BOOL CTradeSystem::Clear( BYTE byType, CCharacter& character )
	{T_B
		CCharacter* pMain = &character;
		if( !pMain->GetPlyMainCha() )
		{
			//pMain->SystemNotice( "���׽�ɫ�����ڣ�" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00010) );
		}

		if( byType == mission::TRADE_CHAR )
		{
			pMain = pMain->GetPlyMainCha();
		}
		else
		{
			if( pMain == pMain->GetPlyMainCha() )
			{
				//pMain->SystemNotice( "���׽�ɫ���Ͳ�ƥ�䣡" );
				pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00017) );
				return FALSE;
			}
		}

		CTradeData* pTradeData = pMain->GetTradeData();
		if( !pTradeData )
		{
			// �ý�ɫ����������!
			return FALSE;
		}

		if( pTradeData->pRequest == pMain )
		{
			// ȡ����ɫ����
			WPACKET packet = GETWPACKET();
			WRITE_CMD(packet, CMD_MC_CHARTRADE );
			WRITE_SHORT(packet, CMD_MC_CHARTRADE_CANCEL );
			WRITE_LONG(packet, pMain->GetID() );
			pTradeData->pAccept->ReflectINFof( pMain, packet );
			pTradeData->pAccept->SetTradeData( NULL );

			// ���������λ����״̬
			pTradeData->pAccept->GetKitbag()->UnLock();
			pTradeData->pAccept->TradeAction( FALSE );
			ResetItemState( *pTradeData->pAccept, *pTradeData );
		}
		else if( pTradeData->pAccept == pMain )
		{
			// ȡ����ɫ����
			WPACKET packet = GETWPACKET();
			WRITE_CMD(packet, CMD_MC_CHARTRADE );
			WRITE_SHORT(packet, CMD_MC_CHARTRADE_CANCEL );
			WRITE_LONG(packet, pMain->GetID() );
			pTradeData->pRequest->ReflectINFof( pMain, packet );
			pTradeData->pRequest->SetTradeData( NULL );
			
			// ���������λ����״̬
			pTradeData->pRequest->GetKitbag()->UnLock();
			pTradeData->pRequest->TradeAction( FALSE );
			ResetItemState( *pTradeData->pRequest, *pTradeData );
		}
		else
		{
			//LG( "Trade", "ɾ����ɫʱ,����佻����Ϣ���ִ���(��ƥ��Ľ�ɫָ��)��"  );
			LG( "Trade", "when delete character,it find error while clear trade information,the error is:(unsuited charcter pointer)��"  );
			return FALSE;
		}

		pTradeData->Free();
		return TRUE;
	T_E}

	BOOL CTradeSystem::AddMoney( BYTE byType, CCharacter& character, DWORD dwCharID, BYTE byOpType, DWORD dwMoney )
	{T_B
		CCharacter* pMain = &character;
		if( !pMain->GetPlyMainCha() )
		{
			//pMain->SystemNotice( "���׽�ɫ�����ڣ�" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00010) );
		}

		if( byType == mission::TRADE_CHAR )
		{
			pMain = pMain->GetPlyMainCha();
		}
		else
		{
			//pMain->SystemNotice( "�������ʹ���type[%d]", byType );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00028), byType );
			return FALSE;
		}

		CTradeData* pTradeData = pMain->GetTradeData();
		if( !pTradeData )
		{
			char szData[128];
			//sprintf( szData, "AddMoney:�ý�ɫ%s����������!", pMain->GetName() );
			//sprintf( szData, RES_STRING(GM_CHARTRADE_CPP_00035), pMain->GetName() );
			_snprintf_s( szData,sizeof(szData),_TRUNCATE, "AddMoney: Cha %s is't in trade\n", pMain->GetName() );
			LG( "trade_error", szData );
			return FALSE;
		}

		if( pMain->GetID() == dwCharID )
		{
			//pMain->SystemNotice( "������Ϣ����,����ȡ�����Լ�ID��ͬ�Ľ��ײ�����" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00033) );
			return FALSE;
		}		
		else if( pTradeData->pRequest->GetID() != dwCharID && pTradeData->pAccept->GetID() != dwCharID )
		{
			//pMain->SystemNotice( "���׶�����Ϣ����" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00036) );
			return FALSE;
		}

		TRADE_DATA* pItemData = NULL;
		// ��֤�Ƿ����������Ʒ��ȡ����Ʒ����
		if( pMain == pTradeData->pRequest )
		{
			// �ж��Ƿ���Բ�����Ʒ
			if( pTradeData->bReqTrade == 1 )
			{
				//
				//pMain->SystemNotice( "�ý�ɫ������Ʒ��֤��ť�Ѿ�����,�������������׽�Ǯ������" );
				pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00037) );
				return FALSE;
			}
			pItemData = &pTradeData->ReqTradeData;
		}
		else if( pMain == pTradeData->pAccept )
		{
			if( pTradeData->bAcpTrade == 1 )
			{
				//pMain->SystemNotice( "�ý�ɫ������Ʒ��֤��ť�Ѿ�����,�������������׽�Ǯ������" );
				pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00037) );
				return FALSE;
			}
			pItemData = &pTradeData->AcpTradeData;
		}
		else
		{
			//pMain->SystemNotice( "�ý�ɫδ�ڽ����У�" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00038) );
			return FALSE;
		}

		if( byOpType == TRADE_DRAGMONEY_ITEM )
		{
			//pMain->SystemNotice( "�϶���Ǯ�������ʹ���" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00039) );
			return FALSE;
		}
		else if( byOpType == TRADE_DRAGMONEY_TRADE )
		{
			DWORD dwCharMoney = (long)pMain->m_CChaAttr.GetAttr( ATTR_GD );
			pItemData->dwMoney = dwMoney;
			if( pItemData->dwMoney > dwCharMoney )
			{
				pItemData->dwMoney = dwCharMoney;
			}
		}
		else
		{
			//pMain->SystemNotice( "�϶���Ǯ�������ʹ���" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00039) );
			return FALSE;
		}

		WPACKET packet = GETWPACKET();
		WRITE_CMD(packet, CMD_MC_CHARTRADE );
		WRITE_SHORT(packet, CMD_MC_CHARTRADE_MONEY );
		WRITE_LONG(packet, pMain->GetID() );
		WRITE_LONG(packet, pItemData->dwMoney );

		pTradeData->pAccept->ReflectINFof( pMain, packet );
		pTradeData->pRequest->ReflectINFof( pMain, packet );
		return TRUE;
	T_E}

	// ���û���ȡ����Ʒ��������
	BOOL CTradeSystem::AddItem( BYTE byType, CCharacter& character, DWORD dwCharID, BYTE byOpType, BYTE byIndex, BYTE byItemIndex, BYTE byCount )
	{T_B
		CCharacter* pMain = &character;
		if( pMain->GetPlayer() == NULL )
		{		
			return FALSE;
		}

		if( !pMain->GetPlyMainCha() )
		{
			//pMain->SystemNotice( "���׽�ɫ�����ڣ�" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00010) );
		}

		if( byType == mission::TRADE_CHAR )
		{
			pMain = pMain->GetPlyMainCha();
		}
		else
		{
			if( pMain == pMain->GetPlyMainCha() )
			{
				//pMain->SystemNotice( "���׽�ɫ���Ͳ�ƥ�䣡" );
				pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00017) );
				return FALSE;
			}
		}

		CKitbag& Bag = *pMain->GetKitbag();
	
		//	2008-8-1	yangyinyu	add	begin!
		//	�������߽�ֹ���ס�
		SItemGrid* pGridCont = Bag.GetGridContByID( byItemIndex );

		if(pGridCont)
		{
			if(	pGridCont->dwDBID )
			{
				pMain->SystemNotice( RES_STRING(GM_CHARACTER_CPP_00143) );
				return	FALSE;
			};
		}
		//	2008-8-1	yangyinyu	add	end!

		CTradeData* pTradeData = pMain->GetTradeData();
		if( !pTradeData )
		{
			char szData[128];
			//sprintf( szData, "AddItem:�ý�ɫ%s����������!", pMain->GetName() );
			//sprintf( szData, RES_STRING(GM_CHARTRADE_CPP_00040), pMain->GetName() );
			_snprintf_s( szData,sizeof(szData),_TRUNCATE, "AddItem: Cha %s is't in trade\n", pMain->GetName() );
			LG( "trade_error", szData );
			return FALSE;
		}

		if( pMain->GetID() == dwCharID )
		{
			//pMain->SystemNotice( "������Ϣ����,����������Ʒ���Լ�ID��ͬ�Ľ��ײ�����" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00041) );
			return FALSE;
		}		
		else if( pTradeData->pRequest->GetID() != dwCharID && pTradeData->pAccept->GetID() != dwCharID )
		{
			//pMain->SystemNotice( "���׶�����Ϣ����" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00036) );
			return FALSE;
		}

		CCharacter* pChar = NULL;
		TRADE_DATA* pItemData = NULL;
		// ��֤�Ƿ����������Ʒ��ȡ����Ʒ����
		if( pMain == pTradeData->pRequest )
		{
			// �ж��Ƿ���Բ�����Ʒ
			if( pTradeData->bReqTrade == 1 )
			{
				
				//pMain->SystemNotice( "�ý�ɫ������Ʒ��֤��ť�Ѿ�����,������������Ʒ�϶�������" );
				pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00042) );
				return FALSE;
			}
			pItemData = &pTradeData->ReqTradeData;
			pChar = pTradeData->pAccept;
		}
		else if( pMain == pTradeData->pAccept )
		{
			if( pTradeData->bAcpTrade == 1 )
			{
				//pMain->SystemNotice( "�ý�ɫ������Ʒ��֤��ť�Ѿ�����,������������Ʒ�϶�������" );
				pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00042) );
				return FALSE;
			}
			pItemData = &pTradeData->AcpTradeData;
			pChar = pTradeData->pRequest;
		}
		else
		{
			//pMain->SystemNotice( "�ý�ɫδ�ڽ��ף�" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00038) );
			return FALSE;
		}
		
		// ���ý�������Ʒ,���ҷ���֪ͨ��Ϣ���ͻ���
		if( byOpType == TRADE_DRAGTO_ITEM )
		{
			if( byIndex >= ROLE_MAXNUM_TRADEDATA )
			{
				//pMain->SystemNotice( "δ֪�Ľ�����λ������Ϣ��" );
				pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00043));
				return FALSE;
			}
			int nCapacity = pMain->GetKitbag()->GetCapacity();
			if( byItemIndex >= nCapacity )
			{
				//pMain->SystemNotice( "δ֪�ĵ�����λ������Ϣ��" );
				pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00044) );
				return FALSE;
			}
			if( pItemData->ItemArray[byIndex].sItemID == 0 )
			{
				//pMain->SystemNotice( "�ý�ɫ�϶��Ľ�����λ��Ʒ�����ڣ�" );
				pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00045) );
				return FALSE;
			}
			if( Bag.GetNum( pItemData->ItemArray[byIndex].byIndex ) > 0 && 
				Bag.GetID( pItemData->ItemArray[byIndex].byIndex ) != pItemData->ItemArray[byIndex].sItemID )
			{
				//pMain->SystemNotice( "ϵͳ��Ʒ�����׼�¼����,��֪ͨ������Ա,лл��ID1= %d, ID2 = %d", 
				//	Bag.GetID( pItemData->ItemArray[byIndex].byIndex ), pItemData->ItemArray[byIndex].sItemID );
				/*pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00046), 
					Bag.GetID( pItemData->ItemArray[byIndex].byIndex ), pItemData->ItemArray[byIndex].sItemID );*/
				char szData[128];
				CFormatParameter param(2);
				param.setDouble( 0, Bag.GetID( pItemData->ItemArray[byIndex].byIndex ));
				param.setDouble( 1, pItemData->ItemArray[byIndex].sItemID );
				RES_FORMAT_STRING( GM_CHARTRADE_CPP_00046, param, szData );
				pMain->SystemNotice( szData );
				return FALSE;
			}

			WPACKET packet = GETWPACKET();
			WRITE_CMD(packet, CMD_MC_CHARTRADE );
			WRITE_SHORT(packet, CMD_MC_CHARTRADE_ITEM );
			WRITE_LONG(packet, pMain->GetID() );
			WRITE_CHAR(packet, TRADE_DRAGTO_ITEM );
			WRITE_CHAR(packet, pItemData->ItemArray[byIndex].byIndex );
			WRITE_CHAR(packet, byIndex );
			WRITE_CHAR(packet, byCount );

			// ��������������Ʒ�״̬
			Bag.Enable( pItemData->ItemArray[byIndex].byIndex );
			pItemData->ItemArray[byIndex].sItemID = 0;
			pItemData->ItemArray[byIndex].byCount = 0;
			pItemData->ItemArray[byIndex].byType = 0;
			pItemData->ItemArray[byIndex].byIndex = 0;
			pItemData->byItemCount--;

			pTradeData->pRequest->ReflectINFof( pMain, packet );
			pTradeData->pAccept->ReflectINFof( pMain, packet );
		}
		else if( byOpType == TRADE_DRAGTO_TRADE )
		{
			if( byIndex >= ROLE_MAXNUM_TRADEDATA )
			{
				//pMain->SystemNotice( "δ֪�Ľ�����λ������Ϣ��" );
				pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00043) );
				return FALSE;
			}
			int nCapacity = pMain->GetKitbag()->GetCapacity();
			if( byItemIndex >= nCapacity )
			{
				//pMain->SystemNotice( "δ֪�ĵ�����λ������Ϣ��" );
				pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00044) );
				return FALSE;
			}

			if( !Bag.HasItem( byItemIndex ) || !Bag.IsEnable( byItemIndex ) )
			{
				//pMain->SystemNotice( "����Ʒ��λ�ѱ���ֹ�϶���" );
				pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00047) );
				return FALSE;
			}
			if( pItemData->ItemArray[byIndex].sItemID != 0 )
			{
				//pMain->SystemNotice( "�ý�����Ʒ��λ�Ѵ�����Ʒ,����ѡλ�ðڷţ�" );
				pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00048) );
				return FALSE;
			}

			CItemRecord* pItem = (CItemRecord*)GetItemRecordInfo( Bag.GetID( byItemIndex ) );
			if( pItem == NULL )
			{
				//pMain->SystemNotice( "��ƷID����,�޷��ҵ�����Ʒ��Ϣ��ID = %d", Bag.GetID( byItemIndex ) );
				pMain->SystemNotice( RES_STRING(GM_CHARSTALL_CPP_00041), Bag.GetID( byItemIndex ) );
				return FALSE;
			}

			if( !pItem->chIsTrade )
			{
				//pMain->SystemNotice( "��Ʒ��%s�����ɽ��ף�", pItem->szName );
				pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00049), pItem->szName );
				return FALSE;
			}

			//if( pItem->sType == enumItemTypeMission )
			//{
			//	pMain->SystemNotice( "������ߡ�%s�������Խ��ף�", pItem->szName );
			//	return FALSE;
			//}
			//else 
			if( pItem->sType == enumItemTypeBoat )
			{
				if( pMain->GetPlayer()->IsLuanchOut() )
				{
					if( Bag.GetDBParam( enumITEMDBP_INST_ID, byItemIndex ) == pMain->GetPlayer()->GetLuanchID() )
					{
						//pMain->SystemNotice( "������ʹ�øô�ֻ,�����Խ��׸ô�ֻ����֤����" );
						pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00050) );
						return FALSE;
					}
				}

				if( !pChar->GetPlayer()->IsBoatFull() )
				{
					USHORT sID  = Bag.GetID( byItemIndex );
					USHORT sNum = pChar->GetPlayer()->GetNumBoat();

					for( int i = 0; i < ROLE_MAXNUM_TRADEDATA; i++ )
					{
						if( sID == pItemData->ItemArray[i].sItemID )
						{
							sNum++;
							if( sNum >= MAX_CHAR_BOAT )
							{
								//pMain->SystemNotice( "�Է��Ѿ�ӵ�����㹻�����Ĵ�ֻ,��������ӵ���´�ֻ��" );
								pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00051) );
								return FALSE;
							}
						}
					}
				}
				else
				{
					//pMain->SystemNotice( "�Է��Ѿ�ӵ�����㹻�����Ĵ�ֻ,��������ӵ���´�ֻ��" );
					pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00051) );
					return FALSE;
				}

				CCharacter* pBoat = pMain->GetPlayer()->GetBoat( (DWORD)Bag.GetDBParam( enumITEMDBP_INST_ID, byItemIndex ) );
				if( !pBoat )
				{
					/*pMain->SystemNotice( "�ô����ݴ���,���ɽ��ף�ID[0x%X]", 
						Bag.GetDBParam( enumITEMDBP_INST_ID, byItemIndex ) );
					LG( "trade_error", "�ô����ݴ���,���ɽ��ף�ID[0x%X]", 
						Bag.GetDBParam( enumITEMDBP_INST_ID, byItemIndex ) );*/
					pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00052), 
						Bag.GetDBParam( enumITEMDBP_INST_ID, byItemIndex ) );
					LG( "trade_error", "The data error of this boat,cannot trade!ID[0x%X]\n", 
						Bag.GetDBParam( enumITEMDBP_INST_ID, byItemIndex ) );
					return FALSE;
				}
				if( !game_db.SaveBoat( *pBoat, enumSAVE_TYPE_OFFLINE ) )
				{
					/*pMain->SystemNotice( "AddItem:���洬ֻ����ʧ�ܣ���ֻ��%s��ID[0x%X]��", pBoat->GetName(), 
						Bag.GetDBParam( enumITEMDBP_INST_ID, byItemIndex ) );
					LG( "trade_error", "AddItem:���洬ֻ����ʧ�ܣ���ֻ��%s��ID[0x%X]��", pBoat->GetName(), 
						Bag.GetDBParam( enumITEMDBP_INST_ID, byItemIndex ) );*/
					/*pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00053), pBoat->GetName(), 
						Bag.GetDBParam( enumITEMDBP_INST_ID, byItemIndex ) );*/
					char szData[128];
					CFormatParameter param(2);
					param.setString( 0, pBoat->GetName());
					param.setLong( 1, Bag.GetDBParam( enumITEMDBP_INST_ID, byItemIndex ) );
					RES_FORMAT_STRING( GM_CHARTRADE_CPP_00053, param, szData );
					pMain->SystemNotice( szData );
					LG( "trade_error", "AddItem:it failed to save boat data!boat��%s��ID[0x%X]\n", pBoat->GetName(), 
						Bag.GetDBParam( enumITEMDBP_INST_ID, byItemIndex ) );
					return FALSE;
				}
			}

			if( byCount == 0 )
			{
				byCount = 1;
			}

			if( byCount > ROLE_MAXNUM_ITEMTRADE )
			{
				byCount = ROLE_MAXNUM_ITEMTRADE;
			}

			if( byCount > Bag.GetNum( byItemIndex ) )
			{
				byCount = (BYTE)Bag.GetNum( byItemIndex );
			}

			pItemData->ItemArray[byIndex].sItemID = Bag.GetID( byItemIndex );
			pItemData->ItemArray[byIndex].byCount = byCount;
			pItemData->ItemArray[byIndex].byIndex = byItemIndex;
			pItemData->byItemCount++;

			// ��ֹ��Ʒ��λ�״̬
			Bag.Disable( byItemIndex );

			WPACKET packet = GETWPACKET();
			WRITE_CMD(packet, CMD_MC_CHARTRADE );
			WRITE_SHORT(packet, CMD_MC_CHARTRADE_ITEM );
			WRITE_LONG(packet, pMain->GetID() );
			WRITE_CHAR(packet, TRADE_DRAGTO_TRADE );
			WRITE_SHORT(packet, pItemData->ItemArray[byIndex].sItemID );
			WRITE_CHAR(packet, pItemData->ItemArray[byIndex].byIndex );
			WRITE_CHAR(packet, byIndex );
			WRITE_CHAR(packet, byCount );
			WRITE_SHORT(packet, pItem->sType );

			if( pItem->sType == enumItemTypeBoat )
			{
				CCharacter* pBoat = pMain->GetPlayer()->GetBoat( (DWORD)Bag.GetDBParam( enumITEMDBP_INST_ID, byItemIndex ) );
				if( pBoat )
				{
					WRITE_CHAR( packet, 1 );
					WRITE_STRING( packet, pBoat->GetName() );
					WRITE_SHORT( packet, (USHORT)pBoat->getAttr( ATTR_BOAT_SHIP ) );
					WRITE_SHORT( packet, (USHORT)pBoat->getAttr( ATTR_LV ) );

					WRITE_LONG( packet, (long)pBoat->getAttr( ATTR_CEXP ) );
					WRITE_LONG( packet, (long)pBoat->getAttr( ATTR_HP ) );

					WRITE_LONG( packet, (long)pBoat->getAttr( ATTR_BMXHP ) );
					WRITE_LONG( packet, (long)pBoat->getAttr( ATTR_SP ) );
					WRITE_LONG( packet, (long)pBoat->getAttr( ATTR_BMXSP ) );
					WRITE_LONG( packet, (long)pBoat->getAttr( ATTR_BMNATK ) );
					WRITE_LONG( packet, (long)pBoat->getAttr( ATTR_BMXATK ) );
					WRITE_LONG( packet, (long)pBoat->getAttr( ATTR_BDEF ) );
					WRITE_LONG( packet, (long)pBoat->getAttr( ATTR_BMSPD ) );
					WRITE_LONG( packet, (long)pBoat->getAttr( ATTR_BASPD ) );
					WRITE_CHAR( packet, (BYTE)pBoat->GetKitbag()->GetUseGridNum() );
					WRITE_CHAR( packet, (BYTE)pBoat->GetKitbag()->GetCapacity() );
					WRITE_LONG( packet, (long)pBoat->getAttr( ATTR_BOAT_PRICE ) );
				}
				else
				{
					WRITE_CHAR( packet, 0 );
				}
			}
			else
			{
				// �õ��ߵ�ʵ������
				SItemGrid* pGridCont = Bag.GetGridContByID( byItemIndex );
				if( !pGridCont )
				{
					//pMain->SystemNotice( "ָ������Ʒ��λ��Ʒʵ����ϢΪ�գ�ID[%d]", byItemIndex );
					pMain->SystemNotice( RES_STRING(GM_CHARSTALL_CPP_00057), byItemIndex );
					return FALSE;
				}

				WRITE_SHORT( packet, pGridCont->sEndure[0] );
				WRITE_SHORT( packet, pGridCont->sEndure[1] );
				WRITE_SHORT( packet, pGridCont->sEnergy[0] );
				WRITE_SHORT( packet, pGridCont->sEnergy[1] );
				WRITE_CHAR( packet, pGridCont->chForgeLv );
				WRITE_CHAR( packet, pGridCont->IsValid() ? 1 : 0 );
				WRITE_LONG(packet, pGridCont->GetDBParam(enumITEMDBP_FORGE));
				WRITE_LONG(packet, pGridCont->GetDBParam(enumITEMDBP_INST_ID));
				if( pGridCont->IsInstAttrValid() ) // ����ʵ������
				{
					WRITE_CHAR( packet, 1 );
					for (int j = 0; j < defITEM_INSTANCE_ATTR_NUM; j++)
					{
						WRITE_SHORT(packet, pGridCont->sInstAttr[j][0]);
						WRITE_SHORT(packet, pGridCont->sInstAttr[j][1]);
					}
				}
				else
				{
					WRITE_CHAR( packet, 0 ); // ������ʵ������
				}
			}

			pTradeData->pRequest->ReflectINFof( pMain, packet );
			pTradeData->pAccept->ReflectINFof( pMain, packet );
		}
		else
		{
			//pMain->SystemNotice( "δ֪����Ʒ�϶�����ָ�" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00054) );
			return FALSE;
		}

		return TRUE;
	T_E}

	BOOL CTradeSystem::ValidateItemData( BYTE byType, CCharacter& character, DWORD dwCharID )
	{T_B
		CCharacter* pMain = &character;
		if( !pMain->GetPlyMainCha() )
		{
			//pMain->SystemNotice( "���׽�ɫ�����ڣ�" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00010) );
		}

		if( byType == mission::TRADE_CHAR )
		{
			pMain = pMain->GetPlyMainCha();
		}
		else
		{
			if( pMain == pMain->GetPlyMainCha() )
			{
				//pMain->SystemNotice( "���׽�ɫ���Ͳ�ƥ�䣡" );
				pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00017) );
				return FALSE;
			}
		}

		CTradeData* pTradeData = pMain->GetTradeData();
		if( !pTradeData )
		{
			char szData[128];
			//sprintf( szData, "ValidateItemData:�ý�ɫ%s����������!", pMain->GetName() );
			//sprintf( szData, RES_STRING(GM_CHARTRADE_CPP_00055), pMain->GetName() );
			_snprintf_s( szData,sizeof(szData),_TRUNCATE, "ValidateItemData: Cha %s is't in trade\n", pMain->GetName() );
			LG( "trade_error", szData );
			return FALSE;
		}

		if( pMain->GetID() == dwCharID )
		{
			//pMain->SystemNotice( "������Ϣ����,����ȡ�����Լ�ID��ͬ�Ľ��ײ�����" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00033) );
			return FALSE;
		}		
		else if( pTradeData->pRequest->GetID() != dwCharID && pTradeData->pAccept->GetID() != dwCharID )
		{
			//pMain->SystemNotice( "���׶�����Ϣ����" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00036) );
			return FALSE;
		}

		// ����ȷ����Ʒ��Ϣ״̬
		if( pMain == pTradeData->pRequest )
		{
			pTradeData->bReqTrade = 1;
		}
		else if( pMain == pTradeData->pAccept )
		{
			pTradeData->bAcpTrade = 1;
		}
		else
		{
			/*pMain->SystemNotice( "���׶�����Ϣ�ڲ�����" );
			LG( "trade_error", "���׶�����Ϣ�ڲ�����" );*/
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00056) );
			LG( "trade_error", "information of trade object  inside error\n" );
			return FALSE;
		}
	
		WPACKET packet = GETWPACKET();
		WRITE_CMD(packet, CMD_MC_CHARTRADE );
		WRITE_SHORT(packet, CMD_MC_CHARTRADE_VALIDATEDATA );
		WRITE_LONG(packet, pMain->GetID() );

		if( pMain == pTradeData->pRequest )
		{
			pTradeData->pAccept->ReflectINFof( pMain, packet );
		}
		else
		{
			pTradeData->pRequest->ReflectINFof( pMain, packet );
		}	
		return TRUE;
	T_E}

	BOOL CTradeSystem::ValidateTrade( BYTE byType, CCharacter& character, DWORD dwCharID )
	{T_B
		MONITOR

		CCharacter* pMain = &character;
		if( !pMain->GetPlyMainCha() )
		{
			//pMain->SystemNotice( "���׽�ɫ�����ڣ�" );
			pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00010) );
		}

		if( byType == mission::TRADE_CHAR )
		{
			pMain = pMain->GetPlyMainCha();
		}
		else
		{
			if( pMain == pMain->GetPlyMainCha() )
			{
				//pMain->SystemNotice( "���׽�ɫ���Ͳ�ƥ�䣡" );
				pMain->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00017) );
				return FALSE;
			}
		}

		CTradeData* pTradeData = pMain->GetTradeData();
		if( !pTradeData )
		{
			char szData[128];
			//sprintf( szData, "ValidateTrade:�ý�ɫ%s����������!", pMain->GetName() );
			//sprintf( szData, RES_STRING(GM_CHARTRADE_CPP_00057), pMain->GetName() );
			_snprintf_s( szData,sizeof(szData),_TRUNCATE, "ValidateTrade: Cha %s is't in trade\n", pMain->GetName() );
			LG( "trade_error", szData );
			return FALSE;
		}

		if( pMain->GetID() == dwCharID )
		{
			//printf( "������Ϣ����,����ȡ�����Լ�ID��ͬ�Ľ��ײ�����" );
			printf( RES_STRING(GM_CHARTRADE_CPP_00033) );
			return FALSE;
		}		
		else if( pTradeData->pRequest->GetID() != dwCharID && pTradeData->pAccept->GetID() != dwCharID )
		{
			//printf( "���׶�����Ϣ����" );
			printf( RES_STRING(GM_CHARTRADE_CPP_00036) );
			return FALSE;
		}

		// ���ý���״̬,������Ƿ�˫����������
		if( pMain == pTradeData->pRequest )
		{
			if( pTradeData->bReqTrade != 1 || pTradeData->bAcpTrade != 1 )
			{
				return FALSE;				
			}
			pTradeData->bReqOk = 1;
		}
		else if( pMain == pTradeData->pAccept )
		{
			if( pTradeData->bReqTrade != 1 || pTradeData->bAcpTrade != 1 )
			{
				return FALSE;
			}
			pTradeData->bAcpOk = 1;
		}

		if( pTradeData->bAcpTrade == 1 && pTradeData->bReqTrade == 1 && 
			pTradeData->bAcpOk == 1 && pTradeData->bReqOk == 1 )
		{
			CCharacter* pRequest = pTradeData->pRequest;
			CCharacter* pAccept  = pTradeData->pAccept;
			CKitbag& ReqBag = *pRequest->GetKitbag();
			CKitbag& AcpBag = *pAccept->GetKitbag();
			DWORD dwReqMoney = (long)pRequest->getAttr( ATTR_GD );
			DWORD dwAcpMoney = (long)pAccept->getAttr( ATTR_GD );

			// �ٴ�У�齻�׽�Ǯ������Ϣ
			if( pTradeData->ReqTradeData.dwMoney > dwReqMoney )
			{
				/*pAccept->SystemNotice( "��ɫ��%s�����׽�Ǯ���ݲ���ȷ,�����Լ������ף�", pRequest->GetName() );
				pRequest->SystemNotice( "��ɫ��%s�����׽�Ǯ���ݲ���ȷ,�����Լ������ף�", pRequest->GetName() );*/
				pAccept->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00058), pRequest->GetName() );
				pRequest->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00058), pRequest->GetName() );
				return FALSE;
			}

			if( pTradeData->AcpTradeData.dwMoney > dwAcpMoney )
			{
				/*pAccept->SystemNotice( "��ɫ��%s�����׽�Ǯ���ݲ���ȷ,�����Լ������ף�", pAccept->GetName() );
				pRequest->SystemNotice( "��ɫ��%s�����׽�Ǯ���ݲ���ȷ,�����Լ������ף�", pAccept->GetName() );*/
				pAccept->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00058), pRequest->GetName() );
				pRequest->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00058), pRequest->GetName() );
				return FALSE;
			}

			// ���������λ����״̬
			ReqBag.UnLock();
			AcpBag.UnLock();
			ResetItemState( *pAccept, *pTradeData );
			ResetItemState( *pRequest, *pTradeData );

			// ���ݽ���˫�������ͽ�Ǯ������Ϣ
			CKitbag ReqBagData, AcpBagData;
			ReqBagData = ReqBag;
			AcpBagData = AcpBag;	

			// 
			ReqBag.SetChangeFlag(false);
			AcpBag.SetChangeFlag(false);
			pRequest->m_CChaAttr.ResetChangeFlag();
			pRequest->SetBoatAttrChangeFlag(false);
			pAccept->m_CChaAttr.ResetChangeFlag();
			pAccept->SetBoatAttrChangeFlag(false);

			// ��ɽ�����Ϣ����
			int nAcpCapacity = pAccept->GetKitbag()->GetCapacity();
			int nReqCapacity = pRequest->GetKitbag()->GetCapacity();
			SItemGrid AcpGrid[ROLE_MAXNUM_TRADEDATA];
			SItemGrid ReqGrid[ROLE_MAXNUM_TRADEDATA];

			// ������߽����Ƿ���Խ���
			char szTemp[128] = "";
			char szTrade[2046] = "";
			//sprintf( szTrade, "������%s�������ݣ�{", pAccept->GetName() );
			//sprintf( szTrade, RES_STRING(GM_CHARTRADE_CPP_00059), pAccept->GetName() );
			_snprintf_s( szTrade,sizeof(szTrade),_TRUNCATE, RES_STRING(GM_CHARTRADE_CPP_00059), pAccept->GetName() );

			//�ж�˫������
			BOOL bBagSucc = true;
			if(!pTradeData->pAccept->HasLeaveBagGrid(pTradeData->ReqTradeData.byItemCount))
			{
				/*pTradeData->pRequest->SystemNotice("�Է������ռ䲻��,����ʧ��!");
				pTradeData->pAccept->SystemNotice("�����ռ䲻��,����ʧ��!");*/
				pTradeData->pRequest->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00060));
				pTradeData->pAccept->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00061));
				bBagSucc = false;
			}
			else if(!pTradeData->pRequest->HasLeaveBagGrid(pTradeData->AcpTradeData.byItemCount))
			{
				/*pTradeData->pAccept->SystemNotice("�Է������ռ䲻��,����ʧ��!");
				pTradeData->pRequest->SystemNotice("�����ռ䲻��,����ʧ��!");*/
				pTradeData->pAccept->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00060));
				pTradeData->pRequest->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00061));
				bBagSucc = false;	
			}
			if(!bBagSucc)
			{
				pAccept->SetTradeData( NULL );
				pRequest->SetTradeData( NULL );
				pTradeData->Free();

				// ȡ����ɫ����״̬
				pTradeData->pAccept->TradeAction( FALSE );
				pTradeData->pRequest->TradeAction( FALSE );

				WPACKET packet = GETWPACKET();
				WRITE_CMD(packet, CMD_MC_CHARTRADE );
				WRITE_SHORT(packet, CMD_MC_CHARTRADE_RESULT );
				WRITE_CHAR(packet, TRADE_FAILER );

				pTradeData->pAccept->ReflectINFof( pMain, packet );
				pTradeData->pRequest->ReflectINFof( pMain, packet );
				return FALSE;
			}

			// ���߽��ײ���
			for( int i = 0; i < ROLE_MAXNUM_TRADEDATA; i++ )
			{
				// 
				if( pTradeData->AcpTradeData.ItemArray[i].sItemID != 0 )
				{
					CItemRecord* pItem = GetItemRecordInfo( pTradeData->AcpTradeData.ItemArray[i].sItemID );
					if( pItem == NULL )
					{
						/*pMain->SystemNotice( "��ƷID����,�޷��ҵ�����Ʒ��Ϣ��ID = %d", pTradeData->AcpTradeData.ItemArray[i].sItemID );
						LG( "trade_error", "��ƷID����,�޷��ҵ�����Ʒ��Ϣ��ID = %d", pTradeData->AcpTradeData.ItemArray[i].sItemID );*/
						pMain->SystemNotice( RES_STRING(GM_CHARSTALL_CPP_00041), pTradeData->AcpTradeData.ItemArray[i].sItemID );
						LG( "trade_error", "res ID error,it cannot find this res information!ID = %d\n", pTradeData->AcpTradeData.ItemArray[i].sItemID );
						return FALSE;
					}
					else
					{
						AcpGrid[i].sNum = pTradeData->AcpTradeData.ItemArray[i].byCount;
						if( pAccept->KbPopItem( true, false, AcpGrid  + i, pTradeData->AcpTradeData.ItemArray[i].byIndex ) != enumKBACT_SUCCESS )
						{
							/*pAccept->SystemNotice( "�ӽ��׽����ߡ�%s��������ȡ��Ʒ��%d����Ʒʧ�ܣ�ID = %d", 
								pAccept->GetName(), pTradeData->AcpTradeData.ItemArray[i].sItemID );
							pRequest->SystemNotice( "�ӽ��׽����ߡ�%s��������ȡ��Ʒ��%d����Ʒʧ�ܣ�ID = %d", 
								pAccept->GetName(), pTradeData->AcpTradeData.ItemArray[i].sItemID );
							LG( "trade_error", "�ӽ��������ߡ�%s��������ȡ��Ʒ��%d����Ʒʧ�ܣ�ID = %d", 
								pAccept->GetName(), pTradeData->AcpTradeData.ItemArray[i].sItemID );*/
							/*pAccept->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00062), 
								pAccept->GetName(), pTradeData->AcpTradeData.ItemArray[i].sItemID );
							pRequest->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00062), 
								pAccept->GetName(), pTradeData->AcpTradeData.ItemArray[i].sItemID );*/
							char szData[128];
							CFormatParameter param(2);
							param.setString( 0, pAccept->GetName());
							param.setDouble( 1, pTradeData->AcpTradeData.ItemArray[i].sItemID );
							RES_FORMAT_STRING( GM_CHARTRADE_CPP_00062, param, szData );
							pAccept->SystemNotice( szData );
							pRequest->SystemNotice( szData );

							LG( "trade_error", "it failed to get trade item from trade asker ��d%��!Item ID = %d\n", 
								pAccept->GetName(), pTradeData->AcpTradeData.ItemArray[i].sItemID );
							return FALSE;
						}

						if( pItem->sType == enumItemTypeBoat )
						{
							CCharacter* pBoat = pAccept->GetPlayer()->GetBoat( (DWORD)AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							if( pBoat )
							{
								//sprintf( szTemp, "%d�Ҵ�ֻ��%s��ID[0x%X],", AcpGrid[i].sNum, pBoat->GetName(),
								//	AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								//sprintf( szTemp, RES_STRING(GM_CHARTRADE_CPP_00063), AcpGrid[i].sNum, pBoat->GetName(),
								//	AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								//_snprintf_s( szTemp,sizeof(szTemp),_TRUNCATE, RES_STRING(GM_CHARTRADE_CPP_00063), AcpGrid[i].sNum, pBoat->GetName(),
								//	AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								CFormatParameter param(3);
								param.setDouble( 0, AcpGrid[i].sNum );
								param.setString( 1, pBoat->GetName());
								param.setLong( 2, AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ));
								RES_FORMAT_STRING( GM_CHARTRADE_CPP_00063, param, szTemp );
								//strcat( szTrade, szTemp );
								strncat_s( szTrade,sizeof(szTrade), szTemp,_TRUNCATE);
							}
							else
							{
								//sprintf( szTemp, "%d�Ҵ�ֻ��δ֪��ֻ����ID[0x%X],", AcpGrid[i].sNum, 
								//sprintf( szTemp, RES_STRING(GM_CHARTRADE_CPP_00064), AcpGrid[i].sNum, 
								//	AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								/*_snprintf_s( szTemp,sizeof(szTemp),_TRUNCATE, RES_STRING(GM_CHARTRADE_CPP_00064), AcpGrid[i].sNum, 
									AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );*/
								CFormatParameter param(2);
								param.setDouble( 0, AcpGrid[i].sNum );
								param.setLong( 1, AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								RES_FORMAT_STRING( GM_CHARTRADE_CPP_00064, param, szTemp );
								//strcat( szTrade, szTemp );
								strncat_s( szTrade,sizeof(szTrade), szTemp,_TRUNCATE);
							}

							if( !pAccept->BoatClear( AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) ) )
							{
								/*pAccept->SystemNotice( "ɾ��%sӵ�еĴ���֤��ӵ�еĴ�ֻʧ�ܣ�ID[0x%X]", 
									pAccept->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								pRequest->SystemNotice( "ɾ��%sӵ�еĴ���֤��ӵ�еĴ�ֻʧ�ܣ�ID[0x%X]", 
									pAccept->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								LG( "trade_error", "ɾ��%s�Ĵ���֤��ӵ�еĴ�ֻʧ�ܣ�DBID[0x%X]", 
									pAccept->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );*/
								/*pAccept->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00065), 
									pAccept->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								pRequest->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00065), 
									pAccept->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );*/
								char szData[128];
								CFormatParameter param(2);
								param.setString( 0,pAccept->GetName());
								param.setLong( 1,AcpGrid[i].GetDBParam(enumITEMDBP_INST_ID));
								RES_FORMAT_STRING( GM_CHARTRADE_CPP_00065, param, szData );
								pAccept->SystemNotice( szData );
								pRequest->SystemNotice( szData );

								LG( "trade_error", "it failed to delete captain confirm boat that %s have !DBID[0x%X]\n", 
									pAccept->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							}
						}
						else
						{
							//sprintf( szTemp, RES_STRING(GM_CHARTRADE_CPP_00096), AcpGrid[i].sNum, pItem->szName );
							//_snprintf_s( szTemp,sizeof(szTemp),_TRUNCATE, RES_STRING(GM_CHARTRADE_CPP_00096), AcpGrid[i].sNum, pItem->szName );
							CFormatParameter param(2);
							param.setLong( 0, AcpGrid[i].sNum );
							param.setString( 1, pItem->szName );
							RES_FORMAT_STRING( GM_CHARTRADE_CPP_00096, param, szTemp );
							//strcat( szTrade, szTemp );
							strncat_s( szTrade,sizeof(szTrade), szTemp,_TRUNCATE);
						}
					}
				}
			}

			
			// sprintf( szTemp, "},������%s�������ݣ�{", pRequest->GetName() );
			//sprintf( szTemp, RES_STRING(GM_CHARTRADE_CPP_00066), pRequest->GetName() );
			_snprintf_s( szTemp,sizeof(szTemp),_TRUNCATE, RES_STRING(GM_CHARTRADE_CPP_00066), pRequest->GetName() );
			//strcat( szTrade, szTemp );
			strncat_s( szTrade,sizeof(szTrade), szTemp,_TRUNCATE);
			for( int i = 0; i < ROLE_MAXNUM_TRADEDATA; i++ )
			{
				if( pTradeData->ReqTradeData.ItemArray[i].sItemID != 0 )
				{
					CItemRecord* pItem = GetItemRecordInfo( pTradeData->ReqTradeData.ItemArray[i].sItemID );
					if( pItem == NULL )
					{
						/*pMain->SystemNotice( "��ƷID����,�޷��ҵ�����Ʒ��Ϣ��ID = %d", pTradeData->ReqTradeData.ItemArray[i].sItemID );
						LG( "trade_error", "��ƷID����,�޷��ҵ�����Ʒ��Ϣ��ID = %d", pTradeData->ReqTradeData.ItemArray[i].sItemID );*/
						pMain->SystemNotice( RES_STRING(GM_CHARSTALL_CPP_00041), pTradeData->ReqTradeData.ItemArray[i].sItemID );
						LG( "trade_error", "res ID error,it cannot find this res information!ID = %d\n", pTradeData->ReqTradeData.ItemArray[i].sItemID );
						return FALSE;
					}
					else
					{
						ReqGrid[i].sNum = pTradeData->ReqTradeData.ItemArray[i].byCount;
						if( pRequest->KbPopItem( true, false, ReqGrid + i, pTradeData->ReqTradeData.ItemArray[i].byIndex ) != enumKBACT_SUCCESS )
						{
							/*pAccept->SystemNotice( "�ӽ��������ߡ�%s��������ȡ��Ʒ��%d����Ʒʧ�ܣ�ID = %d", 
								pRequest->GetName(), pTradeData->ReqTradeData.ItemArray[i].sItemID );
							pRequest->SystemNotice( "�ӽ��������ߡ�%s��������ȡ��Ʒ��%d����Ʒʧ�ܣ�ID = %d", 
								pRequest->GetName(), pTradeData->ReqTradeData.ItemArray[i].sItemID );
							LG( "trade_error", "�ӽ��������ߡ�%s��������ȡ��Ʒ��%d����Ʒʧ�ܣ�ID = %d", 
								pRequest->GetName(), pTradeData->ReqTradeData.ItemArray[i].sItemID );*/
							/*pAccept->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00067), 
								pRequest->GetName(), pTradeData->ReqTradeData.ItemArray[i].sItemID );
							pRequest->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00067), 
								pRequest->GetName(), pTradeData->ReqTradeData.ItemArray[i].sItemID );*/

							char szData[128];
							CFormatParameter param(2);
							param.setString( 0, pRequest->GetName());
							param.setDouble( 1, pTradeData->ReqTradeData.ItemArray[i].sItemID );
							RES_FORMAT_STRING( GM_CHARTRADE_CPP_00067, param, szData );
							pAccept->SystemNotice( szData );
							pRequest->SystemNotice( szData );
							LG( "trade_error", "it failed get res ��%d�� from trade asker��%s��!ID = %d\n", 
								pRequest->GetName(), pTradeData->ReqTradeData.ItemArray[i].sItemID );
							return FALSE;
						}

						if( pItem->sType == enumItemTypeBoat )
						{
							CCharacter* pBoat = pRequest->GetPlayer()->GetBoat( (DWORD)ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							if( pBoat )
							{
								/*sprintf( szTemp, "%d�Ҵ�ֻ��%s��ID[0x%X],", ReqGrid[i].sNum, pBoat->GetName(),
									ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );*/
								//sprintf( szTemp, RES_STRING(GM_CHARTRADE_CPP_00063), ReqGrid[i].sNum, pBoat->GetName(),
								//	ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								/*_snprintf_s( szTemp,sizeof(szTemp),_TRUNCATE, RES_STRING(GM_CHARTRADE_CPP_00063), ReqGrid[i].sNum, pBoat->GetName(),
									ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );*/
								CFormatParameter param(3);
								param.setDouble( 0, ReqGrid[i].sNum );
								param.setString( 1, pBoat->GetName());
								param.setLong( 2, ReqGrid[i].GetDBParam(enumITEMDBP_INST_ID) );
								RES_FORMAT_STRING( GM_CHARTRADE_CPP_00063, param, szTemp );
								//strcat( szTrade, szTemp );
								strncat_s( szTrade,sizeof(szTrade), szTemp,_TRUNCATE);
							}
							else
							{
								/*sprintf( szTemp, "%d�Ҵ�ֻ��δ֪��ֻ����ID[0x%X],", ReqGrid[i].sNum, 
									ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );*/
								//sprintf( szTemp, RES_STRING(GM_CHARTRADE_CPP_00063), ReqGrid[i].sNum, 
								//	ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								/*_snprintf_s( szTemp,sizeof(szTemp),_TRUNCATE, RES_STRING(GM_CHARTRADE_CPP_00064), ReqGrid[i].sNum, 
									ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );*/
								CFormatParameter param(2);
								param.setDouble( 0,  ReqGrid[i].sNum );
								param.setLong( 1, ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ));
								RES_FORMAT_STRING( GM_CHARTRADE_CPP_00064, param, szTemp );

								//strcat( szTrade, szTemp );
								strncat_s( szTrade,sizeof(szTrade), szTemp,_TRUNCATE);
							}

							if( !pRequest->BoatClear( ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) ) )
							{
								/*pAccept->SystemNotice( "ɾ��%s�Ĵ���֤��ӵ�еĴ�ֻʧ�ܣ�ID[0x%X]", 
									pRequest->GetName(), ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								pRequest->SystemNotice( "ɾ��%s�Ĵ���֤��ӵ�еĴ�ֻʧ�ܣ�ID[0x%X]", 
									pRequest->GetName(), ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								LG( "trade_error", "ɾ��%s�Ĵ���֤��ӵ�еĴ�ֻʧ�ܣ�DBID[0x%X]", 
									pRequest->GetName(), ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );*/
								/*pAccept->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00068), 
									pRequest->GetName(), ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								pRequest->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00068), 
									pRequest->GetName(), ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );*/
								char szData[128];
								CFormatParameter param(2);
								param.setString( 0, pRequest->GetName());
								param.setLong( 1, ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								RES_FORMAT_STRING( GM_CHARTRADE_CPP_00068, param, szData );
								pAccept->SystemNotice( szData );
								pRequest->SystemNotice( szData );
								LG( "trade_error", "it failed to delete boat that captain confirm of %s have!DBID[0x%X]\n", 
									pRequest->GetName(), ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							}
						}
						else
						{
							//sprintf( szTemp, "%d��%s,", ReqGrid[i].sNum, pItem->szName );
							_snprintf_s( szTemp,sizeof(szTemp),_TRUNCATE, "%d %s, ", ReqGrid[i].sNum, pItem->szName );
							//strcat( szTrade, szTemp );
							strncat_s( szTrade,sizeof(szTrade), szTemp,_TRUNCATE);
						}
					}
				}
			}
			//strcat( szTrade, "}" );
			strncat_s( szTrade,sizeof(szTrade), "}",_TRUNCATE);

			for( int i = 0; i < ROLE_MAXNUM_TRADEDATA; i++ )
			{
				if( pTradeData->AcpTradeData.ItemArray[i].sItemID != 0 )
				{
					CItemRecord* pItem = GetItemRecordInfo( pTradeData->AcpTradeData.ItemArray[i].sItemID );
					if( pItem == NULL )
					{
						/*pRequest->SystemNotice( "��ƷID����,�޷��ҵ�����Ʒ��Ϣ�����ܸ��������Ʒ,ID = %d", 
							pTradeData->AcpTradeData.ItemArray[i].sItemID );
						pAccept->SystemNotice( "��ƷID����,�޷��ҵ�����Ʒ��Ϣ�����ܸ��������Ʒ,ID = %d", 
							pTradeData->AcpTradeData.ItemArray[i].sItemID );
						LG( "trade_error", "��ƷID����,�޷��ҵ�����Ʒ��Ϣ�����ܸ��������Ʒ,ID = %d", 
							pTradeData->AcpTradeData.ItemArray[i].sItemID );*/
						pRequest->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00069), 
							pTradeData->AcpTradeData.ItemArray[i].sItemID );
						pAccept->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00069), 
							pTradeData->AcpTradeData.ItemArray[i].sItemID );
						LG( "trade_error", "res ID error,it cannot find res information!it cannot give you this res,ID = %d\n", 
							pTradeData->AcpTradeData.ItemArray[i].sItemID );
						continue;
					}

					// �������߸��Ķ�������������					
					USHORT sCount = AcpGrid[i].sNum;
					Short sPushPos = defKITBAG_DEFPUSH_POS;
					Short sPushRet = pRequest->KbPushItem( true, false, AcpGrid + i, sPushPos );

					if( sPushRet == enumKBACT_ERROR_FULL ) // ����������,��������
					{
						// �����Ʒ�����¼�
						USHORT sNum = sCount - AcpGrid[i].sNum;

						CCharacter	*pCCtrlCha = pRequest->GetPlyCtrlCha(), *pCMainCha = pRequest->GetPlyMainCha();
						Long	lPosX, lPosY;
						pCCtrlCha->GetTrowItemPos(&lPosX, &lPosY);
						if( pCCtrlCha->GetSubMap()->ItemSpawn( AcpGrid + i, lPosX, lPosY, enumITEM_APPE_THROW, pCCtrlCha->GetID(), pCMainCha->GetID(), pCMainCha->GetHandle() ) == NULL )
						{
							/*pAccept->SystemNotice( "����ʱ��%s����װ���µ���Ʒ��%s���ŵ�����ʧ�ܣ�������Ʒ��ʧ��ID[%d], Num[%d]", 
								pRequest->GetName(), pItem->szName, AcpGrid[i].sID, AcpGrid[i].sNum );
							pRequest->SystemNotice( "����ʱ��%s����װ���µ���Ʒ��%s���ŵ�����ʧ�ܣ�������Ʒ��ʧ��ID[%d], Num[%d]", 
								pRequest->GetName(), pItem->szName, AcpGrid[i].sID, AcpGrid[i].sNum );
							LG( "trade_error", "Error code[%d],����ʱ��%s����װ���µ���Ʒ��%s���ŵ�����ʧ�ܣ�������Ʒ��ʧ��ID[%d], Num[%d]", 
								sPushRet, pRequest->GetName(), pItem->szName, AcpGrid[i].sID, AcpGrid[i].sNum );*/
							/*pAccept->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00070), 
								pRequest->GetName(), pItem->szName, AcpGrid[i].sID, AcpGrid[i].sNum );
							pRequest->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00070), 
								pRequest->GetName(), pItem->szName, AcpGrid[i].sID, AcpGrid[i].sNum );*/
							char szData[128];
							CFormatParameter param(4);
							param.setString( 0, pRequest->GetName());
							param.setString( 1, pItem->szName );
							param.setDouble( 2, AcpGrid[i].sID );
							param.setDouble( 3, AcpGrid[i].sNum );
							RES_FORMAT_STRING( GM_CHARTRADE_CPP_00070, param, szData );
							pAccept->SystemNotice( szData );
							pRequest->SystemNotice( szData );
							LG( "trade_error", "Error code[%d],when trading,%s bag is full,��%s��failed to put on floor!trade res failed!ID[%d], Num[%d]\n", 
								sPushRet, pRequest->GetName(), pItem->szName, AcpGrid[i].sID, AcpGrid[i].sNum );
						}
					}
					else if( sPushRet != enumKBACT_SUCCESS )
					{						
						/*pAccept->SystemNotice( "����ʱ����Ʒ��%s������%s����ʧ�ܣ�������Ʒ��ʧ��ID[%d], Num[%d]", pItem->szName, pRequest->GetName(), 
							AcpGrid[i].sID, ReqGrid[i].sNum );
						pRequest->SystemNotice( "����ʱ����Ʒ��%s������%s����ʧ�ܣ�������Ʒ��ʧ��ID[%d], Num[%d]", pItem->szName, pRequest->GetName(), 
							AcpGrid[i].sID, ReqGrid[i].sNum );
						LG( "trade_error", "Error code[%d],����ʱ����Ʒ��%s������%s����ʧ�ܣ�������Ʒ��ʧ��ID[%d], Num[%d]", sPushRet, pItem->szName, pRequest->GetName(), 
							AcpGrid[i].sID, ReqGrid[i].sNum );*/
						/*pAccept->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00071), pItem->szName, pRequest->GetName(), 
							AcpGrid[i].sID, ReqGrid[i].sNum );
						pRequest->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00071), pItem->szName, pRequest->GetName(), 
							AcpGrid[i].sID, ReqGrid[i].sNum );*/
						char szData[128];
						CFormatParameter param(4);
						param.setString( 0, pItem->szName );
						param.setString( 1, pRequest->GetName());
						param.setDouble( 2, AcpGrid[i].sID );
						param.setDouble( 3, ReqGrid[i].sNum );
						RES_FORMAT_STRING( GM_CHARTRADE_CPP_00071, param, szData );
						pAccept->SystemNotice( szData );
						pRequest->SystemNotice( szData );
						LG( "trade_error", "Error code[%d],it failed to put res in %s bag when trading res ��%s��!trade res failed!ID[%d], Num[%d]\n", sPushRet, pItem->szName, pRequest->GetName(), 
							AcpGrid[i].sID, ReqGrid[i].sNum );
					}
					else
					{
						AcpGrid[i].sNum = 0;
					}

					if( sPushRet != enumKBACT_ERROR_FULL && pItem->sType == enumItemTypeBoat )
					{
						if( !pRequest->BoatAdd( AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) ) )
						{
							/*pAccept->SystemNotice( "���Ӹ�%s����֤��ӵ�еĴ�ֻʧ�ܣ�ID[0x%X]", 
								pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							pRequest->SystemNotice( "���Ӹ�%s����֤��ӵ�еĴ�ֻʧ�ܣ�ID[0x%X]", 
								pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							LG( "trade_error", "���Ӹ�%s����֤��ӵ�еĴ�ֻʧ�ܣ�DBID[0x%X]", 
								pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );*/
							//pAccept->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00072), 
							//	pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							//pRequest->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00072), 
							//	pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							char szData[128];
							CFormatParameter param(2);
							param.setString( 0, pRequest->GetName() );
							param.setLong( 1, AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ));
							RES_FORMAT_STRING( GM_CHARTRADE_CPP_00072, param, szData );
							pAccept->SystemNotice( szData );
							pRequest->SystemNotice( szData );
							LG( "trade_error", "add to %scaptain confirm it hold boat failed!DBID[0x%X]\n", 
								pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
						}
					}
				}

				// 
				if( pTradeData->ReqTradeData.ItemArray[i].sItemID != 0 )
				{
					CItemRecord* pItem = GetItemRecordInfo( pTradeData->ReqTradeData.ItemArray[i].sItemID );
					if( pItem == NULL )
					{
						/*pRequest->SystemNotice( "��ƷID����,�޷��ҵ�����Ʒ��Ϣ�����ܸ��������Ʒ,ID = %d", 
							pTradeData->ReqTradeData.ItemArray[i].sItemID );
						pAccept->SystemNotice( "��ƷID����,�޷��ҵ�����Ʒ��Ϣ�����ܸ��������Ʒ,ID = %d", 
							pTradeData->ReqTradeData.ItemArray[i].sItemID );
						LG( "trade_error", "��ƷID����,�޷��ҵ�����Ʒ��Ϣ�����ܸ��������Ʒ,ID = %d", 
							pTradeData->ReqTradeData.ItemArray[i].sItemID );*/
						pRequest->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00069), 
							pTradeData->ReqTradeData.ItemArray[i].sItemID );
						pAccept->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00069), 
							pTradeData->ReqTradeData.ItemArray[i].sItemID );
						LG( "trade_error", "res ID error,cannot find this res information!this res cannot give you,ID = %d\n", 
							pTradeData->ReqTradeData.ItemArray[i].sItemID );
						continue;
					}

					// �������߸��Ķ������������
					USHORT sCount = ReqGrid[i].sNum;
					Short sPushPos = defKITBAG_DEFPUSH_POS;
					Short sPushRet = pAccept->KbPushItem( true, false, ReqGrid + i, sPushPos );
					
					if( sPushRet == enumKBACT_ERROR_FULL ) // ����������,��������
					{
						// �����Ʒ�����¼�
						USHORT sNum = sCount - ReqGrid[i].sNum;

						CCharacter	*pCCtrlCha = pAccept->GetPlyCtrlCha(), *pCMainCha = pAccept->GetPlyMainCha();
						Long	lPosX, lPosY;
						pCCtrlCha->GetTrowItemPos(&lPosX, &lPosY);
						if( pCCtrlCha->GetSubMap()->ItemSpawn( ReqGrid + i, lPosX, lPosY, enumITEM_APPE_THROW, pCCtrlCha->GetID(), pCMainCha->GetID(), pCMainCha->GetHandle() ) == NULL )
						{
							/*pAccept->SystemNotice( "����ʱ��%s����װ���µ���Ʒ��%s���ŵ�����ʧ�ܣ�������Ʒ��ʧ��ID[%d], Num[%d]", 
								pAccept->GetName(), pItem->szName, ReqGrid[i].sID, ReqGrid[i].sNum );
							pRequest->SystemNotice( "����ʱ��%s����װ���µ���Ʒ��%s���ŵ�����ʧ�ܣ�������Ʒ��ʧ��ID[%d], Num[%d]", 
								pAccept->GetName(), pItem->szName, ReqGrid[i].sID, ReqGrid[i].sNum );
							LG( "trade_error", "Error code[%d],����ʱ��%s����װ���µ���Ʒ��%s���ŵ�����ʧ�ܣ�������Ʒ��ʧ��ID[%d], Num[%d]", 
								sPushRet, pAccept->GetName(), pItem->szName, ReqGrid[i].sID, ReqGrid[i].sNum );*/
							/*pAccept->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00070), 
								pAccept->GetName(), pItem->szName, ReqGrid[i].sID, ReqGrid[i].sNum );
							pRequest->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00070), 
								pAccept->GetName(), pItem->szName, ReqGrid[i].sID, ReqGrid[i].sNum );*/
							char szData[128];
							CFormatParameter param(4);
							param.setString( 0, pAccept->GetName());
							param.setString( 1, pItem->szName );
							param.setDouble( 2, ReqGrid[i].sID );
							param.setDouble( 3, ReqGrid[i].sNum );
							RES_FORMAT_STRING( GM_CHARTRADE_CPP_00070, param, szData );
							pAccept->SystemNotice( szData );
							pRequest->SystemNotice( szData );
							LG( "trade_error", "Error code[%d],when trading,%s bag is full,��%s��failed to put on floor!trade res failed!ID[%d], Num[%d]\n", 
								sPushRet, pRequest->GetName(), pItem->szName, AcpGrid[i].sID, AcpGrid[i].sNum );
						}
					}
					else if( sPushRet != enumKBACT_SUCCESS )
					{						
						/*pAccept->SystemNotice( "����ʱ����Ʒ��%s������%s����ʧ�ܣ�������Ʒ��ʧ��ID[%d], Num[%d]", pItem->szName, pAccept->GetName(), 
							ReqGrid[i].sID, ReqGrid[i].sNum );
						pRequest->SystemNotice( "����ʱ����Ʒ��%s������%s����ʧ�ܣ�������Ʒ��ʧ��ID[%d], Num[%d]", pItem->szName, pAccept->GetName(), 
							ReqGrid[i].sID, ReqGrid[i].sNum );
						LG( "trade_error", "Error code[%d],����ʱ����Ʒ��%s������%s����ʧ�ܣ�������Ʒ��ʧ��ID[%d], Num[%d]", sPushRet, pItem->szName, pAccept->GetName(), 
							ReqGrid[i].sID, ReqGrid[i].sNum );*/
						//pAccept->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00071), pItem->szName, pAccept->GetName(), 
						//	ReqGrid[i].sID, ReqGrid[i].sNum );
						//pRequest->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00071), pItem->szName, pAccept->GetName(), 
						//	ReqGrid[i].sID, ReqGrid[i].sNum );

						char szData[128];
						CFormatParameter param(4);
						param.setString( 0, pItem->szName );
						param.setString( 1, pAccept->GetName());
						param.setDouble( 2, ReqGrid[i].sID );
						param.setDouble( 3, ReqGrid[i].sNum );
						RES_FORMAT_STRING( GM_CHARTRADE_CPP_00071, param, szData );
						pAccept->SystemNotice( szData );
						pRequest->SystemNotice( szData );

						LG( "trade_error", "Error code[%d],it failed to put res in %s bag when trading res ��%s��!trade res failed!ID[%d], Num[%d]\n", sPushRet, pItem->szName, pRequest->GetName(), 
							AcpGrid[i].sID, ReqGrid[i].sNum );
					}
					else 
					{
						ReqGrid[i].sNum = 0;
					}

					if( sPushRet != enumKBACT_ERROR_FULL && pItem->sType == enumItemTypeBoat )
					{
						if( !pAccept->BoatAdd( ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) ) )
						{
							/*pAccept->SystemNotice( "���Ӹ�%s����֤��ӵ�еĴ�ֻʧ�ܣ�ID[0x%X]", 
								pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							pRequest->SystemNotice( "���Ӹ�%s����֤��ӵ�еĴ�ֻʧ�ܣ�ID[0x%X]", 
								pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							LG( "trade_error", "���Ӹ�%s����֤��ӵ�еĴ�ֻʧ�ܣ�DBID[0x%X]", 
								pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );*/
							/*pAccept->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00072), 
								pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							pRequest->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00072), 
								pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );*/
							char szData[128];
							CFormatParameter param(2);
							param.setString( 0, pRequest->GetName() );
							param.setLong( 1, AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ));
							RES_FORMAT_STRING( GM_CHARTRADE_CPP_00072, param, szData );
							pAccept->SystemNotice( szData );
							pRequest->SystemNotice( szData );
							LG( "trade_error", "add to %scaptain confirm it hold boat failed!DBID[0x%X]\n", 
								pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
						}
					}
				}
			}

			// ��Ǯ
			if( pTradeData->ReqTradeData.dwMoney > 0 )
			{				
				pRequest->setAttr( ATTR_GD, pRequest->getAttr( ATTR_GD ) - pTradeData->ReqTradeData.dwMoney );
				pAccept->setAttr( ATTR_GD, pAccept->getAttr( ATTR_GD ) + pTradeData->ReqTradeData.dwMoney );				
			}

			if( pTradeData->AcpTradeData.dwMoney > 0 )
			{
				pAccept->setAttr( ATTR_GD, pAccept->getAttr( ATTR_GD ) - pTradeData->AcpTradeData.dwMoney );
				pRequest->setAttr( ATTR_GD, pRequest->getAttr( ATTR_GD ) + pTradeData->AcpTradeData.dwMoney );				
			}

			//sprintf( szTemp, "�����߽��׽�Ǯ��%d,�����߽��׽�Ǯ��%d", pTradeData->AcpTradeData.dwMoney, 
				//pTradeData->ReqTradeData.dwMoney );
			//sprintf( szTemp, RES_STRING(GM_CHARTRADE_CPP_00073), pTradeData->AcpTradeData.dwMoney, 
			//	pTradeData->ReqTradeData.dwMoney );
			//_snprintf_s( szTemp,sizeof(szTemp),_TRUNCATE, RES_STRING(GM_CHARTRADE_CPP_00073), pTradeData->AcpTradeData.dwMoney, 
			//	pTradeData->ReqTradeData.dwMoney );
			CFormatParameter param(2);
			param.setLong( 0, pTradeData->AcpTradeData.dwMoney );
			param.setLong( 1, pTradeData->ReqTradeData.dwMoney );
			RES_FORMAT_STRING( GM_CHARTRADE_CPP_00073, param, szTemp );

			//strcat( szTrade, szTemp );
			strncat_s( szTrade,sizeof(szTrade), szTemp,_TRUNCATE);

			pAccept->SetTradeData( NULL );
			pRequest->SetTradeData( NULL );
			pTradeData->Free();	

			// ���ݿ�洢
			game_db.BeginTran();
			if( !pRequest->SaveAssets() || !pAccept->SaveAssets() )
			{
				game_db.RollBack();

				// �������ݿ�洢ʧ��,���ݻָ�
				ReqBag = ReqBagData;
				AcpBag = AcpBagData;
				pRequest->setAttr( ATTR_GD, dwReqMoney );
				pAccept->setAttr( ATTR_GD, dwAcpMoney );

				// �ָ���ֻ������Ϣ
				for( int i = 0; i < ROLE_MAXNUM_TRADEDATA; i++ )
				{
					if( pTradeData->AcpTradeData.ItemArray[i].sItemID != 0 )
					{
						CItemRecord* pItem = GetItemRecordInfo( pTradeData->AcpTradeData.ItemArray[i].sItemID );
						if( pItem == NULL )
						{
							/*pRequest->SystemNotice( "��ƷID����,�޷��ҵ�����Ʒ��Ϣ�����ܸ��������Ʒ,ID = %d", 
							pTradeData->AcpTradeData.ItemArray[i].sItemID );
						pAccept->SystemNotice( "��ƷID����,�޷��ҵ�����Ʒ��Ϣ�����ܸ��������Ʒ,ID = %d", 
							pTradeData->AcpTradeData.ItemArray[i].sItemID );
						LG( "trade_error", "��ƷID����,�޷��ҵ�����Ʒ��Ϣ�����ܸ��������Ʒ,ID = %d", 
							pTradeData->AcpTradeData.ItemArray[i].sItemID );*/
						pRequest->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00069), 
							pTradeData->AcpTradeData.ItemArray[i].sItemID );
						pAccept->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00069), 
							pTradeData->AcpTradeData.ItemArray[i].sItemID );
						LG( "trade_error", "res ID error,it cannot find res information!it cannot give you this res,ID = %d\n", 
							pTradeData->AcpTradeData.ItemArray[i].sItemID );
							continue;
						}

						// �������߸��Ķ�������������					
						if( pItem->sType == enumItemTypeBoat )
						{
							if( !pRequest->BoatClear( AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) ) )
							{
								
								/*pAccept->SystemNotice( "ɾ��%s�Ĵ���֤��ӵ�еĴ�ֻʧ�ܣ�ID[0x%X]", 
									pRequest->GetName(), ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								pRequest->SystemNotice( "ɾ��%s�Ĵ���֤��ӵ�еĴ�ֻʧ�ܣ�ID[0x%X]", 
									pRequest->GetName(), ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								LG( "trade_error", "ɾ��%s�Ĵ���֤��ӵ�еĴ�ֻʧ�ܣ�DBID[0x%X]", 
									pRequest->GetName(), ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );*/
								/*pAccept->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00068), 
									pRequest->GetName(), ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								pRequest->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00068), 
									pRequest->GetName(), ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );*/
								char szData[128];
								CFormatParameter param(2);
								param.setString( 0, pRequest->GetName());
								param.setLong( 1, ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ));
								RES_FORMAT_STRING( GM_CHARTRADE_CPP_00068, param, szData );
								pAccept->SystemNotice( szData );
								pRequest->SystemNotice( szData );
								LG( "trade_error", "it failed to delete boat that captain confirm of %s have!DBID[0x%X]\n", 
									pRequest->GetName(), ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							}

							if( !pAccept->BoatAdd( AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) ) )
							{
								/*pAccept->SystemNotice( "���Ӹ�%s����֤��ӵ�еĴ�ֻʧ�ܣ�ID[0x%X]", 
								pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							pRequest->SystemNotice( "���Ӹ�%s����֤��ӵ�еĴ�ֻʧ�ܣ�ID[0x%X]", 
								pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							LG( "trade_error", "���Ӹ�%s����֤��ӵ�еĴ�ֻʧ�ܣ�DBID[0x%X]", 
								pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );*/
							//pAccept->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00072), 
							//	pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							//pRequest->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00072), 
							//	pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								char szData[128];
								CFormatParameter param(2);
								param.setString( 0, pRequest->GetName() );
								param.setLong( 1, AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ));
								RES_FORMAT_STRING( GM_CHARTRADE_CPP_00072, param, szData );
								pAccept->SystemNotice( szData );
								pRequest->SystemNotice( szData );
								LG( "trade_error", "add to %scaptain confirm it hold boat failed!DBID[0x%X]\n", 
									pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							}
						}
					}

					// 
					if( pTradeData->ReqTradeData.ItemArray[i].sItemID != 0 )
					{
						CItemRecord* pItem = GetItemRecordInfo( pTradeData->ReqTradeData.ItemArray[i].sItemID );
						if( pItem == NULL )
						{
							/*pRequest->SystemNotice( "��ƷID����,�޷��ҵ�����Ʒ��Ϣ�����ܸ��������Ʒ,ID = %d", 
							pTradeData->AcpTradeData.ItemArray[i].sItemID );
						pAccept->SystemNotice( "��ƷID����,�޷��ҵ�����Ʒ��Ϣ�����ܸ��������Ʒ,ID = %d", 
							pTradeData->AcpTradeData.ItemArray[i].sItemID );
						LG( "trade_error", "��ƷID����,�޷��ҵ�����Ʒ��Ϣ�����ܸ��������Ʒ,ID = %d", 
							pTradeData->AcpTradeData.ItemArray[i].sItemID );*/
						pRequest->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00069), 
							pTradeData->AcpTradeData.ItemArray[i].sItemID );
						pAccept->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00069), 
							pTradeData->AcpTradeData.ItemArray[i].sItemID );
						LG( "trade_error", "res ID error,it cannot find res information!it cannot give you this res,ID = %d\n", 
							pTradeData->AcpTradeData.ItemArray[i].sItemID );
							continue;
						}

						// �������߸��Ķ������������
						if( pItem->sType == enumItemTypeBoat )
						{
							if( !pAccept->BoatClear( ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) ) )
							{
								
								/*pAccept->SystemNotice( "ɾ��%s�Ĵ���֤��ӵ�еĴ�ֻʧ�ܣ�ID[0x%X]", 
									pRequest->GetName(), ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								pRequest->SystemNotice( "ɾ��%s�Ĵ���֤��ӵ�еĴ�ֻʧ�ܣ�ID[0x%X]", 
									pRequest->GetName(), ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								LG( "trade_error", "ɾ��%s�Ĵ���֤��ӵ�еĴ�ֻʧ�ܣ�DBID[0x%X]", 
									pRequest->GetName(), ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );*/
								/*pAccept->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00068), 
									pRequest->GetName(), ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								pRequest->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00068), 
									pRequest->GetName(), ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );*/
								char szData[128];
								CFormatParameter param(2);
								param.setString( 0, pRequest->GetName());
								param.setLong( 1, ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
								RES_FORMAT_STRING( GM_CHARTRADE_CPP_00068, param, szData );
								pAccept->SystemNotice( szData );
								pRequest->SystemNotice( szData );
								LG( "trade_error", "it failed to delete boat that captain confirm of %s have!DBID[0x%X]\n", 
									pRequest->GetName(), ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							}

							if( !pRequest->BoatAdd( ReqGrid[i].GetDBParam( enumITEMDBP_INST_ID ) ) )
							{
								/*pAccept->SystemNotice( "���Ӹ�%s����֤��ӵ�еĴ�ֻʧ�ܣ�ID[0x%X]", 
								pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							pRequest->SystemNotice( "���Ӹ�%s����֤��ӵ�еĴ�ֻʧ�ܣ�ID[0x%X]", 
								pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							LG( "trade_error", "���Ӹ�%s����֤��ӵ�еĴ�ֻʧ�ܣ�DBID[0x%X]", 
								pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );*/
							//pAccept->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00072), 
							//	pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							//pRequest->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00072), 
							//	pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							char szData[128];
							CFormatParameter param(2);
							param.setString( 0, pRequest->GetName() );
							param.setLong( 1, AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ));
							RES_FORMAT_STRING( GM_CHARTRADE_CPP_00072, param, szData );
							pAccept->SystemNotice( szData );
							pRequest->SystemNotice( szData );
							LG( "trade_error", "add to %scaptain confirm it hold boat failed!DBID[0x%X]\n", 
								pRequest->GetName(), AcpGrid[i].GetDBParam( enumITEMDBP_INST_ID ) );
							}
						}
					}
				}

				// ֪ͨ�ͻ��˲��Ҽ�¼��־
				/*pRequest->SystemNotice( "����ʧ��,���ݴ洢����" );
				pAccept->SystemNotice( "����ʧ��,���ݴ洢����" );
				LG( "trade_error", "�������ݴ洢���ݿ�ʧ��,�������ݻָ����,���ף����󷽡�%s��ID[0x%X],���ܷ���%s��ID[0x%X]��",
					pRequest->GetName(), pRequest->GetPlayer()->GetDBChaId(), pAccept->GetName(), pAccept->GetPlayer()->GetDBChaId() );*/
				pRequest->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00074) );
				pAccept->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00074) );
				LG( "trade_error", "the trade data failed to memory in DB,trade data resume complete,trade:request one��%s��ID[0x%X],accept one��%s��ID[0x%X].\n",
					pRequest->GetName(), pRequest->GetPlayer()->GetDBChaId(), pAccept->GetName(), pAccept->GetPlayer()->GetDBChaId() );

				// ȡ����ɫ����״̬
				pAccept->TradeAction( FALSE );
				pRequest->TradeAction( FALSE );

				// ��ɫ���׳ɹ�
				WPACKET packet = GETWPACKET();
				WRITE_CMD(packet, CMD_MC_CHARTRADE );
				WRITE_SHORT(packet, CMD_MC_CHARTRADE_RESULT );
				WRITE_CHAR(packet, TRADE_FAILER );

				pAccept->ReflectINFof( pMain, packet );
				pRequest->ReflectINFof( pMain, packet );

				return FALSE;
			}
			else
			{
				// �������ݴ洢�ɹ�
				game_db.CommitTran();
				if( pRequest->IsBoat() )
				{
					char szBoat1[64] = "";
					char szBoat2[64] = "";
					//sprintf( szBoat1, "%s����%s��", pAccept->GetPlyMainCha()->GetName(), pAccept->GetName() );
					//sprintf( szBoat2, "%s����%s��", pRequest->GetPlyMainCha()->GetName(), pRequest->GetName() );
					//sprintf( szBoat1, RES_STRING(GM_CHARTRADE_CPP_00075), pAccept->GetPlyMainCha()->GetName(), pAccept->GetName() );
					//_snprintf_s( szBoat1,sizeof(szBoat1),_TRUNCATE, RES_STRING(GM_CHARTRADE_CPP_00075), pAccept->GetPlyMainCha()->GetName(), pAccept->GetName() );
					CFormatParameter paramBoat1(2);
					paramBoat1.setString( 0, pAccept->GetPlyMainCha()->GetName());
					paramBoat1.setString( 1, pAccept->GetName());
					RES_FORMAT_STRING( GM_CHARTRADE_CPP_00075, paramBoat1, szBoat1);
					//sprintf( szBoat2, RES_STRING(GM_CHARTRADE_CPP_00075), pRequest->GetPlyMainCha()->GetName(), pRequest->GetName() );
					_snprintf_s( szBoat2,sizeof(szBoat2),_TRUNCATE, RES_STRING(GM_CHARTRADE_CPP_00075), pRequest->GetPlyMainCha()->GetName(), pRequest->GetName() );
					CFormatParameter paramBoat2(2);
					paramBoat2.setString( 0, pRequest->GetPlyMainCha()->GetName());
					paramBoat2.setString( 1, pRequest->GetName());
					RES_FORMAT_STRING( GM_CHARTRADE_CPP_00075, paramBoat2, szBoat2);
					TL( CHA_CHA, szBoat1, szBoat2, szTrade );
					
					//	2008-8-27	yangyinyu	add	begin!
					//lgtool_printTrade(
					//	pRequest,	//	�����ߡ�
					//	pAccept,	//	�����ߡ�
					//	pTradeData	//	�������ݡ�
					//	);
					//	2008-8-27	yangyinyu	add	end!

				}
				else
				{
					TL( CHA_CHA, pAccept->GetName(), pRequest->GetName(), szTrade );

					//	2008-8-27	yangyinyu	add	begin!
					//lgtool_printTrade(	pRequest,	pAccept,	pTradeData	);
					//	2008-8-27	yangyinyu	add	end!
				}

				pRequest->LogAssets(enumLASSETS_TRADE);
				pAccept->LogAssets(enumLASSETS_TRADE);
			}

			// ������Ʒ�ɹ����ȡ����
			for( int i = 0; i < ROLE_MAXNUM_TRADEDATA; i++ )
			{
				if( pTradeData->AcpTradeData.ItemArray[i].sItemID != 0 )
				{
					pAccept->RefreshNeedItem( pTradeData->AcpTradeData.ItemArray[i].sItemID );
					BYTE byNum = pTradeData->AcpTradeData.ItemArray[i].byCount - AcpGrid[i].sNum;
					if( byNum )
					{
						pRequest->AfterPeekItem( pTradeData->AcpTradeData.ItemArray[i].sItemID, byNum );
					}
				}

				if( pTradeData->ReqTradeData.ItemArray[i].sItemID != 0 )
				{
					pRequest->RefreshNeedItem( pTradeData->ReqTradeData.ItemArray[i].sItemID );
					BYTE byNum = pTradeData->ReqTradeData.ItemArray[i].byCount - ReqGrid[i].sNum;
					if( byNum )
					{
						pAccept->AfterPeekItem( pTradeData->ReqTradeData.ItemArray[i].sItemID, byNum );
					}
				}
			}

			// ֪ͨ���׽�Ǯ������Ϣ
			char szNotice[255];

			if( pTradeData->ReqTradeData.dwMoney )
			{
				//pAccept->SystemNotice( "���(%s)���õ���%d��Ǯ��", pRequest->GetName(), pTradeData->ReqTradeData.dwMoney );
				CFormatParameter param(2);
				param.setString(0, pRequest->GetName());
				param.setLong(1, pTradeData->ReqTradeData.dwMoney);

				RES_FORMAT_STRING(GM_CHARTRADE_CPP_00076, param, szNotice);

				pAccept->SystemNotice( szNotice );
			}

			if( pTradeData->AcpTradeData.dwMoney )
			{
				//pRequest->SystemNotice( "���(%s)���õ���%d��Ǯ��", pAccept->GetName(), pTradeData->AcpTradeData.dwMoney );
				CFormatParameter param(2);
				param.setString(0, pAccept->GetName());
				param.setLong(1, pTradeData->AcpTradeData.dwMoney);

				RES_FORMAT_STRING(GM_CHARTRADE_CPP_00076, param, szNotice);

				pRequest->SystemNotice( szNotice );
			}

			// ͬ����Ǯ���ݺͱ�������
			pAccept->SynAttr( enumATTRSYN_TRADE );
			pAccept->SyncBoatAttr(enumATTRSYN_TRADE);
			pRequest->SynAttr( enumATTRSYN_TRADE );	
			pRequest->SyncBoatAttr(enumATTRSYN_TRADE);

			pRequest->SynKitbagNew( enumSYN_KITBAG_TRADE );
			pAccept->SynKitbagNew( enumSYN_KITBAG_TRADE );

			// ȡ����ɫ����״̬
			pAccept->TradeAction( FALSE );
			pRequest->TradeAction( FALSE );

			// ��ɫ���׳ɹ�
			WPACKET packet = GETWPACKET();
			WRITE_CMD(packet, CMD_MC_CHARTRADE );
			WRITE_SHORT(packet, CMD_MC_CHARTRADE_RESULT );
			WRITE_CHAR(packet, TRADE_SUCCESS );
			
			pAccept->ReflectINFof( pMain, packet );
			pRequest->ReflectINFof( pMain, packet );
		
			//	2008-8-19	yangyinyu	add	begin!
//			for(	int	i	=	0;	i	<	ROLE_MAXNUM_TRADEDATA;	i	++	)
//			{
//				if(	pTradeData->AcpTradeData.ItemArray[i].sItemID	!=	0	)
//				{
//					//	*
//					SItemGrid*		pGrid	=	&AcpGrid[i];
//					CItemRecord*	pItem	=	GetItemRecordInfo(	pGrid->sID	);
///*
//					static	char	sType[256];
//					_snprintf(	sType,	256,	"��%s����",	pAccept->GetName()	);
//*/
//					::lgtool_PrintGetType(	RES_STRING(GM_CHARACTER_CPP_00144)	);
//					lgtool_PrintItem(	pRequest->GetName(),	pRequest->m_ID,	AcpGrid[i].sNum,	pItem,	pGrid	);
//				}
//
//			};

			//	2008-8-19	yangyinyu	add	end!

		}
		else
		{
			WPACKET packet = GETWPACKET();
			WRITE_CMD(packet, CMD_MC_CHARTRADE );
			WRITE_SHORT(packet, CMD_MC_CHARTRADE_VALIDATE );
			WRITE_LONG(packet, pMain->GetID() );
			if( pMain == pTradeData->pRequest )
			{
				pTradeData->pAccept->ReflectINFof( pMain, packet );
			}
			else
			{
				pTradeData->pRequest->ReflectINFof( pMain, packet );
			}
		}

		return TRUE;
	T_E}

	void CTradeSystem::ResetItemState( CCharacter& character, CTradeData& TradeData )
	{T_B
		int nCapacity = character.GetKitbag()->GetCapacity();
		CKitbag& Bag = *character.GetKitbag();
		TRADE_DATA* pItemData;
		if( &character == TradeData.pAccept )
		{
			pItemData = &TradeData.AcpTradeData;
		}
		else
		{
			pItemData = &TradeData.ReqTradeData;
		}
		
		for( int i = 0; i < ROLE_MAXNUM_TRADEDATA; i++ )
		{
			if( pItemData->ItemArray[i].byIndex < nCapacity )
			{
				Bag.Enable( pItemData->ItemArray[i].byIndex );
			}				
		}
	T_E}

	CStoreSystem::CStoreSystem() : m_bValid(false),m_bTrade(true)
	{T_B
		
	T_E}

    CStoreSystem::~CStoreSystem()
	{T_B

	T_E}

	BOOL CStoreSystem::PushOrder(CCharacter *pCha, long long lOrderID, long lComID, long lNum)
	{T_B
		SOrderData OrderInfo;
		OrderInfo.lOrderID = lOrderID;
		OrderInfo.ChaID = pCha->GetPlayer()->GetDBChaId();
		//strcpy(OrderInfo.ChaName, pCha->GetName());
		strncpy_s( OrderInfo.ChaName, sizeof(OrderInfo.ChaName), pCha->GetName(), _TRUNCATE );
		OrderInfo.lComID = lComID;
		OrderInfo.lNum = lNum;
		OrderInfo.lRecDBID = pCha->GetKitbagTmpRecDBID();
		OrderInfo.dwTickCount = GetTickCount();
		
		m_OrderList.push_back(OrderInfo);
		LG("Store_order", "PushOrder:[OrderID:%I64i][ChaID:%ld][ChaName:%s][ComID:%ld][Num:%ld][RecDBID:%ld][TickCount:%ld]\n", OrderInfo.lOrderID, OrderInfo.ChaID, OrderInfo.ChaName, OrderInfo.lComID, OrderInfo.lNum, OrderInfo.lRecDBID, OrderInfo.dwTickCount);
		return true;
	T_E}
	//Add by sunny.sun 20090519
	BOOL CStoreSystem::PushOrder( DWORD cha_id, string cha_name, long long lOrderID, long lComID, long lNum)
	{T_B
		SOrderData OrderInfo;
		OrderInfo.lOrderID = lOrderID;
		OrderInfo.ChaID = cha_id;
		//strcpy(OrderInfo.ChaName, pCha->GetName());
		strncpy_s( OrderInfo.ChaName, sizeof(OrderInfo.ChaName), cha_name.c_str(), _TRUNCATE );
		OrderInfo.lComID = lComID;
		OrderInfo.lNum = lNum;
		OrderInfo.lRecDBID = 0;
		OrderInfo.dwTickCount = GetTickCount();
		
		m_OrderList.push_back(OrderInfo);
		LG("Store_order", "PushOrder:[OrderID:%I64i][ChaID:%ld][ChaName:%s][ComID:%ld][Num:%ld][RecDBID:%ld][TickCount:%ld]\n", OrderInfo.lOrderID, OrderInfo.ChaID, OrderInfo.ChaName, OrderInfo.lComID, OrderInfo.lNum, OrderInfo.lRecDBID, OrderInfo.dwTickCount);
		return true;
	T_E}

	SOrderData CStoreSystem::PopOrder(long long lOrderID)
	{T_B
		SOrderData OrderInfo;
		BOOL bFound = false;

		vector<SOrderData>::iterator vec_it;
		for(vec_it = m_OrderList.begin(); vec_it != m_OrderList.end(); vec_it++)
		{
			if((*vec_it).lOrderID == lOrderID)
			{
				OrderInfo.ChaID = (*vec_it).ChaID;
				//strcpy(OrderInfo.ChaName, (*vec_it).ChaName);
				strncpy_s( OrderInfo.ChaName, sizeof(OrderInfo.ChaName), (*vec_it).ChaName, _TRUNCATE );
				OrderInfo.lComID = (*vec_it).lComID;
				OrderInfo.lNum = (*vec_it).lNum;
				OrderInfo.lOrderID = (*vec_it).lOrderID;
				OrderInfo.lRecDBID = (*vec_it).lRecDBID;
				OrderInfo.dwTickCount = (*vec_it).dwTickCount;
				m_OrderList.erase(vec_it);
				bFound = TRUE;
				LG("Store_order", "PopOrder:[OrderID:%I64i][ChaID:%ld][ChaName:%s][ComID:%ld][Num:%ld][RecDBID:%ld][TickCount:%ld]\n", OrderInfo.lOrderID, OrderInfo.ChaID, OrderInfo.ChaName, OrderInfo.lComID, OrderInfo.lNum, OrderInfo.lRecDBID, OrderInfo.dwTickCount);
				break;
			}
		}
		if(!bFound)
		{
			OrderInfo.ChaID = 0;
			OrderInfo.lComID = 0;
			OrderInfo.lNum = 0;
			OrderInfo.lOrderID = 0;
			OrderInfo.lRecDBID = 0;
		}
		return OrderInfo;
	T_E}

	BOOL CStoreSystem::HasOrder(long long lOrderID)
	{T_B
		vector<SOrderData>::iterator vec_it;
		for(vec_it = m_OrderList.begin(); vec_it != m_OrderList.end(); vec_it++)
		{
			if((*vec_it).lOrderID == lOrderID)
			{
				return true;
			}			
		}
		return false;
	T_E}

	long CStoreSystem::GetClassId(long lComID)
	{T_B
		map<long,long>::iterator it = m_ItemSearchList.find(lComID);
		if(it != m_ItemSearchList.end())
		{
			return m_ItemSearchList[lComID];
		}
		else
			return 0;
	T_E}

	cChar *CStoreSystem::GetClassName(long lClsID)
	{T_B
		vector<ClassInfo>::iterator vec_it;
		for(vec_it = m_ItemClass.begin(); vec_it != m_ItemClass.end(); vec_it++)
		{
			if((*vec_it).clsID == lClsID)
				return (*vec_it).clsName;
		}
		return NULL;
	T_E}

	SItemData *CStoreSystem::GetItemData(long lComID)
	{T_B
		long lClsID = GetClassId(lComID);
		if(lClsID != 0)
		{
			vector<SItemData>::iterator it;
			for(it = m_ItemList[lClsID].begin(); it != m_ItemList[lClsID].end(); it++)
			{
				if((*it).store_head.comID == lComID)
					return &(*it);
			}
		}
		return NULL;
	T_E}

	BOOL CStoreSystem::DelItemData(long lComID)
	{T_B
		long lClsID = 0;

		map<long,long>::iterator cls_it = m_ItemSearchList.find(lComID);
		if(cls_it != m_ItemSearchList.end())
		{
			lClsID = m_ItemSearchList[lComID];
			m_ItemSearchList.erase(cls_it);
		}

		if(lClsID != 0)
		{
			vector<SItemData>::iterator it;
			for(it = m_ItemList[lClsID].begin(); it != m_ItemList[lClsID].end(); it++)
			{
				if((*it).store_head.comID == lComID)
				{
					m_ItemList[lClsID].erase(it);
					break;
				}
			}
		}

		return TRUE;
	T_E}

    BOOL CStoreSystem::Request( CCharacter *pCha, long lComID )
    {T_B
		if(pCha->IsStoreBuy())
		{
			//pCha->SystemNotice("������һ��������δ������!");
			pCha->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00077));

			WPACKET WtPk	= GETWPACKET();
			WRITE_CMD(WtPk, CMD_MC_STORE_BUY_ASR);
			WRITE_CHAR(WtPk, 0);
			pCha->ReflectINFof(pCha, WtPk);

			return false;
		}

		SItemData *pComData = GetItemData(lComID);
		if(!pComData || pComData->store_head.comNumber == 0)
		{
			//pCha->SystemNotice("����Ʒ�ѳ���!");
			pCha->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00078));

			WPACKET WtPk	= GETWPACKET();
			WRITE_CMD(WtPk, CMD_MC_STORE_BUY_ASR);
			WRITE_CHAR(WtPk, 0);
			pCha->ReflectINFof(pCha, WtPk);

			return false;
		}

		cChar *szClsName = GetClassName(pComData->store_head.comClass);
		if(szClsName)
		{
			//if(!strcmp(szClsName, "�׽��Ա"))
			if(!strcmp(szClsName, RES_STRING(GM_CHARTRADE_CPP_00079)))
			{
				// Modify by lark.li 20080919 begin
				//if(pCha->GetPlayer()->GetVipType() == 0 || )
				//if(pCha->GetPlayer()->GetVipType() == 0)
					// End
				if(pCha->GetPlayer()->GetVipType() == 0 || pCha->m_SChaPart.sTypeID == 1 || pCha->m_SChaPart.sTypeID == 2  )
				{
					//pCha->SystemNotice("ֻ�а׽��Ա�����������Ʒ!");
					pCha->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00080));

					WPACKET WtPk	= GETWPACKET();
					WRITE_CMD(WtPk, CMD_MC_STORE_BUY_ASR);
					WRITE_CHAR(WtPk, 0);
					pCha->ReflectINFof(pCha, WtPk);

					return false;
				}
			}
		}

		short sGridNum = 0;
		ItemInfo *pItem = pComData->pItemArray;
		for(int i = 0; i < pComData->store_head.itemNum; i++)
		{
			CItemRecord* pItemRec = GetItemRecordInfo( pItem->itemID );
			if( pItemRec == NULL )
			{
				//pCha->SystemNotice( "Request: �������Ʒ�������ͣ�ID = %d", pItem->itemID );
				pCha->SystemNotice( RES_STRING(GM_CHARTRADE_CPP_00081), pItem->itemID );

				WPACKET WtPk	= GETWPACKET();
				WRITE_CMD(WtPk, CMD_MC_STORE_BUY_ASR);
				WRITE_CHAR(WtPk, 0);
				pCha->ReflectINFof(pCha, WtPk);

				return false;
			}
			if(pItemRec->GetIsPile())
			{
				sGridNum += 1;
			}
			else
			{
				sGridNum += pItem->itemNum;
			}

			pItem++;
		}

		if (!pCha->HasLeaveBagTempGrid(sGridNum))
		{
			//pCha->PopupNotice("������ʱ�����ռ䲻��!");
			pCha->PopupNotice(RES_STRING(GM_CHARTRADE_CPP_00082));
			return false;
		}
		
		pNetMessage pNm = new NetMessage();
		RoleInfo *pChaInfo = new RoleInfo();

        pChaInfo->actID = pCha->GetPlayer()->GetActLoginID();
		//strcpy(pChaInfo->actName, pCha->GetPlayer()->GetActName());
		strncpy_s( pChaInfo->actName, sizeof(pChaInfo->actName),  pCha->GetPlayer()->GetActName(), _TRUNCATE );
		pChaInfo->chaID = pCha->GetPlayer()->GetDBChaId();
		//strcpy(pChaInfo->chaName, pCha->GetName());
		strncpy_s( pChaInfo->chaName, sizeof(pChaInfo->chaName), pCha->GetName(), _TRUNCATE );
		pChaInfo->moBean = pCha->GetPlayer()->GetMoBean();
		pChaInfo->rplMoney = pCha->GetPlayer()->GetRplMoney();
		pChaInfo->vip = pCha->GetPlayer()->GetVipType();

		BuildNetMessage(pNm, INFO_STORE_BUY, 0, lComID, 0, (unsigned char*)pChaInfo, sizeof(RoleInfo));
		if(HasOrder(pNm->msgHead.msgOrder))
		{
			SAFE_DELETE(pChaInfo);
			FreeNetMessage(pNm);
			//LG("Store_msg", "������[ID:%I64i]�ظ�!\n", pNm->msgHead.msgOrder);
			LG("Store_msg", "order form number [ID:%I64i] repeat!\n", pNm->msgHead.msgOrder);

			WPACKET WtPk	= GETWPACKET();
			WRITE_CMD(WtPk, CMD_MC_STORE_BUY_ASR);
			WRITE_CHAR(WtPk, 0);
			pCha->ReflectINFof(pCha, WtPk);
			//pCha->SystemNotice("�̳ǲ���ʧ��, �������ظ�!");
			pCha->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00083));

			return false;
		}
		if(IsValid() && g_gmsvr->GetTradeServer()->SendData(pNm))
		{
			pCha->SetStoreBuy(true);
			PushOrder(pCha, pNm->msgHead.msgOrder, lComID, 1);
			//LG("Store_record", "��ɫ[%s][ID:%ld]��������Ʒ[ID:%ld]!\n", pChaInfo->chaName, pChaInfo->chaID, lComID);
			LG("Store_record", "character [%s][ID:%ld] order merchandise [ID:%ld]!\n", pChaInfo->chaName, pChaInfo->chaID, lComID);
		}
		else
		{
			WPACKET WtPk	= GETWPACKET();
			WRITE_CMD(WtPk, CMD_MC_STORE_BUY_ASR);
			WRITE_CHAR(WtPk, 0);
			pCha->ReflectINFof(pCha, WtPk);
			//pCha->SystemNotice("�̳ǽ���ʧ��!");
			pCha->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00084));

			//LG("Store_msg", "Request: TradeServer����ʧ��!\n");
		
		LG("Store_msg", "Request: TradeServer send failed!\n");}

		SAFE_DELETE(pChaInfo);
		FreeNetMessage(pNm);

        return true;
    T_E}

	BOOL CStoreSystem::Accept( CCharacter *pCha, long lComID )
	{T_B
		pCha->SetStoreBuy(false);
		SItemData *pComData = GetItemData(lComID);
		if(pComData)
		{
			long lNum = pComData->store_head.itemNum;
			ItemInfo *pItem = pComData->pItemArray;

			while(lNum-- > 0)
			{
				//	2008-8-19	yangyinyu	add	begin!
				//::lgtool_PrintGetType(	RES_STRING(GM_CHARACTER_CPP_00145)	);
				//	2008-8-19	yangyinyu	add	end!

				pCha->AddItem2KitbagTemp((short)pItem->itemID, (short)pItem->itemNum, pItem);
				pItem++;
			}

			//LG("Store_record", "��ɫ[%s][ID:%ld]��������Ʒ[ID:%ld], �ӵ��߲����ɹ�!\n", pCha->GetName(), pCha->GetPlayer()->GetDBChaId(), lComID);
			//LG("Store_record", RES_STRING(GM_CHARTRADE_CPP_00085), pCha->GetName(), pCha->GetPlayer()->GetDBChaId(), lComID);
			char szData[128];
			CFormatParameter param(3);
			param.setString( 0, pCha->GetName() );
			param.setLong( 1, pCha->GetPlayer()->GetDBChaId());
			param.setLong( 2, lComID );
			RES_FORMAT_STRING( GM_CHARTRADE_CPP_00085, param, szData );
			LG("Store_record", szData);

			if(pComData->store_head.comNumber > 0)
			{
				pComData->store_head.comNumber--;
			}

			//ɾ����Ʒ
			if(pComData->store_head.comNumber <= 0 && pComData->store_head.comNumber != -1)
			{
				DelItemData(lComID);
			}
		}
		else
		{
			//LG("Store_msg", "Accept2: �Ҳ�����Ʒ[ID:%ld]!\n", lComID);
			LG("Store_msg", "Accept2: cannot find merchandise [ID:%ld]!\n", lComID);
			return false;
		}
		return true;
	T_E}

	BOOL CStoreSystem::AcceptReturn( long long lOrderID, RoleInfo *ChaInfo )
	{T_B
		CCharacter *pCha = NULL;
		CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(ChaInfo->chaID);
		if(pPlayer)
		{
			pCha = pPlayer->GetMainCha();
		}

		//LG("Store_record", "��ɫ[%s][ID:%ld]��������Ʒ, ���˿�!\n", ChaInfo->chaName, lChaID);
		LG("Store_record", "character[%s][ID:%ld] return money!\n", ChaInfo->chaName, ChaInfo->chaID);

		if(pCha)
		{
			pCha->SetStoreBuy(false);
		}
		return TRUE;
		T_E
	}

    BOOL CStoreSystem::Accept( long long lOrderID, RoleInfo *ChaInfo )
    {T_B
		extern CGameApp *g_pGameApp;

		BOOL bSucc = false;
		SOrderData OrderInfo = PopOrder(lOrderID);
		if(OrderInfo.lOrderID != 0)
		{
			long lChaID = OrderInfo.ChaID;
			long lComID = OrderInfo.lComID;
			CCharacter *pCha = NULL;
			CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(lChaID);
			if(pPlayer)
			{
				pCha = pPlayer->GetMainCha();
			}

			//LG("Store_record", "��ɫ[%s][ID:%ld]��������Ʒ[ID:%ld], �ѿۿ�!\n", ChaInfo->chaName, lChaID, lComID);
			LG("Store_record", "character[%s][ID:%ld] has buy res[ID:%ld], has buckle money!\n", ChaInfo->chaName, lChaID, lComID);
			SItemData *pCData = GetItemData(lComID);
			if(pCData->store_head.comNumber > 0)
			{
				pCData->store_head.comNumber--;
			}

			if(!pCha)
			{
				//LG("Store_msg", "��ɫ[%s][ID:%ld]���뿪!\n", ChaInfo->chaName, lChaID);
				LG("Store_msg", "character[%s][ID:%ld] has leava!\n", ChaInfo->chaName, lChaID);
			}

			//�ӵ���
			if(pCha)
			{
				pCha->SetStoreBuy(false);
				SItemData *pComData = GetItemData(lComID);
				if(pComData)
				{
					long lNum = pComData->store_head.itemNum;
					ItemInfo *pItem = pComData->pItemArray;

					while(lNum-- > 0)
					{
						//	2008-8-19	yangyinyu	add	begin!
						//lgtool_PrintGetType(	"@ʰȡ@"	);
						//	2008-8-19	yangyinyu	add	end!

						pCha->AddItem2KitbagTemp((short)pItem->itemID, (short)pItem->itemNum, pItem);
						pItem++;

					}
					bSucc = true;

					pCha->GetPlayer()->SetMoBean(ChaInfo->moBean);
					pCha->GetPlayer()->SetRplMoney(ChaInfo->rplMoney);
					pCha->GetPlayer()->SetVipType(ChaInfo->vip);
				}
				else
				{
					//LG("Store_msg", "�Ҳ�����Ʒ[ID:%ld]!\n", lComID);
					LG("Store_msg", "cannot finde merchandise[ID:%ld]!\n", lComID);
				}
			}
			else
			{
				//LG("Store_msg", "��ɫ[%s][ID:%ld]���ڱ���ͼ!\n", ChaInfo->chaName, lChaID);
				LG("Store_msg", "character[%s][ID:%ld] don't in this map!\n", ChaInfo->chaName, lChaID);

				BOOL bOnline;
				if(!game_db.IsChaOnline(lChaID, bOnline))
				{
					//LG("Store_msg", "��ȡ��ɫ����״̬ʧ�ܡ�\n");
					LG("Store_msg", "it failed to get character online state��\n");
				}
				else
				{
					if(!bOnline)
					{
						//LG("Store_msg", "��ɫ�����ߡ�\n");
						LG("Store_msg", "character didn't online��\n");

						if(!game_db.SaveStoreItemID(lChaID, lComID))
						{
							//LG("Store_msg", "�������߽�ɫ��Ʒ��Ϣʧ�ܡ�\n");
							LG("Store_msg", "it failed to memory merchandise information who did not online character��\n");
						}
					}
					else
					{
						//LG("Store_msg", "��ɫ[%s][ID:%ld]��������ͼ!\n", ChaInfo->chaName, lChaID);
						LG("Store_msg", "character[%s][ID:%ld]is in other map!\n", ChaInfo->chaName, lChaID);

						BEGINGETGATE();
						CPlayer	*pCPlayer;
						CCharacter	*pChaOut = 0;
						GateServer	*pGateServer;
						while (pGateServer = GETNEXTGATE())
						{
							bool bFound = false;

							if (!BEGINGETPLAYER(pGateServer))
								continue;
							while (pCPlayer = (CPlayer *)GETNEXTPLAYER(pGateServer))
							{
								pChaOut = pCPlayer->GetMainCha();
								if(pChaOut)
								{
									WPACKET WtPk	=GETWPACKET();
									WRITE_CMD(WtPk, CMD_MM_STORE_BUY);
									WRITE_LONG(WtPk, pChaOut->GetID());
									WRITE_LONG(WtPk, lChaID);
									WRITE_LONG(WtPk, lComID);
									//WRITE_LONG(WtPk, ChaInfo->moBean);
									WRITE_LONG(WtPk, ChaInfo->rplMoney);
									pChaOut->ReflectINFof(pChaOut, WtPk);//ͨ��

									bFound = true;
									break;
								}
							}

							if(bFound)
							{
								break;
							}
						}		
					}
				}
			}
			//��¼
			if(bSucc)
			{
				//LG("Store_record", "��ɫ[%s][ID:%ld]��������Ʒ[ID:%ld], �ӵ��߲����ɹ�!\n", pCha->GetName(), lChaID, lComID);
				LG("Store_record", "char [%s][ID:%ld] buy [ID:%ld], give item success!\n", pCha->GetName(), lChaID, lComID);

				//֪ͨ��ҽ��׳ɹ�
				WPACKET WtPk	= GETWPACKET();
				WRITE_CMD(WtPk, CMD_MC_STORE_BUY_ASR);
				WRITE_CHAR(WtPk, 1);
				WRITE_LONG(WtPk, ChaInfo->rplMoney);

				pCha->ReflectINFof(pCha, WtPk);
			}
			else
			{
			}

			//ɾ����Ʒ
			if(pCData->store_head.comNumber <= 0 && pCData->store_head.comNumber != -1)
			{
				DelItemData(lComID);
			}
		}
		else
		{
			//LG("Store_msg", "Accept:�Ҳ�������[ID:%I64i]!\n", lOrderID);
			LG("Store_msg", "Accept:not find order form[ID:%I64i]!\n", lOrderID);

			// �˿��
			pNetMessage pNm = new NetMessage();

			BuildNetMessage(pNm, INFO_STORE_BUY_RETURN, 0, 0, 0, (unsigned char*)ChaInfo, sizeof(RoleInfo));
			pNm->msgHead.msgOrder = lOrderID; // �ϴζ�������

			if(g_gmsvr->GetTradeServer()->SendData(pNm))
			{
				CCharacter *pCha = NULL;
				CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(ChaInfo->chaID);
				if(pPlayer)
				{
					pCha = pPlayer->GetMainCha();
				}
				if(pCha)
					pCha->SetStoreBuy(true);

				LG("Store_record", "character [%s][ID:%ld] order return money\n", ChaInfo->chaName, ChaInfo->chaID);
			}
		}
        return true;
    T_E}

    BOOL CStoreSystem::Cancel( long long lOrderID )
    {T_B
		extern CGameApp *g_pGameApp;

		SOrderData OrderInfo = PopOrder(lOrderID);
		
		CCharacter *pCha = NULL;
		CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(OrderInfo.ChaID);
		if(pPlayer)
		{
			pCha = pPlayer->GetMainCha();
		}

		if(OrderInfo.lOrderID != 0)
		{
			//֪ͨ��ҽ���ʧ��
			if(pCha)
			{
				pCha->SetStoreBuy(false);

				WPACKET WtPk	= GETWPACKET();
				WRITE_CMD(WtPk, CMD_MC_STORE_BUY_ASR);
				WRITE_CHAR(WtPk, 0);
				pCha->ReflectINFof(pCha, WtPk);

				//pCha->SystemNotice("�̳ǽ���ʧ��!");
				pCha->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00084));
				//LG("Store_data", "[%s]�������[ComID:%ld]ʧ��!\n", pCha->GetName(), OrderInfo.lComID);
				LG("Store_data", "[%s]failed to buy prop [ComID:%ld]!\n", pCha->GetName(), OrderInfo.lComID);
			}
		}
		else
		{
			//LG("Store_msg", "Cancel:�Ҳ�������[ID:%I64i]!\n", lOrderID);
			LG("Store_msg", "Cancel:not find order form[ID:%I64i]!\n", lOrderID);
		}
        return true;
    T_E}

    void CStoreSystem::Run( DWORD dwCurTime, DWORD dwIntervalTime, DWORD dwOrderTime )
    {
		try
		{
			static DWORD dwLastTime = 0;
			if(dwCurTime - dwLastTime < dwIntervalTime)
			{
				return;
			}
			else
			{
				dwLastTime = dwCurTime;
			}

			vector<SOrderData>::iterator vec_it;
			for(vec_it = m_OrderList.begin(); vec_it != m_OrderList.end(); vec_it++)
			{
				if(dwCurTime - (*vec_it).dwTickCount > dwOrderTime)
				{
					DWORD dwChaID = (*vec_it).ChaID;
					LG("Store_order", "timeout:[OrderID:%I64i][ChaID:%ld][ChaName:%s][ComID:%ld][Num:%ld][RecDBID:%ld][TickCount:%ld]\n", (*vec_it).lOrderID, (*vec_it).ChaID, (*vec_it).ChaName, (*vec_it).lComID, (*vec_it).lNum, (*vec_it).lRecDBID, (*vec_it).dwTickCount);
					m_OrderList.erase(vec_it);

					CCharacter *pCha = NULL;
					CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(dwChaID);
					if(pPlayer)
					{
						pCha = pPlayer->GetMainCha();
					}
					if(pCha)
					{
						//pCha->SystemNotice("�̳ǲ�����ʱ,�ѱ�ȡ��!");
						pCha->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00095));
					}

					break;
				}
			}
		}
		catch(...)
		{
			LG("Store_Error", "run error\n");
		}
    }

	BOOL CStoreSystem::GetItemList()
	{T_B
		//LG("Store_msg", "�����̳��б�!\n");
		LG("Store_msg", "ask for store list!\n");
		pNetMessage pNm = new NetMessage();
		BuildNetMessage(pNm, INFO_REQUEST_STORE, 0, 0, 0, NULL, 0);
		g_gmsvr->GetTradeServer()->SendData(pNm);
		FreeNetMessage(pNm);
		return true;
	T_E}

	BOOL CStoreSystem::RequestItemList(CCharacter *pCha, long lClsID, short sPage, short sNum)
	{T_B
		map< long, vector<SItemData> >::iterator map_it = m_ItemList.find(lClsID);
		if(map_it != m_ItemList.end())
		{
			WPACKET WtPk	= GETWPACKET();
			WRITE_CMD(WtPk, CMD_MC_STORE_LIST_ASR);
			short sItemNum = 0;
			short sPageNum = static_cast<short>((m_ItemList[lClsID].size() % sNum == 0) ? (m_ItemList[lClsID].size() / sNum) : (m_ItemList[lClsID].size() / sNum + 1));
			WRITE_SHORT(WtPk, sPageNum);
			WRITE_SHORT(WtPk, sPage);
			if(sPage > sPageNum)
			{
				sItemNum = 0;
				//LG("Store_msg", "��������ҳ�泬���˷�Χ!\n");
				LG("Store_msg", "player open-eared page layout over range!\n");
			}
			else if(sPage == sPageNum)
			{
				sItemNum = static_cast<short>(m_ItemList[lClsID].size() - (sPage - 1) * sNum);
			}
			else
			{
				sItemNum = sNum;
			}
			WRITE_SHORT(WtPk, sItemNum);
			vector<SItemData>::iterator it = m_ItemList[lClsID].begin();
			int i;
			for(i = 0; i < (sPage - 1) * sNum; i++)
			{
				it++;
			}
			for(i = 0; i < sItemNum; i++)
			{
				long l_time = (long)((*it).store_head.comExpire);
				if(l_time <= 0)
				{
					l_time = -1;
				}
				else
				{
					l_time -= (long)time(0);
					l_time /= 3600;
					if(l_time < 1)
					{
						l_time = 1;
					}
				}

				WRITE_LONG(WtPk, (*it).store_head.comID);
				WRITE_SEQ(WtPk, (*it).store_head.comName, uShort(strlen((*it).store_head.comName) + 1));
				WRITE_LONG(WtPk, (*it).store_head.comPrice);
				WRITE_SEQ(WtPk, (*it).store_head.comRemark, USHORT(strlen((*it).store_head.comRemark) + 1));
				WRITE_CHAR(WtPk, (*it).store_head.isHot);
				WRITE_LONG(WtPk, static_cast<long>((*it).store_head.comTime));
				WRITE_LONG(WtPk, static_cast<long>((*it).store_head.comNumber));
				WRITE_LONG(WtPk, l_time);

				WRITE_SHORT(WtPk, (*it).store_head.itemNum);
				int j;
				for(j = 0; j < (*it).store_head.itemNum; j++)
				{
					WRITE_SHORT(WtPk, (*it).pItemArray[j].itemID);
					WRITE_SHORT(WtPk, (*it).pItemArray[j].itemNum);
					WRITE_SHORT(WtPk, (*it).pItemArray[j].itemFlute);
					int k;
					for(k = 0; k < 5; k++)
					{
						WRITE_SHORT(WtPk, (*it).pItemArray[j].itemAttrID[k]);
						WRITE_SHORT(WtPk, (*it).pItemArray[j].itemAttrVal[k]);
					}
				}

				it++;
			}
			pCha->ReflectINFof(pCha, WtPk);
		}
		else
		{
			WPACKET WtPk	= GETWPACKET();
			WRITE_CMD(WtPk, CMD_MC_STORE_LIST_ASR);
			WRITE_SHORT(WtPk, 0);
			WRITE_SHORT(WtPk, sPage);
			WRITE_SHORT(WtPk, 0);
			pCha->ReflectINFof(pCha, WtPk);
		}
		
		return true;
	T_E}

	BOOL CStoreSystem::RequestVIP(CCharacter *pCha, short sVipID, short sMonth)
	{T_B
		pNetMessage pNm = new NetMessage();
		RoleInfo *pChaInfo = new RoleInfo();

		pChaInfo->actID = pCha->GetPlayer()->GetActLoginID();
		//strcpy(pChaInfo->actName, pCha->GetPlayer()->GetActName());
		strncpy_s( pChaInfo->actName, sizeof(pChaInfo->actName), pCha->GetPlayer()->GetActName(), _TRUNCATE );
		pChaInfo->chaID = pCha->GetPlayer()->GetDBChaId();
		//strcpy(pChaInfo->chaName, pCha->GetName());
		strncpy_s( pChaInfo->chaName, sizeof(pChaInfo->chaName), pCha->GetName(), _TRUNCATE );
		pChaInfo->moBean = pCha->GetPlayer()->GetMoBean();
		pChaInfo->rplMoney = pCha->GetPlayer()->GetRplMoney();
		pChaInfo->vip = pCha->GetPlayer()->GetVipType();

		DWORD dwVipParam = ((sVipID << 16) & 0xffff0000) | (sMonth & 0x0000ffff);

		BuildNetMessage(pNm, INFO_REGISTER_VIP, 0, dwVipParam, 0, (unsigned char*)pChaInfo, sizeof(RoleInfo));
		if(HasOrder(pNm->msgHead.msgOrder))
		{
			SAFE_DELETE(pChaInfo);
			FreeNetMessage(pNm);
			//LG("Store_msg", "������[ID:%I64i]�ظ�!\n", pNm->msgHead.msgOrder);
			LG("Store_msg", "order form number[ID:%I64i]repeat!\n", pNm->msgHead.msgOrder);

			WPACKET WtPk	= GETWPACKET();
			WRITE_CMD(WtPk, CMD_MC_STORE_VIP);
			WRITE_CHAR(WtPk, 0);
			pCha->ReflectINFof(pCha, WtPk);
			//pCha->SystemNotice("�̳ǲ���ʧ��, �������ظ�!");
			pCha->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00083));

			return false;
		}
		if(IsValid() && g_gmsvr->GetTradeServer()->SendData(pNm))
		{
			PushOrder(pCha, pNm->msgHead.msgOrder, 0, 0);
		}
		else
		{
			WPACKET WtPk	= GETWPACKET();
			WRITE_CMD(WtPk, CMD_MC_STORE_VIP);
			WRITE_CHAR(WtPk, 0);
			pCha->ReflectINFof(pCha, WtPk);

		//	LG("Store_msg", "RequestVIP: TradeServer����ʧ��!\n");
			LG("Store_msg", "RequestVIP: TradeServer send failed!\n");
		}

		SAFE_DELETE(pChaInfo);
		FreeNetMessage(pNm);

		return true;
	T_E}

	BOOL CStoreSystem::AcceptVIP(long long lOrderID, RoleInfo *ChaInfo, DWORD dwVipParam)
	{T_B
		extern CGameApp *g_pGameApp;

		SOrderData OrderInfo = PopOrder(lOrderID);

		if(OrderInfo.lOrderID == 0)
		{
			//LG("Store_msg", "AcceptVIP:�Ҳ�������[ID:%I64i]!\n", lOrderID);
			LG("Store_msg", "AcceptVIP: cannot find order form[ID:%I64i]!\n", lOrderID);
			return false;
		}

		CCharacter *pCha = NULL;
		CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(OrderInfo.ChaID);
		if(pPlayer)
		{
			pCha = pPlayer->GetMainCha();
		}

		if(pCha)
		{
			pCha->ResetStoreTime();
			pCha->GetPlayer()->SetMoBean(ChaInfo->moBean);
			pCha->GetPlayer()->SetRplMoney(ChaInfo->rplMoney);
			pCha->GetPlayer()->SetVipType(ChaInfo->vip);

			WPACKET WtPk	= GETWPACKET();
			WRITE_CMD(WtPk, CMD_MC_STORE_VIP);
			WRITE_CHAR(WtPk, 1);
			WRITE_SHORT(WtPk, HIWORD(dwVipParam));
			WRITE_SHORT(WtPk, LOWORD(dwVipParam));
			WRITE_LONG(WtPk, ChaInfo->rplMoney);
			WRITE_LONG(WtPk, ChaInfo->moBean);
			pCha->ReflectINFof(pCha, WtPk);
			//pCha->PopupNotice("����׽��Ա�ɹ�!");
			pCha->PopupNotice(RES_STRING(GM_CHARTRADE_CPP_00086));
			//LG("Store_data", "[%s]����VIP�ɹ�!\n", pCha->GetName());
			LG("Store_data", "[%s] purchase VIP succeed!\n", pCha->GetName());
		}
		return true;
	T_E}

	BOOL CStoreSystem::CancelVIP(long long lOrderID)
	{T_B
		extern CGameApp *g_pGameApp;

		SOrderData OrderInfo = PopOrder(lOrderID);
		if(OrderInfo.lOrderID != 0)
		{
			CCharacter *pCha = NULL;
			CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(OrderInfo.ChaID);
			if(pPlayer)
			{
				pCha = pPlayer->GetMainCha();
			}

			if(pCha)
			{
				pCha->ResetStoreTime();
				WPACKET WtPk	= GETWPACKET();
				WRITE_CMD(WtPk, CMD_MC_STORE_VIP);
				WRITE_CHAR(WtPk, 0);
				pCha->ReflectINFof(pCha, WtPk);
				//pCha->PopupNotice("����׽��Աʧ��!");
				pCha->PopupNotice(RES_STRING(GM_CHARTRADE_CPP_00087));
				//LG("Store_data", "[%s]����VIPʧ��!\n", pCha->GetName());
				LG("Store_data", "[%s]perchase VIP failed!\n", pCha->GetName());
			}
		}
		else
		{
			//LG("Store_msg", "CancelVIP:�Ҳ�������[ID:%I64i]!\n", lOrderID);
			LG("Store_msg", "CancelVIP:cannot find order form[ID:%I64i]!\n", lOrderID);
		}
		return true;
	T_E}

	BOOL CStoreSystem::RequestChange(CCharacter *pCha, long lNum)
	{T_B
		pNetMessage pNm = new NetMessage();
		RoleInfo *pChaInfo = new RoleInfo();

		pChaInfo->actID = pCha->GetPlayer()->GetActLoginID();
		//strcpy(pChaInfo->actName, pCha->GetPlayer()->GetActName());
		strncpy_s( pChaInfo->actName, sizeof(pChaInfo->actName), pCha->GetPlayer()->GetActName(), _TRUNCATE );
		pChaInfo->chaID = pCha->GetPlayer()->GetDBChaId();
		//strcpy(pChaInfo->chaName, pCha->GetName());
		strncpy_s( pChaInfo->chaName, sizeof(pChaInfo->chaName), pCha->GetName(), _TRUNCATE );
		pChaInfo->moBean = pCha->GetPlayer()->GetMoBean();
		pChaInfo->rplMoney = pCha->GetPlayer()->GetRplMoney();
		pChaInfo->vip = pCha->GetPlayer()->GetVipType();

		BuildNetMessage(pNm, INFO_EXCHANGE_MONEY, 0, lNum, 0, (unsigned char*)pChaInfo, sizeof(RoleInfo));
		if(HasOrder(pNm->msgHead.msgOrder))
		{
			SAFE_DELETE(pChaInfo);
			FreeNetMessage(pNm);
			//LG("Store_msg", "������[ID:%I64i]�ظ�!\n", pNm->msgHead.msgOrder);
			LG("Store_msg", "order form number [ID:%I64i] repeat!\n", pNm->msgHead.msgOrder);

			WPACKET WtPk	= GETWPACKET();
			WRITE_CMD(WtPk, CMD_MC_STORE_CHANGE_ASR);
			WRITE_CHAR(WtPk, 0);
			pCha->ReflectINFof(pCha, WtPk);
			//pCha->SystemNotice("�̳ǲ���ʧ��, �������ظ�!");
			pCha->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00083));

			return false;
		}
		if(IsValid() && g_gmsvr->GetTradeServer()->SendData(pNm))
		{
			PushOrder(pCha, pNm->msgHead.msgOrder, 0, 0);
		}
		else
		{
			WPACKET WtPk	= GETWPACKET();
			WRITE_CMD(WtPk, CMD_MC_STORE_CHANGE_ASR);
			WRITE_CHAR(WtPk, 0);
			pCha->ReflectINFof(pCha, WtPk);

			//LG("Store_msg", "RequestChange: TradeServer����ʧ��!\n");
			LG("Store_msg", "RequestChange: TradeServer send failed!\n");
		}

		SAFE_DELETE(pChaInfo);
		FreeNetMessage(pNm);

		return true;
	T_E}

	BOOL CStoreSystem::AcceptChange(long long lOrderID, RoleInfo *ChaInfo)
	{T_B
		extern CGameApp *g_pGameApp;

		SOrderData OrderInfo = PopOrder(lOrderID);

		if(OrderInfo.lOrderID == 0)
		{
			//LG("Store_msg", "AcceptChange:�Ҳ�������[ID:%I64i]!\n", lOrderID);
			LG("Store_msg", "AcceptChange:cannot find order form [ID:%I64i]!\n", lOrderID);
			return false;
		}

		CCharacter *pCha = NULL;
		CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(OrderInfo.ChaID);
		if(pPlayer)
		{
			pCha = pPlayer->GetMainCha();
		}

		if(pCha)
		{
			pCha->ResetStoreTime();
			pCha->GetPlayer()->SetMoBean(ChaInfo->moBean);
			pCha->GetPlayer()->SetRplMoney(ChaInfo->rplMoney);
			pCha->GetPlayer()->SetVipType(ChaInfo->vip);

			WPACKET WtPk	= GETWPACKET();
			WRITE_CMD(WtPk, CMD_MC_STORE_CHANGE_ASR);
			WRITE_CHAR(WtPk, 1);
			WRITE_LONG(WtPk, ChaInfo->moBean);
			WRITE_LONG(WtPk, ChaInfo->rplMoney);
			pCha->ReflectINFof(pCha, WtPk);
			//LG("Store_data", "[%s]�һ����ҳɹ�!\n", pCha->GetName());
			LG("Store_data", "[%s]change token succeed!\n", pCha->GetName());
		}
		return true;
	T_E}

	BOOL CStoreSystem::CancelChange(long long lOrderID)
	{T_B
		extern CGameApp *g_pGameApp;

		SOrderData OrderInfo = PopOrder(lOrderID);
		if(OrderInfo.lOrderID != 0)
		{
			//֪ͨ��Ҷһ�����ʧ��
			CCharacter *pCha = NULL;
			CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(OrderInfo.ChaID);
			if(pPlayer)
			{
				pCha = pPlayer->GetMainCha();
			}

			if(pCha)
			{
				pCha->ResetStoreTime();
				WPACKET WtPk	= GETWPACKET();
				WRITE_CMD(WtPk, CMD_MC_STORE_CHANGE_ASR);
				WRITE_CHAR(WtPk, 0);
				pCha->ReflectINFof(pCha, WtPk);
				//LG("Store_data", "[%s]�һ�����ʧ��!\n", pCha->GetName());
				LG("Store_data", "[%s]change token failed!\n", pCha->GetName());
			}
		}
		else
		{
			LG("Store_msg", "CancelChange:cannot find order form[ID:%I64i]!\n", lOrderID);
		}
		return true;
	T_E}

	BOOL CStoreSystem::RequestRoleInfo(CCharacter *pCha)
	{T_B
		pNetMessage pNm = new NetMessage();
		RoleInfo *pChaInfo = new RoleInfo();

		pChaInfo->actID = pCha->GetPlayer()->GetActLoginID();
		//strcpy(pChaInfo->actName, pCha->GetPlayer()->GetActName());
		strncpy_s( pChaInfo->actName, sizeof(pChaInfo->actName), pCha->GetPlayer()->GetActName(), _TRUNCATE );
		pChaInfo->chaID = pCha->GetPlayer()->GetDBChaId();
		//strcpy(pChaInfo->chaName, pCha->GetName());
		strncpy_s( pChaInfo->chaName, sizeof(pChaInfo->chaName), pCha->GetName(), _TRUNCATE );

		BuildNetMessage(pNm, INFO_REQUEST_ACCOUNT, 0, 0, 0, (unsigned char*)pChaInfo, sizeof(RoleInfo));
		if(HasOrder(pNm->msgHead.msgOrder))
		{
			SAFE_DELETE(pChaInfo);
			FreeNetMessage(pNm);
			//LG("Store_msg", "������[ID:%I64i]�ظ�!\n", pNm->msgHead.msgOrder);
			LG("Store_msg", "order form number[ID:%I64i]repeat!\n", pNm->msgHead.msgOrder);
			//pCha->SystemNotice("�̳ǲ���ʧ��, �������ظ�!");
			pCha->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00083));

			return false;
		}
		if(IsValid() && g_gmsvr->GetTradeServer()->SendData(pNm))
		{
			PushOrder(pCha, pNm->msgHead.msgOrder, 0, 0);
		}
		else
		{
			BOOL bValid = IsValid();
			if(bValid)
			{
				InValid();
			}

			pCha->GetPlayer()->SetMoBean(0);
			pCha->GetPlayer()->SetRplMoney(0);
			pCha->GetPlayer()->SetVipType(0);
			g_StoreSystem.Open(pCha, 0);

			if(bValid)
			{
				SetValid();
			}

			//LG("Store_msg", "RequestRoleInfo: TradeServer����ʧ��!\n");
			LG("Store_msg", "RequestRoleInfo: TradeServer send failed!\n");
		}

		SAFE_DELETE(pChaInfo);
		FreeNetMessage(pNm);

		return true;
	T_E}

	BOOL CStoreSystem::AcceptRoleInfo(long long lOrderID, RoleInfo *ChaInfo)
	{T_B
		extern CGameApp *g_pGameApp;

		SOrderData OrderInfo = PopOrder(lOrderID);

		if(OrderInfo.lOrderID == 0)
		{
			//LG("Store_msg", "AcceptRoleInfo:�Ҳ�������[ID:%I64i]!\n", lOrderID);
			LG("Store_msg", "AcceptRoleInfo:cannot find order form [ID:%I64i]!\n", lOrderID);
			return false;
		}

		long lChaID = ChaInfo->chaID;
		long lMoBean = ChaInfo->moBean;
		long lRplMoney = ChaInfo->rplMoney;
		long lVip = ChaInfo->vip;
		CCharacter *pCha = NULL;
		CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(lChaID);
		if(pPlayer)
		{
			pCha = pPlayer->GetMainCha();
		}

		if(pCha)
		{
			pCha->ResetStoreTime();
			pCha->GetPlayer()->SetMoBean(lMoBean);
			pCha->GetPlayer()->SetRplMoney(lRplMoney);
			pCha->GetPlayer()->SetVipType(lVip);

			g_StoreSystem.Open(pCha, lVip);
			//LG("Store_data", "[%s]��ȡ�ʻ���Ϣ�ɹ�!\n", pCha->GetName());
			LG("Store_data", "[%s]get account information succeed!\n", pCha->GetName());
		}
		return true;
	T_E}

	BOOL CStoreSystem::CancelRoleInfo(long long lOrderID)
	{T_B
		extern CGameApp *g_pGameApp;

		SOrderData OrderInfo = PopOrder(lOrderID);
		if(OrderInfo.lOrderID == 0)
		{
			//LG("Store_msg", "CancelRoleInfo:�Ҳ�������[ID:%I64i]!\n", lOrderID);
			LG("Store_msg", "CancelRoleInfo:cannot find order form[ID:%I64i]!\n", lOrderID);
			return false;
		}

		long lChaID = OrderInfo.ChaID;
		CCharacter *pCha = NULL;
		CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(lChaID);
		if(pPlayer)
		{
			pCha = pPlayer->GetMainCha();
		}

		if(pCha)
		{
			pCha->GetPlayer()->SetMoBean(0);
			pCha->GetPlayer()->SetRplMoney(0);
			pCha->GetPlayer()->SetVipType(0);
			pCha->ResetStoreTime();
			//g_StoreSystem.Open(pCha, 0);
			/*pCha->SystemNotice("�޷��鵽�����˺�,���̳�ʧ��!");
			LG("Store_data", "[%s]��ȡ�ʻ���Ϣʧ��!\n", pCha->GetName());*/
			pCha->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00088));
			LG("Store_data", "[%s]get account information failed!\n", pCha->GetName());
		}
		return true;
	T_E}

	BOOL CStoreSystem::RequestRecord(CCharacter *pCha, long lNum)
	{T_B
		pNetMessage pNm = new NetMessage();
		RoleInfo *pChaInfo = new RoleInfo();

		pChaInfo->actID = pCha->GetPlayer()->GetActLoginID();
		//strcpy(pChaInfo->actName, pCha->GetPlayer()->GetActName());
		strncpy_s( pChaInfo->actName, sizeof(pChaInfo->actName), pCha->GetPlayer()->GetActName(), _TRUNCATE );
		pChaInfo->chaID = pCha->GetPlayer()->GetDBChaId();
		//strcpy(pChaInfo->chaName, pCha->GetName());
		strncpy_s( pChaInfo->chaName, sizeof(pChaInfo->chaName), pCha->GetName(), _TRUNCATE );
		pChaInfo->moBean = pCha->GetPlayer()->GetMoBean();
		pChaInfo->rplMoney = pCha->GetPlayer()->GetRplMoney();
		pChaInfo->vip = pCha->GetPlayer()->GetVipType();

		BuildNetMessage(pNm, INFO_REQUEST_HISTORY, 0, lNum, 0, (unsigned char*)pChaInfo, sizeof(RoleInfo));
		if(HasOrder(pNm->msgHead.msgOrder))
		{
			SAFE_DELETE(pChaInfo);
			FreeNetMessage(pNm);
			//LG("Store_msg", "������[ID:%I64i]�ظ�!\n", pNm->msgHead.msgOrder);
			LG("Store_msg", "order form number[ID:%I64i]repeat!\n", pNm->msgHead.msgOrder);

			WPACKET WtPk	= GETWPACKET();
			WRITE_CMD(WtPk, CMD_MC_STORE_QUERY);
			WRITE_CHAR(WtPk, 0);
			pCha->ReflectINFof(pCha, WtPk);
			//pCha->SystemNotice("�̳ǲ���ʧ��, �������ظ�!");
			pCha->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00083));

			return false;
		}
		if(IsValid() && g_gmsvr->GetTradeServer()->SendData(pNm))
		{
			PushOrder(pCha, pNm->msgHead.msgOrder, 0, lNum);
		}
		else
		{
			WPACKET WtPk	= GETWPACKET();
			WRITE_CMD(WtPk, CMD_MC_STORE_QUERY);
			WRITE_LONG(WtPk, 0);
			pCha->ReflectINFof(pCha, WtPk);

			//LG("Store_msg", "RequestRecord: TradeServer����ʧ��!\n");
			LG("Store_msg", "RequestRecord: TradeServer send failed!\n");
		}

		SAFE_DELETE(pChaInfo);
		FreeNetMessage(pNm);

		return true;
	T_E}

	BOOL CStoreSystem::AcceptRecord(long long lOrderID, HistoryInfo *pRecord)
	{T_B
		extern CGameApp *g_pGameApp;

		SOrderData OrderInfo = PopOrder(lOrderID);
		if(OrderInfo.lOrderID == 0)
		{
			//LG("Store_msg", "AcceptRecord:�Ҳ�������[ID:%I64i]!\n", lOrderID);
			LG("Store_msg", "AcceptRecord:cannot find order form[ID:%I64i]!\n", lOrderID);
			return false;
		}

		long lNum = OrderInfo.lNum;

		CCharacter *pCha = NULL;
		CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(OrderInfo.ChaID);
		if(pPlayer)
		{
			pCha = pPlayer->GetMainCha();
		}
		
		if(pCha)
		{
			WPACKET WtPk	= GETWPACKET();
			WRITE_CMD(WtPk, CMD_MC_STORE_QUERY);
			WRITE_LONG(WtPk, lNum);
			int i;
			char buff[64];
			_ctime64_s( buff, sizeof(buff),&pRecord->tradeTime); 
			for(i = 0; i < lNum; i++)
			{
				//WRITE_SEQ(WtPk, ctime(&pRecord->tradeTime), USHORT(strlen(ctime(&pRecord->tradeTime)) + 1));
				WRITE_SEQ(WtPk, buff, USHORT(strlen(buff) + 1));
				WRITE_LONG(WtPk, pRecord->comID);
				WRITE_SEQ(WtPk, pRecord->comName, USHORT(strlen(pRecord->comName) + 1));
				WRITE_LONG(WtPk, pRecord->tradeMoney);
				pRecord++;
			}
			pCha->ReflectINFof(pCha, WtPk);
			//LG("Store_data", "[%s]��ѯ���׼�¼�ɹ�!\n", pCha->GetName());
			LG("Store_data", "[%s]query trade note succeed!\n", pCha->GetName());
		}
		return true;
	T_E}

	BOOL CStoreSystem::CancelRecord(long long lOrderID)
	{T_B
		extern CGameApp *g_pGameApp;

		SOrderData OrderInfo = PopOrder(lOrderID);

		if(OrderInfo.lOrderID == 0)
		{
			//LG("Store_msg", "CancelRecord:�Ҳ�������[ID:%I64i]!\n", lOrderID);
			LG("Store_msg", "CancelRecord:cannot find order form[ID:%I64i]!\n", lOrderID);
			return false;
		}
		
		CCharacter *pCha = NULL;
		CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(OrderInfo.ChaID);
		if(pPlayer)
		{
			pCha = pPlayer->GetMainCha();
		}

		if(pCha)
		{
			WPACKET WtPk	= GETWPACKET();
			WRITE_CMD(WtPk, CMD_MC_STORE_QUERY);
			WRITE_LONG(WtPk, 0);
			pCha->ReflectINFof(pCha, WtPk);
			//LG("Store_data", "[%s]��ѯ���׼�¼ʧ��!\n", pCha->GetName());
			LG("Store_data", "[%s]query trade note failed!\n", pCha->GetName());
		}
		return true;
	T_E}

	BOOL CStoreSystem::RequestGMSend(CCharacter *pCha, cChar *szTitle, cChar *szContent)
	{T_B
		pNetMessage pNm = new NetMessage();
		pMailInfo pMi = new MailInfo();

		//strcpy(pMi->title, szTitle);
		strncpy_s( pMi->title, sizeof(pMi->title), szTitle, _TRUNCATE );
		//strcpy(pMi->description, szContent);
		strncpy_s( pMi->description, sizeof(pMi->description), szContent, _TRUNCATE );
		pMi->actID = pCha->GetPlayer()->GetActLoginID();
		pMi->chaID = pCha->GetPlayer()->GetDBChaId();
		//strcpy(pMi->actName, pCha->GetPlayer()->GetActName());
		strncpy_s( pMi->actName, sizeof(pMi->actName), pCha->GetPlayer()->GetActName(), _TRUNCATE );
		//strcpy(pMi->chaName, pCha->GetName());
		strncpy_s( pMi->chaName, sizeof(pMi->chaName), pCha->GetName(), _TRUNCATE );
		pMi->sendTime = time(0);

		BuildNetMessage(pNm, INFO_SND_GM_MAIL, 0, 0, 0, (unsigned char*)pMi, sizeof(MailInfo));
		if(HasOrder(pNm->msgHead.msgOrder))
		{
			SAFE_DELETE(pMi);
			FreeNetMessage(pNm);
			//LG("Store_msg", "������[ID:%I64i]�ظ�!\n", pNm->msgHead.msgOrder);
			LG("Store_msg", "order form number[ID:%I64i]repeat!\n", pNm->msgHead.msgOrder);
			//pCha->SystemNotice("�ʼ�����ʧ��, �������ظ�!");
			pCha->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00089));

			return false;
		}
		if(IsValid() && g_gmsvr->GetTradeServer()->SendData(pNm))
		{
			PushOrder(pCha, pNm->msgHead.msgOrder, 0, 0);
		}
		else
		{
			/*pCha->SystemNotice("GM�ʼ�����ʧ��,������Ѿ����͹�һ���ʼ�,��ȴ�GM�ظ�֮���ٴη���!");
			LG("Store_msg", "RequestGMSend: TradeServer����ʧ��!\n");*/
			pCha->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00090));
			LG("Store_msg", "RequestGMSend: TradeServer send failed!\n");
		}

		SAFE_DELETE(pMi);
		FreeNetMessage(pNm);

		return true;
	T_E}

	BOOL CStoreSystem::AcceptGMSend(long long lOrderID, long lMailID)
	{T_B
		extern CGameApp *g_pGameApp;

		SOrderData OrderInfo = PopOrder(lOrderID);

		if(OrderInfo.lOrderID == 0)
		{
			//LG("Store_msg", "AcceptGMSend:�Ҳ�������[ID:%I64i]!\n", lOrderID);
			LG("Store_msg", "AcceptGMSend:cannot find order form[ID:%I64i]!\n", lOrderID);
			return false;
		}

		CCharacter *pCha = NULL;
		CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(OrderInfo.ChaID);
		if(pPlayer)
		{
			pCha = pPlayer->GetMainCha();
		}

		if(pCha)
		{
			/*pCha->SystemNotice("GM�ʼ����ͳɹ�, [����ID: %ld]!", lMailID);
			LG("Store_data", "[%s]����GM�ʼ��ɹ�!\n", pCha->GetName());*/
			pCha->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00091), lMailID);
			LG("Store_data", "[%s]send GM mail succeed !\n", pCha->GetName());
		}

		return true;
	T_E}

	BOOL CStoreSystem::CancelGMSend(long long lOrderID)
	{T_B
		extern CGameApp *g_pGameApp;

		SOrderData OrderInfo = PopOrder(lOrderID);

		if(OrderInfo.lOrderID == 0)
		{
			//LG("Store_msg", "CancelGMSend:�Ҳ�������[ID:%I64i]!\n", lOrderID);
			LG("Store_msg", "CancelGMSend:cannot find order form[ID:%I64i]!\n", lOrderID);
			return false;
		}

		CCharacter *pCha = NULL;
		CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(OrderInfo.ChaID);
		if(pPlayer)
		{
			pCha = pPlayer->GetMainCha();
		}

		if(pCha)
		{
			/*pCha->SystemNotice("GM�ʼ�����ʧ��,������Ѿ����͹�һ���ʼ�,��ȴ�GM�ظ�֮���ٴη���!");
			LG("Store_data", "[%s]����GM�ʼ�ʧ��!\n", pCha->GetName());*/
			pCha->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00090));
			LG("Store_data", "[%s]send GM mail failed!\n", pCha->GetName());
		}

		return true;
	T_E}

	BOOL CStoreSystem::RequestGMRecv(CCharacter *pCha)
	{T_B
		pNetMessage pNm = new NetMessage();
		RoleInfo *pChaInfo = new RoleInfo();

		pChaInfo->actID = pCha->GetPlayer()->GetActLoginID();
		//strcpy(pChaInfo->actName, pCha->GetPlayer()->GetActName());
		strncpy_s( pChaInfo->actName, sizeof(pChaInfo->actName), pCha->GetPlayer()->GetActName(), _TRUNCATE );
		pChaInfo->chaID = pCha->GetPlayer()->GetDBChaId();
		//strcpy(pChaInfo->chaName, pCha->GetName());
		strncpy_s( pChaInfo->chaName, sizeof(pChaInfo->chaName), pCha->GetName(), _TRUNCATE );
		pChaInfo->moBean = pCha->GetPlayer()->GetMoBean();
		pChaInfo->rplMoney = pCha->GetPlayer()->GetRplMoney();
		pChaInfo->vip = pCha->GetPlayer()->GetVipType();

		BuildNetMessage(pNm, INFO_RCV_GM_MAIL, 0, 0, 0, (unsigned char*)pChaInfo, sizeof(RoleInfo));
		if(HasOrder(pNm->msgHead.msgOrder))
		{
			SAFE_DELETE(pChaInfo);
			FreeNetMessage(pNm);
			//LG("Store_msg", "������[ID:%I64i]�ظ�!\n", pNm->msgHead.msgOrder);
			LG("Store_msg", "order form number [ID:%I64i]repeat!\n", pNm->msgHead.msgOrder);
			//pCha->SystemNotice("�ʼ�����ʧ��, �������ظ�!");
			pCha->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00089));

			return false;
		}
		if(IsValid() && g_gmsvr->GetTradeServer()->SendData(pNm))
		{
			PushOrder(pCha, pNm->msgHead.msgOrder, 0, 0);
		}
		else
		{
			//pCha->SystemNotice("GM�ʼ�����ʧ��!");
			pCha->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00092));
			//LG("Store_msg", "RequestGMRecv: TradeServer����ʧ��!\n");
			LG("Store_msg", "RequestGMRecv: TradeServersend failed!\n");
		}

		SAFE_DELETE(pChaInfo);
		FreeNetMessage(pNm);

		return true;
	T_E}

	BOOL CStoreSystem::AcceptGMRecv(long long lOrderID, MailInfo *pMi)
	{T_B
		extern CGameApp *g_pGameApp;

		SOrderData OrderInfo = PopOrder(lOrderID);

		if(OrderInfo.lOrderID == 0)
		{
			//LG("Store_msg", "AcceptGMRecv:�Ҳ�������[ID:%I64i]!\n", lOrderID);
			LG("Store_msg", "AcceptGMRecv:cannot find order form[ID:%I64i]!\n", lOrderID);
			return false;
		}

		CCharacter *pCha = NULL;
		CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(OrderInfo.ChaID);
		if(pPlayer)
		{
			pCha = pPlayer->GetMainCha();
		}

		if(pCha)
		{
			WPACKET WtPk	= GETWPACKET();
			WRITE_CMD(WtPk, CMD_MC_GM_MAIL);
			WRITE_STRING(WtPk, pMi->title);
			WRITE_STRING(WtPk, pMi->description);
			WRITE_LONG(WtPk, static_cast<long>(pMi->sendTime));
			pCha->ReflectINFof(pCha, WtPk);
			/*pCha->SystemNotice("GM�ʼ��ظ�: [����ID: %ld]!", pMi->id);
			LG("Store_data", "[%s]����GM�ʼ��ɹ�!\n", pCha->GetName());*/
			pCha->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00093), pMi->id);
			LG("Store_data", "[%s] receive GM mail succeed!\n", pCha->GetName());
		}

		return true;
	T_E}

	BOOL CStoreSystem::CancelGMRecv(long long lOrderID)
	{T_B
		extern CGameApp *g_pGameApp;

		SOrderData OrderInfo = PopOrder(lOrderID);

		if(OrderInfo.lOrderID == 0)
		{
			//LG("Store_msg", "CancelGMRecv:�Ҳ�������[ID:%I64i]!\n", lOrderID);
			LG("Store_msg", "CancelGMRecv:cannot find order form[ID:%I64i]!\n", lOrderID);
			return false;
		}

		CCharacter *pCha = NULL;
		CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(OrderInfo.ChaID);
		if(pPlayer)
		{
			pCha = pPlayer->GetMainCha();
		}

		if(pCha)
		{
			WPACKET WtPk	= GETWPACKET();
			WRITE_CMD(WtPk, CMD_MC_GM_MAIL);
			//WRITE_STRING(WtPk, "GMû���ʼ�����!");
			WRITE_STRING(WtPk, "GM do not have mail send to you!");
			WRITE_STRING(WtPk, "");
			WRITE_LONG(WtPk, 0);
			pCha->ReflectINFof(pCha, WtPk);

			//LG("Store_data", "[%s]����GM�ʼ�ʧ��!\n", pCha->GetName());
		
			LG("Store_data", "[%s]receive GM mail failed!\n", pCha->GetName());
		}

		return true;
	T_E}

	BOOL CStoreSystem::GetAfficheList()
	{T_B
		//LG("Store_msg", "���󹫸��б�!\n");
		LG("Store_msg", "ask for affiche list!\n");
		pNetMessage pNm = new NetMessage();
		BuildNetMessage(pNm, INFO_REQUEST_AFFICHE, 0, 0, 0, NULL, 0);
		g_gmsvr->GetTradeServer()->SendData(pNm);
		FreeNetMessage(pNm);
		return true;
	T_E}

	BOOL CStoreSystem::SetItemList(void *pItemList, long lNum)
	{
		try
		{
			//LG("Store_msg", "�����̳ǵ����б�!\n");
			LG("Store_msg", "set store item list!\n");

			ClearItemList();

			int i;
			StoreInfo *pStore = (StoreInfo *)pItemList;
			ItemInfo *pItem = (ItemInfo *)(pStore + 1);
			for(i = 0; i < lNum; i++)
			{
				long lComID = pStore->comID;
				long lClsID = pStore->comClass;
				long lItemNum = pStore->itemNum;
				time_t lComTime = pStore->comTime;

				//������Ʒ�ڵ�
				SItemData ItemNode;
				memcpy(&ItemNode.store_head, pStore, sizeof(StoreInfo));
				if(lItemNum > 0)
				{
					ItemNode.pItemArray = new ItemInfo[lItemNum];
					memcpy(ItemNode.pItemArray, pItem, lItemNum * sizeof(ItemInfo));
				}
				else
					ItemNode.pItemArray = NULL;

				//������Ʒ�б�
				map< long, vector<SItemData> >::iterator map_it = m_ItemList.find(lClsID);
				if(map_it != m_ItemList.end())
				{
					(*map_it).second.push_back(ItemNode);
				}
				else
				{
					vector<SItemData> vecItem;
					vecItem.push_back(ItemNode);
					pair< long, vector<SItemData> > MapNode(lClsID, vecItem);
					m_ItemList.insert(MapNode);
				}

				pair<long,long> SearchNode(lComID, lClsID);
				m_ItemSearchList.insert(SearchNode);

				pStore = (StoreInfo *)(pItem + lItemNum);
				pItem = (ItemInfo *)(pStore + 1);
			}

			//for test
			//LG("Store_info", "�̳���Ʒ:\n");
			LG("Store_info", "store merchandise:\n");
			vector<ClassInfo>::iterator cls_it = m_ItemClass.begin();
			{
				while(cls_it != m_ItemClass.end())
				{
					short sClsID = (*cls_it).clsID;
					map< long, vector<SItemData> >::iterator itemList_it = m_ItemList.find(sClsID);
					if(itemList_it != m_ItemList.end())
					{
						vector<SItemData>::iterator item_it = m_ItemList[sClsID].begin();
						while(item_it != m_ItemList[sClsID].end())
						{
							LG("Store_info", "\t[comID:%ld]\t[comName:%s]\t[comClass:%ld]\t[comPrice:%ld]\t[itemNum:%d]\n", (*item_it).store_head.comID, (*item_it).store_head.comName, (*item_it).store_head.comClass, (*item_it).store_head.comPrice, (*item_it).store_head.itemNum);
							ItemInfo *pItemIt = (*item_it).pItemArray;
							int i;
							for(i = 0; i < (*item_it).store_head.itemNum; i++)
							{
								LG("Store_info", "\t\t[itemID:%d]\t[itemNum:%d]\n", pItemIt->itemID, pItemIt->itemNum);
								pItemIt++;
							}
							item_it++;
						}
					}
					cls_it++;
				}
			}
			LG("Store_info", "\n");
		} catch(std::exception const& e)
		{
			LG("Store_error", "CStoreSystem::SetItemList() %s!\n", e.what());
		}
		catch(...)
		{
			//LG("Store_error", "CStoreSystem::SetItemList() δ֪�쳣!\n");
			LG("Store_error", "CStoreSystem::SetItemList() unknown abnormity!\n");
		}

		return true;
	}

	BOOL CStoreSystem::ClearItemList()
	{T_B
		m_ItemList.clear();
		m_ItemSearchList.clear();
		return true;
	T_E}

	BOOL CStoreSystem::SetItemClass(ClassInfo *pClassList, long lNum)
	{T_B
		//LG("Store_msg", "�����̳ǵ��߷���!\n");
		LG("Store_msg", "set store item sort!\n");
		ClearItemClass();
		while(lNum-- > 0)
		{
			m_ItemClass.push_back(*pClassList);
			pClassList++;
		}

		//for test
		//LG("Store_info", "�̳Ƿ���:\n");
		LG("Store_info", "store sort:\n");
		vector<ClassInfo>::iterator it = m_ItemClass.begin();
		while(it != m_ItemClass.end())
		{
			LG("Store_info", "\t[clsID:%d]\t[clsName:%s]\t[parentID:%d]\n", (*it).clsID, (*it).clsName, (*it).parentID);
			it++;
		}
		LG("Store_info", "\n");

		return true;
	T_E}

	BOOL CStoreSystem::ClearItemClass()
	{T_B
		m_ItemClass.clear();
		return true;
	T_E}

	BOOL CStoreSystem::SetAfficheList(AfficheInfo *pAfficheList, long lNum)
	{T_B
		//LG("Store_msg", "�����̳ǹ����б�!\n");
		LG("Store_msg", "set stroe affiche list!\n");
		ClearAfficheList();
		while(lNum > 0)
		{
			m_AfficheList.push_back(*pAfficheList);
			lNum--;
			pAfficheList++;
		}

		//for test
		//LG("Store_info", "�̳ǹ���:\n");
		LG("Store_info", "store affiche:\n");
		vector<AfficheInfo>::iterator it = m_AfficheList.begin();
		while(it != m_AfficheList.end())
		{
			LG("Store_info", "\t[affiID:%ld]\t[affiTitle:%s]\t[comID:%s]\n", (*it).affiID, (*it).affiTitle, (*it).comID);
			it++;
		}
		LG("Store_info", "\n");

		return true;
	T_E}

	BOOL CStoreSystem::ClearAfficheList()
	{T_B
		m_AfficheList.clear();
		return true;
	T_E}

	BOOL CStoreSystem::Open(CCharacter *pCha, long vip)
	{T_B
		char bValid;
		long lAfficheNum;
		long lClsNum;
		if(!IsValid())
		{
			bValid = 0;
			lAfficheNum = 0;
			lClsNum = 0;
		}
		else
		{
			bValid = 1;
			lAfficheNum = (long)m_AfficheList.size();
			lClsNum = (long)m_ItemClass.size();
		}

		int i;
		if(bValid == 1)
		{
			pCha->ForgeAction();
			pCha->SetStoreEnable(true);
		}
		WPACKET WtPk	= GETWPACKET();
		WRITE_CMD(WtPk, CMD_MC_STORE_OPEN_ASR);
		WRITE_CHAR(WtPk, bValid);	// �̳��Ƿ���

		if(bValid == 1)
		{
			WRITE_LONG(WtPk, vip);
			WRITE_LONG(WtPk, pCha->GetPlayer()->GetMoBean());	// Ħ��
			WRITE_LONG(WtPk, pCha->GetPlayer()->GetRplMoney());	// Ԫ��

			WRITE_LONG(WtPk, lAfficheNum);	// ��������
			for(i = 0; i < lAfficheNum; i++)
			{
				WRITE_LONG(WtPk, m_AfficheList[i].affiID);
				WRITE_SEQ(WtPk, m_AfficheList[i].affiTitle, uShort(strlen(m_AfficheList[i].affiTitle) + 1));
				WRITE_SEQ(WtPk, m_AfficheList[i].comID, uShort(strlen(m_AfficheList[i].comID) + 1));
			}
			WRITE_LONG(WtPk, lClsNum);	// ��������
			for(i = 0; i < lClsNum; i++)
			{
				WRITE_SHORT(WtPk, m_ItemClass[i].clsID);
				WRITE_SEQ(WtPk, m_ItemClass[i].clsName, uShort(strlen(m_ItemClass[i].clsName) + 1));
				WRITE_SHORT(WtPk, m_ItemClass[i].parentID);
			}
		}		
		pCha->ReflectINFof(pCha, WtPk);

		if(bValid != 1)
		{
			//pCha->SystemNotice("�����̳ǻ�δ����,���ڴ��̳���ҳ!");
			pCha->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00094));
		}

		return true;
	T_E}
//2016ע��ˮ������ ����ˮ��ϵͳ�����Ʋ�������
#ifdef SHUI_JING
	BOOL CStoreSystem::RequestActInfo(CCharacter *pCha)
	{T_B
		pNetMessage pNm = new NetMessage();
		RoleInfo *pChaInfo = new RoleInfo();

		pChaInfo->actID = pCha->GetPlayer()->GetActLoginID();
		strncpy_s( pChaInfo->actName, sizeof(pChaInfo->actName), pCha->GetPlayer()->GetActName(), _TRUNCATE );
		pChaInfo->chaID = pCha->GetPlayer()->GetDBChaId();
		strncpy_s( pChaInfo->chaName, sizeof(pChaInfo->chaName), pCha->GetName(), _TRUNCATE );
		pChaInfo->moBean = 0;
		pChaInfo->rplMoney = 0;
		pChaInfo->vip = 0;

		BuildNetMessage(pNm, INFO_REQUEST_ACTINFO, 0, 0, 0, (unsigned char*)pChaInfo, sizeof(RoleInfo));

		if(HasOrder(pNm->msgHead.msgOrder))
		{
			SAFE_DELETE(pChaInfo);
			FreeNetMessage(pNm);
			LG("Crystal_msg", "order form number[ID:%I64i]repeat!\n", pNm->msgHead.msgOrder);
			//pCha->SystemNotice("��ˮ������ƽ̨ʧ��");
			pCha->SystemNotice(RES_STRING(GM_CHARTRADE_CPP_00097));
			return false;
		}
		if( g_gmsvr->GetTradeServer()->SendData(pNm) )
		{
			PushOrder(pCha, pNm->msgHead.msgOrder, 0, 0);
		}
		else
		{
			//BOOL bTrade = IsCrystalTrade();
			//if(bTrade)
			//{
			//	SetCrystalTradeClose();
			//}
			LG("Crystal_msg", "RequestRoleInfo: TradeServer send failed!\n");
		}
		SAFE_DELETE(pChaInfo);
		FreeNetMessage(pNm);
		return true;

	T_E}

	BOOL CStoreSystem::AcceptActInfo(__int64 lOrderID, RoleInfo *ChaInfo )
	{T_B
		extern CGameApp *g_pGameApp;

		SOrderData OrderInfo = PopOrder(lOrderID);

		if(OrderInfo.lOrderID == 0)
		{
			LG("Crystal_msg", "AcceptActInfo:cannot find order form [ID:%I64i]!\n", lOrderID);
			return false;
		}

		long lChaID = ChaInfo->chaID;
		CCharacter *pCha = NULL;
		CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(lChaID);
		if(pPlayer)
		{
			pCha = pPlayer->GetMainCha();
		}

		if(pCha)
		{
			pPlayer->SetRplMoney( ChaInfo->rplMoney );	
			pCha->Cmd_CrystalBuyAndSaleList();
			//LG("Store_data", "[%s]��ȡ�ʻ���Ϣ�ɹ�!\n", pCha->GetName());
			LG("Crystal_msg", "ActName: [%s], ChaID: [%lu], CharacterName: [%s], FlatMoney: [%lu], GD: [%lu] get account crystalmoney information %ld succeed!\n",
				pCha->GetPlayer()->GetActName(), pCha->GetID(), pCha->GetName(), pCha->GetFlatMoney(), pCha->getAttr(ATTR_GD), pPlayer->GetRplMoney() );
		}
		return true;
	T_E}

	BOOL CStoreSystem::CancelActInfo(long long lOrderID)
	{T_B
		extern CGameApp *g_pGameApp;

		SOrderData OrderInfo = PopOrder(lOrderID);
		if(OrderInfo.lOrderID == 0)
		{
			LG("Crystal_msg", "CancelActInfo:cannot find order form[ID:%I64i]!\n", lOrderID);
			return false;
		}

		long lChaID = OrderInfo.ChaID;
		CCharacter *pCha = NULL;
		CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(lChaID);
		if(pPlayer)
		{
			pCha = pPlayer->GetMainCha();
		}
		if(pCha)
		{
			pCha->SystemNotice("�޷���ˮ������ƽ̨");
			LG("Crystal_msg", "[%s]get account information failed!\n", pCha->GetName());
		}
		return true;
	T_E}

	//��
	BOOL CStoreSystem::RequestCrystalAdd( CCharacter* pCha, string act_tName,DWORD cha_tid,  DWORD dwrplMoney, DWORD dcslPrice , DWORD iCslNum, DWORD dcslnum, int type, char quicktype, DWORD m_TAX  )
	{T_B	
		pNetMessage pNm = new NetMessage();

		CrystalInfo* pCryInfo = new CrystalInfo();
		pCryInfo->needNum = iCslNum;
		strncpy_s( pCryInfo->actName, sizeof(pCryInfo->actName), pCha->GetPlayer()->GetActName(), _TRUNCATE );
		pCryInfo->realNum = dcslnum;
		strncpy_s( pCryInfo->act_tName, sizeof(pCryInfo->act_tName), act_tName.c_str(), _TRUNCATE );
		pCryInfo->rplMoney = dwrplMoney;
		pCryInfo->rplPrice = dcslPrice;
		pCryInfo->aMoney = 0;
		pCryInfo->tMoney = 0;
		pCryInfo->type = type;
		pCryInfo->cha_tID = cha_tid;
		pCryInfo->quicktype = quicktype;
		pCryInfo->m_lTax = m_TAX;
		pCryInfo->chaID = pCha->GetID();
		BuildNetMessage(pNm, INFO_CRYSTAL_OP_ADD, 0, 0, 0, (unsigned char*)pCryInfo, sizeof(CrystalInfo));
		if(HasOrder(pNm->msgHead.msgOrder))
		{
			SAFE_DELETE(pCryInfo);
			FreeNetMessage(pNm);
			LG("Crystal_msg", "order form number [ID:%I64i] repeat!\n", pNm->msgHead.msgOrder);
			return false;
		}
		if( IsCrystalTrade() && g_gmsvr->GetTradeServer()->SendData(pNm) )
		{
			PushOrder(pCha, pNm->msgHead.msgOrder, 0, 0);
			LG("Crystal_msg", "ActName: [%s], ChaID: [%lu] && ActName: [%s], ChaID: [%d] RequestCrystalAdd Begin to trade������ Num: (%lu), Price: (%lu) \n",
				pCha->GetPlayer()->GetActName(), pCha->GetID(), act_tName.c_str(), cha_tid, dwrplMoney, dcslPrice );
		}
		else
		{
			//BOOL bTrade = IsCrystalTrade();
			//if(!bTrade)
			//{
			//	SetCrystalTradeClose();
			//}			
			LG("Crystal_msg", "RequestCrystalAdd: TradeServer send failed!\n");
			return false;
		}

		SAFE_DELETE(pCryInfo);
		FreeNetMessage(pNm);

		return true;
	T_E}

	//ȡ������
	BOOL CStoreSystem::RequestCrystalAdd( string act_Name,DWORD cha_id, string cha_name, DWORD dwrplMoney, DWORD dcslPrice , DWORD iCslNum, DWORD dcslnum, DWORD m_TAX  )
	{T_B	
		pNetMessage pNm = new NetMessage();

		CrystalInfo* pCryInfo = new CrystalInfo();
		pCryInfo->needNum = iCslNum;
		strncpy_s( pCryInfo->actName, sizeof(pCryInfo->actName),act_Name.c_str(), _TRUNCATE );
		pCryInfo->realNum = dcslnum;
		strncpy_s( pCryInfo->act_tName, sizeof(pCryInfo->act_tName), act_Name.c_str(), _TRUNCATE );
		pCryInfo->rplMoney = dwrplMoney;
		pCryInfo->rplPrice = dcslPrice;
		pCryInfo->aMoney = 0;
		pCryInfo->tMoney = 0;
		pCryInfo->type = 2;//ȡ����������ˮ��ƽ̨
		pCryInfo->cha_tID = cha_id;
		pCryInfo->quicktype = 0;
		pCryInfo->m_lTax = m_TAX;
		pCryInfo->chaID = cha_id;
		BuildNetMessage(pNm, INFO_CRYSTAL_OP_ADD, 0, 0, 0, (unsigned char*)pCryInfo, sizeof(CrystalInfo));
		if(HasOrder(pNm->msgHead.msgOrder))
		{
			SAFE_DELETE(pCryInfo);
			FreeNetMessage(pNm);
			LG("Crystal_msg", "order form number [ID:%I64i] repeat!\n", pNm->msgHead.msgOrder);
			return false;
		}
		if( IsCrystalTrade() && g_gmsvr->GetTradeServer()->SendData(pNm) )
		{
			PushOrder( cha_id, cha_name, pNm->msgHead.msgOrder, 0, 0);
			LG("Crystal_msg", "ActName: [%s],ChaID: [%lu]  RequestCrystalAdd Begin to ADD������ Num: (%lu), Price: (%lu) \n",
				act_Name.c_str(), cha_id, dwrplMoney, dcslPrice );
		}
		else
		{
			//BOOL bTrade = IsCrystalTrade();
			//if(!bTrade)
			//{
			//	SetCrystalTradeClose();
			//}			
			LG("Crystal_msg", "RequestCrystalAdd: TradeServer send failed!\n");
			return false;
		}

		SAFE_DELETE(pCryInfo);
		FreeNetMessage(pNm);

		return true;
	T_E}
	//�����ˮ������������ˮ��
	BOOL CStoreSystem::AcceptCrystalAdd(long long lOrderID, CrystalInfo* cryInfo)
	{T_B
		extern CGameApp *g_pGameApp;
		SOrderData OrderInfo = PopOrder(lOrderID);
		
		if(OrderInfo.lOrderID == 0 && cryInfo )
		{
			LG("Crystal_msg", "AcceptCrystalAdd:cannot find order form [ID:%I64i]!\n", lOrderID);
			
			// �˿��
			pNetMessage pNm = new NetMessage();
			BuildNetMessage(pNm, INFO_CRYSTAL_OP_ADD_RETURN, 0, 0, 0, (unsigned char*)cryInfo, sizeof(CrystalInfo));
			pNm->msgHead.msgOrder = lOrderID; // �ϴζ�������

			if(g_gmsvr->GetTradeServer()->SendData(pNm))
			{
				CCharacter *pCha = NULL;
				CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(cryInfo->chaID);
				if(pPlayer)
				{
					pCha = pPlayer->GetMainCha();
				}
				if(pCha)
				{
					pCha->SetStoreBuy(true);
				}
				LG("Store_record", "character [%s][ID:%ld] order return money\n",cryInfo->actName, cryInfo->chaID);
			}
			return false;
		}
		long lChaID = OrderInfo.ChaID;
		CCharacter *pCha = NULL;
		CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(lChaID);
		if( cryInfo )
		{	
			//���ȡ������ʱ,���Լ���ˮ��
			if( cryInfo->type == 0 )
			{
				if( pPlayer )
				{
					pCha = pPlayer->GetMainCha();
					if( pCha )
					{
						//�Ҳ���������������ʱ���������ȿ۵�ˮ���ɹ�����Ҫ��˰
						if( cryInfo->quicktype == 1 )
						{
							pCha->GetPlayer()->SetRplMoney( cryInfo->aMoney );
							DWORD fMoney = pCha->GetFlatMoney();
							//DWORD m_TAX = def_TAXBASE*1 + 100*(pCha->GetSaleNum() / 50 + 1)*(15*(pCha->GetSaleNum() / 50 + 1 ) - 10 ) ;
							pCha->SetFlatMoney( fMoney + cryInfo->m_lTax );					
							pCha->Cmd_CrystalBuyAndSaleList();
						}
						else
						{
							//���ȡ������ʱ
							pCha->GetPlayer()->SetRplMoney( cryInfo->aMoney );
							pCha->SystemNotice(RES_STRING(GM_CHARACTERCMD_CPP_00064), cryInfo->rplMoney);
							pCha->Cmd_CancelPlayerSaleTrade( true );
							pCha->Cmd_CrystalBuyAndSaleList();
						}
					}
					LG("Crystal_msg", "ActName: [%s], ChaID: [%lu]  Cancel SaleTrade Successed and Return crystal Num: (%ld) and Now Has crystal Num: (%ld)\n",
						cryInfo->actName, lChaID, cryInfo->rplMoney, cryInfo->aMoney );
					return true;
				}
				else
				{	//��ɫ������ʱ���ǿ�����������˰
					if( cryInfo->quicktype == 1 )
					{
						game_db.SetdwFlatMoney( cryInfo->chaID, cryInfo->m_lTax );
					}
				}
			}
			else if( cryInfo->type == 1 )
			{
				if( pPlayer )
				{
					//��Ҽ�ˮ����ʾ,�����߼�Ǯ
					pPlayer->SetRplMoney( cryInfo->aMoney );
					pCha = pPlayer->GetMainCha();
					if( pCha )
					{
						pCha->Cmd_TradeInfoAsr( 0, true, cryInfo->rplMoney, cryInfo->rplPrice );
					}
					LG("Crystal_msg", "AcceptCrystalAdd ActName: [%s], ChaID: [%lu] Add CrystalNum: (%ld) and Now has CrystalNum: (%ld) \n"
						, cryInfo->actName, lChaID, cryInfo->rplMoney, cryInfo->aMoney );
				}
				CPlayer *ptPlayer = g_pGameApp->GetPlayerByDBID(cryInfo->cha_tID);
				//if( game_db.SetdwFlatMoney( cryInfo->cha_tID, cryInfo->rplPrice * cryInfo->rplMoney))
				//{
					if( ptPlayer )
					{
						pCha = 	ptPlayer->GetMainCha();
						if( pCha )
						{
							DWORD flatmoney = 0;
							flatmoney = pCha->GetFlatMoney();
							//if( game_db.GetdwFlatMoney( pCha->GetName(), flatmoney ) )
							//{
							pCha->SetFlatMoney( flatmoney + cryInfo->rplPrice * cryInfo->rplMoney );
							pCha->Cmd_TradeInfoAsr( 1, true, cryInfo->rplMoney, cryInfo->rplPrice );
							pCha->Cmd_CrystalBuyAndSaleList();
							LG("Crystal_msg", "AcceptCrystalAdd flatcoins ActName: [%s] , ChaID: [%lu] Add flatMoney: (%ld) and Now has flatMoney: (%lu) \n"
								, cryInfo->act_tName, cryInfo->cha_tID, cryInfo->rplPrice * cryInfo->rplMoney, pCha->GetFlatMoney() );
						}
					}
					//}
					else
					{
						WPACKET WtPk  = GETWPACKET();
						WRITE_CMD(WtPk, CMD_MM_NOTICETOCHA);
						WRITE_LONG(WtPk, cryInfo->cha_tID);
						WRITE_SHORT( WtPk,1 );
						WRITE_LONG(WtPk, cryInfo->rplPrice);
						WRITE_LONG(WtPk, cryInfo->rplMoney);
						WRITE_LONG(WtPk, cryInfo->tMoney);
						pCha->ReflectINFof(pCha, WtPk);
						game_db.SetdwFlatMoney( cryInfo->cha_tID, cryInfo->rplPrice * cryInfo->rplMoney);
					}
					LG("Crystal_msg", "AcceptCrystalAdd Successed����!\n");
					return true;
				//}
			}
			else if( cryInfo->type == 2 )
			{
				if( pPlayer )
				{
					// ����ƽ̨��֪ͨ
					pCha = pPlayer->GetMainCha();
					if( pCha )
					{
						pCha->GetPlayer()->SetRplMoney( cryInfo->aMoney );
					pCha->SynGetCrystalState();
						pCha->Cmd_CrystalBuyAndSaleList();
					}
					LG("Crystal_msg", "ActName: [%s], ChaID: [%lu]  Reset Flat Successed and Add crystal Num: (%ld) and Now Has crystal Num: (%ld)\n",
						cryInfo->actName, lChaID, cryInfo->rplMoney, cryInfo->aMoney );
					return true;			
				}
			}
		}
		return false;
	T_E}

	BOOL CStoreSystem::CancelCrystalAdd(long long lOrderID, CrystalInfo* cryInfo )
	{T_B
		extern CGameApp *g_pGameApp;
		SOrderData OrderInfo = PopOrder(lOrderID);
		CCharacter *pCha = NULL;
		CPlayer *pPlayer = NULL;
		if(OrderInfo.lOrderID == 0 && cryInfo )
		{
			LG("Crystal_msg", "CancelCrystalAdd:cannot find order form[ID:%I64i]!\n", lOrderID);
			pPlayer = g_pGameApp->GetPlayerByDBID( cryInfo->chaID );
		}
		else
		{
			long lChaID = OrderInfo.ChaID;
			pPlayer = g_pGameApp->GetPlayerByDBID(lChaID);
		}
		if( cryInfo )
		{
			if( pPlayer )
			{
				pCha = pPlayer->GetMainCha();
			}
			if( cryInfo->type == 0 )
			{
				if( pCha )
				{
					//���ȡ������ʱ,���Լ���ˮ��ʧ��,Ҫ��ʾ����
					LG("Crystal_msg", "AcceptCrystalAddReturn: ActName: [%s] , ChaID: [%lu] Cancel SaleTrade Failed , crystal Num: (%ld) and Now Has crystal Num: (%ld)\n",
						cryInfo->actName, pCha->GetID(), cryInfo->rplMoney, cryInfo->aMoney );
					pCha->Cmd_ResetHangTrade( 1 );
					pCha->Cmd_CancelPlayerSaleTrade( false );
					return true;
				}
				else
				{
					game_db.UpdateHangTradeForFailed( cryInfo->actName, cryInfo->chaID, 0, CrystalTradeType::enumbuy, CrystalTradeType::enumstart);
				}
			}
			else if( cryInfo->type == 2 )
			{
				//ȡ������ʱ,����Ҽ�ˮ��ʧ��,Ҫ��ʾ����
				LG("Crystal_msg", "AcceptCrystalAddReturn: ActName: [%s] , ChaID: [%lu] CrystalTradeFALT Cancel SaleTrade Failed , crystal Num: (%ld)\n",
					cryInfo->actName, pCha->GetID(), cryInfo->rplMoney );
				game_db.ChangeStateByTemp( cryInfo->cha_tID, CrystalTradeType::enumsale, CrystalTradeType::enumhang );

			}
			else
			{	
				//��ˮ��ʧ�ܺ�״̬�ָ�
				LG("Crystal_msg"," AcceptCrystalAddReturn BuyTrade Failed!���� \n");
				if( cryInfo->quicktype == 0 )
				{
					if( pCha )
					{
						if( pCha->Cmd_ChangeFlatForTradeFailed( cryInfo->needNum, cryInfo->realNum, 0, cryInfo->act_tName, cryInfo->cha_tID ) )
						{
							pCha->Cmd_CrystalBuyAndSaleList();
							return true;
						}
					}
					else
						Cmd_ChangeFlatForTradeFailed( cryInfo->needNum, cryInfo->realNum, 0, cryInfo->act_tName, cryInfo->cha_tID, cryInfo->actName, cryInfo->chaID );			
				}
				else if( cryInfo->quicktype == 1 )
				{
					DWORD trademoney = cryInfo->rplPrice * cryInfo->rplMoney;
					if( game_db.ChangeStateByTemp( pCha->GetID(), CrystalTradeType::enumbuy, CrystalTradeType::enumstart ) )
					{
						if( pCha )
						{
							DWORD fMoney = pCha->GetFlatMoney();
							pCha->SetFlatMoney( fMoney + cryInfo->m_lTax + trademoney );
							pCha->Cmd_CrystalBuyAndSaleList();
						}
						else
						{
							game_db.SetdwFlatMoney( cryInfo->chaID, cryInfo->m_lTax + trademoney );
						}
					}
					if( cryInfo->needNum < cryInfo->realNum )
					{
						game_db.UpdateHangTradeForFailed( cryInfo->act_tName, cryInfo->cha_tID,  (cryInfo->needNum <  cryInfo->realNum ? cryInfo->needNum :cryInfo->realNum  ), CrystalTradeType::enumsale, CrystalTradeType::enumhang ) ;
					}
					else
					{
						game_db.UpdateHangTradeForFailed( cryInfo->act_tName, cryInfo->cha_tID, 0, CrystalTradeType::enumsale, CrystalTradeType::enumend );
					}
					pPlayer = g_pGameApp->GetPlayerByDBID( cryInfo->cha_tID );
					if( pPlayer &&( pCha = pPlayer->GetMainCha() ) )
					{
						pCha->Cmd_CrystalBuyAndSaleList();
					}
					return true;
				}
			}
		}
		return false;
	T_E}

	//������Һͽ����� �޸���Ӧˮ��
	BOOL CStoreSystem::RequestCrystalDel( CCharacter* pCha, string act_tName, DWORD cha_tid, DWORD dwrplMoney, DWORD iCslPrice, DWORD iCslNum, DWORD dcslnum, int type, char quicktype, DWORD m_TAX  )
	{T_B
		pNetMessage pNm = new NetMessage();

		CrystalInfo* pCryInfo = new CrystalInfo();
		pCryInfo->needNum = iCslNum;
		strncpy_s( pCryInfo->actName, sizeof(pCryInfo->actName), pCha->GetPlayer()->GetActName(), _TRUNCATE );
		pCryInfo->realNum = dcslnum;
		strncpy_s( pCryInfo->act_tName, sizeof(pCryInfo->act_tName), act_tName.c_str(), _TRUNCATE );
		pCryInfo->rplMoney = dwrplMoney;
		pCryInfo->rplPrice = iCslPrice;
		pCryInfo->aMoney = pCha->GetPlayer()->GetRplMoney();
		pCryInfo->tMoney = 0;
		pCryInfo->type = type;
		pCryInfo->cha_tID = cha_tid;
		pCryInfo->quicktype = quicktype;
		pCryInfo->m_lTax = m_TAX;
		pCryInfo->chaID = pCha->GetID();

		BuildNetMessage(pNm, INFO_CRYSTAL_OP_DEL, 0, 0, 0, (unsigned char*)pCryInfo, sizeof(CrystalInfo));
		if(HasOrder(pNm->msgHead.msgOrder))
		{
			SAFE_DELETE(pCryInfo);
			FreeNetMessage(pNm);
			LG("Crystal_msg", "order form number [ID:%I64i] repeat!\n", pNm->msgHead.msgOrder);
			return false;
		}
		if( IsCrystalTrade() && g_gmsvr->GetTradeServer()->SendData(pNm))
		{
			PushOrder(pCha, pNm->msgHead.msgOrder, 0, 0);
			LG("Crystal_msg", "ActName: [%s],ChaID: [%lu] CrystalNum(%lu) && ActName: [%s], ChaID: [%d]  RequestCrystalDel Begin to trade������ Num: (%lu), Price: (%lu) \n",
				pCha->GetPlayer()->GetActName(), pCha->GetID(),pCryInfo->aMoney, act_tName.c_str(), cha_tid, dwrplMoney, iCslPrice );
		}
		else
		{
			LG("Crystal_msg", "RequestCrystalDel: TradeServer send failed!\n");
			return false;
		}

		SAFE_DELETE(pCryInfo);
		FreeNetMessage(pNm);

		return true;
	T_E}

	//�޸�ˮ���ɹ�,������Һͽ����߽�����Ϣ
	BOOL CStoreSystem::AcceptCrystalDel(long long lOrderID ,CrystalInfo* cryInfo )
	{T_B
	extern CGameApp *g_pGameApp;
	SOrderData OrderInfo = PopOrder(lOrderID);

	if(OrderInfo.lOrderID == 0 && cryInfo)
	{
		// �˿��
		pNetMessage pNm = new NetMessage();
		BuildNetMessage(pNm, INFO_CRYSTAL_OP_DEL_RETURN, 0, 0, 0, (unsigned char*)cryInfo, sizeof(CrystalInfo));
		pNm->msgHead.msgOrder = lOrderID; // �ϴζ�������

		if(g_gmsvr->GetTradeServer()->SendData(pNm))
		{
			CCharacter *pCha = NULL;
			CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(cryInfo->chaID);
			if(pPlayer)
			{
				pCha = pPlayer->GetMainCha();
			}
			if(pCha)
			{
				pCha->SetStoreBuy(true);
			}
			LG("Crystal_msg", "AcceptCrystalDel:cannot find order form [ID:%I64i]!\n", lOrderID);
		}
		return false;
	}
	long lChaID = OrderInfo.ChaID;
	CCharacter *pCha = NULL;
	CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(lChaID);
	if( cryInfo )
	{	
		if( cryInfo->type == 0 )
		{
			if( pPlayer )
			{
				if( pCha = pPlayer->GetMainCha() )
				{
					//��ҹ�����,�Լ���Ǯ��
					pCha->GetPlayer()->SetRplMoney( cryInfo->aMoney );
					LG("Crystal_msg", "ActName: [%s], ChaID: [%lu] Hangs SaleTrade Successed and Deducts crystal Num: (%ld) and Now Has crystal Num: (%ld)\n",
						cryInfo->actName, pCha->GetID(), cryInfo->rplMoney, cryInfo->aMoney );
				}
				if( cryInfo->quicktype == 1)
				{
					DWORD chaid = 0;//������ҵ�cha_id
					string act_tName;//������ҵ��˺�����
					if( game_db.CheckHasMatchAccord( chaid, act_tName, cryInfo->rplPrice, cryInfo->needNum, CrystalTradeType::enumbuy ))
					{
						//����
						if( pCha->Cmd_ChangeFlatAfterTrade( cryInfo->needNum, cryInfo->needNum, CrystalTradeType::enumsale, (char*)act_tName.c_str(), chaid ))
						{
							//����ˮ��
							if( !g_StoreSystem.RequestCrystalDel( pCha, act_tName, chaid, cryInfo->needNum, cryInfo->rplPrice, cryInfo->needNum, cryInfo->needNum, 1, cryInfo->quicktype, cryInfo->m_lTax  ))
							{
								//����
								pCha->Cmd_ChangeFlatForTradeFailed( cryInfo->needNum, cryInfo->needNum, CrystalTradeType::enumbuy, (char*)act_tName.c_str(), chaid);
							}
						}
					}
					else
					{
						WPACKET pk = GETWPACKET();
						WRITE_CMD( pk, CMD_MC_REQUEST_BUY_ASR );
						WRITE_CHAR( pk, '0');
						WRITE_CHAR( pk, '6');
						pCha->ReflectINFof(pCha,pk);
						if( game_db.UpdateHangTrade( (char*)pPlayer->GetActName(), pPlayer->GetID(), 0, CrystalTradeType::enumsale, CrystalTradeType::enumstart  ) )
						{
							if(!g_StoreSystem.RequestCrystalAdd( pCha, pPlayer->GetActName(), pPlayer->GetID(), cryInfo->rplMoney, cryInfo->rplPrice, cryInfo->rplMoney, cryInfo->rplMoney, 0, cryInfo->quicktype, cryInfo->m_lTax  ))
							{
								LG("Crystal_msg","ActName: [%s], ChaID: [%lu] Hangs Quick Trade Failed and Not return crystal Num: (%ld) \n", pPlayer->GetActName(), pPlayer->GetID(),cryInfo->rplMoney);
							}
						}
					}

				}

				else
				{
					//�鿴�Ƿ���ڷ��ϵĽ�����
					pCha->Cmd_CheckHasAccordForTrade( cryInfo->needNum, cryInfo->rplPrice, 0, 0 );
				}
				return true;
			}
			else
			{
				LG("Crystal_msg","AcceptCrystalDel: cannot find the player[%s] and just hang the trade!", cryInfo->actName );
			}
		}
		else
		{	

			CPlayer *ptPlayer = g_pGameApp->GetPlayerByDBID( cryInfo->cha_tID );
			if( ptPlayer )
				{
					ptPlayer->SetRplMoney( cryInfo->tMoney );
					CCharacter* ptCha = NULL;
					ptCha = ptPlayer->GetMainCha();
					if( ptCha )
					{
					ptCha->Cmd_TradeInfoAsr( 0, true, cryInfo->rplMoney, cryInfo->rplPrice );
					ptCha->Cmd_CrystalBuyAndSaleList();
				}
			}
			else
			{
				if( pCha = pPlayer->GetMainCha()) 
				{
					WPACKET WtPk  = GETWPACKET();
					WRITE_CMD(WtPk, CMD_MM_NOTICETOCHA);
					WRITE_LONG(WtPk, cryInfo->cha_tID);
					WRITE_SHORT(WtPk, 0);
					WRITE_LONG(WtPk, cryInfo->rplPrice);
					WRITE_LONG(WtPk, cryInfo->rplMoney);
					WRITE_LONG(WtPk, cryInfo->tMoney);
					pCha->ReflectINFof(pCha, WtPk);
				}
			}
			LG("Crystal_msg", "AcceptCrystalDel ActName: [%s] , ChaID: [%lu] Add CrystalNum: (%ld) and Now has CrystalNum: (%ld) \n"
				, cryInfo->act_tName, cryInfo->cha_tID, cryInfo->rplMoney, cryInfo->tMoney );
			if( pPlayer )
			{
				if( pCha = pPlayer->GetMainCha() )
				{
					//��ҼӺ�����
					long flatmoney = pCha->GetFlatMoney();
					pCha->SetFlatMoney( flatmoney + cryInfo->rplPrice * cryInfo->rplMoney );
					pCha->Cmd_TradeInfoAsr( 1, true, cryInfo->rplMoney, cryInfo->rplPrice );
					pCha->Cmd_CrystalBuyAndSaleList();
					LG("Crystal_msg", "AcceptCrystalDel ActName: [%s] , ChaID: [%lu] Deducts CrystalNum: (%ld) and  Add  flatmoney: (%ld) \n"
						, cryInfo->actName, pCha->GetID(), cryInfo->rplMoney, cryInfo->rplPrice * cryInfo->rplMoney );
					return true;
				}
			}
			else
			{
				game_db.SetdwFlatMoney( cryInfo->chaID, cryInfo->rplPrice * cryInfo->rplMoney );
			}

		}
	}
		return false;
	T_E}
	//ˮ������ʧ��
	BOOL CStoreSystem::CancelCrystalDel(long long lOrderID, CrystalInfo* cryInfo )
	{T_B
		extern CGameApp *g_pGameApp;
		SOrderData OrderInfo = PopOrder(lOrderID);
		CCharacter *pCha = NULL;
		CPlayer *pPlayer = NULL;

		LG("Crystal_msg", "CancelCrystalDel:cannot find order form[ID:%I64i]!\n", lOrderID);
		pPlayer = g_pGameApp->GetPlayerByDBID( cryInfo->chaID );

		if( cryInfo )
		{
			if(pPlayer)
			{
				pCha = pPlayer->GetMainCha();
			}
			if(pCha)
			{
				if( cryInfo->type == 0 )
				{
					//���Լ�����ʱ��ˮ��ʧ��,��Ҫȡ������
					LG("Crystal_msg", "AcceptCrystalDelReturn: ActName: [%s] , ChaID: [%lu] Hang SaleTrade Failed , crystal Num: (%ld) and Now Has crystal Num: (%ld)\n",
						cryInfo->actName, pCha->GetID(), cryInfo->rplMoney, cryInfo->aMoney );
					if( game_db.UpdateHangTrade( cryInfo->actName, cryInfo->chaID, cryInfo->rplMoney, CrystalTradeType::enumsale, CrystalTradeType::enumstart ) )
					{
						long flatmoney = pCha->GetFlatMoney();
						pCha->SetFlatMoney( flatmoney + cryInfo->m_lTax );	
						pCha->Cmd_CrystalBuyAndSaleList();
					}
				}
				else
				{
					LG("Crystal_msg","AcceptCrystalDelReturn: ActName:[%s],ChaID:[%ld] Del Crystal Successed,but Trade(Add crystal[%ld] to another ActName:[%s] ChaID:[%ld] Failed !)", cryInfo->actName, cryInfo->chaID,
						cryInfo->rplMoney, cryInfo->act_tName, cryInfo->cha_tID );
					//�ָ���ҵ��������ͽ����ߵ���(δ����)
					//��ˮ��ʧ�ܺ�״̬�ָ�
					LG("Crystal_msg","AcceptCrystalDelReturn: Just Return Hang the Trade!");
					if( pCha->Cmd_ChangeFlatForTradeFailed( cryInfo->needNum, cryInfo->realNum, 1, cryInfo->act_tName, cryInfo->cha_tID ))
					{
						pCha->Cmd_CrystalBuyAndSaleList();
					}					
				}
				LG("Crystal_msg", "[%s]cancel trade information failed !\n", pCha->GetName());
			}
			else
			{
				if( cryInfo->type == 0 )
				{
					game_db.SetdwFlatMoney( cryInfo->chaID, cryInfo->m_lTax );
				}
				else
				{
					//�ҵ��ָ�
					Cmd_ChangeFlatForTradeFailed( cryInfo->needNum, cryInfo->realNum, 1, cryInfo->act_tName, cryInfo->cha_tID, cryInfo->actName, cryInfo->chaID );
				}
			}

		}
		return false;
	T_E}

	//�ֿ���ˮ����,�����ҵ� ��Ǯ
	BOOL CStoreSystem::AcceptCrystalAddReturn( long long lOrderID, CrystalInfo* cryInfo )
	{T_B
		extern CGameApp *g_pGameApp;
		CCharacter *pCha = NULL;
		CPlayer *pPlayer = NULL;
		LG("Crystal_msg", "AcceptCrystalAddReturn: order form[ID:%I64i]!\n", lOrderID);
		if( cryInfo )
		{
			pPlayer = g_pGameApp->GetPlayerByDBID( cryInfo->chaID );
			if( pPlayer )
			{
				pCha = pPlayer->GetMainCha();
			}
			if( cryInfo->type == 0 )
			{
				if( pCha )
				{
					//���ȡ������ʱ,���Լ���ˮ��ʧ��,Ҫ��ʾ����
					LG("Crystal_msg", "AcceptCrystalAddReturn: ActName: [%s] , ChaID: [%lu] Cancel SaleTrade Failed , crystal Num: (%ld) and Now Has crystal Num: (%ld)\n",
						cryInfo->actName, pCha->GetID(), cryInfo->rplMoney, cryInfo->aMoney );
					pCha->Cmd_ResetHangTrade( 1 );
					pCha->SetStoreBuy( false );
					pCha->Cmd_CancelPlayerSaleTrade( false );
					return true;
				}
				else
				{
					game_db.UpdateHangTradeForFailed( cryInfo->actName, cryInfo->chaID, 0, CrystalTradeType::enumbuy, CrystalTradeType::enumstart);
				}
			}
			else if( cryInfo->type == 2 )
			{
				//ȡ������ʱ,����Ҽ�ˮ��ʧ��,Ҫ��ʾ����
				LG("Crystal_msg", "AcceptCrystalAddReturn: ActName: [%s] , ChaID: [%lu] CrystalTradeFALT Cancel SaleTrade Failed , crystal Num: (%ld)\n",
					cryInfo->actName, pCha->GetID(), cryInfo->rplMoney );
				game_db.ChangeStateByTemp( cryInfo->cha_tID, CrystalTradeType::enumsale, CrystalTradeType::enumhang );

			}
			else
			{	
				//��ˮ��ʧ�ܺ�״̬�ָ�
				LG("Crystal_msg"," AcceptCrystalAddReturn BuyTrade Failed!���� \n");
				if( cryInfo->quicktype == 0 )
				{
					if( pCha )
					{
						if( pCha->Cmd_ChangeFlatForTradeFailed( cryInfo->needNum, cryInfo->realNum, 0, cryInfo->act_tName, cryInfo->cha_tID ) )
						{
							pCha->SetStoreBuy( false );
							pCha->Cmd_CrystalBuyAndSaleList();
							return true;
						}
					}
					else
						Cmd_ChangeFlatForTradeFailed( cryInfo->needNum, cryInfo->realNum, 0, cryInfo->act_tName, cryInfo->cha_tID, cryInfo->actName, cryInfo->chaID );			
				}
				else if( cryInfo->quicktype == 1 )
				{
					DWORD trademoney = cryInfo->rplPrice * cryInfo->rplMoney;
					if( game_db.ChangeStateByTemp( pCha->GetID(), CrystalTradeType::enumbuy, CrystalTradeType::enumstart ) )
					{
						if( pCha )
						{
							DWORD fMoney = pCha->GetFlatMoney();
							pCha->SetFlatMoney( fMoney + cryInfo->m_lTax + trademoney );
							pCha->SetStoreBuy( false );
							pCha->Cmd_CrystalBuyAndSaleList();
						}
						else
						{
							game_db.SetdwFlatMoney( cryInfo->chaID, cryInfo->m_lTax + trademoney );
						}
					}
					if( cryInfo->needNum < cryInfo->realNum )
					{
						game_db.UpdateHangTradeForFailed( cryInfo->act_tName, cryInfo->cha_tID,  (cryInfo->needNum <  cryInfo->realNum ? cryInfo->needNum :cryInfo->realNum  ), CrystalTradeType::enumsale, CrystalTradeType::enumhang ) ;
					}
					else
					{
						game_db.UpdateHangTradeForFailed( cryInfo->act_tName, cryInfo->cha_tID, 0, CrystalTradeType::enumsale, CrystalTradeType::enumend );
					}
					pPlayer = g_pGameApp->GetPlayerByDBID( cryInfo->cha_tID );
					if( pPlayer &&( pCha = pPlayer->GetMainCha() ) )
					{
						pCha->Cmd_CrystalBuyAndSaleList();
					}
					return true;
				}
			}

		}
		return false;
	T_E}


	BOOL CStoreSystem::CancelCrystalAddReturn( long long lOrderID, CrystalInfo* cryInfo )
	{T_B
		LG("Crystal_msg","CancelCrystalAddReturn: order form[ID:%I64i]!\n", lOrderID );
		if( cryInfo )
		{
			LG("Crystal_msg","CancelCrystalAddReturn: player [%s] && player[%s] order form[ID:%I64i]!\n", lOrderID, cryInfo->actName, cryInfo->act_tName );
		}
		return true;
	T_E}

	BOOL CStoreSystem::AcceptCrystalDelReturn( long long lOrderID, CrystalInfo* cryInfo )
	{T_B
		extern CGameApp *g_pGameApp;
		CCharacter *pCha = NULL;
		CPlayer *pPlayer = NULL;
		pPlayer = g_pGameApp->GetPlayerByDBID( cryInfo->chaID );
		if( cryInfo )
		{
			if(pPlayer)
			{
				pCha = pPlayer->GetMainCha();
			}
			if(pCha)
			{
				if( cryInfo->type == 0 )
				{
					//���Լ�����ʱ��ˮ��ʧ��,��Ҫȡ������
					LG("Crystal_msg", "AcceptCrystalDelReturn: ActName: [%s] , ChaID: [%lu] Hang SaleTrade Failed , crystal Num: (%ld) and Now Has crystal Num: (%ld)\n",
						cryInfo->actName, pCha->GetID(), cryInfo->rplMoney, cryInfo->aMoney );
					if( game_db.UpdateHangTrade( cryInfo->actName, cryInfo->chaID, cryInfo->rplMoney, CrystalTradeType::enumsale, CrystalTradeType::enumstart ) )
					{
						long flatmoney = pCha->GetFlatMoney();
						pCha->SetFlatMoney( flatmoney + cryInfo->m_lTax );	
						pCha->SetStoreBuy( false );
						pCha->Cmd_CrystalBuyAndSaleList();
					}
				}
				else
				{
					LG("Crystal_msg","AcceptCrystalDelReturn: ActName:[%s],ChaID:[%ld] Del Crystal Successed,but Trade(Add crystal[%ld] to another ActName:[%s] ChaID:[%ld] Failed !)", cryInfo->actName, cryInfo->chaID,
						cryInfo->rplMoney, cryInfo->act_tName, cryInfo->cha_tID );
					//�ָ���ҵ��������ͽ����ߵ���(δ����)
					//��ˮ��ʧ�ܺ�״̬�ָ�
					LG("Crystal_msg","AcceptCrystalDelReturn: Just Return Hang the Trade!");
					if( pCha->Cmd_ChangeFlatForTradeFailed( cryInfo->needNum, cryInfo->realNum, 1, cryInfo->act_tName, cryInfo->cha_tID ))
					{
						pCha->SetStoreBuy( false );
						pCha->Cmd_CrystalBuyAndSaleList();
					}					
				}
				LG("Crystal_msg", "[%s]cancel trade information failed !\n", pCha->GetName());
			}
			else
			{
				if( cryInfo->type == 0 )
				{
					game_db.SetdwFlatMoney( cryInfo->chaID, cryInfo->m_lTax );
				}
				else
				{
					//�ҵ��ָ�
					Cmd_ChangeFlatForTradeFailed( cryInfo->needNum, cryInfo->realNum, 1, cryInfo->act_tName, cryInfo->cha_tID, cryInfo->actName, cryInfo->chaID );
				}
			}
		}
		return false;
	T_E}

	BOOL CStoreSystem::CancelCrystalDelReturn( long long lOrderID, CrystalInfo* cryInfo )
	{T_B
		LG("Crystal_msg","CancelCrystalDelReturn: order form[ID:%I64i]!\n", lOrderID );
		if( cryInfo )
		{
			LG("Crystal_msg","CancelCrystalDelReturn: player [%s] && player[%s] order form[ID:%I64i]!\n", lOrderID, cryInfo->actName, cryInfo->act_tName );
		}
		return true;
	T_E}

#endif
}