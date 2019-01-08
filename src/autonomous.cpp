#include "main.h"
#include "robot/motors.h"
#include "robot/robot_functions.hpp"

void autonomous() {
  //Start tasks to control puncher and lift
  Task reloadPuncherAutonTask (reloadPuncherAuton);
  Task liftTask (liftControl);

  //All autons go here
  switch (getAutonomous()){
    case 1:{
      
    }
    case 2:{

    }
    case 3:{

    }
    case 4:{

    }
    case 5:{

    }
    case 6:{

    }
    case 7:{

    }
    case 8:{

    }
    case 9:{

    }
    case 10:{

    }
    case 11:{

    }
    case 12:{

    }
  }
}
