#include "CalendarEventService.h"
#include <algorithm>

using namespace Pinetime::Controllers;

CalendarEventService::CalendarEventService() : eventCount(0) {
  std::memset(events.data(), 0, events.size() * sizeof(CalendarEvent));
}

bool CalendarEventService::AddEvent(const CalendarEvent& event) {
  if (!event.IsValid()) {
    return false;
  }

  // Check if event already exists (update case)
  int existingIdx = FindEventIndex(event.timestamp);
  if (existingIdx >= 0) {
    events[existingIdx] = event;
    return true;
  }

  // New event - check if we have space
  if (eventCount >= MaxEvents) {
    return false;  // No space - could implement LRU eviction here if needed
  }

  // Find insertion point to keep sorted by timestamp
  size_t insertIdx = 0;
  while (insertIdx < eventCount && events[insertIdx].timestamp < event.timestamp) {
    insertIdx++;
  }

  // Shift events right to make room
  for (int i = eventCount; i > (int)insertIdx; i--) {
    events[i] = events[i - 1];
  }

  events[insertIdx] = event;
  eventCount++;
  return true;
}

bool CalendarEventService::RemoveEvent(uint64_t timestamp) {
  int idx = FindEventIndex(timestamp);
  if (idx < 0) {
    return false;
  }

  // Shift events left to fill gap
  for (int i = idx; i < (int)eventCount - 1; i++) {
    events[i] = events[i + 1];
  }

  eventCount--;
  std::memset(&events[eventCount], 0, sizeof(CalendarEvent));
  return true;
}

int CalendarEventService::FindEventIndex(uint64_t timestamp) const {
  for (int i = 0; i < eventCount; i++) {
    if (events[i].timestamp == timestamp) {
      return i;
    }
  }
  return -1;
}

bool CalendarEventService::GetNextEvent(CalendarEvent& outEvent, uint64_t currentTime) {
  if (eventCount == 0) {
    return false;
  }

  // Find next upcoming or current event
  for (int i = 0; i < eventCount; i++) {
    if (events[i].timestamp >= currentTime) {
      outEvent = events[i];
      return true;
    }
  }

  return false;
}

void CalendarEventService::ClearAllEvents() {
  std::memset(events.data(), 0, events.size() * sizeof(CalendarEvent));
  eventCount = 0;
}

void CalendarEventService::OnCalendarEventWrite(const uint8_t* data, size_t length) {
  // Protocol format (packed):
  // [0]: Operation (1=add, 2=remove, 3=clear)
  // [1-8]: Timestamp (uint64_t, big-endian)
  // [9-10]: Duration (uint16_t, big-endian)
  // [11-74]: Title (null-terminated string)
  
  if (length < 1) {
    return;
  }

  uint8_t operation = data[0];

  switch (operation) {
    case 3:  // Clear all events
      ClearAllEvents();
      break;

    case 2:  // Remove event
      if (length >= 9) {
        uint64_t timestamp = 0;
        for (int i = 0; i < 8; i++) {
          timestamp = (timestamp << 8) | data[1 + i];
        }
        RemoveEvent(timestamp);
      }
      break;

    case 1:  // Add/update event
      if (length >= 11) {
        CalendarEvent event;

        // Parse timestamp (big-endian)
        event.timestamp = 0;
        for (int i = 0; i < 8; i++) {
          event.timestamp = (event.timestamp << 8) | data[1 + i];
        }

        // Parse duration (big-endian)
        event.duration = ((uint16_t)data[9] << 8) | data[10];

        // Parse title
        size_t titleLen = std::min(length - 11, (size_t)63);
        std::memcpy(event.title, &data[11], titleLen);
        event.title[titleLen] = '\0';

        AddEvent(event);
      }
      break;
  }
}

void CalendarEventService::UpdateDisplayString(char* buffer, size_t maxLen, uint64_t currentTime) {
  if (!buffer || maxLen == 0) {
    return;
  }

  buffer[0] = '\0';

  CalendarEvent nextEvent;
  if (!GetNextEvent(nextEvent, currentTime)) {
    snprintf(buffer, maxLen, "[CALENDAR] No upcoming events");
    return;
  }

  // Format: "[CALENDAR] Event: HH:MM - Title"
  time_t eventTime = nextEvent.timestamp;
  struct tm* timeinfo = localtime(&eventTime);

  if (nextEvent.IsNow(currentTime)) {
    snprintf(buffer, maxLen, "[CALENDAR] NOW: %02d:%02d %s",
             timeinfo->tm_hour, timeinfo->tm_min, nextEvent.title);
  } else {
    snprintf(buffer, maxLen, "[CALENDAR] %02d:%02d %s",
             timeinfo->tm_hour, timeinfo->tm_min, nextEvent.title);
  }
}

void CalendarEventService::FormatTimeString(uint64_t timestamp, char* buffer, size_t maxLen) {
  if (!buffer || maxLen < 6) {
    return;
  }

  time_t t = timestamp;
  struct tm* timeinfo = localtime(&t);
  snprintf(buffer, maxLen, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
}

void CalendarEventService::CompactEvents() {
  // Remove any null/invalid events and compact
  uint8_t writeIdx = 0;
  for (uint8_t readIdx = 0; readIdx < eventCount; readIdx++) {
    if (events[readIdx].IsValid()) {
      if (readIdx != writeIdx) {
        events[writeIdx] = events[readIdx];
      }
      writeIdx++;
    }
  }
  eventCount = writeIdx;
}
