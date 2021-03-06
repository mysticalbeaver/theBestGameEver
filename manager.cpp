#include <iostream>
#include <string>
#include <iomanip>
#include "twowaymultisprite.h"
#include "playersprite.h"
#include "missilesprite.h"
#include "explodingSprite.h"
#include "aaline.h"
#include "sprite.h"
#include "gamedata.h"
#include "manager.h"
#include "sound.h"

Manager::~Manager() { 
  for (unsigned i = 0; i < sprites.size(); ++i) {
    delete sprites[i];
  }
for (unsigned i = 0; i < crabz.size(); ++i) {
    delete crabz[i];
  }
for (unsigned i = 0; i < explodedCrabs.size(); ++i) {
    delete explodedCrabs[i];
  }
for (unsigned i = 0; i < explodedBirds.size(); ++i) {
    delete explodedBirds[i];
  }
  for (unsigned i = 0; i < depthMakers.size(); ++i) {
    delete depthMakers[i];
  }
}
Manager::Manager() :
  env( SDL_putenv(const_cast<char*>("SDL_VIDEO_CENTERED=center")) ),
  io( IOManager::getInstance() ),
  clock( Clock::getInstance() ),
  screen( io.getScreen() ),
  back("back", Gamedata::getInstance().getXmlInt("back/factor") ),
  middle("middle", Gamedata::getInstance().getXmlInt("middle/factor") ),
  front("front", Gamedata::getInstance().getXmlInt("front/factor") ),
  viewport( Viewport::getInstance() ),
  sprites(),
  crabz(),
  depthMakers(),
  explodedBirds(),
  explodedCrabs(),
  currentSprite(0),

  makeVideo( false ),
  frameCount( 0 ),
  username(  Gamedata::getInstance().getXmlStr("username") ),
  title( Gamedata::getInstance().getXmlStr("screenTitle") ),
  frameMax( Gamedata::getInstance().getXmlInt("frameMax") ),
  scoreThatYouHaveReceivedBasedOnTheNumberOfSpritesThatHaveExploded(0),
  isHoldingDownTheSpaceBarKey(0),
  godMode(false)
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    throw string("Unable to initialize SDL: ");
  }
  SDL_WM_SetCaption(title.c_str(), NULL);
  atexit(SDL_Quit);
  unsigned int n = Gamedata::getInstance().getXmlInt("numberofarticunos");
  unsigned int u = Gamedata::getInstance().getXmlInt("numberofcrabsthatareinthegame");

  sprites.reserve(2);
  depthMakers.reserve(n);
  crabz.reserve(u);
  
  sprites.push_back( new PlayerSprite("cloudgun") );

  for(unsigned int i = 0; i < u; ++i) {
    crabz.push_back( new MultiSprite("crab"));
  }
  
//make sure you put the paintsprites on last
  for (unsigned int i = 0; i < n; ++i) {
    depthMakers.push_back(new paintSprite("Articuno"));
  }

//scale the velocity of the paintsprites by the size
  unsigned int count = 0;
  std::vector<Drawable*>::iterator ptr = depthMakers.begin();
  while (ptr != depthMakers.end()) {
    count++;
    if (count < n/3)
      (*ptr)->setVelocity(Vector2f(Gamedata::getInstance().getXmlFloat
      ((*ptr)->getName()+"/scale1")*Gamedata::getInstance().getXmlInt
      ((*ptr)->getName()+"/speedX"),
      Gamedata::getInstance().getXmlFloat
      ((*ptr)->getName()+"/scale1")*Gamedata::getInstance().getXmlInt
      ((*ptr)->getName()+"/speedY")));
    else if(count < 2*n/3)
      (*ptr)->setVelocity(Vector2f(Gamedata::getInstance().getXmlFloat
      ((*ptr)->getName()+"/scale2")*Gamedata::getInstance().getXmlInt
      ((*ptr)->getName()+"/speedX"),
      Gamedata::getInstance().getXmlFloat
      ((*ptr)->getName()+"/scale2")*Gamedata::getInstance().getXmlInt
      ((*ptr)->getName()+"/speedY")));
    else if(count <= n)
      (*ptr)->setVelocity(Vector2f(Gamedata::getInstance().getXmlFloat
      ((*ptr)->getName()+"/scale3")*Gamedata::getInstance().getXmlInt
      ((*ptr)->getName()+"/speedX"),
      Gamedata::getInstance().getXmlFloat
      ((*ptr)->getName()+"/scale3")*Gamedata::getInstance().getXmlInt
      ((*ptr)->getName()+"/speedY")));
    ptr++;
  }      
  viewport.setObjectToTrack(sprites[0]);
}

