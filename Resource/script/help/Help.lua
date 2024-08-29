--------------------------------------------------------------------------
--									--
--									--
--				Help.lua 				--
--									--
--									--
--------------------------------------------------------------------------
print( "Loading Help.lua" )


--初始化
ClearHelpNPC()  
HELP_ADDHELPNPC_LUA_000001 = GetResString("HELP_ADDHELPNPC_LUA_000001")
AddHelpNPC(HELP_ADDHELPNPC_LUA_000001)
dofile(GetResPath("script/help/monsterhelp.lua"))


--白银城酒吧, 酒鬼的帮助功能

--格式为: 关键字 + 内容

--游戏正式的帮助信息


HELP_HELP_LUA_000001 = GetResString("HELP_HELP_LUA_000001")
HELP_HELP_LUA_000002 = GetResString("HELP_HELP_LUA_000002")
AddHelpInfo(HELP_HELP_LUA_000002,   HELP_HELP_LUA_000001)
HELP_HELP_LUA_000003 = GetResString("HELP_HELP_LUA_000003")
HELP_HELP_LUA_000004 = GetResString("HELP_HELP_LUA_000004")
AddHelpInfo(HELP_HELP_LUA_000004,     HELP_HELP_LUA_000003)
HELP_HELP_LUA_000005 = GetResString("HELP_HELP_LUA_000005")
HELP_HELP_LUA_000006 = GetResString("HELP_HELP_LUA_000006")
AddHelpInfo(HELP_HELP_LUA_000006,   HELP_HELP_LUA_000005)
HELP_HELP_LUA_000007 = GetResString("HELP_HELP_LUA_000007")
HELP_HELP_LUA_000008 = GetResString("HELP_HELP_LUA_000008")
AddHelpInfo(HELP_HELP_LUA_000008,     HELP_HELP_LUA_000007)
HELP_HELP_LUA_000009 = GetResString("HELP_HELP_LUA_000009")
HELP_HELP_LUA_000010 = GetResString("HELP_HELP_LUA_000010")
AddHelpInfo(HELP_HELP_LUA_000010, HELP_HELP_LUA_000009)
HELP_HELP_LUA_000011 = GetResString("HELP_HELP_LUA_000011")
HELP_HELP_LUA_000012 = GetResString("HELP_HELP_LUA_000012")
AddHelpInfo(HELP_HELP_LUA_000012, HELP_HELP_LUA_000011)
HELP_HELP_LUA_000013 = GetResString("HELP_HELP_LUA_000013")
HELP_HELP_LUA_000014 = GetResString("HELP_HELP_LUA_000014")
AddHelpInfo(HELP_HELP_LUA_000014,   HELP_HELP_LUA_000013)
HELP_HELP_LUA_000015 = GetResString("HELP_HELP_LUA_000015")
HELP_HELP_LUA_000016 = GetResString("HELP_HELP_LUA_000016")
AddHelpInfo(HELP_HELP_LUA_000016, HELP_HELP_LUA_000015)
HELP_HELP_LUA_000015 = GetResString("HELP_HELP_LUA_000015")
HELP_HELP_LUA_000017 = GetResString("HELP_HELP_LUA_000017")
AddHelpInfo(HELP_HELP_LUA_000017, HELP_HELP_LUA_000015)
HELP_HELP_LUA_000018 = GetResString("HELP_HELP_LUA_000018")
HELP_HELP_LUA_000019 = GetResString("HELP_HELP_LUA_000019")
AddHelpInfo(HELP_HELP_LUA_000019,     HELP_HELP_LUA_000018)
HELP_HELP_LUA_000020 = GetResString("HELP_HELP_LUA_000020")
HELP_HELP_LUA_000021 = GetResString("HELP_HELP_LUA_000021")
AddHelpInfo(HELP_HELP_LUA_000021,   HELP_HELP_LUA_000020)
HELP_HELP_LUA_000022 = GetResString("HELP_HELP_LUA_000022")
HELP_HELP_LUA_000023 = GetResString("HELP_HELP_LUA_000023")
AddHelpInfo(HELP_HELP_LUA_000023,     HELP_HELP_LUA_000022)
HELP_HELP_LUA_000024 = GetResString("HELP_HELP_LUA_000024")
HELP_HELP_LUA_000025 = GetResString("HELP_HELP_LUA_000025")
AddHelpInfo(HELP_HELP_LUA_000025,     HELP_HELP_LUA_000024)
HELP_HELP_LUA_000026 = GetResString("HELP_HELP_LUA_000026")
HELP_HELP_LUA_000027 = GetResString("HELP_HELP_LUA_000027")
AddHelpInfo(HELP_HELP_LUA_000027,   HELP_HELP_LUA_000026)
HELP_HELP_LUA_000028 = GetResString("HELP_HELP_LUA_000028")
HELP_HELP_LUA_000029 = GetResString("HELP_HELP_LUA_000029")
AddHelpInfo(HELP_HELP_LUA_000029,   HELP_HELP_LUA_000028)
HELP_HELP_LUA_000030 = GetResString("HELP_HELP_LUA_000030")
HELP_HELP_LUA_000031 = GetResString("HELP_HELP_LUA_000031")
AddHelpInfo(HELP_HELP_LUA_000031,   HELP_HELP_LUA_000030)
HELP_HELP_LUA_000032 = GetResString("HELP_HELP_LUA_000032")
HELP_HELP_LUA_000033 = GetResString("HELP_HELP_LUA_000033")
AddHelpInfo(HELP_HELP_LUA_000033,   HELP_HELP_LUA_000032)
--......





























