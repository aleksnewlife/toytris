#include "HelloWorldScene.h"
#include <cocostudio/CocoStudio.h>
#include <ui/CocosGUI.h>
#include <base/ccMacros.h>
#include <base/CCTouch.h>
#include <base/CCEvent.h>
#include <base/CCTouch.h>
#include <base/CCEvent.h>

#include "SpriteCreator.h"
#include "GameField.h"

#include <memory>
#include <vector>

USING_NS_CC;

using namespace cocostudio::timeline;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("MainScene.csb");

    addChild(rootNode);

	{	//Init Logic

		const unsigned int MAXBOXES = 4;
		const char* BoxesPrites[] = { "Blue_al.png", "Green_al.png", "Red_al.png", "Yelow_al.png" }; //with alpha

		std::vector<std::string> SpriteNames(BoxesPrites, std::end(BoxesPrites));

		m_SpritesCreator.reset(new Game::SpriteCreator("res/Boxes_al.plist", SpriteNames));
		unsigned int spwidth, spheight;
		m_SpritesCreator->GetFirstSpriteSize(spwidth, spheight);
		m_GameField.reset( new Game::GameField(this, m_SpritesCreator, spwidth, (spheight * 10), 15, 10));
		m_GameLine3Controller.reset(new Game::Line3Controller(m_GameField));

		schedule(schedule_selector(HelloWorld::Updtate), 0.1f);

		m_GameLine3Controller->Update(0.0f); //First Update

	}

	{
		EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);
		listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
		listener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
		listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
		listener->onTouchCancelled = CC_CALLBACK_2(HelloWorld::onTouchCancelled, this);
		this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
	}

    return true;
}

void HelloWorld::Updtate(float dt)
{
	m_GameLine3Controller->Update(dt);
	m_GameField->Update(dt);
}

bool HelloWorld::onTouchBegan(Touch *touch, Event *event)
{
	m_GameLine3Controller->onTouchBegan(touch, event);
	return true;
}

void HelloWorld::onTouchMove(Touch *touch, Event *event)
{
	m_GameLine3Controller->onTouchMove(touch, event);
}

void HelloWorld::onTouchEnded(Touch *touch, Event *event)
{
	m_GameLine3Controller->onTouchEnded(touch, event);
}

void HelloWorld::onTouchCancelled(Touch *touch, Event *event)
{
	m_GameLine3Controller->onTouchCancelled(touch, event);
}
