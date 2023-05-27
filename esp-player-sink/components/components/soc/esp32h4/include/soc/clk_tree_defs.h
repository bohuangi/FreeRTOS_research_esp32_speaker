/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*
 ************************ ESP32H4 Root Clock Source ***************************
 * 1) Internal 8MHz RC Oscillator: RC_FAST (usually referred as FOSC or CK8M/CLK8M in TRM and reg. description)
 *
 *    This RC oscillator generates a ~8.5MHz clock signal output as the RC_FAST_CLK.
 *
 *    The exact frequency of RC_FAST_CLK cannot be computed in runtime through calibration. You can output the RC_FAST
 *    clock signal to a gpio pin in order to get the frequency through an oscillscope or a logic analyzer.
 *
 * 2) External 32MHz Crystal Clock: XTAL
 *
 * 3) Internal 136kHz RC Oscillator: RC_SLOW (usually referrred as RTC in TRM or reg. description)
 *
 *    This RC oscillator generates a ~136kHz clock signal output as the RC_SLOW_CLK. The exact frequency of this clock
 *    can be computed in runtime through calibration.
 *
 * 4) External 32kHz Crystal Clock (optional): XTAL32K
 *
 *    The clock source for this XTAL32K_CLK can be either a 32kHz crystal connecting to the XTAL_32K_P and XTAL_32K_N
 *    pins or a 32kHz clock signal generated by an external circuit. The external signal must be connected to the
 *    XTAL_32K_P pin.
 *
 *    XTAL32K_CLK can also be calibrated to get its exact frequency.
 *
 * 5) Internal 32kHz RC Oscillator: RC32K
 *
 *    The exact frequency of this clock can be computed in runtime through calibration.
 */

/* With the default value of CK8M_DFREQ = 600, RC_FAST clock frequency nears 7 MHz +/- 7% */  //<---- DFREQ to be adjusted! */
#define SOC_CLK_RC_FAST_FREQ_APPROX         7000000      /*!< Approximate RC_FAST_CLK frequency in Hz */
/* With the default value of DCAP = 128 */                                                    //<---- DCAP to be adjusted!
#define SOC_CLK_RC_SLOW_FREQ_APPROX         130000       /*!< Approximate RC_SLOW_CLK frequency in Hz */
/* With the default value of DFREQ = 707  */                                                  //<---- DFREQ to be adjusted!
#define SOC_CLK_RC32K_FREQ_APPROX           32768        /*!< Approximate RC32K_CLK frequency in Hz */
#define SOC_CLK_XTAL32K_FREQ_APPROX         32768        /*!< Approximate XTAL32K_CLK frequency in Hz */

// Naming convention: SOC_ROOT_CLK_{loc}_{type}_[attr]
// {loc}: EXT, INT
// {type}: XTAL, RC
// [attr] - optional: [frequency], FAST, SLOW
/**
 * @brief Root clock
 */
typedef enum {
    SOC_ROOT_CLK_INT_RC_FAST,          /*!< Internal 8MHz RC oscillator */
    SOC_ROOT_CLK_INT_RC_SLOW,          /*!< Internal 136kHz RC oscillator */
    SOC_ROOT_CLK_EXT_XTAL,             /*!< External 32MHz crystal */
    SOC_ROOT_CLK_EXT_XTAL32K,          /*!< External 32kHz crystal/clock signal */
    SOC_ROOT_CLK_INT_RC32K             /*!< Internal 32kHz RC oscillator */
} soc_root_clk_t;

/**
 * @brief CPU_CLK mux inputs, which are the supported clock sources for the CPU_CLK
 * @note Enum values are matched with the register field values on purpose
 */
typedef enum {
    SOC_CPU_CLK_SRC_XTAL = 0,              /*!< Select XTAL_CLK as CPU_CLK source */
    SOC_CPU_CLK_SRC_PLL = 1,               /*!< Select PLL_CLK as CPU_CLK source (PLL_CLK is the output of 32MHz crystal oscillator frequency multiplier, 96MHz) */
    SOC_CPU_CLK_SRC_RC_FAST = 2,           /*!< Select RC_FAST_CLK as CPU_CLK source */
    SOC_CPU_CLK_SRC_XTAL_D2 = 3,           /*!< Select XTAL_D2_CLK as CPU_CLK source */
    SOC_CPU_CLK_SRC_INVALID,               /*!< Invalid CPU_CLK source */
} soc_cpu_clk_src_t;

/**
 * @brief RTC_SLOW_CLK mux inputs, which are the supported clock sources for the RTC_SLOW_CLK
 * @note Enum values are matched with the register field values on purpose
 */
typedef enum {
    SOC_RTC_SLOW_CLK_SRC_RC_SLOW = 0,      /*!< Select RC_SLOW_CLK as RTC_SLOW_CLK source */
    SOC_RTC_SLOW_CLK_SRC_XTAL32K = 1,      /*!< Select XTAL32K_CLK as RTC_SLOW_CLK source */
    SOC_RTC_SLOW_CLK_SRC_RC32K = 2,        /*!< Select RC32K_CLK as RTC_SLOW_CLK source */
    SOC_RTC_SLOW_CLK_SRC_INVALID,          /*!< Invalid RTC_SLOW_CLK source */
} soc_rtc_slow_clk_src_t;

