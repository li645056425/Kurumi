local msgViewBase = require("app.common.msgViewBase")
local GameMain = class("GameMain", msgViewBase)

local RoleConfig = require("app.config.RoleConfig")

local loadAllRole = false
local playerRole = "hero_shizhuak_dao"
local otherRole = {"hero_shizshuishoufu_dao"}--, "hero_shizmaonv_dao", "hero_shizhuak_dao"}

function GameMain:onCreate()

    GameMain.super.onCreate(self)

    local mapConfig = require("app.config.MapConfig")

    local word = GameWord:create()
    mapConfig:loadMap(word, "map3")
	self:addChild(word)

    self.word = word

    local uiPage = _MyG.loadStudioFile("HomeSceneUI", self)
    self:addChild(uiPage.root)

    

    local ss = cc.SpriteFrameCache:getInstance():getSpriteFrameByName("rect_attack_a.png")
    if ss == nil then
        print("rect attack png is nil")
    else
        print("rect attack png is not nil")
    end
    cc.SpriteFrameCache:getInstance():addSpriteFrames("rect_collide.plist")
    print(cc.TextureCache:getInstance():getDescription())
    print(cc.TextureCache:getInstance():getCachedTextureInfo())
    print("begin-------------------------------------------------\n\n\n\n")


    self:onClickLoad(nil)
end

function GameMain:loadFinish()
    -- local hero = require("app.actor.role.Hero_dao"):create()
    -- self.word:addActor(hero)
    -- self.word:setLocalPlayer(hero)

    -- _MyG.PlayerController:setPlayer(hero)

    -- self.hero = hero


    -- local winSize = cc.Director:getInstance():getVisibleSize()

    -- local controlUI = require("app.ui.ControlUI"):create()
    -- self.word:addChild(controlUI, 1)


    -- local allRole = {}
    -- local count = 1
    -- for k,v in pairs(RoleConfig) do
    --     allRole[count] = v.Armature
    --     count = count + 1
    -- end

    -- hero:changeRole(playerRole)
    
    -- count = 1

    -- if loadAllRole then
    --     local callbackEntry = nil
    
    --     local function localfunc(time)
    --         print("time = ", time)
    --         hero = require("app.actor.role.Hero_dao"):create()
    --         hero:setActorPosition(300 + 70 * count, 0)
    --         hero:setActorType(AT_MONSTER)
    --         hero:changeRole(allRole[count])
    --         self.word:addActor(hero)
    
    --         count = count + 1
    --         if count > #allRole then
    --             print("load finish")
    --             cc.Director:getInstance():getScheduler():unscheduleScriptEntry(callbackEntry)
    --         end
    --     end
    
    --     local scheduler=cc.Director:getInstance():getScheduler()
    --     callbackEntry = scheduler:scheduleScriptFunc(localfunc,1,false)
    -- else
    --     local callbackEntry = nil
    
    --     local function localfunc(time)
    --         print("time = ", time)
    --         hero = require("app.actor.role.Hero_dao"):create()
    --         hero:setActorPosition(300 + 100 * count, 0)
    --         hero:setActorType(AT_MONSTER)
    --         hero:changeRole(otherRole[count])
    --         self.word:addActor(hero)
    
    --         count = count + 1
    --         if count > #otherRole then
    --             print("load finish")

    --             cc.Director:getInstance():getScheduler():unscheduleScriptEntry(callbackEntry)


    --             local ChangQiang = require("app.actor.monster.Monster_ChangQiangWS"):create()
    --             ChangQiang:setActorPosition(800 + 100 * count, 0)
    --             self.word:addActor(ChangQiang)
    --         end
    --     end
    
    --     local scheduler=cc.Director:getInstance():getScheduler()
    --     if #otherRole > 0 then
    --         callbackEntry = scheduler:scheduleScriptFunc(localfunc,1,false)
    --     end
    -- end



    local hero = require("app.actor.role.Hero_dao"):create()
    self.word:addActor(hero)
    self.word:setLocalPlayer(hero)

    _MyG.PlayerController:setPlayer(hero)

    self.hero = hero
    hero:changeRole(playerRole)

    local hero1 = require("app.actor.role.Hero_dao"):create()
    hero1:changeRole(playerRole)
    hero1:setActorPositionInValidRect({x = 500, y = 0})
    hero1:setActorType(AT_MONSTER)
    self.word:addActor(hero1)


    local winSize = cc.Director:getInstance():getVisibleSize()

    local controlUI = require("app.ui.ControlUI"):create()
    self.word:addChild(controlUI, 1)

    local ss = cc.SpriteFrameCache:getInstance():getSpriteFrameByName("rect_attack_a.png")
    if ss == nil then
        print("rect attack png is nil")
    else
        print("rect attack png is not nil")
    end

    -- for i = 1, 1 do
    -- local MS = require("app.actor.monster.Monster_Shengbo"):create()
    -- MS:setActorPosition(200, 0)
    -- self.word:addActor(MS)
    -- end

    -- for i = 1, 1 do
    -- local MS = require("app.actor.monster.Monster_LeiShen"):create()
    -- MS:setActorPosition(200, 0)
    -- self.word:addActor(MS)
    -- end
end

function GameMain:onClickLoad(sender)

    _MyG.Loading:showLoading(nil, self)

    -- self:loadFinish()
    -- print(cc.TextureCache:getInstance():getDescription())
    -- print(cc.TextureCache:getInstance():getCachedTextureInfo())



    if self.isLoad then
        return
    end
    self.isLoad = true
    self.printUpdate = true

    local count = 0
    for k,v in pairs(RoleConfig) do
        count = count + 1
    end


    if not loadAllRole then

        local loadRole = {}
        loadRole[1] = RoleConfig[playerRole].ExportJson

        for i=1,#otherRole do
            loadRole[i + 1] = RoleConfig[otherRole[i]].ExportJson
        end

        for i = 1, #loadRole do
            ccs.ArmatureDataManager:getInstance():addArmatureFileInfoAsync(loadRole[i], function(jindu)
                print(jindu)
                if jindu >= 1.0 and self.printUpdate then
                    _MyG.Loading:hideLoding()
                    self.printUpdate = false
                    self:loadFinish()
                end
            end)
        end
    else
        for k,v in pairs(RoleConfig) do
            ccs.ArmatureDataManager:getInstance():addArmatureFileInfoAsync(v.ExportJson, function(jindu)
                print(jindu)
                if jindu >= 1.0 and self.printUpdate then
                    _MyG.Loading:hideLoding()
                    self.printUpdate = false
                    self:loadFinish()
                end
            end)
        end
    end
end

function GameMain:updateLogin(delay)
   if self.printUpdate then
        print(delay)
   end 
end

return GameMain