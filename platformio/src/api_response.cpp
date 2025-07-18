/* API response deserialization for esp32-weather-epd.
 * Copyright (C) 2022-2024  Luke Marzen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <vector>
#include <ArduinoJson.h>
#include "api_response.h"
#include "config.h"

DeserializationError deserializeTempestCall(Stream &json,
                                        tempest_resp_t &r)
{
  int i;
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, json);
#if DEBUG_LEVEL >= 1
  Serial.println("[debug] doc.overflowed() : "
                 + String(doc.overflowed()));
#endif
#if DEBUG_LEVEL >= 2
  serializeJsonPretty(doc, Serial);
#endif
  if (error) {
    return error;
  }

  r.lat             = doc["latitude"]            .as<float>();
  r.lon             = doc["longitude"]            .as<float>();
  r.timezone        = doc["timezone"]       .as<const char *>();
  r.timezone_offset = doc["timezone_offset_minutes"].as<int>();

  JsonObject current = doc["current_conditions"];
  r.current.dt         = current["time"]        .as<int64_t>();
  r.current.temp       = current["air_temperature"].as<float>(); // Default Celcius
  r.current.feels_like = current["feels_like"].as<float>(); // Default Celcius
  r.current.pressure   = current["sea_level_pressure"]  .as<int>(); // Default mb
  r.current.humidity   = current["relative_humidity"]  .as<int>();
  r.current.dew_point  = current["dew_point"] .as<float>(); // Default Celcius
  r.current.solar_radiation = current["solar_radiation"]       .as<float>();
  r.current.uvi        = current["uv"]       .as<float>();
  r.current.wind_speed = current["wind_avg"].as<float>(); // default m/s
  r.current.wind_gust  = current["wind_gust"] .as<float>(); // default m/s
  r.current.wind_deg   = current["wind_direction"]  .as<int>();
  r.current.rain_1h    = current["precip_accum_local_day"].as<float>();
  r.current.sunrise    = doc["forecast"]["daily"][0]["sunrise"].as<int64_t>();
  r.current.sunset     = doc["forecast"]["daily"][0]["sunset"].as<int64_t>();
  r.current.weather.description = current["conditions"].as<const char *>();
  r.current.weather.icon        = current["icon"]       .as<const char *>();
  r.current.wbgt = current["wet_bulb_globe_temperature"].as<float>();
  r.current.ltg_1hr = current["lightning_strike_count_last_1hr"].as<int>();
  r.current.ltg_3hr = current["lightning_strike_count_last_3hr"].as<int>();
  i = 0;
  for (JsonObject hourly : doc["forecast"]["hourly"].as<JsonArray>())
  {
    r.hourly[i].dt         = hourly["time"]        .as<int64_t>();
    r.hourly[i].temp       = hourly["air_temperature"]      .as<float>();
    r.hourly[i].feels_like = hourly["feels_like"].as<float>();
    r.hourly[i].pressure   = hourly["sea_level_pressure"]  .as<int>();
    r.hourly[i].humidity   = hourly["relative_humidity"]  .as<int>();
    r.hourly[i].uvi        = hourly["uv"]       .as<float>();
    r.hourly[i].wind_speed = hourly["wind_avg"].as<float>();
    r.hourly[i].wind_gust  = hourly["wind_gust"] .as<float>();
    r.hourly[i].wind_deg   = hourly["wind_direction"]  .as<int>();
    r.hourly[i].pop        = hourly["precip_probablility"]       .as<float>();
    r.hourly[i].rain_1h    = hourly["precip"].as<float>();
    r.hourly[i].weather.description = hourly["conditions"].as<const char *>();
    r.hourly[i].weather.icon        = hourly["icon"]       .as<const char *>();

    if (i == NUM_HOURS - 1)
    {
      break;
    }
    ++i;
  }

  i = 0;
  for (JsonObject daily : doc["forecast"]["daily"].as<JsonArray>())
  {
    r.daily[i].dt         = daily["day_start_local"]        .as<int64_t>();
    r.daily[i].sunrise    = daily["sunrise"]   .as<int64_t>();
    r.daily[i].sunset     = daily["sunset"]    .as<int64_t>();
    r.daily[i].temp.min   = daily["air_temp_low"]  .as<float>();
    r.daily[i].temp.max   = daily["air_temp_high"]  .as<float>();
    r.daily[i].pop        = daily["precip_probablility"].as<float>();
    r.daily[i].weather.description = daily["conditions"].as<const char *>();
    r.daily[i].weather.icon        = daily["icon"].as<const char *>();

    if (i == NUM_DAYS - 1)
    {
      break;
    }
    ++i;
  }

#if DISPLAY_ALERTS
  i = 0;
  for (JsonObject alerts : doc["alerts"].as<JsonArray>())
  {
    wx_alerts_t new_alert = {};
    // new_alert.sender_name = alerts["sender_name"].as<const char *>();
    new_alert.event       = alerts["event"]      .as<const char *>();
    new_alert.start       = alerts["start"]      .as<int64_t>();
    new_alert.end         = alerts["end"]        .as<int64_t>();
    // new_alert.description = alerts["description"].as<const char *>();
    r.alerts.push_back(new_alert);

    if (i == NUM_ALERTS - 1)
    {
      break;
    }
    ++i;
  }
#endif

  return error;
} // end deserializeTempestCall