void Manager::draw() const {

  back.draw();
  for (unsigned i = 0; i < depthMakers.size()/3; ++i) {
    depthMakers[i]->draw();
  }
  middle.draw();
  for (unsigned i = depthMakers.size()/3; i < 2*depthMakers.size()/3; ++i) {
    depthMakers[i]->draw();
  }
  front.draw();
  for (unsigned i = 2*depthMakers.size()/3; i < depthMakers.size(); ++i) {
    depthMakers[i]->draw();
  }	
  for (unsigned int i = 0; i < crabz.size(); ++i) {
    crabz[i]->draw();
  }
  for (unsigned int i = 0; i < sprites.size() ; ++i) {
    sprites[i]->draw();
  }

  io.printMessageAt(title, 10, 567);
  viewport.draw();
}

void Manager::makeFrame() {
  std::stringstream strm;
  strm << "frames/" << username<< '.' 
       << std::setfill('0') << std::setw(4) 
       << frameCount++ << ".bmp";
  std::string filename( strm.str() );
  std::cout << "Making frame: " << filename << std::endl;
  SDL_SaveBMP(screen, filename.c_str());
}

void Manager::drawHUD(SDL_Surface* screen, int x, int y) {
  int hudHeight = Gamedata::getInstance().getXmlInt("hudHeight");
  int hudWidth = Gamedata::getInstance().getXmlInt("hudWidth");

  const Uint32 BLUE = SDL_MapRGB(screen->format, 0, 0, 0xff);
  Draw_AALine(screen, x, y+hudHeight/2, 
                      x+hudWidth,y+hudHeight/2, 
                      hudHeight, 0xff, 0xff, 0xff, 0xff/2);
  Draw_AALine(screen, x,y, x+hudWidth,y, BLUE);
  Draw_AALine(screen, x,y+hudHeight, x+hudWidth,y+hudHeight, BLUE);
  io.printMessageAt(Gamedata::getInstance().getXmlStr("hudWelcomeMsg/text"), 
						  Gamedata::getInstance().getXmlInt("hudWelcomeMsg/xLoc"), 
						  Gamedata::getInstance().getXmlInt("hudWelcomeMsg/yLoc"));
  
  // will print the fps at 22, 83 and the elapsed seconds at 22, 103
  clock.draw();
 
  io.printMessageAt(Gamedata::getInstance().getXmlStr("hudMoveCannonMsg/text"), 
						  Gamedata::getInstance().getXmlInt("hudMoveCannonMsg/xLoc"), 
						  Gamedata::getInstance().getXmlInt("hudMoveCannonMsg/yLoc"));
  io.printMessageAt(Gamedata::getInstance().getXmlStr("hudMoveUpMsg/text"), 
						  Gamedata::getInstance().getXmlInt("hudMoveUpMsg/xLoc"), 
						  Gamedata::getInstance().getXmlInt("hudMoveUpMsg/yLoc"));
  io.printMessageAt(Gamedata::getInstance().getXmlStr("hudMoveDownMsg/text"), 
						  Gamedata::getInstance().getXmlInt("hudMoveDownMsg/xLoc"), 
						  Gamedata::getInstance().getXmlInt("hudMoveDownMsg/yLoc"));
  io.printMessageAt(Gamedata::getInstance().getXmlStr("hudPowerBarMsg/text"), 
						  Gamedata::getInstance().getXmlInt("hudPowerBarMsg/xLoc"), 
						  Gamedata::getInstance().getXmlInt("hudPowerBarMsg/yLoc"));
  io.printMessageAt(Gamedata::getInstance().getXmlStr("hudPowerBarMsg/text2"), 
						  Gamedata::getInstance().getXmlInt("hudPowerBarMsg/xLoc2"), 
						  Gamedata::getInstance().getXmlInt("hudPowerBarMsg/yLoc2"));
}

void Manager::drawScoreHUD() {
  std::string scoreString;
  std::stringstream convert;
  convert << scoreThatYouHaveReceivedBasedOnTheNumberOfSpritesThatHaveExploded;
  scoreString = convert.str();
	
  std::string scoreMessage = Gamedata::getInstance().getXmlStr("score/text") + " " + scoreString;
  io.printMessageCenteredAt(scoreMessage, Gamedata::getInstance().getXmlInt("score/yLoc"));
}

void Manager::hitCrabMessage() {
   io.printMessageCenteredAtBigger(Gamedata::getInstance().getXmlStr("hitCrab/text"), 
										Gamedata::getInstance().getXmlInt("hitCrab/yLoc"));
}

void Manager::resetMessage() {
   io.printMessageCenteredAtBigger(Gamedata::getInstance().getXmlStr("reset/text"), 
										Gamedata::getInstance().getXmlInt("reset/yLoc"));
}

void Manager::timeResetMessage() {
   io.printMessageCenteredAtBigger("Replay",Gamedata::getInstance().getXmlInt("reset/yLoc"));
}

