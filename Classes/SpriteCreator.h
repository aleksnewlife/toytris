#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <time.h>

#include <platform/CCPlatformMacros.h>

#include "Utilites.h"
#include "SpriteCreator.h"

namespace cocos2d
{
class SpriteFrameCache;
class Sprite;
}; // namespace cocos2d

namespace Game
{

USING_NS_CC;

class SpriteCreator: Utilites::NonCopyable
{	
public:
	typedef std::shared_ptr<SpriteCreator> sharedptr_type;

	SpriteCreator(const std::string& sSpritesFileName, const std::vector<std::string>& vSpritesToLoad);
	virtual ~SpriteCreator();

	unsigned int GetTypeCounter() const { return m_vBasicSprites.size(); };
	void GetFirstSpriteSize(unsigned int& width, unsigned int& height) const;

	Sprite* CreateNewSpriteByType(unsigned int uiTypeID) const;

private:
	typedef std::vector<Sprite*> VectorOfSprites;

	VectorOfSprites   m_vBasicSprites;

};


} // namespace Game
