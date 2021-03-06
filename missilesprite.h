#ifndef MISSILESPRITE__H
#define MISSILESPRITE__H
#include <string>
#include <vector>
#include "drawable.h"
#include "collisionStrategy.h"

class MissileSprite : public Drawable {
public:
  MissileSprite(const std::string&, float x, float y, int power);
  virtual ~MissileSprite() { } 

  virtual void draw() const;
  virtual void update(Uint32 ticks);
  virtual const Frame* getFrame() const { 
    return frames[currentFrame]; 
  }

  bool collidedWith(const Drawable* d) const {
    return strategy->execute(*this, *d);
  }
  void setCollisionStrategy(int index) {
    strategy = strategies[index];
  }

  float distance(float x1, float y1, float x2, float y2) const;

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
  int time;

  void advanceFrame(Uint32 ticks);

private:
  MissileSprite(const MissileSprite&);
  MissileSprite& operator=(const MissileSprite&);
  
  std::vector<CollisionStrategy*> strategies;
  CollisionStrategy * strategy;
};
#endif
