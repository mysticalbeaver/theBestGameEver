#include <iostream>
#include <string>
#include <iomanip>
#include "twowaymultisprite.h"
#include "playersprite.h"
#include "multisprite.h"
#include "missilesprite.h"
#include "paintSprite.h"
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
  depthMakers(),
  currentSprite(0),

  makeVideo( false ),
  frameCount( 0 ),
  username(  Gamedata::getInstance().getXmlStr("username") ),
  title( Gamedata::getInstance().getXmlStr("screenTitle") ),
  frameMax( Gamedata::getInstance().getXmlInt("frameMax") )
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    throw string("Unable to initialize SDL: ");
  }
  SDL_WM_SetCaption(title.c_str(), NULL);
  atexit(SDL_Quit);
  unsigned int n = Gamedata::getInstance().getXmlInt("numberofarticunos");
  unsigned int j = Gamedata::getInstance().getXmlInt("numberofswirls");
  sprites.reserve(n+j+3);
  
  sprites.push_back( new PlayerSprite("cloudgun") );
  sprites.push_back( new TwoWayMultiSprite("mewtwo") );
  sprites.push_back( new MultiSprite("crab") );
  
//make sure you put the paintsprites on last
  for (unsigned int i = 0; i < n; ++i){
  	depthMakers.push_back(new paintSprite("Articuno"));
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
	
  //draw the sprites in reverse so mew and mewtwo are on top of all
  //the articunos and swirls
  for (int i = sprites.size() - 1; i >= 0; --i) {
    sprites[i]->draw();
  }

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

void Manager::play() {
  SDL_Event event;
  bool done = false;
  clock.start();

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
 			if (event.type ==  SDL_QUIT) { 
				done = true; 
				break;
			}
			if(event.type == SDL_KEYDOWN) {
        			if (keystate[SDLK_ESCAPE] || keystate[SDLK_q]) {
          				done = true;
          				break;
        			}
        			if ( keystate[SDLK_t] ) {
          				switchSprite();
        			}
        			if ( keystate[SDLK_p] ) {
          				if ( clock.isPaused() )
						clock.unpause(); 
         				else 
						clock.pause();
        			}
        			if ( keystate[SDLK_F3] ) {
        				clock.toggleSloMo();
        			}
				   if( keystate[SDLK_w] ) {
					   sprites[0]->up();
				   }
					if ( keystate[SDLK_s] ) {
	 					sprites[0]->down();
					}
					if (keystate[SDLK_RETURN] and not shot) {
						sprites.push_back(new MissileSprite("Articuno",sprites[0]->X(),sprites[0]->Y(),bar.getLen()));
						bar.reset();
						viewport.setObjectToTrack(sprites[sprites.size()-1]);
						timeSinceMissile = 0;
						shot = true;
						sound[6];
					}
        			if ( keystate[SDLK_F4] && !makeVideo ) {
        				std::cout << "Making video frames" << std::endl;
        				makeVideo = true;
        			}
					if ( keystate[SDLK_F1] ) {
		     				counter = 0;
		     		}
					if (keystate[SDLK_SPACE]) {
							// need to make it so that it continually 
							// fills the heatlth bar
		     				bar.powerUp();
		    		}
		   } 
    		else if( event.type == SDL_KEYUP ) {
				sprites[0]->stopMove();
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
			std::vector<paintSprite*>::iterator sprite = depthMakers.begin() + ((2*depthMakers.size())/3);
			while ( sprite != depthMakers.end() ) {
				if (not dynamic_cast<ExplodingSprite*>(*sprite)) {
				if ( static_cast<MissileSprite*>(sprites[sprites.size()-1])->collidedWith(*sprite) ) {
					paintSprite* temp = (*sprite);
					(*sprite) = new ExplodingSprite(const_cast<paintSprite&>(**sprite));
					delete temp;
					shot = false;
					viewport.setObjectToTrack(sprites[0]);
					break; 
				}
				}
    				++sprite;
  			}
  		}

		if (shot == true and sprites[sprites.size()-1]->Y() > viewport.getVH()) {
			shot = false;
			viewport.setObjectToTrack(sprites[0]);
			sprites.pop_back();
		}
	}
}
