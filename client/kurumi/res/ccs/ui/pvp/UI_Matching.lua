--------------------------------------------------------------
-- This file was automatically generated by Cocos Studio.
-- Do not make changes to this file.
-- All changes will be lost.
--------------------------------------------------------------

local luaExtend = require "LuaExtend"

-- using for layout to decrease count of local variables
local layout = nil
local localLuaFile = nil
local innerCSD = nil
local innerProject = nil
local localFrame = nil

local Result = {}
------------------------------------------------------------
-- function call description
-- create function caller should provide a function to 
-- get a callback function in creating scene process.
-- the returned callback function will be registered to 
-- the callback event of the control.
-- the function provider is as below :
-- Callback callBackProvider(luaFileName, node, callbackName)
-- parameter description:
-- luaFileName  : a string, lua file name
-- node         : a Node, event source
-- callbackName : a string, callback function name
-- the return value is a callback function
------------------------------------------------------------
function Result.create(callBackProvider)

local result={}
setmetatable(result, luaExtend)

--Create Layer
local Layer=cc.Node:create()
Layer:setName("Layer")
layout = ccui.LayoutComponent:bindLayoutComponent(Layer)
layout:setSize({width = 1136.0000, height = 640.0000})

--Create Image_2
local Image_2 = ccui.ImageView:create()
Image_2:ignoreContentAdaptWithSize(false)
Image_2:loadTexture("plist/ui/fragment/4-1.jpg",0)
Image_2:setLayoutComponentEnabled(true)
Image_2:setName("Image_2")
Image_2:setTag(151)
Image_2:setCascadeColorEnabled(true)
Image_2:setCascadeOpacityEnabled(true)
Image_2:setPosition(568.0000, 320.0000)
layout = ccui.LayoutComponent:bindLayoutComponent(Image_2)
layout:setPositionPercentXEnabled(true)
layout:setPositionPercentYEnabled(true)
layout:setPositionPercentX(0.5000)
layout:setPositionPercentY(0.5000)
layout:setPercentWidthEnabled(true)
layout:setPercentHeightEnabled(true)
layout:setPercentWidth(1.0000)
layout:setPercentHeight(1.0000)
layout:setSize({width = 1136.0000, height = 640.0000})
Layer:addChild(Image_2)

--Create Text_Status
local Text_Status = ccui.Text:create()
Text_Status:ignoreContentAdaptWithSize(true)
Text_Status:setTextAreaSize({width = 0, height = 0})
Text_Status:setFontName("fonts/huakang.ttf")
Text_Status:setFontSize(40)
Text_Status:setString([[匹配中...]])
Text_Status:setLayoutComponentEnabled(true)
Text_Status:setName("Text_Status")
Text_Status:setTag(152)
Text_Status:setCascadeColorEnabled(true)
Text_Status:setCascadeOpacityEnabled(true)
Text_Status:setPosition(579.3481, 311.1584)
layout = ccui.LayoutComponent:bindLayoutComponent(Text_Status)
layout:setPositionPercentX(0.5100)
layout:setPositionPercentY(0.4862)
layout:setPercentWidth(0.1364)
layout:setPercentHeight(0.0625)
layout:setSize({width = 155.0000, height = 40.0000})
layout:setLeftMargin(501.8481)
layout:setRightMargin(479.1519)
layout:setTopMargin(308.8416)
layout:setBottomMargin(291.1584)
Layer:addChild(Text_Status)

--Create Button_2
local Button_2 = ccui.Button:create()
Button_2:ignoreContentAdaptWithSize(false)
cc.SpriteFrameCache:getInstance():addSpriteFrames("plist/ui/ui2.plist")
Button_2:loadTextureNormal("fbjmt01.png",1)
Button_2:setTitleFontSize(14)
Button_2:setTitleColor({r = 65, g = 65, b = 70})
Button_2:setScale9Enabled(true)
Button_2:setCapInsets({x = 15, y = 11, width = 34, height = 42})
Button_2:setLayoutComponentEnabled(true)
Button_2:setName("Button_2")
Button_2:setTag(153)
Button_2:setCascadeColorEnabled(true)
Button_2:setCascadeOpacityEnabled(true)
Button_2:setPosition(1084.4680, 583.5734)
if callBackProvider~=nil then
      Button_2:addClickEventListener(callBackProvider("UI_Matching.lua", Button_2, "onClickBack"))
end
layout = ccui.LayoutComponent:bindLayoutComponent(Button_2)
layout:setPositionPercentX(0.9546)
layout:setPositionPercentY(0.9118)
layout:setPercentWidth(0.0563)
layout:setPercentHeight(0.1000)
layout:setSize({width = 64.0000, height = 64.0000})
layout:setLeftMargin(1052.4680)
layout:setRightMargin(19.5316)
layout:setTopMargin(24.4266)
layout:setBottomMargin(551.5734)
Layer:addChild(Button_2)

--Create Animation
result['animation'] = ccs.ActionTimeline:create()
  
result['animation']:setDuration(0)
result['animation']:setTimeSpeed(1.0000)
--Create Animation List

result['root'] = Layer
return result;
end

return Result

