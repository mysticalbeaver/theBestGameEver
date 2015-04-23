#include "paintSprite.h"
#include "gamedata.h"
#include "frameFactory.h"
#include "frame.h"
#include "SDL/SDL_rotozoom.h"

void paintSprite::advanceFrame(Uint32 ticks) {
	timeSinceLastFrame += ticks;
	if (timeSinceLastFrame > frameInterval) {
    currentFrame = (currentFrame+1) % numberOfFrames;
		timeSinceLastFrame = 0;
	}
}

paintSprite::paintSprite( const std::string& name) :
  Drawable(name, 
           Vector2f(Gamedata::getInstance().getRandInRange(0,10*Gamedata::getInstance().getXmlInt(name+"/startLoc/x")), 
                    Gamedata::getInstance().getRandInRange(0,Gamedata::getInstance().getXmlInt(name+"/startLoc/y"))), 
           Vector2f(0,0)
           ),
  frames( FrameFactory::getInstance().getPaintFrames(name) ),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
  scale(Gamedata::getInstance().getRandInRange(0,Gamedata::getInstance().getXmlInt("startscale"))),

  currentFrame(0),
  numberOfFrames( Gamedata::getInstance().getXmlInt(name+"/frames") ),
  frameInterval( Gamedata::getInstance().getXmlInt(name+"/frameInterval") ),
  timeSinceLastFrame( 0 ),
  frameWidth(frames[0]->getWidth()),
  frameHeight(frames[0]->getHeight())
{ 
//change this in future version. Generating a random and then scaling it makes no sense
setVelocity(Vector2f(scale*Gamedata::getInstance().getRandInRange(0,Gamedata::getInstance().getXmlInt(name+"/speedX")),scale*Gamedata::getInstance().getRandInRange(0,Gamedata::getInstance().getXmlInt(name+"/speedY"))));
}

paintSprite::paintSprite(const paintSprite& s) :
  Drawable(s), 
  frames(s.frames),
  worldWidth( s.worldWidth ),
  worldHeight( s.worldHeight ),
  scale( s.scale ),
  currentFrame(s.currentFrame),
  numberOfFrames( s.numberOfFrames ),
  frameInterval( s.frameInterval ),
  timeSinceLastFrame( s.timeSinceLastFrame ),
  frameWidth( s.frameWidth ),
  frameHeight( s.frameHeight )
  { }

void paintSprite::draw() const { 
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  frames[currentFrame]->draw(x, y);
}

void paintSprite::update(Uint32 ticks) { 
  advanceFrame(ticks);

  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
  setPosition(getPosition() + incr);
   	
  if ( Y() < 0) {
    velocityY( abs( velocityY() ) );
  }
  if ( Y() > worldHeight-frameHeight) {
    velocityY( -abs( velocityY() ) );
  }

  if ( X() < 0) {
    velocityX( abs( velocityX() ) );
  }
  if ( X() > worldWidth-frameWidth) {
    velocityX( -abs( velocityX() ));
  }  

}
