#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include "clock.h"
#include "gamedata.h"
#include "ioManager.h"

Clock& Clock::getInstance() {
  if ( SDL_WasInit(SDL_INIT_VIDEO) == 0) {
    throw std::string("Must init SDL before Clock");
  }
  static Clock clock; 
  return clock;
}

Clock::Clock() :
  ticks(0),
  started(false), 
  paused(false), 
  sloMo(false),
  capped(Gamedata::getInstance().getXmlInt("framesAreCapped")),
  frameCap(Gamedata::getInstance().getXmlInt("frameCap")),
  sumOfTicks(0),
  frames(0),
  time(1001),
  framecount(0),
  begin(0),
  resetOffset(0)
  {
  start();
}

Clock::Clock(const Clock& c) :
  ticks(c.ticks), 
  started(c.started), 
  paused(c.paused), 
  sloMo(c.sloMo),
  capped(c.capped),
  frameCap(c.frameCap),
  sumOfTicks(c.sumOfTicks),
  frames(c.frames),
  time(c.time),
  framecount(c.framecount),
  begin(c.begin),
  resetOffset(c.resetOffset)
  {
  start();
}

void Clock::draw() const { 
  IOManager::getInstance().printMessageValueAt("fps: ", frames, 
																Gamedata::getInstance().getXmlInt("fpsLocX"), 
																Gamedata::getInstance().getXmlInt("fpsLocY"));
  IOManager::getInstance().printMessageValueAt("Seconds Elapsed: ", getSeconds(), 
																Gamedata::getInstance().getXmlInt("getSecondsLocX"),
																Gamedata::getInstance().getXmlInt("getSecondsLocY"));
}

void Clock::update() {
	int cticks = SDL_GetTicks();
	ticks = (cticks-resetOffset) - sumOfTicks;

	//turning off the capped frame rate is done by changing the xml value
	//framesAreCapped to 0
	if (capped) {
		while (ticks <= ceil(1000/frameCap))
			ticks = (SDL_GetTicks()-resetOffset) - sumOfTicks;
	} 
	
	//calculate fps by counting the number of frames drawn in a second
	if (time < 1000)
		time = SDL_GetTicks() - begin;
	else {
		frames = framecount;
		begin = SDL_GetTicks();
		framecount = 0;
		time = 0;
	}
		 
	if (paused) {
		ticks = 0;
		frames = 0;
	}
	else if (sloMo) {
		ticks = 1;
		sumOfTicks = SDL_GetTicks() - resetOffset;
	}
	else {
		sumOfTicks += ticks;
	}
	framecount++;
}

unsigned int Clock::getTicksSinceLastFrame() const {
		return ticks; 
}

void Clock::toggleSloMo() {
 sloMo = !sloMo;
}

int Clock::getFps() const {
	if (frames > 0)
  		return frames;
  	else
  		return 0;
}

void Clock::start() { 
  started = true; 
  paused = false; 
}

void Clock::reset() {
	resetOffset += sumOfTicks;
	sumOfTicks = 0;
}

void Clock::pause() { 
  paused = !paused;
}

void Clock::unpause() { 
  sumOfTicks = SDL_GetTicks();
  paused = !paused;
}