--开发人员留言区

--项目名词解释
HELP_HELP_LUA_000034 = GetResString("HELP_HELP_LUA_000034")
AddHelpInfo("mindwave",  HELP_HELP_LUA_000034)
HELP_HELP_LUA_000035 = GetResString("HELP_HELP_LUA_000035")
AddHelpInfo("mindpower", HELP_HELP_LUA_000035)
HELP_HELP_LUA_000036 = GetResString("HELP_HELP_LUA_000036")
HELP_HELP_LUA_000037 = GetResString("HELP_HELP_LUA_000037")
AddHelpInfo(HELP_HELP_LUA_000037,      HELP_HELP_LUA_000036)
HELP_HELP_LUA_000038 = GetResString("HELP_HELP_LUA_000038")
HELP_HELP_LUA_000039 = GetResString("HELP_HELP_LUA_000039")
AddHelpInfo(HELP_HELP_LUA_000039,    HELP_HELP_LUA_000038)
HELP_HELP_LUA_000040 = GetResString("HELP_HELP_LUA_000040")
HELP_HELP_LUA_000041 = GetResString("HELP_HELP_LUA_000041")
AddHelpInfo(HELP_HELP_LUA_000041,  HELP_HELP_LUA_000040)
HELP_HELP_LUA_000042 = GetResString("HELP_HELP_LUA_000042")
HELP_HELP_LUA_000043 = GetResString("HELP_HELP_LUA_000043")
AddHelpInfo(HELP_HELP_LUA_000043 ,       HELP_HELP_LUA_000042)

--策划区
HELP_HELP_LUA_000044 = GetResString("HELP_HELP_LUA_000044")
AddHelpInfo("paco",      HELP_HELP_LUA_000044)
HELP_HELP_LUA_000045 = GetResString("HELP_HELP_LUA_000045")
AddHelpInfo("robin",     HELP_HELP_LUA_000045)
AddHelpInfo("sk",        "He is a Designer,Not a Planner!")
HELP_HELP_LUA_000046 = GetResString("HELP_HELP_LUA_000046")
AddHelpInfo("aki",       HELP_HELP_LUA_000046)
HELP_HELP_LUA_000047 = GetResString("HELP_HELP_LUA_000047")
AddHelpInfo("baby",      HELP_HELP_LUA_000047)
HELP_HELP_LUA_000048 = GetResString("HELP_HELP_LUA_000048")
AddHelpInfo("sage",      HELP_HELP_LUA_000048)
HELP_HELP_LUA_000048 = GetResString("HELP_HELP_LUA_000048")
AddHelpInfo("idle",      HELP_HELP_LUA_000048)
HELP_HELP_LUA_000049 = GetResString("HELP_HELP_LUA_000049")
AddHelpInfo("mars",      HELP_HELP_LUA_000049)
HELP_HELP_LUA_000050 = GetResString("HELP_HELP_LUA_000050")
AddHelpInfo("koyo",      HELP_HELP_LUA_000050)

