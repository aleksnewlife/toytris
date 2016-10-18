
#include "GameController.h"
#include <base/CCEventListener.h>
#include <base/CCEventListenerTouch.h>
//#include <base/CCTouch.h>
//#include <base/CCEvent.h>

namespace Game
{

GameController::GameController(GameField::sharedptr_type spGameField) 
	: m_GameField(spGameField)
{

};

void GameController::Update(float dt)
{
	InternalUpdate(dt);
}

} // namespace Game
