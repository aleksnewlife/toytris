#include <CCPlatformMacros.h>
#include <base/CCConsole.h>
#include <2d/CCActionInterval.h>
#include <2d/CCSprite.h>

#include "Line3Controller.h"

#include <cstdlib> //random
#include <sstream>

USING_NS_CC;

namespace Game
{

//touches consts
#define TOUCHDIRTHRESHOLD 45.0f

//shuffle data
#define MAXSHUFFLINGCYCLES 10

//merging
#define MOVESPRITETIME 0.3f
#define WAITINGTIMEDELTAX 0.02f
#define WAITINGTIMEDELTAY 0.01f
#define FADEOUTTIME	0.6f
#define TINTCOLOR_R 0
#define TINTCOLOR_G 0
#define TINTCOLOR_B 0
//new llines
#define FADEINTIME 0.2f
#define MOVEDOWNTIME 1.0f
#define MOVEDOWNVERTDELTADELIM 0.8f
// swap
#define SWAPTIME 0.3f
#define FASTSWAPTIME 0.17f

#define CHECKALINE3VERT(i, j, field, flags, marked) \
	if(field[i][j].m_Type != 0 &&						\
	   field[i][j].m_Type == field[i][j + 1].m_Type &&  \
	   field[i][j].m_Type == field[i][j + 2].m_Type)	\
	{													\
		flags[i][j] = 1;								\
		flags[i][j + 1] = 1;							\
		flags[i][j + 2] = 1;							\
		marked = true;									\
	}													\

#define CHECKALINE3HOR(i, j, field, flags, marked) \
	if(field[i][j].m_Type != 0 &&						\
	   field[i][j].m_Type == field[i + 1][j].m_Type &&  \
	   field[i][j].m_Type == field[i + 2][j].m_Type)	\
	{													\
		flags[i][j] = 1;								\
		flags[i + 1][j] = 1;							\
		flags[i + 2][j] = 1;							\
		marked = true;									\
	}													\


Line3Controller::Line3Controller(GameField::sharedptr_type spGameField)
	: GameController(spGameField)
	, m_GameStage(lgsNone)
	, m_TouchDirection(tmdNone)
	, m_NeedMerge(false)
{

}

Line3Controller::~Line3Controller()
{
	ClearActions();
}

void Line3Controller::SetNewGameStage(const Line3GameStage& newStage)
{
	//TODO:GS: check transition if needed
	m_OldGameStage = m_GameStage;
	m_GameStage = newStage;

	const char* stageTostr[lgsCOUNT] = { "lgsNone", "lgsNewGame", "lgsWatingPlayerActivity", "lgsMergeLines", "lgsGenerateNewLines"};

	CCLOG(" Line3Controlle NEW GAME STAGE: %s", stageTostr[m_GameStage]);
}

void Line3Controller::InternalUpdate(float dt)
{
	switch(m_GameStage)
	{
	case lgsNone:
		RecreateMarkers(m_GameMarkers);
	case lgsNewGame:
		ClearMarkers(m_GameMarkers);
		//m_GameField->Randomize();
		m_GameField->GenerateUniqField();
		m_GameField->SetVisible(true);
		SetNewGameStage(lgsMergeLines);
		break;

	case lgsMergeLines:
		if(IsAllActionsDone(true))
		{
			m_NeedMerge = false;
			if(Merge_MergeCellsBegin())
			{
				if(CheckPossibleMoves(true))
					SetNewGameStage(lgsWatingPlayerActivity); // complete
				else
					SetNewGameStage(lgsShuffling);
			}
			else
				SetNewGameStage(lgsGenerateNewLines);
		}
		break;

	case lgsGenerateNewLines:
		if(IsAllActionsDone(true))
		{
			Megre_MergeCellsEnd();
			if(IsSequiresNewLines())
				Generate_NewCellsAndAnimate();
			SetNewGameStage(lgsMergeLines); //complete
		}
		break;

	case lgsWatingPlayerActivity:
		if(IsAllActionsDone(true))
		{
			if(m_NeedMerge)
			{
				SetNewGameStage(lgsMergeLines);
			}
		}
		break;
	case lgsShuffling:
		if(IsAllActionsDone(true))
		{
			ShuffleCells(MAXSHUFFLINGCYCLES);
			SetNewGameStage(lgsMergeLines);
		}
		break;
	}
}

void Line3Controller::AddAction(Action* action)
{
	action->retain();
	m_Actions.push_back(action);
}

bool Line3Controller::IsAllActionsDone(bool clearOnDone)
{
	//TODO:GS: optimization
	for (unsigned int i = 0; i < m_Actions.size(); i++)
	{
		if(!m_Actions[i]->isDone())
			return false;
	}
	if(clearOnDone)
	{
		ClearActions();
	}
	return true;
}

void Line3Controller::ClearActions()
{
	for (unsigned int i = 0; i < m_Actions.size(); i++)
	{
		m_Actions[i]->release();
	}
	std::vector<Action*> tmpVec;
	tmpVec.swap(m_Actions);
}

void Line3Controller::ClearMarkers(GameMarkers& gameMarkes)
{
	for (unsigned int i = 0; i < gameMarkes.size(); ++i)
		for (unsigned int j = 0; j < gameMarkes[i].size(); ++j)
		{
			gameMarkes[i][j] = 0;
		}
}

void Line3Controller::RecreateMarkers(GameMarkers& gameMarkes)
{
	{
		GameMarkers empyMarkers;
		empyMarkers.swap(gameMarkes);
	}
	gameMarkes.resize(m_GameField->GetFieldWidth());
	for (unsigned int i = 0; i < gameMarkes.size(); ++i)
	{
		gameMarkes[i].resize(m_GameField->GetFieldHeight());
	}
}

bool Line3Controller::CheckAndMark3InLine(unsigned int xmin, unsigned int ymin, unsigned int xmax, unsigned int ymax, GameMarkers& markes)
{
	bool Res = false;
	const GameField::GameFieldData& gameFieldData = m_GameField->GetFieldData();
	for (unsigned int i = xmin; i <= xmax; ++i)
	{
		for (unsigned int j = ymin; j <= ymax; ++j)
		{
			if(j <= ymax - 2)
				CHECKALINE3VERT(i, j, gameFieldData, markes, Res);
			if(i <= xmax - 2)
				CHECKALINE3HOR(i, j, gameFieldData, markes, Res);
		}
	}
	return Res;
}

bool Line3Controller::IsSequiresNewLines()
{
	const GameField::GameFieldData& gameFieldData = m_GameField->GetFieldData();
	for (unsigned int i = 0; i < gameFieldData.size(); ++i)
	{
		if(gameFieldData[i][0].m_Type == GameItem::gitFree)
			return true;
	}
	return false;
}

void Line3Controller::Megre_MergeCellsEnd()
{
	if(m_GameFieldCommands.size())
	{
		m_GameField->ApplyCommands(m_GameFieldCommands);
		m_GameField->ApplyNewXYForAllGameItems();
	}
}

bool Line3Controller::Merge_MergeCellsBegin()
{
	if(Merge_MarkGameCellsByRules())
		return true; //complete
	else
	{
		Merge_ApplyMovementDownAndAnimate();
		return false; // wait for actions
	}
}

void Line3Controller::Merge_ApplyMovementDownAndAnimate()
{
	GameField::CommandsQueue empty;
	m_GameFieldCommands.swap(empty);

	const GameField::GameFieldData& gameFieldData = m_GameField->GetFieldData();
	float waitTime;
	for (unsigned int i = 0; i < gameFieldData.size(); ++i)
	{
		//waitTime = i * WAITINGTIMEDELTAX;
		waitTime = 0.0f; //((rand() % 10) + 1) / 10;
		for (int j = (int)gameFieldData[i].size() - 1; j >= 0; --j)
		{
			Sprite* sprite = gameFieldData[i][j].m_Sprite;
			if(m_GameMarkers[i][j] == 1 && sprite)
			{
				Action* action = Spawn::create(TintTo::create(FADEOUTTIME / 3, TINTCOLOR_R, TINTCOLOR_G, TINTCOLOR_B), FadeOut::create(FADEOUTTIME / 2), nullptr);
				sprite->runAction(action);
				sprite->getParent()->reorderChild(sprite, 0);
				//sprite->setZOrder(0);
				AddAction(action);

				{
					GameFieldCommand removeItemCommand(GameFieldCommand::cRemoveGameItem, i, j);
					m_GameFieldCommands.push(removeItemCommand);
				}
			}
			if(m_GameMarkers[i][j] < 0 && sprite)
			{
				unsigned int newY = j - m_GameMarkers[i][j];
				Vec2 newPosition = m_GameField->GetXYPosition(i, newY); 

				Action* spriteAction = MoveTo::create(MOVESPRITETIME, newPosition);
				if(waitTime > 0)
					spriteAction = Sequence::create(DelayTime::create(waitTime), spriteAction, nullptr);
				sprite->runAction(spriteAction);
				sprite->getParent()->reorderChild(sprite, 1);
				AddAction(spriteAction);

				waitTime += WAITINGTIMEDELTAY;

				{
					GameFieldCommand setNewXYForItem(GameFieldCommand::cSetNewXYForGameItem, i, j);
					setNewXYForItem.m_new_field_x = i;
					setNewXYForItem.m_new_field_y = newY;
					m_GameFieldCommands.push(setNewXYForItem);
				}
			}
		}
	}
}

bool Line3Controller::Merge_MarkGameCellsByRules()
{
	ClearMarkers(m_GameMarkers);
	const GameField::GameFieldData& gameFieldData = m_GameField->GetFieldData();
	
	CheckAndMark3InLine(0, 0, m_GameMarkers.size() - 1, m_GameMarkers[0].size() - 1, m_GameMarkers); 

	bool Complete = true;
	for (unsigned int i = 0; i < m_GameMarkers.size(); ++i)
	{
		int currOffset = 0;
		for (int j = (int)m_GameMarkers[i].size() - 1; j >= 0 ; --j)
		{
			if(m_GameMarkers[i][j] == 1)
			{
				currOffset -= 1;
				Complete = false;
				continue;
			}
			m_GameMarkers[i][j] = currOffset;
		}
	}
	return Complete;
}

void Line3Controller::Generate_NewCellsAndAnimate()
{
	float fSpriteHeight = m_GameField->GetSpriteWidth();
	const GameField::GameFieldData& gameFieldData = m_GameField->GetFieldData();
	for (unsigned int i = 0; i < gameFieldData.size(); ++i)
	{
		float timeToMoveDown = MOVEDOWNTIME;
		float delayTime = 0.0;
		for (int j = (int)gameFieldData[i].size() - 1;  j >= 0; --j)
		{
			if(gameFieldData[i][j].m_Type == GameItem::gitFree)
			{
				m_GameField->RandomizeCell(i, j, false, false);

				Sprite* sprite = gameFieldData[i][j].m_Sprite;
				Vec2 firstPosition = m_GameField->GetXYPosition(i, 0); 
				firstPosition.y += fSpriteHeight;
				sprite->setPosition(firstPosition);
				sprite->setVisible(true);
				sprite->setOpacity(0);

				Action* moveDownAction = MoveTo::create(timeToMoveDown, m_GameField->GetXYPosition(i, j));
				Action* seq = Sequence::create(DelayTime::create(delayTime), FadeIn::create(FADEINTIME), moveDownAction, nullptr);
				sprite->runAction(seq);
				AddAction(seq);

				delayTime += 0.1f;
				//break; //generate one in row
			}
			timeToMoveDown *= MOVEDOWNVERTDELTADELIM;
		}
	}
}

bool Line3Controller::AnimateAndSwap(unsigned int x, unsigned int y, TouchMoveDirection dir)
{
	ClearActions();
	unsigned int swap_x = x; 
	unsigned int swap_y = y;
	switch (dir)
	{
	case Game::Line3Controller::tmdUp:
		swap_y = y - 1;
		break;
	case Game::Line3Controller::tmdDown:
		swap_y = y + 1;
		break;
	case Game::Line3Controller::tmdLeft:
		swap_x = x + 1;
		break;
	case Game::Line3Controller::tmdRight:
		swap_x = x - 1;
		break;
	default:
		break;
	}
	const GameField::GameFieldData& gameFieldData = m_GameField->GetFieldData();
	Sprite* pSprite1 = gameFieldData[x][y].m_Sprite;
	Sprite* pSprite2 = gameFieldData[swap_x][swap_y].m_Sprite;
	Vec2 fromPos = m_GameField->GetXYPosition(x, y);
	Vec2 toPos = m_GameField->GetXYPosition(swap_x, swap_y);
	m_GameField->SwapGameItems(x, y, swap_x, swap_y, false);

	FiniteTimeAction* actionFrw = MoveTo::create(SWAPTIME, toPos);
	FiniteTimeAction* actionRev = MoveTo::create(SWAPTIME, fromPos);
	FiniteTimeAction* actionDelay = DelayTime::create(0.01f);

	GameMarkers tmpMarkers;
	RecreateMarkers(tmpMarkers);
	bool bSwaped = CheckAndMark3InLine(0, 0, gameFieldData.size() - 1, gameFieldData[0].size() - 1, tmpMarkers);
	if(!bSwaped)
	{
		m_GameField->SwapGameItems(swap_x, swap_y, x, y, false); //rollback
		FiniteTimeAction* seqFrw = Sequence::create(actionFrw, actionDelay, actionRev, nullptr);
		FiniteTimeAction* seqRev = Sequence::create(actionRev, actionDelay, actionFrw, nullptr);
		actionFrw = seqFrw;
		actionRev = seqRev;
	}

	pSprite1->runAction(actionFrw);
	pSprite2->runAction(actionRev);
	AddAction(actionFrw);
	AddAction(actionRev);
	
	return bSwaped;
}

bool Line3Controller::CountUpCell(GameMarkers& markers, const GameItem::GameItemType& type, const unsigned int& x, const unsigned int& y, unsigned int& counter)
{
	const int COUNTER_Threshold = 3;
	const GameField::GameFieldData& gameFieldData = m_GameField->GetFieldData();
	bool bRes = false;
	if(gameFieldData[x][y].m_Type == type)
	{
		++counter;
	}
	else
	{
		markers[x][y] += counter;
		counter = 0;
		if(markers[x][y] >= COUNTER_Threshold)
			bRes = true; // move is possible
	}
	return bRes;
}

bool Line3Controller::GetMovingSchemeForCellType(GameMarkers& markers, GameItem::GameItemType type, bool skipCheckingWhenMovePossible)
{
	bool bRes = false;
	const GameField::GameFieldData& gameFieldData = m_GameField->GetFieldData();
	unsigned int rowSize;
	unsigned int colSize = gameFieldData.size();
	unsigned int counter;
	unsigned int counterRev;
	unsigned int revj;
	unsigned int revi;
	for (unsigned int i = 0; i < colSize; i++)
	{
		counter = counterRev = 0;
		rowSize = gameFieldData[i].size();
		for (unsigned int j = 0; j < rowSize; j++)
		{
			revj = (rowSize - 1) - j;
			if(CountUpCell(markers, type, i, j, counter))
				bRes = true;
			if(CountUpCell(markers, type, i, revj, counterRev))
				bRes = true;
		}
		if(skipCheckingWhenMovePossible && bRes)
			return true;
	}
		
	rowSize = gameFieldData[0].size();
	for (unsigned int j = 0; j < rowSize; j++)
	{
		counter = counterRev = 0;
		for (unsigned int i = 0; i < colSize; i++)
		{
			revi = (colSize - 1) - i;
			if(CountUpCell(markers, type, i, j, counter))
				bRes = true;
			if(CountUpCell(markers, type, revi, j, counterRev))
				bRes = true;
		}
		if(skipCheckingWhenMovePossible && bRes)
			return true;
	}

	return bRes;
}

bool Line3Controller::CheckPossibleMoves(bool skipCheckingWhenMovePossible /*= false*/)
{
	bool bRes = false;
	GameMarkers markers;
	RecreateMarkers(markers);

	for(unsigned int type = 1; type < GameItem::gitCount; ++type)
	{
		bRes = GetMovingSchemeForCellType(markers, static_cast<GameItem::GameItemType>(type), skipCheckingWhenMovePossible);
		if(bRes && skipCheckingWhenMovePossible)
			return true;
		ClearMarkers(markers);
	}
	return bRes;
}

void Line3Controller::ShuffleCells(unsigned int cycles)
{
	GameMarkers markers;
	RecreateMarkers(markers);

	const GameField::GameFieldData& gameFieldData = m_GameField->GetFieldData();
	unsigned int xrnd, x2rnd;
	unsigned int yrnd, y2rnd;
	for (unsigned int i = 0; i < cycles; i++)
	{
		while(true)
		{
			xrnd = rand() % gameFieldData.size();
			yrnd = rand() % gameFieldData[0].size();
			x2rnd = rand() % gameFieldData.size();
			y2rnd = rand() % gameFieldData[0].size();
			if((xrnd == x2rnd && yrnd == y2rnd) || 
			   (gameFieldData[xrnd][yrnd].m_Type == gameFieldData[x2rnd][y2rnd].m_Type) ||
			   (markers[xrnd][yrnd] == 1 || markers[x2rnd][y2rnd] == 1)
			  )
			{
				continue;
			}
			else
			{
				//CCLOG("Shuffle %d %d <-> %d %d", xrnd, yrnd, x2rnd, y2rnd);
				Vec2 fromPos = m_GameField->GetXYPosition(xrnd, yrnd);
				Vec2 toPos = m_GameField->GetXYPosition(x2rnd, y2rnd);
				FiniteTimeAction* actionFrw = MoveTo::create(FASTSWAPTIME, toPos);
				FiniteTimeAction* actionRev = MoveTo::create(FASTSWAPTIME, fromPos);
				gameFieldData[xrnd][yrnd].m_Sprite->runAction(actionFrw);
				gameFieldData[x2rnd][y2rnd].m_Sprite->runAction(actionRev);
				AddAction(actionFrw);
				AddAction(actionRev);

				markers[xrnd][yrnd] = 1;
				markers[x2rnd][y2rnd] = 1;
				m_GameField->SwapGameItems(xrnd, yrnd, x2rnd, y2rnd, false);
			}

			break;
		}
	}
}

void Line3Controller::DEBUGOUT(const GameMarkers& markers)
{
	CCLOG("GameField: ");
	if(!markers.size()) return;

	unsigned int maxY = markers[0].size();
	for (unsigned int j = 0; j < maxY; j++)
	{
		std::stringstream ssItem;
		for (unsigned int i = 0; i < markers.size(); i++)
		{
			ssItem << markers[i][j] << '\t';

		}
		std::string dbgline(ssItem.str());
		CCLOG("%s", dbgline.c_str());
	}
	CCLOG("=======");
}

/// Inputs

bool Line3Controller::onTouchBegan(Touch* touch, Event *event)
{
	if(m_GameStage != lgsWatingPlayerActivity) return false;

	m_TouchPoint = touch->getLocation();
	
	return true;
}

void Line3Controller::onTouchMove(Touch* touch, Event *event)
{
	/**/
}

void Line3Controller::onTouchEnded(Touch* touch, Event *event)
{
	if(m_GameStage != lgsWatingPlayerActivity) return;

	Vec2 TouchedPoint2 = touch->getLocation();
	if(m_TouchPoint.distance(TouchedPoint2) < TOUCHDIRTHRESHOLD)
		return;

	unsigned int first_x, first_y;
	unsigned int second_x, second_y;
	if(!m_GameField->GetFieldPosition(m_TouchPoint, first_x, first_y))
		return;
	if(!m_GameField->GetFieldPosition(TouchedPoint2, second_x, second_y))
		return;

	m_TouchDirection = tmdNone;
	int xDiff = second_x - first_x;
	int yDiff = second_y - first_y;
	if(xDiff > 0) m_TouchDirection = tmdLeft;
	else if(xDiff < 0) m_TouchDirection = tmdRight;
	if(yDiff > 0) m_TouchDirection = tmdDown;
	else if(yDiff < 0) m_TouchDirection = tmdUp;

	if(m_TouchDirection != tmdNone)
	{
		m_NeedMerge = AnimateAndSwap(first_x, first_y, m_TouchDirection);
	}

}

void Line3Controller::onTouchCancelled(Touch* touch, Event *event)
{
	/**/
}

} // namespace Game
