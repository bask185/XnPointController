
#include "roundRobinTasks.h"
#include "src/io.h"
#include "src/macros.h"

extern void processRoundRobinTasks(void) 
{
    static unsigned char taskCounter = 0;

// HIGH PRIORITY ROUND ROBIN TASKS


// LOW PRIORITY ROUND ROBIN TASKS
    switch( ++ taskCounter)
    {
    default: 
        taskCounter = 0;
        /* fill in a task */
        break;

    case 1:
        /* fill in a task */
        break;

    case 2:
        /* fill in a task */
        break;
    }
}