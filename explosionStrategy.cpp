#include "explosionStrategy.h"
#include "gamedata.h"
#include "frameFactory.h"

Chunker::~Chunker() {
  for ( unsigned int i = 0; i < frames.size(); ++i ) {
    delete frames[i]; // ExplodingSprite made them, so it deletes them
  }
  chunks.clear();     // clearing a vector is a waste of time, but ...
  freeList.clear(); 
}

void Chunker::draw() const { 
  std::list<Chunk>::const_iterator ptr = chunks.begin();
  while (ptr != chunks.end()) {
    ptr->draw();
    ++ptr;
  }
}

void Chunker::update(Uint32 ticks) { 
  std::list<Chunk>::iterator ptr = chunks.begin();
  while (ptr != chunks.end()) {
    ptr->update(ticks);
    if (ptr->goneTooFar()) {  // Check to see if we should free a chunk
      freeList.push_back(*ptr);
      ptr = chunks.erase(ptr);
    }   
    else ++ptr;
  }
} 

void Chunker::makeChunks(unsigned int n) { 
  // Break the SDL_Surface into n*n squares; where each square
  // has width and height of frameWidth/n and frameHeight/n
  unsigned int chunk_width = std::max(1u, sprite.getFrame()->getWidth()/n);
  unsigned int chunk_height = std::max(1u, sprite.getFrame()->getHeight()/n);
  int speedx = static_cast<int>(sprite.velocityX()); // Wanna test for zero...
  int speedy = static_cast<int>(sprite.velocityY()); // Make sure it's an int.
  if (speedx == 0) speedx = 1; // Make sure it's not 0;
  if (speedy == 0) speedy = 1; // Make sure it's not 0;

  // Get the SDL_Surface so we can chunk it:
  SDL_Surface* spriteSurface(sprite.getFrame()->getSurface()); 
  Sint16 source_y =sprite.getFrame()->getSourceY();
  while  (source_y < sprite.getFrame()->getHeight() ) {
    Sint16 source_x = sprite.getFrame()->getSourceX();
    while ( source_x < sprite.getFrame()->getWidth() ) {
      // Give each chunk it's own speed/direction:
      float sx = (rand() % speedx + 40) * (rand()%2?-1:1); // 'cause %0 is 
      float sy = (rand() % speedy + 40) * (rand()%2?-1:1); // float except
      Frame* frame = 
            new Frame(spriteSurface, chunk_width, chunk_height,
                  source_x, source_y
                );
      Chunk chunk(
                Vector2f(sprite.X()+source_x,   // x coord of destination 
                         sprite.Y()+source_y), // y coord of destination
                Vector2f(sx, sy),
                sprite.getName()+"/chunk",
                frame);
      // chunks uses value semantics, as does frames, but there's
      // a big difference:
      chunks.push_back(chunk);
      frames.push_back(frame);
      source_x += chunk_width;
    }
    source_y += chunk_height;
  }
}

void Splosion::draw() const { 
  if (totalTimeSploding < 1500) 
  	frames[currentFrame]->draw(x, y-125);
}

void Splosion::update(Uint32 ticks) {
  totalTimeSploding += ticks;
  if (totalTimeSploding < 1500) 
  	advanceFrame(ticks);
}


void Splosion::advanceFrame(Uint32 ticks) {
	timeSinceLastFrame += ticks;
	if (timeSinceLastFrame > frameInterval) {
    currentFrame = (currentFrame+1) % numberOfFrames;
		timeSinceLastFrame = 0;
	}
}

Splosion::Splosion(const std::string& name, int xpos, int ypos) : 
  x(xpos),
  y(ypos),  
  frames( FrameFactory::getInstance().getFrames(name) ),
  currentFrame(0),
  numberOfFrames( Gamedata::getInstance().getXmlInt(name+"/frames") ),
  frameInterval( Gamedata::getInstance().getXmlInt(name+"/frameInterval") ),
  timeSinceLastFrame( 0 ),
  totalTimeSploding( 0 )
{ }




