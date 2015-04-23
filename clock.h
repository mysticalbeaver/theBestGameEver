#include <SDL.h>
#include <string>
#include <deque>

class Manager;

class Clock {
public:
  static Clock& getInstance();  // This class is a Singleton
  unsigned int getTicksSinceLastFrame() const;
  unsigned int getTicksSinceInit() const { return sumOfTicks; }
  unsigned int getSeconds() const { return (sumOfTicks-resetOffset)/1000;  }

  void toggleSloMo();
  bool isStarted() const { return started; }
  bool isPaused() const  { return paused;  }
  int getFps() const;

  void start();
  void pause();
  void unpause();
  void draw() const;  // For debugging
  void update();  // Increments time/ticks
  void reset();

private:
  unsigned int ticks;
  bool started;
  bool paused;
  bool sloMo;
  bool capped;
  unsigned int frameCap;
  unsigned int sumOfTicks;
  unsigned int frames;
  unsigned int time;
  unsigned int framecount;
  unsigned int begin;
  unsigned int resetOffset;


  Clock();
  Clock(const Clock&);
  Clock&operator=(const Clock&);
};
