
�
C2Fight.protomsg"{
JoinFightReq
	fightUUID (R	fightUUID
playerID (RplayerID
frame (Rframe"
MsgId
None 
Id�"�
FightWorldInfo
frame (Rframe

svr_status (R	svrStatus
mapId (RmapId

randomSeed (R
randomSeed
uuidSeed (RuuidSeed$
isOfflineMode (RisOfflineMode
roles (Rroles"r
JoinFightAck
code (Rcode1
	worldInfo (2.msg.FightWorldInfoR	worldInfo"
MsgId
None 
Id�"a
PlayerLoadingReq
percent (Rpercent
finish (Rfinish"
MsgId
None 
Id�"s
PlayerLoadingAck
pid (Rpid
percent (Rpercent
finish (Rfinish"
MsgId
None 
Id�"B
PlayerReadyNotify
pid (Rpid"
MsgId
None 
Id�"F
PlayerExitFightNotify
pid (Rpid"
MsgId
None 
Id�"U
PlayerFrameInput
pid (Rpid
frame (Rframe
key_down (RkeyDown"_
RunNextFrameReq
frame (Rframe
key_down (RkeyDown"
MsgId
None 
Id�"{
RunNextFrameAck
	nextFrame (R	nextFrame-
frames (2.msg.PlayerFrameInputRframes"
MsgId
None 
Id�"2
PushFrameInputBegin"
MsgId
None 
Id�"0
PushFrameInputEnd"
MsgId
None 
Id�"z
PushFrameInput
	lastFrame (R	lastFrame-
frames (2.msg.PlayerFrameInputRframes"
MsgId
None 
Id�">
PlayerRecords-
frames (2.msg.PlayerFrameInputRframes"I
ExitFightReq
	fightUUID (R	fightUUID"
MsgId
None 
Id�"?
ExitFightAck
code (Rcode"
MsgId
None 
Id�"U
Ping
	timestamp (R	timestamp
ping (Rping"
MsgId
None 
Id�"A
Pong
	timestamp (R	timestamp"
MsgId
None 
Id��"0
PingInfo
pid (Rpid
ping (Rping"P
PushPingInfo#
infos (2.msg.PingInfoRinfos"
MsgId
None 
Id��BHbproto3