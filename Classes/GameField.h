#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <math/Vec2.h>
#include "Utilites.h"
#include "SpriteCreator.h"

namespace cocos2d
{
class Layer;
class Sprite;
}

namespace Game
{

struct GameItem
{
	enum GameItemType {gitFree, gitRed, gitBlue, gitGreen, gitYelow, gitCount };

	GameItem()
		: m_X(0)
		, m_Y(0)
		, m_newX(0)
		, m_newY(0)
		, m_ReadyToMove(false)
		, m_Type(gitFree)
		, m_Sprite(NULL)
	{ /**/ }

	GameItem(unsigned int x, unsigned int y, GameItemType type, Sprite* sprite)
		: m_X(x)
		, m_Y(y)
		, m_newX(x)
		, m_newY(y)
		, m_ReadyToMove(false)
		, m_Type(type)
		, m_Sprite(sprite)
	{ /**/ }
	
	unsigned int m_X;
	unsigned int m_Y;
	unsigned int m_newX;
	unsigned int m_newY;
	bool		 m_ReadyToMove;
	GameItemType m_Type;
	Sprite* m_Sprite;
};

struct GameFieldCommand
{
	enum Command {  cCreateGameItem, 
					cReplaceGameItemByNewItem, 
					cReplaceGameItemByItem, 
					cRemoveGameItem, 
					cSetNewXYForGameItem, 
					cApplyNewXYForAllGameItems,
					cRandomizeCell,
					cSwapGameItems
	};

	explicit GameFieldCommand(Command command)
		: m_Command(command)
	{ /**/ };

	GameFieldCommand(Command command, unsigned int x, unsigned int y, bool initSpritePos = true)
		: m_Command(command)
		, m_field_x(x)
		, m_field_y(y)
		, m_initSpritePos(initSpritePos)
	{ /**/ };

	Command m_Command;
	//Params
	unsigned int m_field_x;
	unsigned int m_field_y;
	// Additional params
	union {
		unsigned int		   m_old_field_x;
		unsigned int		   m_new_field_x;
		GameItem::GameItemType m_type;
	};
	union {
		unsigned int m_old_field_y;
		unsigned int m_new_field_y;
	};

	bool m_initSpritePos;

private:
	GameFieldCommand() { /**/ };

};

class GameField: Utilites::NonCopyable
{
public:
	typedef std::shared_ptr<GameField> sharedptr_type;
	typedef std::vector< std::vector<GameItem> > GameFieldData;
	typedef std::queue<GameFieldCommand> CommandsQueue;

public:

	GameField(Layer* pLayer, SpriteCreator::sharedptr_type pSpritesHelper, float posX, float posY, unsigned int elWidth, unsigned int elHeight);
	virtual ~GameField();

	void InitSpritePositions();

	inline const GameFieldData& GetFieldData() const { return m_Field; }

	const unsigned int GetFieldWidth() const { return m_Field.size(); };
	const unsigned int GetFieldHeight() const { if(m_Field.size()) return m_Field[0].size(); else return 0;};

	void Clear();
	void Randomize();
	void Randomize(unsigned int from_field_x, unsigned int from_field_y, unsigned int width, unsigned int height);
	void RandomizeFreeCells();
	void GenerateUniqField();

	bool IsVisible() const;
	void SetVisible(bool bVisible);
	void Update(float dt);

	void CreateGameItem(unsigned int field_x, unsigned int field_y, GameItem::GameItemType type, bool initSpritePos = true, bool visible = true);
	void ReplaceGameItemByNewItem(unsigned int field_x, unsigned int field_y, GameItem::GameItemType type, bool initSpritePos = true, bool visible = true);
	void ReplaceGameItemByItem(unsigned int field_x, unsigned int field_y, unsigned int old_field_x, unsigned int old_field_y, bool initSpritePos = true);
	void RemoveGameItem(unsigned int field_x, unsigned int field_y);
	void SetNewXYForGameItem(unsigned int field_x, unsigned int field_y, unsigned int new_field_x, unsigned int new_field_y);
	void ApplyNewXYForAllGameItems();
	void RandomizeCell(unsigned int field_x, unsigned int field_y, bool initSpritePos = true, bool visible = true);
	void SwapGameItems(unsigned int field_x, unsigned int field_y, unsigned int field_x2, unsigned int field_y2, bool initSpritePos = true);

	void ApplyCommands(CommandsQueue& queue);

	inline Vec2 GetXYPosition(unsigned int field_x, unsigned int field_y) const
	{ 
		return Vec2(m_X + m_HalfSpriteWidth + field_x * m_SpriteWidth , m_Y + m_HalfSpriteHeight - field_y * m_SpriteHeight ); 
	};

	inline bool GetFieldPosition(Vec2 pos, unsigned int& x, unsigned int& y) const
	{
		x = static_cast<unsigned int>((pos.x - m_X) / m_SpriteWidth);
		y = static_cast<unsigned int>((m_Y - pos.y + m_SpriteHeight) / m_SpriteHeight);
		return (x >= 0 && x < m_Field.size()) && (y >= 0 && y < m_Field[0].size());
	}

	inline float GetSpriteWidth() const { return m_SpriteWidth; }
	inline float GetSpriteHeight() const { return m_SpriteHeight; }

	void DEBUGOUT();

protected:
	void InitSpritePosition(unsigned int field_x, unsigned int field_y, Sprite* sprite);
	void AddSprite(Sprite* sprite);
	void RemoveSprite(Sprite* sprite);
	
private:
	float							m_X;
	float							m_Y;
	unsigned int					m_SpriteWidth;
	unsigned int					m_HalfSpriteWidth;
	unsigned int					m_SpriteHeight;
	unsigned int					m_HalfSpriteHeight;
	Layer*							m_Layer;
	SpriteCreator::sharedptr_type   m_SpritesCreator;
	GameFieldData					m_Field;

};


} // namespace Game
