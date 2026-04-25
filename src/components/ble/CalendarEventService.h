#pragma once

#include <cstdint>
#include <cstring>
#include <array>
#include <chrono>

namespace Pinetime::Controllers {

  class CalendarEventService {
  public:
    // Event data structure - packed to minimize memory usage
    struct CalendarEvent {
      uint64_t timestamp;    // Unix timestamp (seconds)
      uint16_t duration;     // Duration in minutes
      char title[64];        // Event title
      
      bool IsValid() const { return timestamp > 0; }
      
      // Get end time
      uint64_t GetEndTime() const {
        return timestamp + (duration * 60);
      }
      
      // Check if event is happening now
      bool IsNow(uint64_t currentTime) const {
        return currentTime >= timestamp && currentTime < GetEndTime();
      }
      
      // Check if event is upcoming (within next hour)
      bool IsUpcoming(uint64_t currentTime) const {
        return currentTime < timestamp && (timestamp - currentTime) < 3600;
      }
    };

    static constexpr size_t MaxEvents = 10;  // Fixed-size array to avoid dynamic allocation
    using EventArray = std::array<CalendarEvent, MaxEvents>;

    CalendarEventService();
    ~CalendarEventService() = default;

    // Add or update an event
    bool AddEvent(const CalendarEvent& event);
    
    // Remove an event by timestamp
    bool RemoveEvent(uint64_t timestamp);
    
    // Get current/next event for display
    bool GetNextEvent(CalendarEvent& outEvent, uint64_t currentTime);
    
    // Get all events (for debugging/syncing)
    const EventArray& GetAllEvents() const { return events; }
    
    // Clear all events
    void ClearAllEvents();
    
    // Get event count
    uint8_t GetEventCount() const { return eventCount; }
    
    // BLE callback - process write to calendar characteristic
    void OnCalendarEventWrite(const uint8_t* data, size_t length);
    
    // Update display string based on current events
    void UpdateDisplayString(char* buffer, size_t maxLen, uint64_t currentTime);

  private:
    EventArray events;
    uint8_t eventCount = 0;
    
    // Find event index by timestamp
    int FindEventIndex(uint64_t timestamp) const;
    
    // Shift events down to maintain order
    void CompactEvents();
    
    // Format time string for display
    static void FormatTimeString(uint64_t timestamp, char* buffer, size_t maxLen);
  };
}
