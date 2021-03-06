#ifndef CHUNK__H
#define CHUNK__H
#include "sprite.h"
#include "explosionStrategy.h"

class ExplodingSprite : public Sprite {
public:
  ExplodingSprite(const Sprite& s);
  ~ExplodingSprite(){};
  void draw() const {
    strategy->draw();
  }
  void update(Uint32 ticks) {
    strategy->update(ticks);
  }
  void setExplosionStrategy(int index) {
    strategy = strategies[index];
  }

private:
  ExplodingSprite(const ExplodingSprite&); // Explicit disallow (Item 6)
  ExplodingSprite& operator=(const ExplodingSprite&); // (Item 6)

  std::vector<ExplosionStrategy*> strategies;
  ExplosionStrategy * strategy;
};

/*
class ExplodingSprite : public Sprite {
public:
  ExplodingSprite(const Sprite& s);
  ~ExplodingSprite();
  virtual void draw() const;
  virtual void update(Uint32 ticks);
  void makeChunks(unsigned int);
  unsigned int chunkCount() const { return chunks.size(); }
  unsigned int freeCount()  const { return freeList.size(); }
private:
  std::list<Chunk> chunks; // An ExplodingSprite is a list of sprite chunks
  std::list<Chunk> freeList; // When a chunk gets out of range it goes here
  std::vector<Frame*> frames; // Each chunk has a Frame
  ExplodingSprite(const ExplodingSprite&); // Explicit disallow (Item 6)
  ExplodingSprite& operator=(const ExplodingSprite&); // (Item 6)
}; */
#endif
