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
//#include "smartSprite.h"

Manager::~Manager() { 
  for (unsigned i = 0; i < sprites.size(); ++i) {
    delete sprites[i];
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
  smartBirds(),
  depthMakers(),
  currentSprite(0),

  makeVideo( false ),
  frameCount( 0 ),
  username(  Gamedata::getInstance().getXmlStr("username") ),
  title( Gamedata::getInstance().getXmlStr("screenTitle") ),
  frameMax( Gamedata::getInstance().getXmlInt("frameMax") ),
  scoreThatYouHaveReceivedBasedOnTheNumberOfSpritesThatHaveExploded(0),
  isHoldingDownTheSpaceBarKey(0)
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    throw string("Unable to initialize SDL: ");
  }
  SDL_WM_SetCaption(title.c_str(), NULL);
  atexit(SDL_Quit);
  unsigned int n = Gamedata::getInstance().getXmlInt("numberofarticunos");
  unsigned int u = Gamedata::getInstance().getXmlInt("numberofcrabsthatareinthegame");

  depthMakers.reserve(n);
  crabz.reserve(u);
  
  sprites.push_back( new PlayerSprite("cloudgun") );
  //sprites.push_back( new TwoWayMultiSprite("mewtwo") );

	for(unsigned int i = 0; i < u; ++i) {
		crabz.push_back( new MultiSprite("crab"));
	}
  
//make sure you put the paintsprites on last
  for (unsigned int i = 0; i < n; ++i) {
  		depthMakers.push_back(new paintSprite("Articuno"));
  	}	

  /*for (unsigned int i = 0; i < 5; ++i) {
		smartBirds.push_back(new SmartSprite("crab"));
	}  */
  
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
	
  for (int i = crabz.size() - 1; i >= 0; --i) {
    crabz[i]->draw();
  }

  //draw the sprites in reverse so mew and mewtwo are on top of all
  //the articunos and swirls
  for (int i = sprites.size() - 1; i >= 0; --i) {
    sprites[i]->draw();
  }

  /*for ( unsigned int i = 0; i < smartSprites.size(); ++i) {
		smartSprites.draw();
	} */

  //io.printMessageAt("Press T to switch sprites", 10, 70);
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

void Manager::switchSprite() {
  currentSprite = (currentSprite + 1) % 3;
  viewport.setObjectToTrack(sprites[currentSprite]);
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

  // also need to add :
  // health monitor  
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
	
	// restore the crabz
   std::vector<Drawable*>::iterator pos = crabz.begin();
	while ( pos != crabz.end() ) {
		if (dynamic_cast<ExplodingSprite*>(*pos)) {
			ExplodingSprite* temp = static_cast<ExplodingSprite*>(*pos);
			(*pos) = new MultiSprite((*pos)->getName());
			delete temp;
		} else {
			++pos;
		}
	}

	// restore the Arduinos
	std::vector<Drawable*>::iterator sprite = depthMakers.begin() + ((2*depthMakers.size())/3);
	while(sprite != depthMakers.end() ) {
		if(dynamic_cast<ExplodingSprite*>(*sprite)) {
			std::cout << "restoring an Arduino" << std::endl;
			ExplodingSprite* temp = static_cast<ExplodingSprite*>(*sprite);
			(*sprite) = new paintSprite((*sprite)->getName());
			delete temp;
		} else {
			++sprite;
		}
	}

	// reset the clock
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

  Health bar;
  SDLSound sound;

  int timeSinceMissile = 0;
  bool shot = false;

	while ( not done ) {
	Uint8 *keystate = SDL_GetKeyState(NULL);
		while ( SDL_PollEvent(&event) ) {
		std::cout<< event.key.keysym.sym << std::endl;
 			if (event.type ==  SDL_QUIT) { 
				done = true; 
				break;
			}
	    		if( event.type == SDL_KEYUP ) {
    			switch( event.key.keysym.sym ){
                    		case SDLK_s:
                    		case SDLK_w:
					sprites[0]->stopMove();
					break;                    				                       		
                    		case SDLK_SPACE:
                    			if(isHoldingDownTheSpaceBarKey) {
						bar.powerUp();
						isHoldingDownTheSpaceBarKey = 0;
						sprites.push_back(new MissileSprite("Articuno",sprites[0]->X(),sprites[0]->Y(),bar.getLen()));
						bar.reset();
						shot = true;
						viewport.setObjectToTrack(sprites[sprites.size()-1]);
						timeSinceMissile = 0;
						sound[6];
					}	
					break;
				default:
					break;		
				}	
      		}
			if(event.type == SDL_KEYDOWN) {
			switch( event.key.keysym.sym ){
        			case SDLK_ESCAPE:
        			case SDLK_q: 
          				done = true;
          				break;
        			case SDLK_p:
          				if ( clock.isPaused() )
						clock.unpause(); 
         				else 
						clock.pause();
        				break;
				case SDLK_r:
					reset();
					break;
        			case SDLK_F3:
        				clock.toggleSloMo();
        				break;
				case SDLK_w:
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
		     			counter = 0;
		     			break;
				case SDLK_SPACE:
		     			bar.powerUp();
					isHoldingDownTheSpaceBarKey = 1;
					break;
					/*if (keystate[SDLK_PLUS] || keystate[SDLK_EQUALS]) { 
					  if ( !keyCatch ) {
						 keyCatch = true;
						 SmartSprite::incrSafeDistance(); 
					  }
					}
					if (keystate[SDLK_MINUS]) { 
					  if ( !keyCatch ) {
						 keyCatch = true;
						 SmartSprite::decrSafeDistance();
					  }
					} */
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
		counter++;
        	bar.draw(); 
		SDL_Flip(screen);
		update();
		bar.update(clock.getTicksSinceLastFrame());
		timeSinceMissile += clock.getTicksSinceLastFrame();

		if (shot == true) {
			std::vector<Drawable*>::iterator sprite = depthMakers.begin() + ((2*depthMakers.size())/3);
			while ( sprite != depthMakers.end() ) {
				if (not dynamic_cast<ExplodingSprite*>(*sprite)) {
					if ( static_cast<MissileSprite*>(sprites[sprites.size()-1])->collidedWith(*sprite) ) {
						paintSprite* temp = static_cast<paintSprite*>(*sprite);
						Sprite weGonnaBlowThisOneUp((*sprite)->getName(),(*sprite)->getPosition(),
							(*sprite)->getVelocity());
						(*sprite) = new ExplodingSprite(const_cast<Sprite&>(weGonnaBlowThisOneUp));
						delete temp;
						shot = false;
						viewport.setObjectToTrack(sprites[0]);
						++scoreThatYouHaveReceivedBasedOnTheNumberOfSpritesThatHaveExploded;
						break;
					}
				}
    			++sprite;
  			}
		}
		if (shot == true) {
			std::vector<Drawable*>::iterator sprite2 = crabz.begin();
			while ( sprite2 != crabz.end() ) {
				if (not dynamic_cast<ExplodingSprite*>(*sprite2)) {
					if ( static_cast<MissileSprite*>(sprites[sprites.size()-1])->collidedWith(*sprite2) ) {
						MultiSprite* temp = static_cast<MultiSprite*>(*sprite2);
						Sprite weGonnaBlowThisOneUp((*sprite2)->getName(),(*sprite2)->getPosition(),
							(*sprite2)->getVelocity());
						(*sprite2) = new ExplodingSprite(const_cast<Sprite&>(weGonnaBlowThisOneUp));
						delete temp;
						shot = false;
						viewport.setObjectToTrack(sprites[0]);
						++scoreThatYouHaveReceivedBasedOnTheNumberOfSpritesThatHaveExploded;
						break;
					}
				}
    			++sprite2;
  			}
  		}
			

		if (shot == true and sprites[sprites.size()-1]->Y() > viewport.getVH()) {
			shot = false;
			viewport.setObjectToTrack(sprites[0]);
			sprites.pop_back();
		}
	}
}
