#include <iostream>
#include <cmath>
#include "explodingSprite.h"

ExplodingSprite::ExplodingSprite(const Sprite& s) :
  Sprite(s), 
  strategies(),
  strategy( NULL )
{ 
  Sprite chunks(getName(),getPosition(),getVelocity()); 
  strategies.push_back( new Chunker(const_cast<Sprite&>(chunks)) );
  strategies.push_back( new Splosion("explo",chunks.X(),chunks.Y()) );
  strategy = strategies[1];
}


