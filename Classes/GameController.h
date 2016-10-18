#pragma once

#include <time.h>
#include "Utilites.h"
#include "GameField.h"

namespace cocos2d
{
	class Touch;
	class Event;
}

namespace Game
{

class GameController: Utilites::NonCopyable
{

public:
	GameController(GameField::sharedptr_type spGameField);
	virtual ~GameController() { /**/ };

	void Update(float dt);

public:
	virtual bool onTouchBegan(Touch* touch, Event *event) { return false; }
    virtual void onTouchMove(Touch* touch, Event *event) { /**/ }
    virtual void onTouchEnded(Touch* touch, Event *event) { /**/ }
    virtual void onTouchCancelled(Touch* touch, Event *event) { /**/ }

protected:
	virtual void InternalUpdate(float dt) = 0;

protected:
	GameField::sharedptr_type			 m_GameField;
};


} // namespace Game
