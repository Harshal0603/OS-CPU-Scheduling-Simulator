#include "GanttChart.h"

#include <algorithm>
#include <iostream>
#include <string>

void printGantt(const std::vector<GanttSegment>& segments) {
    if (segments.empty()) {
        return;
    }

    std::string labelLine;
    std::string timeLine;

    for (const GanttSegment& seg : segments) {
        const std::string startText = std::to_string(seg.startTime);
        const std::string endText = std::to_string(seg.endTime);

        // The cell must be wide enough for the label AND for its time numbers,
        // otherwise big numbers would overlap the next time on the line below.
        int width = static_cast<int>(seg.taskId.size()) + 2;
        width = std::max(width, static_cast<int>(startText.size()) + 1);
        width = std::max(width, static_cast<int>(endText.size()) + 1);

        // Centre the label inside the cell.
        int padding = width - static_cast<int>(seg.taskId.size());
        int leftPad = padding / 2;
        int rightPad = padding - leftPad;

        labelLine += "|" + std::string(leftPad, ' ') + seg.taskId + std::string(rightPad, ' ');

        // Each segment occupies (1 bar + width) characters; the start time sits under the bar.
        timeLine += startText + std::string(width + 1 - startText.size(), ' ');
    }

    labelLine += "|";
    timeLine += std::to_string(segments.back().endTime);

    std::cout << labelLine << "\n" << timeLine << "\n";
}
