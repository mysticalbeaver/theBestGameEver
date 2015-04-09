#include "frameFactory.h"
#include "extractSurface.h"
#include "ioManager.h"
#include "vector2f.h"

FrameFactory::FrameFactory() : 
  gdata( Gamedata::getInstance() ), 
  surfaces(),
  multiSurfaces(),
  frames(),
  multiFrames()
{}

FrameFactory::~FrameFactory() {

  std::map<std::string, Frame*>::iterator pos1 = frames.begin();
  std::map<std::string, SDL_Surface*>::iterator pos2 = surfaces.begin();
  
  while (pos1 != frames.end()) {
  	//delete the pointer and then erase all the elements afterwards
  	delete pos1->second;
  	pos1++;
  }

  while (pos2 != surfaces.end()) {
  	SDL_FreeSurface(pos2->second);
  	pos2++;
  }

  surfaces.clear();
  frames.clear();
  
  //for multiframe sprites you must delete each member of the map as well
  //as each member of the vector corresponding to a member of the map	
  std::map<std::string, std::vector<Frame*> >::iterator pos3 = multiFrames.begin();
  std::map<std::string, std::vector<SDL_Surface*> >::iterator pos4 = multiSurfaces.begin();

  while (pos3 != multiFrames.end()) { 
  	std::vector<Frame*>::iterator pos5 = pos3->second.begin();
  	int i = 0;
  	//same as before, delete the pointer and then erase all the elements
  	//but do this for each vector which is a member of the map

  	while (pos5 != pos3->second.end()) {
  		delete pos3->second[i];
  		i++;
  		pos5++;
  	}
  	pos3->second.clear();
  	pos3++;	
  }

    while (pos4 != multiSurfaces.end()) {
  	std::vector<SDL_Surface*>::iterator pos6 = pos4->second.begin();
  	int i = 0;
  	while (pos6 != pos4->second.end()) {
  		SDL_FreeSurface(pos4->second[i]);
  		i++;
  		pos6++;
  	}
  	pos4->second.clear(); 
  	pos4++;
 }
 
 multiFrames.clear();
 multiSurfaces.clear();	
}

FrameFactory& FrameFactory::getInstance() {
  static FrameFactory factory;
  return factory;
}

Frame* FrameFactory::getFrame(const std::string& name) {
  //if it is already in the map then return it
  std::map<std::string, Frame*>::const_iterator pos = frames.find(name);
  if (pos != frames.end())
  	return pos->second;
  
  //if it is not in the map then get a new surface 		
  SDL_Surface * surface =
    IOManager::getInstance().loadAndSet(
        gdata.getXmlStr(name+"/file"),
        gdata.getXmlBool(name+"/transparency"));
  Uint16 srcX = gdata.getXmlInt(name+"/srcX");
  Uint16 srcY = gdata.getXmlInt(name+"/srcY");
  Uint16 width = gdata.getXmlInt(name+"/width");
  Uint16 height = gdata.getXmlInt(name+"/height");
  
  //add to vector of single frame surfaces  
  surfaces[name] = surface;
  Frame * const frame = new Frame(name, surface);
  frames[name] = frame;
  return frames[name];
}

std::vector<Frame*> FrameFactory::getFrames(const std::string& name) {
  // First search map to see if we've already made it:
  std::map<std::string, std::vector<Frame*> >::const_iterator 
    pos = multiFrames.find(name); 
  if ( pos != multiFrames.end() ) {
    return pos->second;
  }

  // It wasn't in the map, so we have to make the vector of Frames:
  SDL_Surface* surface = IOManager::
     getInstance().loadAndSet(gdata.getXmlStr(name+"/file"), true);
  unsigned numberOfFrames = gdata.getXmlInt(name+"/frames");
  std::vector<Frame*> frames;
  std::vector<SDL_Surface*> surfaces;
  frames.reserve(numberOfFrames);
  surfaces.reserve(numberOfFrames);
  Uint16 srcX = gdata.getXmlInt(name+"/srcX");
  Uint16 srcY = gdata.getXmlInt(name+"/srcY");
  Uint16 width = gdata.getXmlInt(name+"/width");
  Uint16 height = gdata.getXmlInt(name+"/height");

  SDL_Surface* surf;
  for (unsigned i = 0; i < numberOfFrames; ++i) {
    unsigned frameX = i * width + srcX;
   surf = ExtractSurface::getInstance().
               get(surface, width, height, frameX, srcY); 
    surfaces.push_back( surf );
    frames.push_back( new Frame(name, surf) );
  }
  SDL_FreeSurface(surface);
  multiSurfaces[name] = surfaces;
  multiFrames[name] = frames;
  return multiFrames[name];
}