--程序区
HELP_HELP_LUA_000051 = GetResString("HELP_HELP_LUA_000051")
AddHelpInfo(HELP_HELP_LUA_000051,      "force is strong with this one!")
HELP_HELP_LUA_000052 = GetResString("HELP_HELP_LUA_000052")
AddHelpInfo("jerry",     HELP_HELP_LUA_000052)
HELP_HELP_LUA_000053 = GetResString("HELP_HELP_LUA_000053")
AddHelpInfo("claude",    HELP_HELP_LUA_000053)
HELP_HELP_LUA_000054 = GetResString("HELP_HELP_LUA_000054")
AddHelpInfo("jacky",     HELP_HELP_LUA_000054)
HELP_HELP_LUA_000055 = GetResString("HELP_HELP_LUA_000055")
AddHelpInfo("adnor",     HELP_HELP_LUA_000055)
HELP_HELP_LUA_000056 = GetResString("HELP_HELP_LUA_000056")
AddHelpInfo("arcol",     HELP_HELP_LUA_000056)
HELP_HELP_LUA_000048 = GetResString("HELP_HELP_LUA_000048")
AddHelpInfo("knight",    HELP_HELP_LUA_000048)
HELP_HELP_LUA_000057 = GetResString("HELP_HELP_LUA_000057")
AddHelpInfo("michael",   HELP_HELP_LUA_000057)
HELP_HELP_LUA_000058 = GetResString("HELP_HELP_LUA_000058")
AddHelpInfo("jack",      HELP_HELP_LUA_000058)
HELP_HELP_LUA_000059 = GetResString("HELP_HELP_LUA_000059")
AddHelpInfo("lemon",     HELP_HELP_LUA_000059)
HELP_HELP_LUA_000060 = GetResString("HELP_HELP_LUA_000060")
AddHelpInfo("david",     HELP_HELP_LUA_000060)
HELP_HELP_LUA_000061 = GetResString("HELP_HELP_LUA_000061")
AddHelpInfo("philip",    HELP_HELP_LUA_000061)
HELP_HELP_LUA_000062 = GetResString("HELP_HELP_LUA_000062")
AddHelpInfo("mikcal",    HELP_HELP_LUA_000062)
HELP_HELP_LUA_000063 = GetResString("HELP_HELP_LUA_000063")
AddHelpInfo("jampe",     HELP_HELP_LUA_000063)

--美术区
HELP_HELP_LUA_000064 = GetResString("HELP_HELP_LUA_000064")
HELP_HELP_LUA_000065 = GetResString("HELP_HELP_LUA_000065")
AddHelpInfo(HELP_HELP_LUA_000065,     HELP_HELP_LUA_000064)
HELP_HELP_LUA_000066 = GetResString("HELP_HELP_LUA_000066")
AddHelpInfo("gsc",       HELP_HELP_LUA_000066)
HELP_HELP_LUA_000067 = GetResString("HELP_HELP_LUA_000067")
AddHelpInfo("gooncoo",   HELP_HELP_LUA_000067)
HELP_HELP_LUA_000068 = GetResString("HELP_HELP_LUA_000068")
AddHelpInfo("sean",      HELP_HELP_LUA_000068)
HELP_HELP_LUA_000069 = GetResString("HELP_HELP_LUA_000069")
AddHelpInfo("xiaojinjin",HELP_HELP_LUA_000069)
HELP_HELP_LUA_000070 = GetResString("HELP_HELP_LUA_000070")
HELP_HELP_LUA_000071 = GetResString("HELP_HELP_LUA_000071")
AddHelpInfo(HELP_HELP_LUA_000071,   HELP_HELP_LUA_000070)
HELP_HELP_LUA_000072 = GetResString("HELP_HELP_LUA_000072")
AddHelpInfo("potion",    HELP_HELP_LUA_000072)
HELP_HELP_LUA_000073 = GetResString("HELP_HELP_LUA_000073")
HELP_HELP_LUA_000074 = GetResString("HELP_HELP_LUA_000074")
AddHelpInfo(HELP_HELP_LUA_000074, HELP_HELP_LUA_000073)
HELP_HELP_LUA_000075 = GetResString("HELP_HELP_LUA_000075")
AddHelpInfo("paul",      HELP_HELP_LUA_000075)
HELP_HELP_LUA_000076 = GetResString("HELP_HELP_LUA_000076")
HELP_HELP_LUA_000077 = GetResString("HELP_HELP_LUA_000077")
AddHelpInfo(HELP_HELP_LUA_000077,      HELP_HELP_LUA_000076)

--美食介绍
HELP_HELP_LUA_000078 = GetResString("HELP_HELP_LUA_000078")
HELP_HELP_LUA_000079 = GetResString("HELP_HELP_LUA_000079")
AddHelpInfo(HELP_HELP_LUA_000079, HELP_HELP_LUA_000078)

--汽车
HELP_HELP_LUA_000080 = GetResString("HELP_HELP_LUA_000080")
HELP_HELP_LUA_000081 = GetResString("HELP_HELP_LUA_000081")
AddHelpInfo(HELP_HELP_LUA_000081, HELP_HELP_LUA_000080)
