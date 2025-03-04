-- @Author: fangcheng
-- @Date:   2019-06-06 22:08:04
-- @remark: 

local InputLayer = class("InputLayer", G_Class.SuperNode)

local InputKey = Const.InputKey

local KEY_CODE_MAP = {
    [cc.KeyCode.KEY_A] = InputKey.G_KEY_MOVE_LEFT,
    [cc.KeyCode.KEY_D] = InputKey.G_KEY_MOVE_RIGHT,
    [cc.KeyCode.KEY_W] = InputKey.G_KEY_MOVE_UP,
    [cc.KeyCode.KEY_S] = InputKey.G_KEY_MOVE_DOWN,
    [cc.KeyCode.KEY_SPACE] = InputKey.G_KEY_JUMP,
    [cc.KeyCode.KEY_J] = InputKey.G_KEY_SKILL_1,

}

-- 按键输入
propertyReadOnly(InputLayer, "iInputKey", 0)

function InputLayer:ctor()
    self.ui = G_Helper.loadStudioFile("ccs.ui.fight.UI_InputLayer", self)
    self:addChild(self.ui.root)

    self:initTouch()
    self:initKeyboard()
    self:initPing()

    self:enableNodeEvents()
end

function InputLayer:onEnter()
    G_SysEventEmitter:on(SysEvent.FIGHT_UPLOAD_INPUT, handler(self, self.onUploadInput), self)
end

function InputLayer:onExit()
    G_SysEventEmitter:offByTag(self)
end

function InputLayer:onUploadInput()
    _MyG.FightManager:sendRunNextFrameReq(self.iInputKey)
end

function InputLayer:initTouch()

	local beginPos = {x = self.ui.sprite_BG:getPositionX(), y = self.ui.sprite_BG:getPositionY()}
	local touchBox = self.ui.sprite_BG:getBoundingBox()

	local size = self.ui.sprite_BG:getContentSize()

	local touchR = self.ui.sprite_Red:getContentSize().width * 0.5
	local centerPos = {x = 0, y = 0}

	local touchLayer = self

    local cancelKey = 0
    cancelKey = GTools:U32_OR(cancelKey, InputKey.G_KEY_MOVE_LEFT)
    cancelKey = GTools:U32_OR(cancelKey, InputKey.G_KEY_MOVE_RIGHT)
    cancelKey = GTools:U32_OR(cancelKey, InputKey.G_KEY_MOVE_UP)
    cancelKey = GTools:U32_OR(cancelKey, InputKey.G_KEY_MOVE_DOWN)


	local function normal()
		self.ui.sprite_BG:setPosition(beginPos)
		self.ui.sprite_Move:setPosition({x = 0, y = 0})
		self.ui.sprite_Move:setVisible(false)
		self.ui.sprite_Red:setVisible(false)
		self.ui.sprite_Ori:setOpacity(180)
	end
	normal()

	local function onTouchBegin(touch,event)
		local curPos = touchLayer:convertToNodeSpace(touch:getLocation())
		if cc.rectContainsPoint(touchBox, curPos) then
			self.ui.sprite_BG:setPosition(curPos)
			self.ui.sprite_Move:setPosition(curPos)
			self.ui.sprite_Move:setVisible(true)
			self.ui.sprite_Ori:setOpacity(255)
			centerPos = curPos
			return true
		end
		return false
	end

	local function onTouchMove(touch,event)
		local curPos = touchLayer:convertToNodeSpace(touch:getLocation())

		local V_R = { x = curPos.x - centerPos.x, y = curPos.y - centerPos.y }
		local length = cc.pGetLength(V_R)
		if length < 5 then
			return
		end

		local angle = cc.pToAngleSelf(V_R)
		local radian = -angle * 57.29577951

		local moveR = touchR - 36
		if length <= moveR then
			self.ui.sprite_Move:setPosition(curPos)
		else
			angle = angle
			local x = math.cos(angle) * moveR + centerPos.x
			local y = math.sin(angle) * moveR + centerPos.y
			self.ui.sprite_Move:setPosition({x = x, y = y})
		end

		self.ui.sprite_Red:setVisible(true)
		self.ui.sprite_Red:setRotation(radian)

		if length < 35 then
			return
		end

		-- 上
		if radian < -30 and radian > -150 then
            self:onKeyDown(InputKey.G_KEY_MOVE_UP)
		-- 下
		elseif radian >= 60 and radian < 120 then
            self:onKeyDown(InputKey.G_KEY_MOVE_DOWN)
		end
		if math.abs(V_R.x) < 0.001 then
			return
		end

		-- 右
		if V_R.x > 0 then
            self:onKeyDown(InputKey.G_KEY_MOVE_RIGHT)
		-- 左
		else
            self:onKeyDown(InputKey.G_KEY_MOVE_LEFT)
		end
	end

	local function onTouchEnd(touch,event)
		normal()
		self:onKeyUp(cancelKey)
	end

	local listener = cc.EventListenerTouchOneByOne:create();
	listener:registerScriptHandler(onTouchBegin,cc.Handler.EVENT_TOUCH_BEGAN);
	listener:registerScriptHandler(onTouchMove,cc.Handler.EVENT_TOUCH_MOVED);
	listener:registerScriptHandler(onTouchEnd,cc.Handler.EVENT_TOUCH_ENDED);
	listener:registerScriptHandler(onTouchEnd,cc.Handler.EVENT_TOUCH_CANCELLED);
	cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(listener, touchLayer)
