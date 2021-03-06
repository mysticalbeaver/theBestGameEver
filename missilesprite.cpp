#include <cmath>
#include "missilesprite.h"
#include "gamedata.h"
#include "frameFactory.h"

void MissileSprite::advanceFrame(Uint32 ticks) {
	timeSinceLastFrame += ticks;
	if (timeSinceLastFrame > frameInterval) {
    currentFrame = (currentFrame+1) % numberOfFrames;
		timeSinceLastFrame = 0;
	}
}

MissileSprite::MissileSprite( const std::string& name, float startx, float starty, int power) :
  Drawable(name, 
//get 90 and 30 from xml as gun barrel offset
           Vector2f(startx+Gamedata::getInstance().getXmlInt(name+"/barreloffsetx"),
		starty+Gamedata::getInstance().getXmlInt(name+"/barreloffsety")), 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/speedX")*power, 0)
           ),
  frames( FrameFactory::getInstance().getFrames(name) ),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),

  currentFrame(0),
  numberOfFrames( Gamedata::getInstance().getXmlInt(name+"/frames") ),
  frameInterval( Gamedata::getInstance().getXmlInt(name+"/frameInterval") ),
  timeSinceLastFrame( 0 ),
  frameWidth(frames[0]->getWidth()),
  frameHeight(frames[0]->getHeight()),
  time(0),
  strategies(),
  strategy( NULL )
{ strategies.push_back( new MidPointCollisionStrategy );
  strategies.push_back( new RectangularCollisionStrategy );
  strategies.push_back( new PerPixelCollisionStrategy );
  strategy = strategies[2];
}

void MissileSprite::draw() const { 
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  frames[currentFrame]->draw(x, y);
}

float MissileSprite::distance(float x1, float y1, float x2, float y2) const {
  float x = x1-x2, y = y1-y2;
  return hypot(x, y);
}

void MissileSprite::update(Uint32 ticks) { 
  time += ticks;
  advanceFrame(ticks);

//no acceleration in x direction so d=v*t
  float x = getVelocity()[0] * static_cast<float>(ticks) * 0.001;
//v0 in y dir is 0 so we only need quadratic term
  float y = time*time*0.00001;

  Vector2f incr = Vector2f(x,y);
  setPosition(getPosition() + incr);
   	
  if ( Y()  > worldHeight) {
    velocityX( 0 );
  }  
}
