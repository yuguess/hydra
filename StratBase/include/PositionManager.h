#ifndef POSITION_MANAGER_H
#define POSITION_MANAGER_H

class PositionManager {

public:
  enum NetPosition {
    LONG_POSITION = 0,
    SHORT_POSITION,
    EMPTY
  };

  int updatePNL();
  int updatePosition();
  NetPosition getPosition();
};

#endif 