/**
 * @brief RTC_FAST_CLK mux inputs, which are the supported clock sources for the RTC_FAST_CLK
 * @note Enum values are matched with the register field values on purpose
 */
typedef enum {
    SOC_RTC_FAST_CLK_SRC_XTAL_D2 = 0,      /*!< Select XTAL_D2_CLK (may referred as XTAL_CLK_DIV_2) as RTC_FAST_CLK source */
    SOC_RTC_FAST_CLK_SRC_XTAL_DIV = SOC_RTC_FAST_CLK_SRC_XTAL_D2, /*!< Alias name for `SOC_RTC_FAST_CLK_SRC_XTAL_D2` */
    SOC_RTC_FAST_CLK_SRC_RC_FAST = 1,      /*!< Select RC_FAST_CLK as RTC_FAST_CLK source */
    SOC_RTC_FAST_CLK_SRC_INVALID,          /*!< Invalid RTC_FAST_CLK source */
} soc_rtc_fast_clk_src_t;

// Naming convention: SOC_MOD_CLK_{[upstream]clock_name}_[attr]
// {[upstream]clock_name}: AHB etc.
// [attr] - optional: FAST, SLOW, D<divider>, F<freq>
/**
 * @brief Supported clock sources for modules (CPU, peripherals, RTC, etc.)
 *
 * @note enum starts from 1, to save 0 for special purpose
 */
typedef enum {
    // For CPU domain
    SOC_MOD_CLK_CPU = 1,                       /*!< CPU_CLK can be sourced from XTAL, PLL, RC_FAST, or XTAL_D2 by configuring soc_cpu_clk_src_t */
    // For RTC domain
    SOC_MOD_CLK_RTC_FAST,                      /*!< RTC_FAST_CLK can be sourced from XTAL_D2 or RC_FAST by configuring soc_rtc_fast_clk_src_t */
    SOC_MOD_CLK_RTC_SLOW,                      /*!< RTC_SLOW_CLK can be sourced from RC_SLOW, XTAL32K, or RC32K by configuring soc_rtc_slow_clk_src_t */
    // For digital domain: peripherals, WIFI, BLE
    SOC_MOD_CLK_AHB,                           /*< AHB_CLK sources from CPU with a configurable divider */
    SOC_MOD_CLK_APB,                           /*< APB_CLK source is derived from AHB clock */
    SOC_MOD_CLK_XTAL32K,                       /*< XTAL32K_CLK comes from the external 32kHz crystal, passing a clock gating to the peripherals */
    SOC_MOD_CLK_RC_FAST,                       /*< RC_FAST_CLK comes from the internal 8MHz rc oscillator, passing a clock gating to the peripherals */
    SOC_MOD_CLK_XTAL,                          /*< XTAL_CLK comes from the external 32MHz crystal */
    SOC_MOD_CLK_PLL,                           /*< PLL_CLK is the output of 32MHz crystal oscillator frequency multiplier, 96MHz */
} soc_module_clk_t;


//////////////////////////////////////////////////GPTimer///////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of GPTimer
 *
 * The following code can be used to iterate all possible clocks:
 * @code{c}
 * soc_periph_gptimer_clk_src_t gptimer_clks[] = (soc_periph_gptimer_clk_src_t)SOC_GPTIMER_CLKS;
 * for (size_t i = 0; i< sizeof(gptimer_clks) / sizeof(gptimer_clks[0]); i++) {
 *     soc_periph_gptimer_clk_src_t clk = gptimer_clks[i];
 *     // Test GPTimer with the clock `clk`
 * }
 * @endcode
 */
#define SOC_GPTIMER_CLKS {SOC_MOD_CLK_AHB, SOC_MOD_CLK_XTAL}

/**
 * @brief Type of GPTimer clock source
 */
typedef enum {
    GPTIMER_CLK_SRC_AHB = SOC_MOD_CLK_AHB,     /*!< Select AHB as the source clock */
    GPTIMER_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,   /*!< Select XTAL as the source clock */
    GPTIMER_CLK_SRC_DEFAULT = SOC_MOD_CLK_AHB, /*!< Select AHB as the default choice */
} soc_periph_gptimer_clk_src_t;

/**
 * @brief Type of Timer Group clock source, reserved for the legacy timer group driver
 */
typedef enum {
    TIMER_SRC_CLK_AHB = SOC_MOD_CLK_AHB,     /*!< Timer group clock source is AHB */
    TIMER_SRC_CLK_XTAL = SOC_MOD_CLK_XTAL,   /*!< Timer group clock source is XTAL */
    TIMER_SRC_CLK_DEFAULT = SOC_MOD_CLK_AHB, /*!< Timer group clock source default choice is AHB */
} soc_periph_tg_clk_src_legacy_t;

//////////////////////////////////////////////////RMT///////////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of RMT
 */
