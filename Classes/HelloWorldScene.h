#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include <cocos2d.h>
#include "GameField.h"
#include "SpriteCreator.h"
#include "Line3Controller.h"

namespace cocos2d
{
	class Touch;
	class Event;
}

USING_NS_CC;

class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

protected:
	void Updtate(float dt);

	bool onTouchBegan(Touch* touch, Event *event);
    void onTouchMove(Touch* touch, Event *event);
    void onTouchEnded(Touch* touch, Event *event);
    void onTouchCancelled(Touch* touch, Event *event);

private:
	Game::SpriteCreator::sharedptr_type		m_SpritesCreator;
	Game::GameField::sharedptr_type			m_GameField;
	Game::Line3Controller::sharedptr_type   m_GameLine3Controller;
};

#endif // __HELLOWORLD_SCENE_H__
