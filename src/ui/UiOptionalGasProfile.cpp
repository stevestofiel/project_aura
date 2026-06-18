// SPDX-FileCopyrightText: 2025-2026 Volodymyr Papush (21CNCStudio)
// SPDX-License-Identifier: GPL-3.0-or-later
// GPL-3.0-or-later: https://www.gnu.org/licenses/gpl-3.0.html
// Want to use this code in a commercial product while keeping modifications proprietary?
// Purchase a Commercial License: see COMMERCIAL_LICENSE_SUMMARY.md

#include "ui/UiOptionalGasProfile.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

namespace UiOptionalGasProfile {
namespace {

constexpr Profile kFallbackProfile{
    OptionalGasType::None,
    "Gas",
    "Optional gas",
    1,
    1,
    1.0f,
    2.0f,
    3.0f,
    0.0f,
    1.0f,
    100.0f,
};

// Keep in sync with dashboard OPTIONAL_GAS_PROFILES until these values are served by firmware state/API.
constexpr Profile kProfiles[] = {
    {OptionalGasType::NH3, "NH3", "Ammonia (NH3)", 0, 0, 5.0f, 25.0f, 35.0f, 5.0f, 10.0f, 100.0f},
    {OptionalGasType::SO2, "SO2", "Sulfur dioxide (SO2)", 1, 2, 0.05f, 0.10f, 2.0f, 0.05f, 0.5f, 100.0f},
    {OptionalGasType::NO2, "NO2", "Nitrogen dioxide (NO2)", 1, 2, 0.05f, 0.10f, 1.0f, 0.05f, 0.3f, 100.0f},
    {OptionalGasType::H2S, "H2S", "Hydrogen sulfide (H2S)", 0, 1, 0.5f, 1.0f, 10.0f, 0.5f, 2.0f, 100.0f},
    {OptionalGasType::O3, "O3", "Ozone (O3)", 1, 2, 0.05f, 0.10f, 0.50f, 0.05f, 0.2f, 100.0f},
};

void trim_decimal(char *buf) {
    if (!buf) {
        return;
    }
    char *dot = strchr(buf, '.');
    if (!dot) {
        return;
    }
    char *end = buf + strlen(buf);
    while (end > dot + 1 && *(end - 1) == '0') {
        --end;
        *end = '\0';
    }
    if (end == dot + 1 && *dot == '.') {
        *dot = '\0';
    }
}

void format_number(float value, uint8_t decimals, char *buf, size_t buf_size) {
    if (!buf || buf_size == 0) {
        return;
    }
    if (!isfinite(value)) {
        snprintf(buf, buf_size, "--");
        return;
    }
    switch (decimals) {
        case 0:
            snprintf(buf, buf_size, "%.0f", value);
            break;
        case 1:
            snprintf(buf, buf_size, "%.1f", value);
            break;
        case 2:
        default:
            snprintf(buf, buf_size, "%.2f", value);
            break;
    }
    trim_decimal(buf);
}

uint8_t display_decimals_for_value(float value, uint8_t decimals, uint8_t fallback) {
    uint8_t normalized = fallback <= 2 ? fallback : 1;
    if (decimals <= 2) {
        normalized = decimals;
    }
    if (normalized == 2 && isfinite(value) && value >= 1.0f) {
        return 1;
    }
    return normalized;
}

} // namespace

const Profile &forType(OptionalGasType type) {
    for (const Profile &profile : kProfiles) {
        if (profile.type == type) {
            return profile;
        }
    }
    return kFallbackProfile;
}

bool isKnown(OptionalGasType type) {
    return forType(type).type != OptionalGasType::None;
}

void formatValue(const Profile &profile, float ppm, char *buf, size_t buf_size) {
    format_number(ppm, profile.value_decimals, buf, buf_size);
}

void formatValue(const Profile &profile, float ppm, uint8_t decimals, char *buf, size_t buf_size) {
    format_number(ppm,
                  display_decimals_for_value(ppm, decimals, profile.value_decimals),
                  buf,
                  buf_size);
}

void formatThreshold(const Profile &profile, float ppm, char *buf, size_t buf_size) {
    format_number(ppm, profile.threshold_decimals, buf, buf_size);
}

void formatBandLabel(const Profile &profile, uint8_t band, char *buf, size_t buf_size) {
    if (!buf || buf_size == 0) {
        return;
    }

    char green[16];
    char yellow[16];
    char orange[16];
    formatThreshold(profile, profile.green_max_ppm, green, sizeof(green));
    formatThreshold(profile, profile.yellow_max_ppm, yellow, sizeof(yellow));
    formatThreshold(profile, profile.orange_max_ppm, orange, sizeof(orange));

    switch (band) {
        case 0:
            snprintf(buf,
                     buf_size,
                     "Low: <=%s ppm\nLowest reference band for %s",
                     green,
                     profile.label);
            break;
        case 1:
            snprintf(buf,
                     buf_size,
                     "Slight elevation: >%s-%s ppm\nKeep air moving and watch trend",
                     green,
                     yellow);
            break;
        case 2:
            snprintf(buf,
                     buf_size,
                     "Elevated: >%s-%s ppm\nVentilate and check source",
                     yellow,
                     orange);
            break;
        case 3:
        default:
            snprintf(buf,
                     buf_size,
                     "High: >%s ppm\nReduce exposure; verify with safety equipment",
                     orange);
            break;
    }
}

} // namespace UiOptionalGasProfile
