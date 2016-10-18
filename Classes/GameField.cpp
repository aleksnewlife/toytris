
#include "GameField.h"
#include <2d/CCSprite.h>
#include <2d/CCLayer.h>

#include <cstdlib> //random
#include <time.h>  //time
#include <sstream>
#include <utility>

namespace Game
{

GameField::GameField(Layer* pLayer, SpriteCreator::sharedptr_type pSpritesHelper, float posX, float posY, unsigned int elWidth, unsigned int elHeight)
	: m_Layer(pLayer)
	, m_SpritesCreator(pSpritesHelper)
	, m_X(posX)
	, m_Y(posY)
{
	m_SpritesCreator->GetFirstSpriteSize(m_SpriteWidth, m_SpriteHeight);
	m_HalfSpriteWidth = m_SpriteWidth / 2;
	m_HalfSpriteHeight = m_SpriteHeight / 2;

	m_Field.resize(elWidth);
	for (unsigned int i = 0; i < m_Field.size(); i++)
	{
		m_Field[i].resize(elHeight);
	}

}

GameField::~GameField()
{
	Clear(); //TODO:GS: RAII
}

void GameField::Clear()
{
	Sprite* pSprite;
	for (unsigned int i = 0; i < m_Field.size(); i++)
		for (unsigned int j = 0; j < m_Field[i].size(); j++)
		{
			pSprite = m_Field[i][j].m_Sprite;
			if(pSprite)
			{
				RemoveSprite(pSprite);
				m_Field[i][j].m_Sprite = NULL;
			}
		}
}

bool GameField::IsVisible() const
{
	return m_Layer->isVisible();
}

void GameField::InitSpritePositions()
{
	for (unsigned int i = 0; i < m_Field.size(); i++)
		for (unsigned int j = 0; j < m_Field[i].size(); j++)
		{
			if(m_Field[i][j].m_Sprite)
				InitSpritePosition(i, j, m_Field[i][j].m_Sprite);
		}	
}

void GameField::SetVisible(bool bVisible)
{
	m_Layer->setVisible(bVisible);
}

void GameField::Update(float dt)
{
	/* Nothing */
}

void GameField::Randomize()
{
	Randomize(0, 0, m_Field.size(), m_Field[0].size());
}

void GameField::Randomize(unsigned int from_field_x, unsigned int from_field_y, unsigned int width, unsigned int height)
{
	Clear();
	srand (time(NULL));
	unsigned int someType;
	for (unsigned int i = from_field_x; i < from_field_x + width; i++)
		for (unsigned int j = from_field_y; j < from_field_y + height; j++)
		{
			someType = (rand() % m_SpritesCreator->GetTypeCounter()) + 1;
			ReplaceGameItemByNewItem(i, j, (GameItem::GameItemType)someType, true);
		}
}

void GameField::RandomizeFreeCells()
{
	srand (time(NULL));
	unsigned int someType;
	for (unsigned int i = 0; i < m_Field.size(); i++)
		for (unsigned int j = 0; j < m_Field[0].size(); j++)
			if(m_Field[i][j].m_Type == GameItem::gitFree)
			{
				someType = (rand() % m_SpritesCreator->GetTypeCounter()) + 1;
				CreateGameItem(i, j, (GameItem::GameItemType)someType, true);
			}
}

void GameField::GenerateUniqField()
{
	Clear();
	srand(time(NULL));
	const unsigned int maxType = m_SpritesCreator->GetTypeCounter();
	std::vector<std::pair<bool, GameItem::GameItemType> > vTypes(maxType);
	for (unsigned int i = 0; i < vTypes.size(); i++)
	{
		vTypes[i].first = false;
		vTypes[i].second = static_cast<GameItem::GameItemType>(i + 1);
	}


	auto randomLambda = [] (unsigned int max) { return (rand() % max); };

	std::vector<GameItem::GameItemType> selTypes(maxType);
	unsigned int selTypeID = 0;
	volatile unsigned int i = randomLambda(maxType);
	while(selTypeID < maxType)
	{
		if(!vTypes[i].first)
		{
			vTypes[i].first = true;
			selTypes[selTypeID] = vTypes[i].second;
			++selTypeID;
			i = randomLambda(maxType);
		}
		else
		{
			++i;
			i = (i < maxType) ? i : i % maxType;
		}
	}

	unsigned int typePairID = 0;
	unsigned int typeID;
	unsigned int typeIDModifier = 0;
	for (unsigned int i = 0; i < m_Field.size(); i++)
	{
		for (unsigned int j = 0; j < m_Field[0].size(); j++)
		{
			typeIDModifier = (j + 2) % 2;
			typeID = selTypes[typePairID * 2 + typeIDModifier];
			CreateGameItem(i, j, static_cast<GameItem::GameItemType>(typeID));
		}
		++typePairID;
		typePairID = ((typePairID + 2) % 2);
	}
}

void GameField::InitSpritePosition(unsigned int field_x, unsigned int field_y, Sprite* sprite)
{
	if(!sprite) return;
	const Vec2& newPos = GetXYPosition(field_x, field_y);
	sprite->setPosition(newPos);
}

void GameField::AddSprite(Sprite* sprite)
{
	sprite->retain();
	m_Layer->addChild(sprite);
}

void GameField::RemoveSprite(Sprite* sprite)
{
	sprite->release();
	m_Layer->removeChild(sprite);
}

void GameField::CreateGameItem(unsigned int field_x, unsigned int field_y, GameItem::GameItemType type, bool initSpritePos /*= true*/, bool visible /*= true*/)
{
	if(m_Field[field_x][field_y].m_Sprite) return;

	Sprite* pSprite = m_SpritesCreator->CreateNewSpriteByType((unsigned int) type);
	pSprite->setVisible(visible);
	AddSprite(pSprite);
	m_Field[field_x][field_y] = GameItem(field_x, field_y, type, pSprite);

	if(initSpritePos)
		InitSpritePosition(field_x, field_y, pSprite);

}

void GameField::ReplaceGameItemByNewItem(unsigned int field_x, unsigned int field_y, GameItem::GameItemType type, bool initSpritePos /*= true*/, bool visible /*= true*/)
{
	if(m_Field[field_x][field_y].m_Sprite)
		RemoveSprite(m_Field[field_x][field_y].m_Sprite);

	Sprite* pSprite = m_SpritesCreator->CreateNewSpriteByType((unsigned int) type);
	pSprite->setVisible(visible);
	AddSprite(pSprite);
	m_Field[field_x][field_y] = GameItem(field_x, field_y, type, pSprite);

	if(initSpritePos)
		InitSpritePosition(field_x, field_y, pSprite);

}

void GameField::ReplaceGameItemByItem(unsigned int field_x, unsigned int field_y, unsigned int old_field_x, unsigned int old_field_y, bool initSpritePos /*= true*/)
{
	if(m_Field[field_x][field_y].m_Sprite)
		RemoveSprite(m_Field[field_x][field_y].m_Sprite);

	GameItem& oldItem = m_Field[old_field_x][old_field_y];
	m_Field[field_x][field_y] = GameItem(field_x, field_y, oldItem.m_Type, oldItem.m_Sprite);
	

	if(initSpritePos)
		InitSpritePosition(field_x, field_y, oldItem.m_Sprite);

	oldItem = GameItem(old_field_x, old_field_y, GameItem::gitFree, NULL); // clear old item
}

void GameField::RemoveGameItem(unsigned int field_x, unsigned int field_y)
{
	if(m_Field[field_x][field_y].m_Sprite)
		RemoveSprite(m_Field[field_x][field_y].m_Sprite);
	m_Field[field_x][field_y] = GameItem(field_x, field_y, GameItem::gitFree, NULL);
}

void GameField::SetNewXYForGameItem(unsigned int field_x, unsigned int field_y, unsigned int new_field_x, unsigned int new_field_y)
{
	m_Field[field_x][field_y].m_newX = new_field_x;
	m_Field[field_x][field_y].m_newY = new_field_y;
	m_Field[field_x][field_y].m_ReadyToMove = true;
}

void GameField::ApplyNewXYForAllGameItems()
{
	for (unsigned int i = 0; i < m_Field.size(); ++i)
		for (int j = (int)m_Field[i].size() - 1; j >= 0; --j) // reverse sequence
		{
			if(m_Field[i][j].m_Type != GameItem::gitFree && m_Field[i][j].m_ReadyToMove)
			{
				ReplaceGameItemByItem(m_Field[i][j].m_newX, m_Field[i][j].m_newY, m_Field[i][j].m_X, m_Field[i][j].m_Y);
			}
		}
}

void GameField::RandomizeCell(unsigned int field_x, unsigned int field_y, bool initSpritePos /*= true*/, bool visible /*= true*/)
{
	unsigned int someType = (rand() % m_SpritesCreator->GetTypeCounter()) + 1;
	CreateGameItem(field_x, field_y, (GameItem::GameItemType)someType, initSpritePos, visible);
}

void GameField::SwapGameItems(unsigned int field_x, unsigned int field_y, unsigned int field_x2, unsigned int field_y2, bool initSpritePos /*= true*/)
{
	GameItem& fromGI = m_Field[field_x][field_y];
	GameItem& toGI = m_Field[field_x2][field_y2];
	std::swap(fromGI.m_Sprite, toGI.m_Sprite);
	std::swap(fromGI.m_Type, toGI.m_Type);

	if(initSpritePos)
	{
		InitSpritePosition(field_x, field_y, fromGI.m_Sprite);
		InitSpritePosition(field_x2, field_y2, toGI.m_Sprite);
	}
}

void GameField::ApplyCommands(CommandsQueue& queue)
{
	while(queue.size())
	{
		const GameFieldCommand& commandItem = queue.front();
		switch(commandItem.m_Command)
		{
		case GameFieldCommand::cCreateGameItem:
			CreateGameItem(commandItem.m_field_x, commandItem.m_field_y, commandItem.m_type, commandItem.m_initSpritePos);
			break;
		case GameFieldCommand::cReplaceGameItemByNewItem:
			ReplaceGameItemByNewItem(commandItem.m_field_x, commandItem.m_field_y, commandItem.m_type, commandItem.m_initSpritePos);
			break;
		case GameFieldCommand::cReplaceGameItemByItem:
			ReplaceGameItemByItem(commandItem.m_field_x, commandItem.m_field_y, commandItem.m_old_field_x, commandItem.m_old_field_y, commandItem.m_initSpritePos);
			break;
		case GameFieldCommand::cRemoveGameItem:
			RemoveGameItem(commandItem.m_field_x, commandItem.m_field_y);
			break;
		case GameFieldCommand::cSetNewXYForGameItem:
			SetNewXYForGameItem(commandItem.m_field_x, commandItem.m_field_y, commandItem.m_new_field_x, commandItem.m_new_field_y);
			break;
		case GameFieldCommand::cApplyNewXYForAllGameItems:
			ApplyNewXYForAllGameItems();
			break;
		case GameFieldCommand::cRandomizeCell:
			RandomizeCell(commandItem.m_field_x, commandItem.m_field_y);
			break;
		case GameFieldCommand::cSwapGameItems:
			SwapGameItems(commandItem.m_field_x, commandItem.m_field_y, commandItem.m_new_field_x, commandItem.m_new_field_y, commandItem.m_initSpritePos);
		}
		queue.pop();
	}
}

void GameField::DEBUGOUT()
{
	CCLOG("GameField: ");
	if(!m_Field.size()) return;
	unsigned int maxY = m_Field[0].size();
	for (unsigned int j = 0; j < maxY; j++)
	{
		std::stringstream ssItem;
		for (unsigned int i = 0; i < m_Field.size(); i++)
		{
			ssItem << m_Field[i][j].m_X << ":" << m_Field[i][j].m_Y << "  " 
				   << m_Field[i][j].m_newX << ":" << m_Field[i][j].m_newY << "  "
				   << m_Field[i][j].m_Type << " | ";

		}
		std::string dbgline(ssItem.str());
		//CCLOG("%s", dbgline.c_str());
		CCLOG("%s", dbgline.c_str());
	}
	CCLOG("=======");
}

} // namespace Game