void Manager::update() {
  clock.update();
  Uint32 ticks = clock.getTicksSinceLastFrame();
  for (int i = sprites.size()-1; i >= 0; --i) {
    sprites[i]->update(ticks);
  }
  for (unsigned i = 0; i < crabz.size(); ++i) {
    crabz[i]->update(ticks);
  }
  for (unsigned i = 0; i < depthMakers.size(); ++i) {
    depthMakers[i]->update(ticks);
  }
  if ( makeVideo && frameCount < frameMax ) {
    makeFrame();
  }
  back.update();
  middle.update();
  front.update();
  viewport.update(); // always update viewport last
}

void Manager::reset() {
  sprites[0]->stopMove();
  isHoldingDownTheSpaceBarKey = 0;
  scoreThatYouHaveReceivedBasedOnTheNumberOfSpritesThatHaveExploded = 0;

//restoring the crabs by replacing each explodedsprite in the vector
//with a crab from the object pool
  std::vector<Drawable*>::iterator pos = crabz.begin();
  std::vector<Drawable*>::iterator ec = explodedCrabs.begin();
  while( pos != crabz.end() ) {
    if (dynamic_cast<ExplodingSprite*>(*pos)) {
      ExplodingSprite* temp = static_cast<ExplodingSprite*>(*pos);
      (*pos) = (*ec);
      delete temp;
      ++ec;			
      ++pos;
    } 
    else 
      ++pos;
  }	

//restore the birds in the same way
  std::vector<Drawable*>::iterator sprite = depthMakers.begin() + ((2*depthMakers.size())/3);
  std::vector<Drawable*>::iterator eb = explodedBirds.begin();
  while(sprite != depthMakers.end() ) {
    if(dynamic_cast<ExplodingSprite*>(*sprite)) {
      ExplodingSprite* temp = static_cast<ExplodingSprite*>(*sprite);
      (*sprite) = (*eb);
      delete temp;
      ++eb;	
      ++sprite;
    }
   else 
      ++sprite;

  }
  clock.reset(); 
}

