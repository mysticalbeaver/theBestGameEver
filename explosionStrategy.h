#include "sprite.h"
#include "chunk.h"
#include <vector>
#include <list>

class ExplosionStrategy {

public:
  virtual void update(Uint32 ticks) = 0;
  virtual void draw() const = 0;
  virtual ~ExplosionStrategy() {}
};

class Chunker : public ExplosionStrategy {
public:
  ~Chunker();
  Chunker(const Sprite& s): sprite(s), chunks(), freeList(), frames() 
{ makeChunks(Gamedata::getInstance().getXmlInt(s.getName()+"/chunk/size")); }
  virtual void update(Uint32 ticks);
  virtual void draw() const;
  void makeChunks(unsigned int);
  unsigned int chunkCount() const { return chunks.size(); }
  unsigned int freeCount()  const { return freeList.size(); }
private:
  const Sprite& sprite;
  std::list<Chunk> chunks; 
  std::list<Chunk> freeList;
  std::vector<Frame*> frames;
};

class Splosion : public ExplosionStrategy {
public:
  Splosion(const std::string& name,int x,int y);
  virtual void update(Uint32 ticks);
  virtual void draw() const;
  void advanceFrame(Uint32 ticks);

private:
  int x;
  int y;
  const std::vector<Frame *> frames;
  unsigned currentFrame;
  unsigned numberOfFrames;
  unsigned frameInterval;
  float timeSinceLastFrame;
  int totalTimeSploding;
};
