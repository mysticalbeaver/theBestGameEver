#include <string>
#include <vector>
#include "drawable.h"

class FlatMultiSprite : public Drawable {
public:
  FlatMultiSprite(const std::string&);
  FlatMultiSprite(const FlatMultiSprite&);
  virtual ~FlatMultiSprite() { } 

  virtual void draw() const;
  virtual void update(Uint32 ticks);
  virtual const Frame* getFrame() const { 
    return frames[currentFrame]; 
  }

protected:
  const std::vector<Frame *> frames;
  int worldWidth;
  int worldHeight;

  unsigned currentFrame;
  unsigned numberOfFrames;
  unsigned frameInterval;
  float timeSinceLastFrame;
  int frameWidth;
  int frameHeight;

  void advanceFrame(Uint32 ticks);
};
