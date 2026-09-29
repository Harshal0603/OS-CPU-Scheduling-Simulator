#ifndef GANTTCHART_H
#define GANTTCHART_H

#include <vector>
#include "Task.h"

// Prints a text Gantt chart, e.g.
//   | T1 | T2 | IDLE | T3 |
//   0    5    8      10   14
void printGantt(const std::vector<GanttSegment>& segments);

#endif
