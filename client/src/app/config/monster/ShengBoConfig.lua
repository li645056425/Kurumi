local ShengBoConfig = 
{
	ExportJson = "monster/shengbo/shengbo.ExportJson",
	Armature = "shengbo",

	StateConfig = 
	{
		State_Stand = {"fstand"},
		State_Run = {"frun_2"},
		State_Turn = {"fturn_1", "fturn_2"},
		State_Hit = {"hit"},
		State_Attack1 = {"attack1"},
		State_Attack2 = {"attack2"},
		State_Kill = {"skill1"},
		
		-- State_Collapase_Up = {"collapse_1", "collapse_2"},
		-- State_Collapase_Down = {"collapse_3", "collapse_4"},
		State_Collapase_Up = {"collapse_1", "collapse_3"},
		State_Collapase_Down = {"collapse_4"},
		
		State_Collapase_EndToStand = {"collapse_5", "collapse_6", "collapse_7"},
		State_Collapase_EndToDead = {"collapse_5", "collapse_6", "dead"},
		State_Dead	= {"collapse_3", "collapse_5", "dead"},
	}
}

-- 基础属性配置
ShengBoConfig.BaseConfig = 
{
	-- 移动速度
	MoveVelocity = 500.0 / BOX2D_PIXEL_TO_METER,

	-- 受到攻击时冲力
	HitImpluse = -150.0 / BOX2D_PIXEL_TO_METER,

	-- 受到攻击并向后抛 
	-- 抛起冲力(瞬间冲力)
	CollapseYImpluse = 500.0 / BOX2D_PIXEL_TO_METER,

	-- 向后冲力
	CollapseXImpluse = -100.0 / BOX2D_PIXEL_TO_METER,

}

-- AI
ShengBoConfig.AI_CONFIG = 
{
	ATTACK_MIN_RANGE = 400,
	ATTACK_INTERVAL = 1.0,
}

--跟随配置
ShengBoConfig.FOLLOW_CONFIG = 
{
	--状态切换最小间隔时间
	STATE_CHAGE_MIN_TIME = 0.5,
	--状态切换最大间隔时间
	STATE_CHAGE_MAX_TIME = 1.5,

	--跟随最大值
	FOLLOW_MAX = 800,
	--跟随最小值
	FOLLOW_MIN = 100,
	
	--小于最小值时的配置
	FOLLOW_L_MIN = 
	{
		--切换方向最小间隔时间
		updateOriMinTime = 0.5,
		--切换方向最大间隔时间
		updateOriMaxTime = 0.8,
		--移动状态下权重
		changeOriWidget = {
			FOLLOW = 3,	--跟随
			BACK   = 3,	--后退
			NONE   = 1, --不做任何操作（休息）
		},
		--状态权重
		STATE_WEIGHT = 
		{
			SLEEP = 0,
			MOVE  = 1,
			OTHER = 6
		},
	},
	--大于最大值时的配置
	FOLLOW_G_MAX = 
	{
		updateOriMinTime = 0.3,
		updateOriMaxTime = 0.5,
		changeOriWidget = {
			FOLLOW = 2,
			BACK   = 2,
			NONE   = 3,
		},
		STATE_WEIGHT = 
		{
			SLEEP = 2,
			MOVE  = 8,
			OTHER = 0
		},
	},
	--最大值和最小值之间
	FOLLOW_E_RANGE = 
	{
		updateOriMinTime = 0.3,
		updateOriMaxTime = 0.5,
		changeOriWidget = {
			FOLLOW = 4,
			BACK   = 2,
			NONE   = 1,
		},
		STATE_WEIGHT = 
		{
			SLEEP = 0,
			MOVE  = 6,
			OTHER = 3
		},
	},
}

return ShengBoConfig