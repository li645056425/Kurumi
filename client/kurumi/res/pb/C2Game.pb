
�
C2Game.protomsg"c
PlayerAccountInfo
playerID (RplayerID
name (	Rname

arrRoleIds (R
arrRoleIds"�
RoleDetailedInfo
roleId (RroleId
name (	Rname
lv (Rlv

occupation (R
occupation
jsonData (	RjsonData"�
RoleSimpleInfo
roleId (RroleId
name (	Rname
lv (Rlv

occupation (R
occupation
jsonData (	RjsonData"s
LoginReq
token (	Rtoken
account (	Raccount
playerID (RplayerID"
MsgId
None 
Id��"i
LoginAck
code (Rcode,
infos (2.msg.PlayerAccountInfoRinfos"
MsgId
None 
Id"u
EnterGameReq
token (	Rtoken
playerID (RplayerID
roleID (RroleID"
MsgId
None 
IdÚ"r
EnterGameAck
code (Rcode1
roleinfo (2.msg.RoleDetailedInfoRroleinfo"
MsgId
None 
IdĚ"`
CreateRoleReq

occupation (R
occupation
name (	Rname"
MsgId
None 
IdŚ"X
CreateRoleAck
code (Rcode
roleID (RroleID"
MsgId
None 
Idƚ"x
ModifyRoleDataReq
roleId (RroleId
jsonData (	RjsonData
name (	Rname"
MsgId
None 
Idǚ"D
ModifyRoleDataAck
code (Rcode"
MsgId
None 
IdȚ">
ExitGameNtf
code (Rcode"
MsgId
None 
Idʚ"&
PVEFightInfo
roleId (RroleId"�
StartPVEFightReq
carbonId (RcarbonId$
isOfflineMode (RisOfflineMode'
roles (2.msg.PVEFightInfoRroles"
MsgId
None 
Id˚"C
StartPVEFightAck
code (Rcode"
MsgId
None 
Id̚"E
StartPVPFightReq
foeId (RfoeId"
MsgId
None 
Id͚"C
StartPVPFightAck
code (Rcode"
MsgId
None 
IdΚ".
StopPVPMatchReq"
MsgId
None 
IdϚ"B
StopPVPMatchAck
code (Rcode"
MsgId
None 
IdК"�
StartFightNTF
code (Rcode
	fightUUID (R	fightUUID
fightIP (	RfightIP
	fightPort (R	fightPort
netType (RnetType"
MsgId
None 
IdҚ"T
PushRoleData'
info (2.msg.RoleSimpleInfoRinfo"
MsgId
None 
IdԚBHbproto3