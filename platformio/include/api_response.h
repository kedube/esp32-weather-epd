/* API response deserialization declarations for esp32-weather-epd.
 * Copyright (C) 2022-2023  Luke Marzen
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

#ifndef __API_RESPONSE_H__
#define __API_RESPONSE_H__

#include <cstdint>
#include <vector>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>

#define NUM_HOURS        48 // 48
#define NUM_DAYS          8 // 8
#define NUM_ALERTS         8 // OpenWeatherMaps does not specify a limit, but if you need more alerts you are probably doomed.

typedef struct weather_condition
{
  String  description;      // Weather condition within the group (full list of weather conditions). Get the output in your language
  String  icon;             // Weather icon id.
} weather_condition_t;

/*
 * Units – default: kelvin, metric: Celsius, imperial: Fahrenheit.
 */
typedef struct wx_temp
{
  float   min;              // Min daily temperature.
  float   max;              // Max daily temperature.
} wx_temp_t;


/*
 * Current weather data API response
 */
typedef struct wx_current
{
  int64_t dt;               // Current time, Unix, UTC
  int64_t sunrise;          // Sunrise time, Unix, UTC
  int64_t sunset;           // Sunset time, Unix, UTC
  float   temp;             // Temperature. Units - default: Celsius
  float   feels_like;       // Temperature. This temperature parameter accounts for the human perception of weather. Units – Celsius
  int     pressure;         // Atmospheric pressure on the sea level, mb
  int     humidity;         // Humidity, %
  float   dew_point;        // Atmospheric temperature (varying according to pressure and humidity) below which water droplets begin to condense and dew can form. Units – default: Celsius
  float   solar_radiation;  // Current Solar Radiation
  float   uvi;              // Current UV index
  float   wind_speed;       // Wind speed. Wind speed. Units – default: metre/sec,
  float   wind_gust;        // (where available) Wind gust. Units – default: metre/sec
  int     wind_deg;         // Wind direction, degrees (meteorological)
  float   rain_day;          // (where available) Rain volume for local day, mm
  float   wbgt;             // Wetbulb Globe Temp, Celsius
  int64_t last_strike_time;
  weather_condition_t         weather;
} wx_current_t;

/*
 * Hourly forecast weather data API response
 */
typedef struct wx_hourly
{
  int64_t dt;               // Time of the forecasted data, unix, UTC
  float   temp;             // Temperature. Units - default: kelvin, metric: Celsius, imperial: Fahrenheit.
  float   feels_like;       // Temperature. This temperature parameter accounts for the human perception of weather. Units – default: kelvin, metric: Celsius, imperial: Fahrenheit.
  int     pressure;         // Atmospheric pressure on the sea level, hPa
  int     humidity;         // Humidity, %
  float   uvi;              // Current UV index
  float   wind_speed;       // Wind speed. Wind speed. Units – default: metre/sec, metric: metre/sec, imperial: miles/hour.
  float   wind_gust;        // (where available) Wind gust. Units – default: metre/sec, metric: metre/sec, imperial: miles/hour.
  int     wind_deg;         // Wind direction, degrees (meteorological)
  float   pop;              // Probability of precipitation. The values of the parameter vary between 0 and 1, where 0 is equal to 0%, 1 is equal to 100%
  float   rain_1h;          // (where available) Rain volume for last hour, mm
  weather_condition_t         weather;
} wx_hourly_t;

/*
 * Daily forecast weather data API response
 */
typedef struct wx_daily
{
  int64_t dt;               // Time of the forecasted data, unix, UTC
  int64_t sunrise;          // Sunrise time, Unix, UTC
  int64_t sunset;           // Sunset time, Unix, UTC
  wx_temp_t            temp;
  float   pop;              // Probability of precipitation. The values of the parameter vary between 0 and 1, where 0 is equal to 0%, 1 is equal to 100%
  weather_condition_t         weather;
} wx_daily_t;

/*
 * National weather alerts data from major national weather warning systems
 */
typedef struct wx_alerts
{
  String  sender_name;      // Name of the alert source.
  String  event;            // Alert event name
  int64_t start;            // Date and time of the start of the alert, Unix, UTC
  int64_t end;              // Date and time of the end of the alert, Unix, UTC
  String  description;      // Description of the alert
} wx_alerts_t;

/*
 * Response from Tempest Better Forecast API
 *
 * https://weatherflow.github.io/Tempest/api/swagger/#!/forecast/getBetterForecast
 */
typedef struct tempest_resp
{
  float   lat;              // Geographical coordinates of the location (latitude)
  float   lon;              // Geographical coordinates of the location (longitude)
  String  timezone;         // Timezone name for the requested location
  int     timezone_offset;  // Shift in seconds from UTC
  wx_current_t   current;

  wx_hourly_t    hourly[NUM_HOURS];
  wx_daily_t     daily[NUM_DAYS];
  std::vector<wx_alerts_t> alerts;
} tempest_resp_t;

/*
 * Coordinates from the specified location (latitude, longitude)
 */
typedef struct coordinates
{
  float   lat;
  float   lon;
} coordinates_t;

DeserializationError deserializeNWSCall(Stream &json,
                                        tempest_resp_t &r);

DeserializationError deserializeTempestCall(Stream &json,
                                        tempest_resp_t &r);

#endif

