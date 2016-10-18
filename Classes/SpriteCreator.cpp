
#include "SpriteCreator.h"

#include <2d/CCSprite.h>
#include <2d/CCSpriteFrameCache.h>

#include <algorithm>


USING_NS_CC;

namespace Game
{

const float DEFAULT_ALIVETIME_UNUSEDSPRITE = 10.0f; //sec

SpriteCreator::SpriteCreator(const std::string& sSpritesFileName, const std::vector<std::string>& vSpritesToLoad)
	: m_vBasicSprites(vSpritesToLoad.size())
{
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(sSpritesFileName);
	for (unsigned int i = 0; i < vSpritesToLoad.size(); i++)
	{
		m_vBasicSprites[i] = Sprite::createWithSpriteFrameName(vSpritesToLoad[i]);
		m_vBasicSprites[i]->retain();
	}
	
}

SpriteCreator::~SpriteCreator()
{
	for (unsigned int i = 0; i < m_vBasicSprites.size(); i++)
	{
		m_vBasicSprites[i]->release();
	}

}

void SpriteCreator::GetFirstSpriteSize(unsigned int& width, unsigned int& height) const
{
	width = height = 0;
	if(m_vBasicSprites.size())
	{
		Rect rect = m_vBasicSprites[0]->getSpriteFrame()->getRect();
		width = rect.getMaxX();
		height = rect.getMaxY();
	}
}

Sprite* SpriteCreator::CreateNewSpriteByType(unsigned int uiTypeID) const
{
	if(uiTypeID == 0 || uiTypeID > m_vBasicSprites.size()) return NULL;
	Sprite* pNewSprite = Sprite::createWithSpriteFrame(m_vBasicSprites[uiTypeID - 1]->getSpriteFrame());
	return pNewSprite;
}

} // namespace Game
