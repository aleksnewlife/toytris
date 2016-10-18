#pragma once

#include "GameController.h"
#include "GameField.h"

#include <vector>
#include <memory>

namespace Game
{

class Line3Controller: public GameController
{
public:
	typedef std::shared_ptr<Line3Controller> sharedptr_type;

	Line3Controller(GameField::sharedptr_type spGameField);
	virtual ~Line3Controller();

protected:
	virtual void InternalUpdate(float dt);

protected:
	enum TouchMoveDirection {tmdNone, tmdUp, tmdDown, tmdLeft, tmdRight };
	enum Line3GameStage { lgsNone, lgsNewGame, lgsWatingPlayerActivity, lgsMergeLines, lgsGenerateNewLines, lgsShuffling, lgsCOUNT };
	typedef std::vector<std::vector<int>> GameMarkers;	

	void SetNewGameStage(const Line3GameStage& newStage);

	bool CheckAndMark3InLine(unsigned int xmin, unsigned int ymin, unsigned int xmax, unsigned int ymax, GameMarkers& markes);
	bool IsSequiresNewLines();
	bool AnimateAndSwap(unsigned int x, unsigned int y, TouchMoveDirection dir);
	bool CheckPossibleMoves(bool skipCheckingWhenMovePossible = false);
	bool GetMovingSchemeForCellType(GameMarkers& markers, GameItem::GameItemType type, bool skipCheckingWhenMovePossible);
	bool CountUpCell(GameMarkers& markes, const GameItem::GameItemType& type, const unsigned int& x, const unsigned int& y, unsigned int& counter);
	void ShuffleCells(unsigned int cycles);

	bool Merge_MergeCellsBegin();
	void Megre_MergeCellsEnd();
	bool Merge_MarkGameCellsByRules();
	void Merge_ApplyMovementDownAndAnimate();
	
	void Generate_NewCellsAndAnimate();

	void AddAction(Action* action);
	bool IsAllActionsDone(bool clearOnDone);
	void ClearActions();

	void ClearMarkers(GameMarkers& gameMarkes);
	void RecreateMarkers(GameMarkers& gameMarkes);
	
	void DEBUGOUT(const GameMarkers& markers);

public:
	virtual bool onTouchBegan(Touch* touch, Event *event);
    virtual void onTouchMove(Touch* touch, Event *event);
    virtual void onTouchEnded(Touch* touch, Event *event);
    virtual void onTouchCancelled(Touch* touch, Event *event);

private:
	Line3GameStage			 m_GameStage;
	Line3GameStage			 m_OldGameStage;

	GameMarkers			     m_GameMarkers;
	GameField::CommandsQueue m_GameFieldCommands;
	std::vector<Action*>     m_Actions;
	bool					 m_NeedMerge;

	//Touch handling data
	Vec2					 m_TouchPoint;
	TouchMoveDirection		 m_TouchDirection;


};

} // namespace Game
