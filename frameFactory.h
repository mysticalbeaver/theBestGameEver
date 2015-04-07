#include <string>
#include <vector>
#include <map>
#include "frame.h"
#include "gamedata.h"

class FrameFactory {
public:
  static FrameFactory& getInstance();
  ~FrameFactory();

  Frame* getFrame(const std::string&);

  std::vector<Frame*> getFrames(const std::string&);

private:
  const Gamedata& gdata;
  
  std::map<std::string, SDL_Surface*> surfaces;
  std::map<std::string, std::vector<SDL_Surface*> > multiSurfaces;

  std::map<std::string, Frame*> frames;
  std::map<std::string, std::vector<Frame*> > multiFrames;

  FrameFactory();
  FrameFactory(const FrameFactory&);
  FrameFactory& operator=(const FrameFactory&);
};
