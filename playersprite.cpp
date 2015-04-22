#include "playersprite.h"
#include "gamedata.h"
#include "frameFactory.h"

PlayerSprite::PlayerSprite( const std::string& name) :
  Drawable(name, 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x"), 
                    Gamedata::getInstance().getXmlInt(name+"/startLoc/y")), 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/speedX"), 
                    Gamedata::getInstance().getXmlInt(name+"/speedY"))
           ),
  frame( FrameFactory::getInstance().getFrame(name) ),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
  frameWidth(frame->getWidth()),
  frameHeight(frame->getHeight())
{ }

PlayerSprite::PlayerSprite(const PlayerSprite& s) :
  Drawable(s), 
  frame(s.frame),
  worldWidth( s.worldWidth ),
  worldHeight( s.worldHeight ),
  frameWidth( s.frameWidth ),
  frameHeight( s.frameHeight )
  { }

void PlayerSprite::draw() const { 
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  frame->draw(x, y);
}

PlayerSprite& PlayerSprite::operator=(const PlayerSprite& rhs) {
  Drawable::operator=( rhs );
  frame = rhs.frame;
  frameWidth = rhs.frameWidth;
  frameHeight = rhs.frameHeight;
  worldWidth = rhs.worldWidth;
  worldHeight = rhs.worldHeight;
  return *this;
}

void PlayerSprite::update(Uint32 ticks) { 

  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
  setPosition(getPosition() + incr);

  if (!moving)
    velocityY(0.98*velocityY());
   	
  if ( Y() < 0) {
    velocityY( 0 );
  }
  if ( Y() > worldHeight-frameHeight) {
    velocityY( 0 );
  }
    velocityX( 0 ); 
}

