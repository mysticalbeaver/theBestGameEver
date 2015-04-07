#ifndef PLAYERSPRITE__H
#define PLAYERSPRITE__H
#include <string>
#include <vector>
#include "drawable.h"

class PlayerSprite : public Drawable {
public:
  PlayerSprite(const std::string&);
  PlayerSprite(const PlayerSprite&);
  virtual ~PlayerSprite() { }
  PlayerSprite& operator=(const PlayerSprite&); 

  virtual void draw() const;
  virtual void update(Uint32 ticks);
  virtual const Frame* getFrame() const { 
    return frame; 
  }

protected:
  const Frame * frame;
  int worldWidth;
  int worldHeight;
  int frameWidth;
  int frameHeight;
};
#endif
