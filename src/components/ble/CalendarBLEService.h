#pragma once

#include "host/ble_uuid.h"
#include "host/ble_gatt.h"
#include <cstdint>

namespace Pinetime::Controllers {
  
  // Forward declaration
  class CalendarEventService;

  // Static reference to service (set during initialization)
  extern CalendarEventService* g_calendarService;

  // UUIDs for Calendar Event Service
  // Service: 00060000-78fc-48fe-8e23-433b3a1942d0
  // Write Characteristic: 00060001-78fc-48fe-8e23-433b3a1942d0
  // Notify Characteristic: 00060002-78fc-48fe-8e23-433b3a1942d0

  static const ble_uuid128_t calendarServiceUuid = 
    BLE_UUID128_INIT(0x00, 0x06, 0x00, 0x00, 0x78, 0xfc, 0x48, 0xfe,
                     0x8e, 0x23, 0x43, 0x3b, 0x3a, 0x19, 0x42, 0xd0);

  static const ble_uuid128_t calendarEventWriteUuid =
    BLE_UUID128_INIT(0x00, 0x06, 0x00, 0x01, 0x78, 0xfc, 0x48, 0xfe,
                     0x8e, 0x23, 0x43, 0x3b, 0x3a, 0x19, 0x42, 0xd0);

  static const ble_uuid128_t calendarEventNotifyUuid =
    BLE_UUID128_INIT(0x00, 0x06, 0x00, 0x02, 0x78, 0xfc, 0x48, 0xfe,
                     0x8e, 0x23, 0x43, 0x3b, 0x3a, 0x19, 0x42, 0xd0);

  // Callback for write operations (receive calendar events)
  static int calendar_event_write_callback(uint16_t conn_handle,
                                          uint16_t attr_handle,
                                          struct ble_gatt_access_ctxt* ctxt,
                                          void* arg) {
    if (ctxt->op != BLE_GATT_ACCESS_OP_WRITE_CHR) {
      return 0;
    }

    if (g_calendarService == nullptr) {
      return BLE_ATT_ERR_UNLIKELY;
    }

    // Extract data from mbuf
    size_t len = OS_MBUF_PKTLEN(ctxt->om);
    if (len == 0 || len > 256) {  // Reasonable size limit
      return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    uint8_t buffer[256];
    int rc = ble_hs_mbuf_to_flat(ctxt->om, buffer, sizeof(buffer), NULL);
    if (rc != 0) {
      return BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    // Forward to calendar service for processing
    g_calendarService->OnCalendarEventWrite(buffer, len);
    return 0;
  }

  // Callback for read/notify operations
  static int calendar_event_notify_callback(uint16_t conn_handle,
                                           uint16_t attr_handle,
                                           struct ble_gatt_access_ctxt* ctxt,
                                           void* arg) {
    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
      if (g_calendarService == nullptr) {
        return BLE_ATT_ERR_UNLIKELY;
      }

      // Return current event count as status
      uint8_t eventCount = g_calendarService->GetEventCount();
      int rc = os_mbuf_append(ctxt->om, &eventCount, sizeof(eventCount));
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }
    return 0;
  }

  // GATT service definition for calendar events
  static const struct ble_gatt_svc_def calendar_gatt_svcs[] = {
    {
      .type = BLE_GATT_SVC_TYPE_PRIMARY,
      .uuid = &calendarServiceUuid.u,
      .characteristics = (struct ble_gatt_chr_def[]) {
        {
          // Write characteristic - receive calendar events from companion app
          .uuid = &calendarEventWriteUuid.u,
          .access_cb = calendar_event_write_callback,
          .arg = nullptr,
          .descriptors = nullptr,
          .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP,
          .min_key_size = 0,
          .val_handle = nullptr,
          .cpfd = nullptr,
        },
        {
          // Notify characteristic - status/count notifications
          .uuid = &calendarEventNotifyUuid.u,
          .access_cb = calendar_event_notify_callback,
          .arg = nullptr,
          .descriptors = nullptr,
          .flags = BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ,
          .min_key_size = 0,
          .val_handle = nullptr,
          .cpfd = nullptr,
        },
        { 0 }  // End of characteristics - REQUIRED
      }
    },
    { 0 }  // End of services - REQUIRED
  };

  // Integration function - call during BLE initialization
  inline void RegisterCalendarEventService(CalendarEventService* service) {
    g_calendarService = service;
    // Add to GATT server
    ble_gatts_add_svcs(calendar_gatt_svcs);
  }
}

/*
  INTEGRATION CHECKLIST:
  
  1. Include this file in your BLE initialization code
  
  2. In your BLE setup function (e.g., BleManager::Init()):
     ```cpp
     auto* calendarService = new Pinetime::Controllers::CalendarEventService();
     Pinetime::Controllers::RegisterCalendarEventService(calendarService);
     ble_gatts_add_svcs(Pinetime::Controllers::calendar_gatt_svcs);
     ```
  
  3. Pass calendarService to WatchFaceTerminal constructor
  
  4. Ensure MTU negotiation happens (set to 256+ to handle 75+ byte packets):
     ```cpp
     ble_hs_cfg.att_mtu = 256;
     ```
  
  5. Test with nRF Connect or Gadgetbridge (FederAndInk fork)
     - Service UUID: 00060000-78fc-48fe-8e23-433b3a1942d0
     - Write char:   00060001-78fc-48fe-8e23-433b3a1942d0
     - Notify char:  00060002-78fc-48fe-8e23-433b3a1942d0
*/
