#include <unity.h>
#include <string.h>

#include "ui/UiOptionalGasProfile.h"

namespace {

using OptionalGasType = DfrOptionalGasSensor::OptionalGasType;

void assertFormatValue(OptionalGasType type, float ppm, const char *expected) {
    char buf[16] = {0};
    UiOptionalGasProfile::formatValue(UiOptionalGasProfile::forType(type), ppm, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING(expected, buf);
}

void assertFormatValueWithDecimals(OptionalGasType type, float ppm, uint8_t decimals, const char *expected) {
    char buf[16] = {0};
    UiOptionalGasProfile::formatValue(UiOptionalGasProfile::forType(type), ppm, decimals, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING(expected, buf);
}

void assertBandContains(OptionalGasType type, uint8_t band, const char *needle) {
    char buf[128] = {0};
    UiOptionalGasProfile::formatBandLabel(UiOptionalGasProfile::forType(type), band, buf, sizeof(buf));
    TEST_ASSERT_NOT_NULL(strstr(buf, needle));
}

} // namespace

void setUp() {}

void tearDown() {}

void test_optional_gas_values_match_dfrobot_resolution() {
    assertFormatValue(OptionalGasType::NH3, 12.6f, "13");
    assertFormatValue(OptionalGasType::H2S, 8.4f, "8");
    assertFormatValue(OptionalGasType::SO2, 7.54f, "7.5");
    assertFormatValue(OptionalGasType::NO2, 5.55f, "5.6");
    assertFormatValue(OptionalGasType::O3, 0.14f, "0.1");
}

void test_optional_gas_values_can_use_source_decimal_places() {
    assertFormatValueWithDecimals(OptionalGasType::O3, 0.23f, 2, "0.23");
    assertFormatValueWithDecimals(OptionalGasType::O3, 0.23f, 1, "0.2");
    assertFormatValueWithDecimals(OptionalGasType::O3, 0.20f, 2, "0.2");
    assertFormatValueWithDecimals(OptionalGasType::O3, 1.24f, 2, "1.2");
    assertFormatValueWithDecimals(OptionalGasType::O3, 1.26f, 2, "1.3");
}

void test_optional_gas_threshold_labels_keep_reference_precision() {
    assertBandContains(OptionalGasType::NH3, 0, "<=5 ppm");
    assertBandContains(OptionalGasType::H2S, 0, "<=0.5 ppm");
    assertBandContains(OptionalGasType::SO2, 0, "<=0.05 ppm");
    assertBandContains(OptionalGasType::O3, 2, ">0.1-0.5 ppm");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_optional_gas_values_match_dfrobot_resolution);
    RUN_TEST(test_optional_gas_values_can_use_source_decimal_places);
    RUN_TEST(test_optional_gas_threshold_labels_keep_reference_precision);
    return UNITY_END();
}