end

function InputLayer:onKeyDown(key)
    
    if key == InputKey.G_KEY_MOVE_LEFT then
        self:onKeyUp(InputKey.G_KEY_MOVE_RIGHT)
    elseif key == InputKey.G_KEY_MOVE_RIGHT then
        self:onKeyUp(InputKey.G_KEY_MOVE_LEFT)
    elseif key == InputKey.G_KEY_MOVE_UP then
        self:onKeyUp(InputKey.G_KEY_MOVE_DOWN)
    elseif key == InputKey.G_KEY_MOVE_DOWN then
        self:onKeyUp(InputKey.G_KEY_MOVE_UP)
    end

    self.iInputKey = GTools:U32_BIT_SET(self.iInputKey, key)
end

function InputLayer:onKeyUp(key)
    self.iInputKey = GTools:U32_BIT_REMOVE(self.iInputKey, key)
end

function InputLayer:initKeyboard()
	 --键盘事件  
    local function onKeyPressed(keyCode, event)
        -- print("keyCode", keyCode)
        if KEY_CODE_MAP[keyCode] then
            self:onKeyDown(KEY_CODE_MAP[keyCode])
        end
    end
  
    local function onKeyReleased(keyCode, event)
        if KEY_CODE_MAP[keyCode] then
            self:onKeyUp(KEY_CODE_MAP[keyCode])
        end
    end  
  
    local listener = cc.EventListenerKeyboard:create()  
    listener:registerScriptHandler(onKeyPressed, cc.Handler.EVENT_KEYBOARD_PRESSED)  
    listener:registerScriptHandler(onKeyReleased, cc.Handler.EVENT_KEYBOARD_RELEASED)  
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(listener, self) 
end

function InputLayer:initPing()
	self:schedule(function()
		local ping = _MyG.FightManager:getPing()
		self.ui.Text_Ping:setString(string.format("Ping:%d", ping))
	end, 0.5)
end


function InputLayer:onClickChangeWeapon(sender)
	-- local player = _MyG.PlayerController:getPlayer()
	-- _MyG.PlayerDispatcher:call("control_changeWeapon", player)
end

function InputLayer:onTouchSkill(sender, eventType)
	local userData = tonumber(sender.UserData[1])

	if eventType == ccui.TouchEventType.began then
		self:onKeyDown(InputKey["G_KEY_SKILL_" .. userData])
	elseif eventType == ccui.TouchEventType.moved then
	else
		self:onKeyUp(InputKey["G_KEY_SKILL_" .. userData])
	end
end


return InputLayer