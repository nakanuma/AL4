#pragma once

#include "Input.h"

#include "Float2.h"
#include "Float3.h"

struct MoveKeys
{
	BYTE up    [2]  = {DIK_UP    , DIK_W};
	BYTE down  [2]  = {DIK_DOWN  , DIK_S};
	BYTE left  [2]  = {DIK_LEFT  , DIK_A};
	BYTE right [2]  = {DIK_RIGHT , DIK_D};
	BYTE attack     =  DIK_SPACE;
};
const MoveKeys moveKeys{};

class Player;
class IPlayerState
{
public:
	IPlayerState(Player* player):player_(player)
	{
		input_ = Input::GetInstance();
	}
	virtual ~IPlayerState() {}

	virtual void Initialize() = 0;
	virtual void Update() = 0;

protected:
	Player* player_;
	Input* input_;
};

///======================================================
/// 通常状態
///======================================================
class NeutralPlayerState
	:public IPlayerState
{
public:
	NeutralPlayerState(Player* player):IPlayerState(player) {}
	~NeutralPlayerState()override {}

	void Initialize()override;
	void Update()override;
private:
};

///======================================================
/// 移動状態
///======================================================
class MovingPlayerState
	:public IPlayerState
{
public:
	MovingPlayerState(Player* player,Float2 moveVal);
	~MovingPlayerState()override;

	void Initialize()override;
	void Update()override;
private:
	Float2 fromAddress_; // 移動前 の アドレス
	Float2 forAddress_;  // 移動後 の アドレス

	 // float beforeRotate_;
	 // float afterRotate_;

	float fullTime_;
	float currentTime_;

	float jumpHeight_;
};

///======================================================
/// チャージ 状態
///======================================================
class ChargePlayerState
	:public IPlayerState
{
public:
	ChargePlayerState(Player* player);
	~ChargePlayerState()override;

	void Initialize()override;
	void Update()override;
private:
	float fullTime_;
	float currentTime_;

	Float3 movedHandOffset_;
};

///======================================================
/// 攻撃 状態
///======================================================
class AttackPlayerState
	:public IPlayerState
{
public:
	AttackPlayerState(Player* player):IPlayerState(player) {}
	~AttackPlayerState()override;

	void Initialize()override;
	void Update()override;
private:
	float fullTime_;
	float currentTime_;
};

///======================================================
/// ノックバック 状態
///======================================================
class KnockBackPlayerState
	:public IPlayerState
{
public:
	KnockBackPlayerState(Player* player,Float2 moveVal):IPlayerState(player) {}
	~KnockBackPlayerState()override;

	void Initialize()override;
	void Update()override;
private:
	Float2 fromAddress_; // 移動前 の アドレス
	Float2 forAddress_;  // 移動後 の アドレス

	float fullTime_;
	float currentTime_;

	float jumpHeight_;
};
