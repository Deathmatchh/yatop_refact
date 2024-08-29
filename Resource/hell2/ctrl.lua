--此文件中，凡是可能被多次执行的函数，函数名都要加上地图名前缀

function config(map)
    MapCanSavePos(map, 0) --设置地图是否保存角色位置（地图，坐标，方向），此设置影响该地图的所有副本
    MapCanPK(map, 1) --设置地图是否可以PK，此设置影响该地图的所有副本
    --MapCopyNum(map, 1) --设置地图的副本数目，如果不调用该语句，则使用默认值1
    --MapCopyStartType(map, 1) --设置地图立即开始
    SingleMapCopyPlyNum(map, 300)   --设置一个副本的玩家数
    MapCanTeam(map , 1)
    MapType ( map , 4 )
end


function get_map_entry_pos_hell2()   --设置入口的位置的坐标（坐标（米））

	local POS_X=72
	local POS_Y=179
	return POS_X , POS_Y

end

function init_entry(map)
    SetMapEntryMapName(map, "hell") --设置入口的位置（地图名，坐标（米））
    SetMapEntryTime(map, "2007/1/19/21/30", "1/0/0", "0/0/30", "0/5/30") --设置入口的时间，地图对象，首次开启时间（年/月/日/时/分），以后再次开启的间隔（日/时/分，全０表示只有首次开启），每次开启到入口消失的间隔（日/时/分，全０表示永不消失），每次开启到地图关闭的间隔（日/时/分，全０表示永不关闭）。。

end

function after_enter_hell2( role , map_copy )

local cha_name=GetChaDefaultName(role)

HELL2_CTRL_LUA_000001 = GetResString("HELL2_CTRL_LUA_000001")
HELL2_CTRL_LUA_000002 = GetResString("HELL2_CTRL_LUA_000002")
SystemNotice ( role, HELL2_CTRL_LUA_000002..cha_name..HELL2_CTRL_LUA_000001 ) 

end

function before_leave_hell2 ( role )

end

function map_copy_first_run_hell2( map_copy )



end

function map_copy_run_hell2( map_copy )

			
	if CheckMonsterDead ( AZRAEL[9] ) == 1 then
		
		if CRY[9]==0 then
			HELL2_CTRL_LUA_000003 = GetResString("HELL2_CTRL_LUA_000003")
			local Notice_all = HELL2_CTRL_LUA_000003
			MapCopyNotice ( map_copy , Notice_all )
			CRY[9]=1
		end
	end
		
	if CRY[9]==1  then
		
		if HELLCLOSETIME==300 then

			HELL2_CTRL_LUA_000004 = GetResString("HELL2_CTRL_LUA_000004")
			Notice (HELL2_CTRL_LUA_000004)
			HELLCLOSETIME = HELLCLOSETIME - 60
		end

		local closetime = HELLCLOSETIME
		for i = 1 , MAXNOTICE , 1 do
			if closetime ==NOTICETIME[i] then
				HELL_CTRL_LUA_000008 = GetResString("HELL_CTRL_LUA_000008")
				HELL2_CTRL_LUA_000005 = GetResString("HELL2_CTRL_LUA_000005")
				local Notice_all = HELL2_CTRL_LUA_000005..closetime..HELL_CTRL_LUA_000008
				MapCopyNotice ( map_copy ,Notice_all )
			end
		end
		HELLCLOSETIME = HELLCLOSETIME-1

		if HELLCLOSETIME == 0 then


			HELLCLOSETIME = 300
			CRY[9]=0
			CloseMapEntry ( "hell2" )
			CloseMapCopy ( "hell2" )

		end
	end
end

--每5秒执行一次的
function map_run_hell2( map )

end

--地图关闭时执行
function map_copy_close_hell2 ( map_copy )

			HELLCLOSETIME = 300
			CRY[9]=0

end
		
--地图开关判断——————————————————————————————————————————

function can_open_entry_hell2( map ) 

--	local time = GetTickCount ( ) 
	local Now_Week = GetNowWeek()
	if Now_Week >=5 and Now_Week < 7 then
		return 1
	end

		return 0

end 