void Manager::play() {
  SDL_Event event;
  bool done = false;
  clock.start();
 
  isHoldingDownTheSpaceBarKey = 0;

  int counter = 0;
  int startX = Gamedata::getInstance().getXmlInt("hudStartX");
  int startY = Gamedata::getInstance().getXmlInt("hudStartY");

  int hitCrabTime = 50;
  int resetTime = 50;
  int timeResetTime = 50;
  int LetUsTakeSomeTimeToLookAtThePrettyExplosion = 0;

  Health bar;
  SDLSound sound;

  int timeSinceMissile = 0;
  bool shot = false;

  while ( not done ) {
//auto reset after 1 minute of play
    if (clock.getSeconds() > 60) {
//in case you are powering up while it resets
      if(bar.getLen() != 0) {
	bar.powerUp();
	bar.reset();
      }
      reset();
      timeResetTime = 0;
    }
    while ( SDL_PollEvent(&event) ) {
      if (event.type ==  SDL_QUIT ) { 
	done = true; 
	break;
      }
      if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_p) {
        if(clock.isPaused() )
	  clock.unpause();
	else
	  clock.pause();
//in case you pause while powering up
	if(bar.getLen() != 0) {
	  bar.powerUp();
	  bar.reset();
	}
      }
      if( event.type == SDL_KEYUP && not clock.isPaused() ) {
    	switch( event.key.keysym.sym ){
          case SDLK_s:
          case SDLK_w:
	    sprites[0]->stopMove();
            break;                    				                       		
          case SDLK_SPACE:
            if(isHoldingDownTheSpaceBarKey) {
	      bar.powerUp();
	      isHoldingDownTheSpaceBarKey = 0;
              sprites.push_back(new MissileSprite("bomb",sprites[0]->X(),sprites[0]->Y(),bar.getLen()));
	      bar.reset();
	      shot = true;
	      viewport.setObjectToTrack(sprites[sprites.size()-1]);
	      timeSinceMissile = 0;
	      sound[0];
	    }	
	    break;
	  default:
	    break;		
	}	
      }
      if(event.type == SDL_KEYDOWN && not clock.isPaused() ) {
        switch( event.key.keysym.sym ){
          case SDLK_g:
	    godMode = !godMode;
	    break;
          case SDLK_ESCAPE:
          case SDLK_q: 
            done = true;
            break;
	  case SDLK_r:
//in case you manually reset while powering up
	    if(bar.getLen() != 0) {
		bar.powerUp();
		bar.reset();
	    }
	    reset();
	    resetTime = 0;
	    counter = 0;
	    break;
          case SDLK_F3:
            clock.toggleSloMo();
            break;
          case SDLK_w:
//dont let it move and then power up it cause weird things
//but powering up and then moving is fine
	    if (!isHoldingDownTheSpaceBarKey)
	      sprites[0]->up();
	    break;
	  case SDLK_s:
	    if(!isHoldingDownTheSpaceBarKey)
	      sprites[0]->down();
	    break;
          case SDLK_F4:
            if(!makeVideo) {
              std::cout << "Making video frames" << std::endl;
              makeVideo = true;
            }
            break;
	  case SDLK_F1:
	    counter = 100;
	    break;
	  case SDLK_SPACE:
	    if (!shot) {
	      bar.powerUp();
	      isHoldingDownTheSpaceBarKey = 1;
	    }
	    break;
	  default:
	    break;
        }
      }
    } 
    draw(); 
// draw the HUD for the first 3 seconds
    if(counter < 300) {
      drawHUD(screen, startX, startY);
    } 
    drawScoreHUD();
    counter++;
    bar.draw(); 
	
    update();
    bar.update(clock.getTicksSinceLastFrame());
    timeSinceMissile += clock.getTicksSinceLastFrame();

    if (LetUsTakeSomeTimeToLookAtThePrettyExplosion > 0)
    LetUsTakeSomeTimeToLookAtThePrettyExplosion++;
      if(!godMode) {
//only do collision detection with a missile in the air
	if (shot == true) {
//only look at the paintsprites in the foreground
	  std::vector<Drawable*>::iterator sprite = depthMakers.begin() + ((2*depthMakers.size())/3);
	  while ( sprite != depthMakers.end() ) {
//dont do collision detections on already exploded birds
	    if (not dynamic_cast<ExplodingSprite*>(*sprite)) {
	      if ( static_cast<MissileSprite*>(sprites[sprites.size()-1])->collidedWith(*sprite) ) {
		paintSprite* temp = static_cast<paintSprite*>(*sprite);
		Sprite weGonnaBlowThisOneUp((*sprite)->getName(),(*sprite)->getPosition(),(*sprite)->getVelocity());
//making a sprite out of a paintsprite loses the scaling so the explosion will start with a frame bigger than the sprite it is destroying
//because the size of the paintsprite is scaled down from its original
		(*sprite) = new ExplodingSprite(const_cast<Sprite&>(weGonnaBlowThisOneUp));
		static_cast<ExplodingSprite*>(*sprite)->setExplosionStrategy(0);
//object pool only keeps the birds. the explosions are not kept because they must be constructed with parameters from the birds that
//are not constant	
                explodedBirds.push_back(temp);
		shot = false;
		viewport.setObjectToTrack(*sprite);
		LetUsTakeSomeTimeToLookAtThePrettyExplosion++;
		++scoreThatYouHaveReceivedBasedOnTheNumberOfSpritesThatHaveExploded;
//pop is expensive but there are only 2 sprites in the sprite vector
		sprites.pop_back();
		break;
	      }
	    }
            ++sprite;
  	  }
	}
	hitCrabTime++;
	resetTime++;
	timeResetTime++;
	if (shot == true) {
	  std::vector<Drawable*>::iterator sprite2 = crabz.begin();
	  while ( sprite2 != crabz.end() ) {
	    if (not dynamic_cast<ExplodingSprite*>(*sprite2)) {
	      if ( static_cast<MissileSprite*>(sprites[sprites.size()-1])->collidedWith(*sprite2) ) {
	        MultiSprite* temp = static_cast<MultiSprite*>(*sprite2);
		Sprite weGonnaBlowThisOneUp((*sprite2)->getName(),(*sprite2)->getPosition(),(*sprite2)->getVelocity());
		(*sprite2) = new ExplodingSprite(const_cast<Sprite&>(weGonnaBlowThisOneUp));				
		static_cast<ExplodingSprite*>(*sprite2)->setExplosionStrategy(1);	
		hitCrabTime = 0;
		explodedCrabs.push_back(temp);
		shot = false;
		viewport.setObjectToTrack(*sprite2);
		LetUsTakeSomeTimeToLookAtThePrettyExplosion++;
		scoreThatYouHaveReceivedBasedOnTheNumberOfSpritesThatHaveExploded+=5;
		sprites.pop_back();
		break;
	      }
	    }
    	    ++sprite2;
  	  }
  	}
      }
      if(hitCrabTime < 50) {
	hitCrabMessage();
      }
      if(LetUsTakeSomeTimeToLookAtThePrettyExplosion == 10) {
	LetUsTakeSomeTimeToLookAtThePrettyExplosion = 0;
	viewport.setObjectToTrack(sprites[0]);
      }
      if (resetTime < 50) {
	resetMessage();
      }
      if (timeResetTime < 50) {
	timeResetMessage();
      }

      SDL_Flip(screen);
//get rid of the missile if it goes out of the world
      if (shot == true and sprites[sprites.size()-1]->Y() > viewport.getVH()) {
        shot = false;
	viewport.setObjectToTrack(sprites[0]);
	sprites.pop_back();
      }
    }
  }