#define SOC_RMT_CLKS {SOC_MOD_CLK_AHB, SOC_MOD_CLK_XTAL}

/**
 * @brief Type of RMT clock source
 */
typedef enum {
    RMT_CLK_SRC_AHB = SOC_MOD_CLK_AHB,         /*!< Select AHB clock as the source clock */
    RMT_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,       /*!< Select XTAL as the source clock */
    RMT_CLK_SRC_DEFAULT = SOC_MOD_CLK_AHB,     /*!< Select AHB as the default choice */
} soc_periph_rmt_clk_src_t;

/**
 * @brief Type of RMT clock source, reserved for the legacy RMT driver
 */
typedef enum {
    RMT_BASECLK_AHB = SOC_MOD_CLK_AHB,     /*!< RMT source clock is AHB */
    RMT_BASECLK_XTAL = SOC_MOD_CLK_XTAL,   /*!< RMT source clock is XTAL */
    RMT_BASECLK_DEFAULT = SOC_MOD_CLK_AHB, /*!< RMT source clock default choice is AHB */
} soc_periph_rmt_clk_src_legacy_t;

//////////////////////////////////////////////////Temp Sensor///////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of Temperature Sensor
 */
#define SOC_TEMP_SENSOR_CLKS {SOC_MOD_CLK_XTAL, SOC_MOD_CLK_RC_FAST}

/**
 * @brief Type of Temp Sensor clock source
 */
typedef enum {
    TEMPERATURE_SENSOR_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,       /*!< Select XTAL as the source clock */
    TEMPERATURE_SENSOR_CLK_SRC_RC_FAST = SOC_MOD_CLK_RC_FAST, /*!< Select RC_FAST as the source clock */
    TEMPERATURE_SENSOR_CLK_SRC_DEFAULT = SOC_MOD_CLK_XTAL,    /*!< Select XTAL as the default choice */
} soc_periph_temperature_sensor_clk_src_t;

///////////////////////////////////////////////////UART/////////////////////////////////////////////////////////////////

/**
 * @brief Type of UART clock source, reserved for the legacy UART driver
 */
typedef enum {
    UART_SCLK_AHB = SOC_MOD_CLK_AHB,     /*!< UART source clock is AHB CLK */
    UART_SCLK_RTC = SOC_MOD_CLK_RC_FAST, /*!< UART source clock is RC_FAST */
    UART_SCLK_XTAL = SOC_MOD_CLK_XTAL,   /*!< UART source clock is XTAL */
    UART_SCLK_DEFAULT = SOC_MOD_CLK_AHB, /*!< UART source clock default choice is AHB */
} soc_periph_uart_clk_src_legacy_t;

///////////////////////////////////////////////////// I2S //////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of
 */
#define SOC_I2S_CLKS {SOC_MOD_CLK_PLL, SOC_MOD_CLK_XTAL}

/**
 * @brief I2S clock source enum
 */
typedef enum {
    I2S_CLK_SRC_DEFAULT = SOC_MOD_CLK_PLL,        /*!< Select SOC_MOD_CLK_PLL as the default source clock  */
    I2S_CLK_SRC_PLL_96M = SOC_MOD_CLK_PLL,        /*!< Select PLL as the source clock */
    I2S_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,          /*!< Select XTAL as the source clock */
} soc_periph_i2s_clk_src_t;

/////////////////////////////////////////////////I2C////////////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of I2C
 */
#define SOC_I2C_CLKS {SOC_MOD_CLK_XTAL, SOC_MOD_CLK_RC_FAST}

/**
 * @brief Type of I2C clock source.
 */
typedef enum {
    I2C_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,
    I2C_CLK_SRC_RC_FAST = SOC_MOD_CLK_RC_FAST,
    I2C_CLK_SRC_DEFAULT = SOC_MOD_CLK_XTAL,
} soc_periph_i2c_clk_src_t;

//////////////////////////////////////////////////SDM//////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of SDM
 */
#define SOC_SDM_CLKS {SOC_MOD_CLK_APB}

/**
 * @brief Sigma Delta Modulator clock source
 */
typedef enum {
    SDM_CLK_SRC_APB = SOC_MOD_CLK_APB,     /*!< Select APB as the source clock */
    SDM_CLK_SRC_DEFAULT = SOC_MOD_CLK_APB, /*!< Select APB as the default clock choice */
} soc_periph_sdm_clk_src_t;

//////////////////////////////////////////////////TWAI/////////////////////////////////////////////////////////////////

/**
 * @brief Array initializer for all supported clock sources of TWAI
 */
#define SOC_TWAI_CLKS {SOC_MOD_CLK_APB}

/**
 * @brief TWAI clock source
 */
typedef enum {
    TWAI_CLK_SRC_APB = SOC_MOD_CLK_APB,     /*!< Select APB as the source clock */
    TWAI_CLK_SRC_DEFAULT = SOC_MOD_CLK_APB, /*!< Select APB as the default clock choice */
} soc_periph_twai_clk_src_t;

#ifdef __cplusplus
}
#endif
