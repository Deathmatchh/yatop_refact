//----------------------------------------------------------------------
// ����:����
// ����:lh 2004-07-19
// ����޸�����:2004-10-09
//----------------------------------------------------------------------
#pragma once
#include "UICommand.h"
#include "CompCommand.h"

class CItemRecord;
struct SGameAttr;
struct xShipInfo;
struct NET_CHARTRADE_BOATDATA;
class CStoneInfo;
class CItemRefineInfo;
class CItemRefineEffectInfo;

namespace GUI
{

struct SItemForge
{
  bool IsForge;	 // �Ƿ���
  int  nHoleNum; // �м�����
  int  nLevel;	 // �����ȼ�

  CStoneInfo* pStoneInfo[3];	   // ���ű�ʯ,Ϊ���ޱ�ʯ
  int		  nStoneLevel[3];	   // ��Ӧ�ı�ʯ�ȼ�
  char		  szStoneHint[3][256]; // ��ʯ˵��
  int		  nStoneNum;		   // ��ʯ����

  static SItemForge& Convert(DWORD v, int nItemID = -1);

  static float GetAlpha(int nTotalLevel); // �����ܵȼ�,�õ���Чalpha

  public:								// Ϊ������
  int					 nStoneType[3]; // ��ʯ����,û��Ϊ-1
  CItemRefineInfo*		 pRefineInfo;
  CItemRefineEffectInfo* pEffectInfo;
  int					 nEffectLevel;

  private:
  void Refresh(int nItemID);
};

// ��Դ��SItemGrid,��ʵ�������Բ�һ����ʵ�������� = SItemGrid��ʵ�������� + ��ȡ����
struct SItemHint
{
  short sID;
  short sNum;
  short sEndure[2];
  short sEnergy[2];
  char	chForgeLv;
  long	lDBParam[enumITEMDBP_MAXNUM];
  short sInstAttr[ITEMATTR_CLIENT_MAX];

  void Convert(SItemGrid& ItemGrid, CItemRecord* pInfo);
};

class CItemCommand : public CCommandObj
{
  enum
  {
	SOLID_ALPHA	  = 0xa0000000,
	INVALID_COLOR = 0x00ff0000,
  };

  public:
  CItemCommand(CItemRecord* pItem, bool isIconInStore = false);
  CItemCommand(const CItemCommand& rhs);
  CItemCommand& operator=(const CItemCommand& rhs);
  ~CItemCommand();
  ITEM_CLONE(CItemCommand)

  virtual void RenderEnergy(int x, int y);
  virtual void SaleRender(int x, int y, int nWidth, int nHeight);
  virtual void Render(int x, int y);
  virtual void OwnDefRender(int x, int y, int nWidth, int nHeight);

  virtual bool		  IsDragFast();
  virtual int		  GetTotalNum();
  virtual bool		  IsAllowThrow();
  virtual bool		  UseCommand();
  virtual bool		  MouseDown();
  virtual void		  SetTotalNum(int num);
  virtual const char* GetName();

  void		 SetData(const SItemGrid& item);
  SItemGrid& GetData() { return _ItemData; }

  void SetBoatHint(const NET_CHARTRADE_BOATDATA* const pBoat);

  virtual void SetIsValid(bool v);
  virtual bool GetIsValid();

  virtual bool GetIsPile();
  virtual int  GetPrice();

  void SetIsSolid(bool v);
  bool GetIsSolid();

  CItemRecord* GetItemInfo() { return _pItem; }

  int GetThrowLink(); // �ɹ�>=0,ʧ�ܷ���-1

  void SetPrice(int n) { _nPrice = n; }

  SItemForge& GetForgeInfo();
  string	  GetStoneHint(int nLevel = -1); // �õ���ʯ��hint������Ϊ��1�ǵ��߱�����hint������Ϊ����ȼ���hint

  static void ClearCoolDown() { _mapCoolDown.clear(); } // ���߼��� COOLDOWN ��Ϣ���

  protected:
  virtual void AddHint(int x, int y, int index = -1);

  void PUSH_HINT(const char* str, int value, DWORD color = COLOR_WHITE, int index = -1);

  void _Copy(const CItemCommand& rhs);
  //int     _GetValue( int nItemAttrType, SItemGrid& item );
  int  _GetValue(int nItemAttrType, SItemHint& item);
  void _ShowWork(CItemRecord* pItem, SGameAttr* pAttr, int index = -1);										 // ������ʾ���ߵ�ְҵ����
  void _ShowFusionWork(CItemRecord* pAppearItem, CItemRecord* pEquipItem, SGameAttr* pAttr, int index = -1); // ������ʾ�ۺϺ���ߵ�ְҵ����
  void _AddDescriptor(int index = -1);
  void _ShowWork(xShipInfo* pInfo, SGameAttr* pAttr, int index = -1); // ������ʾ����ְҵ����
  void _ShowBody(int index = -1);									  // ������ʾ��ɫ����
  void _ShowFusionBody(CItemRecord* pEquipItem, int index = -1);	  // ������ʾ�ۺϺ��ɫ����
  void AddRelatedHint(int x, int y);

  protected:
  //void	_PushItemAttr( int attr, SItemGrid& item, DWORD color=COLOR_WHITE );
  void _PushItemAttr(int attr, SItemHint& item, DWORD color = COLOR_WHITE, int index = -1);
  void _PushValue(const char* szFormat, int attr, SItemHint& item, DWORD color = COLOR_WHITE, int index = -1);

  private:
  CGuiPic*	   _pImage;
  CItemRecord* _pItem;
  SItemGrid	   _ItemData;
  int		   _nPrice;

  private:
  DWORD					  _dwColor; // ��Ч��ɫ��ʾ, ��ʵ���͸��
  NET_CHARTRADE_BOATDATA* _pBoatHint;

  static map<int, DWORD> _mapCoolDown; // ������һ�ηŵĵ��߼��ܵ�ʱ��
};

// ��������
inline void CItemCommand::SetIsValid(bool v)
{
  _dwColor = (_dwColor & 0xff000000) | (v ? 0x00ffffff : INVALID_COLOR);
}

inline bool CItemCommand::GetIsValid()
{
  return (_dwColor & 0x00ffffff) == 0x00ffffff;
}

inline void CItemCommand::SetIsSolid(bool v)
{
  _dwColor = (_dwColor & 0x00ffffff) | (v ? 0xff000000 : SOLID_ALPHA);
}

inline bool CItemCommand::GetIsSolid()
{
  return (_dwColor & 0xff000000) == 0xff000000;
}

} // namespace GUI
// add by ning.yan 2008-11-11 begin
extern char _lock_pos_;
extern long _lock_item_id_;
extern long _lock_grid_id_;
extern long _lock_fusion_item_id_;
extern bool g_yyy_add_lock_item_wait_return_state; //	�Ѿ�����������Ϣ���ȴ������������������״̬��
												   // end