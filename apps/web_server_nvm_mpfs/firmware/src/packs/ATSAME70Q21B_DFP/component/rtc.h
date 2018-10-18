/**
 * \brief Header file for SAME/SAME70 RTC
 *
 * Copyright (c) 2017-2018 Microchip Technology Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/* file generated from device description version 2018-09-19T14:04:45Z */
#ifndef SAME_SAME70_RTC_MODULE_H
#define SAME_SAME70_RTC_MODULE_H

/** \addtogroup SAME_SAME70 Real-time Clock
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR SAME70_RTC                                   */
/* ========================================================================== */

/* -------- RTC_CR : (RTC Offset: 0x00) (R/W 32) Control Register -------- */
#define RTC_CR_UPDTIM_Pos                     (0U)                                           /**< (RTC_CR) Update Request Time Register Position */
#define RTC_CR_UPDTIM_Msk                     (0x1U << RTC_CR_UPDTIM_Pos)                    /**< (RTC_CR) Update Request Time Register Mask */
#define RTC_CR_UPDTIM(value)                  (RTC_CR_UPDTIM_Msk & ((value) << RTC_CR_UPDTIM_Pos))
#define RTC_CR_UPDCAL_Pos                     (1U)                                           /**< (RTC_CR) Update Request Calendar Register Position */
#define RTC_CR_UPDCAL_Msk                     (0x1U << RTC_CR_UPDCAL_Pos)                    /**< (RTC_CR) Update Request Calendar Register Mask */
#define RTC_CR_UPDCAL(value)                  (RTC_CR_UPDCAL_Msk & ((value) << RTC_CR_UPDCAL_Pos))
#define RTC_CR_TIMEVSEL_Pos                   (8U)                                           /**< (RTC_CR) Time Event Selection Position */
#define RTC_CR_TIMEVSEL_Msk                   (0x3U << RTC_CR_TIMEVSEL_Pos)                  /**< (RTC_CR) Time Event Selection Mask */
#define RTC_CR_TIMEVSEL(value)                (RTC_CR_TIMEVSEL_Msk & ((value) << RTC_CR_TIMEVSEL_Pos))
#define   RTC_CR_TIMEVSEL_MINUTE_Val          (0U)                                           /**< (RTC_CR) Minute change  */
#define   RTC_CR_TIMEVSEL_HOUR_Val            (1U)                                           /**< (RTC_CR) Hour change  */
#define   RTC_CR_TIMEVSEL_MIDNIGHT_Val        (2U)                                           /**< (RTC_CR) Every day at midnight  */
#define   RTC_CR_TIMEVSEL_NOON_Val            (3U)                                           /**< (RTC_CR) Every day at noon  */
#define RTC_CR_TIMEVSEL_MINUTE                (RTC_CR_TIMEVSEL_MINUTE_Val << RTC_CR_TIMEVSEL_Pos) /**< (RTC_CR) Minute change Position  */
#define RTC_CR_TIMEVSEL_HOUR                  (RTC_CR_TIMEVSEL_HOUR_Val << RTC_CR_TIMEVSEL_Pos) /**< (RTC_CR) Hour change Position  */
#define RTC_CR_TIMEVSEL_MIDNIGHT              (RTC_CR_TIMEVSEL_MIDNIGHT_Val << RTC_CR_TIMEVSEL_Pos) /**< (RTC_CR) Every day at midnight Position  */
#define RTC_CR_TIMEVSEL_NOON                  (RTC_CR_TIMEVSEL_NOON_Val << RTC_CR_TIMEVSEL_Pos) /**< (RTC_CR) Every day at noon Position  */
#define RTC_CR_CALEVSEL_Pos                   (16U)                                          /**< (RTC_CR) Calendar Event Selection Position */
#define RTC_CR_CALEVSEL_Msk                   (0x3U << RTC_CR_CALEVSEL_Pos)                  /**< (RTC_CR) Calendar Event Selection Mask */
#define RTC_CR_CALEVSEL(value)                (RTC_CR_CALEVSEL_Msk & ((value) << RTC_CR_CALEVSEL_Pos))
#define   RTC_CR_CALEVSEL_WEEK_Val            (0U)                                           /**< (RTC_CR) Week change (every Monday at time 00:00:00)  */
#define   RTC_CR_CALEVSEL_MONTH_Val           (1U)                                           /**< (RTC_CR) Month change (every 01 of each month at time 00:00:00)  */
#define   RTC_CR_CALEVSEL_YEAR_Val            (2U)                                           /**< (RTC_CR) Year change (every January 1 at time 00:00:00)  */
#define RTC_CR_CALEVSEL_WEEK                  (RTC_CR_CALEVSEL_WEEK_Val << RTC_CR_CALEVSEL_Pos) /**< (RTC_CR) Week change (every Monday at time 00:00:00) Position  */
#define RTC_CR_CALEVSEL_MONTH                 (RTC_CR_CALEVSEL_MONTH_Val << RTC_CR_CALEVSEL_Pos) /**< (RTC_CR) Month change (every 01 of each month at time 00:00:00) Position  */
#define RTC_CR_CALEVSEL_YEAR                  (RTC_CR_CALEVSEL_YEAR_Val << RTC_CR_CALEVSEL_Pos) /**< (RTC_CR) Year change (every January 1 at time 00:00:00) Position  */
#define RTC_CR_Msk                            (0x00030303UL)                                 /**< (RTC_CR) Register Mask  */

/* -------- RTC_MR : (RTC Offset: 0x04) (R/W 32) Mode Register -------- */
#define RTC_MR_HRMOD_Pos                      (0U)                                           /**< (RTC_MR) 12-/24-hour Mode Position */
#define RTC_MR_HRMOD_Msk                      (0x1U << RTC_MR_HRMOD_Pos)                     /**< (RTC_MR) 12-/24-hour Mode Mask */
#define RTC_MR_HRMOD(value)                   (RTC_MR_HRMOD_Msk & ((value) << RTC_MR_HRMOD_Pos))
#define RTC_MR_PERSIAN_Pos                    (1U)                                           /**< (RTC_MR) PERSIAN Calendar Position */
#define RTC_MR_PERSIAN_Msk                    (0x1U << RTC_MR_PERSIAN_Pos)                   /**< (RTC_MR) PERSIAN Calendar Mask */
#define RTC_MR_PERSIAN(value)                 (RTC_MR_PERSIAN_Msk & ((value) << RTC_MR_PERSIAN_Pos))
#define RTC_MR_NEGPPM_Pos                     (4U)                                           /**< (RTC_MR) NEGative PPM Correction Position */
#define RTC_MR_NEGPPM_Msk                     (0x1U << RTC_MR_NEGPPM_Pos)                    /**< (RTC_MR) NEGative PPM Correction Mask */
#define RTC_MR_NEGPPM(value)                  (RTC_MR_NEGPPM_Msk & ((value) << RTC_MR_NEGPPM_Pos))
#define RTC_MR_CORRECTION_Pos                 (8U)                                           /**< (RTC_MR) Slow Clock Correction Position */
#define RTC_MR_CORRECTION_Msk                 (0x7FU << RTC_MR_CORRECTION_Pos)               /**< (RTC_MR) Slow Clock Correction Mask */
#define RTC_MR_CORRECTION(value)              (RTC_MR_CORRECTION_Msk & ((value) << RTC_MR_CORRECTION_Pos))
#define RTC_MR_HIGHPPM_Pos                    (15U)                                          /**< (RTC_MR) HIGH PPM Correction Position */
#define RTC_MR_HIGHPPM_Msk                    (0x1U << RTC_MR_HIGHPPM_Pos)                   /**< (RTC_MR) HIGH PPM Correction Mask */
#define RTC_MR_HIGHPPM(value)                 (RTC_MR_HIGHPPM_Msk & ((value) << RTC_MR_HIGHPPM_Pos))
#define RTC_MR_OUT0_Pos                       (16U)                                          /**< (RTC_MR) RTCOUT0 OutputSource Selection Position */
#define RTC_MR_OUT0_Msk                       (0x7U << RTC_MR_OUT0_Pos)                      /**< (RTC_MR) RTCOUT0 OutputSource Selection Mask */
#define RTC_MR_OUT0(value)                    (RTC_MR_OUT0_Msk & ((value) << RTC_MR_OUT0_Pos))
#define   RTC_MR_OUT0_NO_WAVE_Val             (0U)                                           /**< (RTC_MR) No waveform, stuck at '0'  */
#define   RTC_MR_OUT0_FREQ1HZ_Val             (1U)                                           /**< (RTC_MR) 1 Hz square wave  */
#define   RTC_MR_OUT0_FREQ32HZ_Val            (2U)                                           /**< (RTC_MR) 32 Hz square wave  */
#define   RTC_MR_OUT0_FREQ64HZ_Val            (3U)                                           /**< (RTC_MR) 64 Hz square wave  */
#define   RTC_MR_OUT0_FREQ512HZ_Val           (4U)                                           /**< (RTC_MR) 512 Hz square wave  */
#define   RTC_MR_OUT0_ALARM_TOGGLE_Val        (5U)                                           /**< (RTC_MR) Output toggles when alarm flag rises  */
#define   RTC_MR_OUT0_ALARM_FLAG_Val          (6U)                                           /**< (RTC_MR) Output is a copy of the alarm flag  */
#define   RTC_MR_OUT0_PROG_PULSE_Val          (7U)                                           /**< (RTC_MR) Duty cycle programmable pulse  */
#define RTC_MR_OUT0_NO_WAVE                   (RTC_MR_OUT0_NO_WAVE_Val << RTC_MR_OUT0_Pos)   /**< (RTC_MR) No waveform, stuck at '0' Position  */
#define RTC_MR_OUT0_FREQ1HZ                   (RTC_MR_OUT0_FREQ1HZ_Val << RTC_MR_OUT0_Pos)   /**< (RTC_MR) 1 Hz square wave Position  */
#define RTC_MR_OUT0_FREQ32HZ                  (RTC_MR_OUT0_FREQ32HZ_Val << RTC_MR_OUT0_Pos)  /**< (RTC_MR) 32 Hz square wave Position  */
#define RTC_MR_OUT0_FREQ64HZ                  (RTC_MR_OUT0_FREQ64HZ_Val << RTC_MR_OUT0_Pos)  /**< (RTC_MR) 64 Hz square wave Position  */
#define RTC_MR_OUT0_FREQ512HZ                 (RTC_MR_OUT0_FREQ512HZ_Val << RTC_MR_OUT0_Pos) /**< (RTC_MR) 512 Hz square wave Position  */
#define RTC_MR_OUT0_ALARM_TOGGLE              (RTC_MR_OUT0_ALARM_TOGGLE_Val << RTC_MR_OUT0_Pos) /**< (RTC_MR) Output toggles when alarm flag rises Position  */
#define RTC_MR_OUT0_ALARM_FLAG                (RTC_MR_OUT0_ALARM_FLAG_Val << RTC_MR_OUT0_Pos) /**< (RTC_MR) Output is a copy of the alarm flag Position  */
#define RTC_MR_OUT0_PROG_PULSE                (RTC_MR_OUT0_PROG_PULSE_Val << RTC_MR_OUT0_Pos) /**< (RTC_MR) Duty cycle programmable pulse Position  */
#define RTC_MR_OUT1_Pos                       (20U)                                          /**< (RTC_MR) RTCOUT1 Output Source Selection Position */
#define RTC_MR_OUT1_Msk                       (0x7U << RTC_MR_OUT1_Pos)                      /**< (RTC_MR) RTCOUT1 Output Source Selection Mask */
#define RTC_MR_OUT1(value)                    (RTC_MR_OUT1_Msk & ((value) << RTC_MR_OUT1_Pos))
#define   RTC_MR_OUT1_NO_WAVE_Val             (0U)                                           /**< (RTC_MR) No waveform, stuck at '0'  */
#define   RTC_MR_OUT1_FREQ1HZ_Val             (1U)                                           /**< (RTC_MR) 1 Hz square wave  */
#define   RTC_MR_OUT1_FREQ32HZ_Val            (2U)                                           /**< (RTC_MR) 32 Hz square wave  */
#define   RTC_MR_OUT1_FREQ64HZ_Val            (3U)                                           /**< (RTC_MR) 64 Hz square wave  */
#define   RTC_MR_OUT1_FREQ512HZ_Val           (4U)                                           /**< (RTC_MR) 512 Hz square wave  */
#define   RTC_MR_OUT1_ALARM_TOGGLE_Val        (5U)                                           /**< (RTC_MR) Output toggles when alarm flag rises  */
#define   RTC_MR_OUT1_ALARM_FLAG_Val          (6U)                                           /**< (RTC_MR) Output is a copy of the alarm flag  */
#define   RTC_MR_OUT1_PROG_PULSE_Val          (7U)                                           /**< (RTC_MR) Duty cycle programmable pulse  */
#define RTC_MR_OUT1_NO_WAVE                   (RTC_MR_OUT1_NO_WAVE_Val << RTC_MR_OUT1_Pos)   /**< (RTC_MR) No waveform, stuck at '0' Position  */
#define RTC_MR_OUT1_FREQ1HZ                   (RTC_MR_OUT1_FREQ1HZ_Val << RTC_MR_OUT1_Pos)   /**< (RTC_MR) 1 Hz square wave Position  */
#define RTC_MR_OUT1_FREQ32HZ                  (RTC_MR_OUT1_FREQ32HZ_Val << RTC_MR_OUT1_Pos)  /**< (RTC_MR) 32 Hz square wave Position  */
#define RTC_MR_OUT1_FREQ64HZ                  (RTC_MR_OUT1_FREQ64HZ_Val << RTC_MR_OUT1_Pos)  /**< (RTC_MR) 64 Hz square wave Position  */
#define RTC_MR_OUT1_FREQ512HZ                 (RTC_MR_OUT1_FREQ512HZ_Val << RTC_MR_OUT1_Pos) /**< (RTC_MR) 512 Hz square wave Position  */
#define RTC_MR_OUT1_ALARM_TOGGLE              (RTC_MR_OUT1_ALARM_TOGGLE_Val << RTC_MR_OUT1_Pos) /**< (RTC_MR) Output toggles when alarm flag rises Position  */
#define RTC_MR_OUT1_ALARM_FLAG                (RTC_MR_OUT1_ALARM_FLAG_Val << RTC_MR_OUT1_Pos) /**< (RTC_MR) Output is a copy of the alarm flag Position  */
#define RTC_MR_OUT1_PROG_PULSE                (RTC_MR_OUT1_PROG_PULSE_Val << RTC_MR_OUT1_Pos) /**< (RTC_MR) Duty cycle programmable pulse Position  */
#define RTC_MR_THIGH_Pos                      (24U)                                          /**< (RTC_MR) High Duration of the Output Pulse Position */
#define RTC_MR_THIGH_Msk                      (0x7U << RTC_MR_THIGH_Pos)                     /**< (RTC_MR) High Duration of the Output Pulse Mask */
#define RTC_MR_THIGH(value)                   (RTC_MR_THIGH_Msk & ((value) << RTC_MR_THIGH_Pos))
#define   RTC_MR_THIGH_H_31MS_Val             (0U)                                           /**< (RTC_MR) 31.2 ms  */
#define   RTC_MR_THIGH_H_16MS_Val             (1U)                                           /**< (RTC_MR) 15.6 ms  */
#define   RTC_MR_THIGH_H_4MS_Val              (2U)                                           /**< (RTC_MR) 3.91 ms  */
#define   RTC_MR_THIGH_H_976US_Val            (3U)                                           /**< (RTC_MR) 976 us  */
#define   RTC_MR_THIGH_H_488US_Val            (4U)                                           /**< (RTC_MR) 488 us  */
#define   RTC_MR_THIGH_H_122US_Val            (5U)                                           /**< (RTC_MR) 122 us  */
#define   RTC_MR_THIGH_H_30US_Val             (6U)                                           /**< (RTC_MR) 30.5 us  */
#define   RTC_MR_THIGH_H_15US_Val             (7U)                                           /**< (RTC_MR) 15.2 us  */
#define RTC_MR_THIGH_H_31MS                   (RTC_MR_THIGH_H_31MS_Val << RTC_MR_THIGH_Pos)  /**< (RTC_MR) 31.2 ms Position  */
#define RTC_MR_THIGH_H_16MS                   (RTC_MR_THIGH_H_16MS_Val << RTC_MR_THIGH_Pos)  /**< (RTC_MR) 15.6 ms Position  */
#define RTC_MR_THIGH_H_4MS                    (RTC_MR_THIGH_H_4MS_Val << RTC_MR_THIGH_Pos)   /**< (RTC_MR) 3.91 ms Position  */
#define RTC_MR_THIGH_H_976US                  (RTC_MR_THIGH_H_976US_Val << RTC_MR_THIGH_Pos) /**< (RTC_MR) 976 us Position  */
#define RTC_MR_THIGH_H_488US                  (RTC_MR_THIGH_H_488US_Val << RTC_MR_THIGH_Pos) /**< (RTC_MR) 488 us Position  */
#define RTC_MR_THIGH_H_122US                  (RTC_MR_THIGH_H_122US_Val << RTC_MR_THIGH_Pos) /**< (RTC_MR) 122 us Position  */
#define RTC_MR_THIGH_H_30US                   (RTC_MR_THIGH_H_30US_Val << RTC_MR_THIGH_Pos)  /**< (RTC_MR) 30.5 us Position  */
#define RTC_MR_THIGH_H_15US                   (RTC_MR_THIGH_H_15US_Val << RTC_MR_THIGH_Pos)  /**< (RTC_MR) 15.2 us Position  */
#define RTC_MR_TPERIOD_Pos                    (28U)                                          /**< (RTC_MR) Period of the Output Pulse Position */
#define RTC_MR_TPERIOD_Msk                    (0x3U << RTC_MR_TPERIOD_Pos)                   /**< (RTC_MR) Period of the Output Pulse Mask */
#define RTC_MR_TPERIOD(value)                 (RTC_MR_TPERIOD_Msk & ((value) << RTC_MR_TPERIOD_Pos))
#define   RTC_MR_TPERIOD_P_1S_Val             (0U)                                           /**< (RTC_MR) 1 second  */
#define   RTC_MR_TPERIOD_P_500MS_Val          (1U)                                           /**< (RTC_MR) 500 ms  */
#define   RTC_MR_TPERIOD_P_250MS_Val          (2U)                                           /**< (RTC_MR) 250 ms  */
#define   RTC_MR_TPERIOD_P_125MS_Val          (3U)                                           /**< (RTC_MR) 125 ms  */
#define RTC_MR_TPERIOD_P_1S                   (RTC_MR_TPERIOD_P_1S_Val << RTC_MR_TPERIOD_Pos) /**< (RTC_MR) 1 second Position  */
#define RTC_MR_TPERIOD_P_500MS                (RTC_MR_TPERIOD_P_500MS_Val << RTC_MR_TPERIOD_Pos) /**< (RTC_MR) 500 ms Position  */
#define RTC_MR_TPERIOD_P_250MS                (RTC_MR_TPERIOD_P_250MS_Val << RTC_MR_TPERIOD_Pos) /**< (RTC_MR) 250 ms Position  */
#define RTC_MR_TPERIOD_P_125MS                (RTC_MR_TPERIOD_P_125MS_Val << RTC_MR_TPERIOD_Pos) /**< (RTC_MR) 125 ms Position  */
#define RTC_MR_Msk                            (0x3777FF13UL)                                 /**< (RTC_MR) Register Mask  */

/* -------- RTC_TIMR : (RTC Offset: 0x08) (R/W 32) Time Register -------- */
#define RTC_TIMR_SEC_Pos                      (0U)                                           /**< (RTC_TIMR) Current Second Position */
#define RTC_TIMR_SEC_Msk                      (0x7FU << RTC_TIMR_SEC_Pos)                    /**< (RTC_TIMR) Current Second Mask */
#define RTC_TIMR_SEC(value)                   (RTC_TIMR_SEC_Msk & ((value) << RTC_TIMR_SEC_Pos))
#define RTC_TIMR_MIN_Pos                      (8U)                                           /**< (RTC_TIMR) Current Minute Position */
#define RTC_TIMR_MIN_Msk                      (0x7FU << RTC_TIMR_MIN_Pos)                    /**< (RTC_TIMR) Current Minute Mask */
#define RTC_TIMR_MIN(value)                   (RTC_TIMR_MIN_Msk & ((value) << RTC_TIMR_MIN_Pos))
#define RTC_TIMR_HOUR_Pos                     (16U)                                          /**< (RTC_TIMR) Current Hour Position */
#define RTC_TIMR_HOUR_Msk                     (0x3FU << RTC_TIMR_HOUR_Pos)                   /**< (RTC_TIMR) Current Hour Mask */
#define RTC_TIMR_HOUR(value)                  (RTC_TIMR_HOUR_Msk & ((value) << RTC_TIMR_HOUR_Pos))
#define RTC_TIMR_AMPM_Pos                     (22U)                                          /**< (RTC_TIMR) Ante Meridiem Post Meridiem Indicator Position */
#define RTC_TIMR_AMPM_Msk                     (0x1U << RTC_TIMR_AMPM_Pos)                    /**< (RTC_TIMR) Ante Meridiem Post Meridiem Indicator Mask */
#define RTC_TIMR_AMPM(value)                  (RTC_TIMR_AMPM_Msk & ((value) << RTC_TIMR_AMPM_Pos))
#define RTC_TIMR_Msk                          (0x007F7F7FUL)                                 /**< (RTC_TIMR) Register Mask  */

/* -------- RTC_CALR : (RTC Offset: 0x0C) (R/W 32) Calendar Register -------- */
#define RTC_CALR_CENT_Pos                     (0U)                                           /**< (RTC_CALR) Current Century Position */
#define RTC_CALR_CENT_Msk                     (0x7FU << RTC_CALR_CENT_Pos)                   /**< (RTC_CALR) Current Century Mask */
#define RTC_CALR_CENT(value)                  (RTC_CALR_CENT_Msk & ((value) << RTC_CALR_CENT_Pos))
#define RTC_CALR_YEAR_Pos                     (8U)                                           /**< (RTC_CALR) Current Year Position */
#define RTC_CALR_YEAR_Msk                     (0xFFU << RTC_CALR_YEAR_Pos)                   /**< (RTC_CALR) Current Year Mask */
#define RTC_CALR_YEAR(value)                  (RTC_CALR_YEAR_Msk & ((value) << RTC_CALR_YEAR_Pos))
#define RTC_CALR_MONTH_Pos                    (16U)                                          /**< (RTC_CALR) Current Month Position */
#define RTC_CALR_MONTH_Msk                    (0x1FU << RTC_CALR_MONTH_Pos)                  /**< (RTC_CALR) Current Month Mask */
#define RTC_CALR_MONTH(value)                 (RTC_CALR_MONTH_Msk & ((value) << RTC_CALR_MONTH_Pos))
#define RTC_CALR_DAY_Pos                      (21U)                                          /**< (RTC_CALR) Current Day in Current Week Position */
#define RTC_CALR_DAY_Msk                      (0x7U << RTC_CALR_DAY_Pos)                     /**< (RTC_CALR) Current Day in Current Week Mask */
#define RTC_CALR_DAY(value)                   (RTC_CALR_DAY_Msk & ((value) << RTC_CALR_DAY_Pos))
#define RTC_CALR_DATE_Pos                     (24U)                                          /**< (RTC_CALR) Current Day in Current Month Position */
#define RTC_CALR_DATE_Msk                     (0x3FU << RTC_CALR_DATE_Pos)                   /**< (RTC_CALR) Current Day in Current Month Mask */
#define RTC_CALR_DATE(value)                  (RTC_CALR_DATE_Msk & ((value) << RTC_CALR_DATE_Pos))
#define RTC_CALR_Msk                          (0x3FFFFF7FUL)                                 /**< (RTC_CALR) Register Mask  */

/* -------- RTC_TIMALR : (RTC Offset: 0x10) (R/W 32) Time Alarm Register -------- */
#define RTC_TIMALR_SEC_Pos                    (0U)                                           /**< (RTC_TIMALR) Second Alarm Position */
#define RTC_TIMALR_SEC_Msk                    (0x7FU << RTC_TIMALR_SEC_Pos)                  /**< (RTC_TIMALR) Second Alarm Mask */
#define RTC_TIMALR_SEC(value)                 (RTC_TIMALR_SEC_Msk & ((value) << RTC_TIMALR_SEC_Pos))
#define RTC_TIMALR_SECEN_Pos                  (7U)                                           /**< (RTC_TIMALR) Second Alarm Enable Position */
#define RTC_TIMALR_SECEN_Msk                  (0x1U << RTC_TIMALR_SECEN_Pos)                 /**< (RTC_TIMALR) Second Alarm Enable Mask */
#define RTC_TIMALR_SECEN(value)               (RTC_TIMALR_SECEN_Msk & ((value) << RTC_TIMALR_SECEN_Pos))
#define RTC_TIMALR_MIN_Pos                    (8U)                                           /**< (RTC_TIMALR) Minute Alarm Position */
#define RTC_TIMALR_MIN_Msk                    (0x7FU << RTC_TIMALR_MIN_Pos)                  /**< (RTC_TIMALR) Minute Alarm Mask */
#define RTC_TIMALR_MIN(value)                 (RTC_TIMALR_MIN_Msk & ((value) << RTC_TIMALR_MIN_Pos))
#define RTC_TIMALR_MINEN_Pos                  (15U)                                          /**< (RTC_TIMALR) Minute Alarm Enable Position */
#define RTC_TIMALR_MINEN_Msk                  (0x1U << RTC_TIMALR_MINEN_Pos)                 /**< (RTC_TIMALR) Minute Alarm Enable Mask */
#define RTC_TIMALR_MINEN(value)               (RTC_TIMALR_MINEN_Msk & ((value) << RTC_TIMALR_MINEN_Pos))
#define RTC_TIMALR_HOUR_Pos                   (16U)                                          /**< (RTC_TIMALR) Hour Alarm Position */
#define RTC_TIMALR_HOUR_Msk                   (0x3FU << RTC_TIMALR_HOUR_Pos)                 /**< (RTC_TIMALR) Hour Alarm Mask */
#define RTC_TIMALR_HOUR(value)                (RTC_TIMALR_HOUR_Msk & ((value) << RTC_TIMALR_HOUR_Pos))
#define RTC_TIMALR_AMPM_Pos                   (22U)                                          /**< (RTC_TIMALR) AM/PM Indicator Position */
#define RTC_TIMALR_AMPM_Msk                   (0x1U << RTC_TIMALR_AMPM_Pos)                  /**< (RTC_TIMALR) AM/PM Indicator Mask */
#define RTC_TIMALR_AMPM(value)                (RTC_TIMALR_AMPM_Msk & ((value) << RTC_TIMALR_AMPM_Pos))
#define RTC_TIMALR_HOUREN_Pos                 (23U)                                          /**< (RTC_TIMALR) Hour Alarm Enable Position */
#define RTC_TIMALR_HOUREN_Msk                 (0x1U << RTC_TIMALR_HOUREN_Pos)                /**< (RTC_TIMALR) Hour Alarm Enable Mask */
#define RTC_TIMALR_HOUREN(value)              (RTC_TIMALR_HOUREN_Msk & ((value) << RTC_TIMALR_HOUREN_Pos))
#define RTC_TIMALR_Msk                        (0x00FFFFFFUL)                                 /**< (RTC_TIMALR) Register Mask  */

/* -------- RTC_CALALR : (RTC Offset: 0x14) (R/W 32) Calendar Alarm Register -------- */
#define RTC_CALALR_MONTH_Pos                  (16U)                                          /**< (RTC_CALALR) Month Alarm Position */
#define RTC_CALALR_MONTH_Msk                  (0x1FU << RTC_CALALR_MONTH_Pos)                /**< (RTC_CALALR) Month Alarm Mask */
#define RTC_CALALR_MONTH(value)               (RTC_CALALR_MONTH_Msk & ((value) << RTC_CALALR_MONTH_Pos))
#define RTC_CALALR_MTHEN_Pos                  (23U)                                          /**< (RTC_CALALR) Month Alarm Enable Position */
#define RTC_CALALR_MTHEN_Msk                  (0x1U << RTC_CALALR_MTHEN_Pos)                 /**< (RTC_CALALR) Month Alarm Enable Mask */
#define RTC_CALALR_MTHEN(value)               (RTC_CALALR_MTHEN_Msk & ((value) << RTC_CALALR_MTHEN_Pos))
#define RTC_CALALR_DATE_Pos                   (24U)                                          /**< (RTC_CALALR) Date Alarm Position */
#define RTC_CALALR_DATE_Msk                   (0x3FU << RTC_CALALR_DATE_Pos)                 /**< (RTC_CALALR) Date Alarm Mask */
#define RTC_CALALR_DATE(value)                (RTC_CALALR_DATE_Msk & ((value) << RTC_CALALR_DATE_Pos))
#define RTC_CALALR_DATEEN_Pos                 (31U)                                          /**< (RTC_CALALR) Date Alarm Enable Position */
#define RTC_CALALR_DATEEN_Msk                 (0x1U << RTC_CALALR_DATEEN_Pos)                /**< (RTC_CALALR) Date Alarm Enable Mask */
#define RTC_CALALR_DATEEN(value)              (RTC_CALALR_DATEEN_Msk & ((value) << RTC_CALALR_DATEEN_Pos))
#define RTC_CALALR_Msk                        (0xBF9F0000UL)                                 /**< (RTC_CALALR) Register Mask  */

/* -------- RTC_SR : (RTC Offset: 0x18) (R/  32) Status Register -------- */
#define RTC_SR_ACKUPD_Pos                     (0U)                                           /**< (RTC_SR) Acknowledge for Update Position */
#define RTC_SR_ACKUPD_Msk                     (0x1U << RTC_SR_ACKUPD_Pos)                    /**< (RTC_SR) Acknowledge for Update Mask */
#define RTC_SR_ACKUPD(value)                  (RTC_SR_ACKUPD_Msk & ((value) << RTC_SR_ACKUPD_Pos))
#define   RTC_SR_ACKUPD_FREERUN_Val           (0U)                                           /**< (RTC_SR) Time and calendar registers cannot be updated.  */
#define   RTC_SR_ACKUPD_UPDATE_Val            (1U)                                           /**< (RTC_SR) Time and calendar registers can be updated.  */
#define RTC_SR_ACKUPD_FREERUN                 (RTC_SR_ACKUPD_FREERUN_Val << RTC_SR_ACKUPD_Pos) /**< (RTC_SR) Time and calendar registers cannot be updated. Position  */
#define RTC_SR_ACKUPD_UPDATE                  (RTC_SR_ACKUPD_UPDATE_Val << RTC_SR_ACKUPD_Pos) /**< (RTC_SR) Time and calendar registers can be updated. Position  */
#define RTC_SR_ALARM_Pos                      (1U)                                           /**< (RTC_SR) Alarm Flag Position */
#define RTC_SR_ALARM_Msk                      (0x1U << RTC_SR_ALARM_Pos)                     /**< (RTC_SR) Alarm Flag Mask */
#define RTC_SR_ALARM(value)                   (RTC_SR_ALARM_Msk & ((value) << RTC_SR_ALARM_Pos))
#define   RTC_SR_ALARM_NO_ALARMEVENT_Val      (0U)                                           /**< (RTC_SR) No alarm matching condition occurred.  */
#define   RTC_SR_ALARM_ALARMEVENT_Val         (1U)                                           /**< (RTC_SR) An alarm matching condition has occurred.  */
#define RTC_SR_ALARM_NO_ALARMEVENT            (RTC_SR_ALARM_NO_ALARMEVENT_Val << RTC_SR_ALARM_Pos) /**< (RTC_SR) No alarm matching condition occurred. Position  */
#define RTC_SR_ALARM_ALARMEVENT               (RTC_SR_ALARM_ALARMEVENT_Val << RTC_SR_ALARM_Pos) /**< (RTC_SR) An alarm matching condition has occurred. Position  */
#define RTC_SR_SEC_Pos                        (2U)                                           /**< (RTC_SR) Second Event Position */
#define RTC_SR_SEC_Msk                        (0x1U << RTC_SR_SEC_Pos)                       /**< (RTC_SR) Second Event Mask */
#define RTC_SR_SEC(value)                     (RTC_SR_SEC_Msk & ((value) << RTC_SR_SEC_Pos))
#define   RTC_SR_SEC_NO_SECEVENT_Val          (0U)                                           /**< (RTC_SR) No second event has occurred since the last clear.  */
#define   RTC_SR_SEC_SECEVENT_Val             (1U)                                           /**< (RTC_SR) At least one second event has occurred since the last clear.  */
#define RTC_SR_SEC_NO_SECEVENT                (RTC_SR_SEC_NO_SECEVENT_Val << RTC_SR_SEC_Pos) /**< (RTC_SR) No second event has occurred since the last clear. Position  */
#define RTC_SR_SEC_SECEVENT                   (RTC_SR_SEC_SECEVENT_Val << RTC_SR_SEC_Pos)    /**< (RTC_SR) At least one second event has occurred since the last clear. Position  */
#define RTC_SR_TIMEV_Pos                      (3U)                                           /**< (RTC_SR) Time Event Position */
#define RTC_SR_TIMEV_Msk                      (0x1U << RTC_SR_TIMEV_Pos)                     /**< (RTC_SR) Time Event Mask */
#define RTC_SR_TIMEV(value)                   (RTC_SR_TIMEV_Msk & ((value) << RTC_SR_TIMEV_Pos))
#define   RTC_SR_TIMEV_NO_TIMEVENT_Val        (0U)                                           /**< (RTC_SR) No time event has occurred since the last clear.  */
#define   RTC_SR_TIMEV_TIMEVENT_Val           (1U)                                           /**< (RTC_SR) At least one time event has occurred since the last clear.  */
#define RTC_SR_TIMEV_NO_TIMEVENT              (RTC_SR_TIMEV_NO_TIMEVENT_Val << RTC_SR_TIMEV_Pos) /**< (RTC_SR) No time event has occurred since the last clear. Position  */
#define RTC_SR_TIMEV_TIMEVENT                 (RTC_SR_TIMEV_TIMEVENT_Val << RTC_SR_TIMEV_Pos) /**< (RTC_SR) At least one time event has occurred since the last clear. Position  */
#define RTC_SR_CALEV_Pos                      (4U)                                           /**< (RTC_SR) Calendar Event Position */
#define RTC_SR_CALEV_Msk                      (0x1U << RTC_SR_CALEV_Pos)                     /**< (RTC_SR) Calendar Event Mask */
#define RTC_SR_CALEV(value)                   (RTC_SR_CALEV_Msk & ((value) << RTC_SR_CALEV_Pos))
#define   RTC_SR_CALEV_NO_CALEVENT_Val        (0U)                                           /**< (RTC_SR) No calendar event has occurred since the last clear.  */
#define   RTC_SR_CALEV_CALEVENT_Val           (1U)                                           /**< (RTC_SR) At least one calendar event has occurred since the last clear.  */
#define RTC_SR_CALEV_NO_CALEVENT              (RTC_SR_CALEV_NO_CALEVENT_Val << RTC_SR_CALEV_Pos) /**< (RTC_SR) No calendar event has occurred since the last clear. Position  */
#define RTC_SR_CALEV_CALEVENT                 (RTC_SR_CALEV_CALEVENT_Val << RTC_SR_CALEV_Pos) /**< (RTC_SR) At least one calendar event has occurred since the last clear. Position  */
#define RTC_SR_TDERR_Pos                      (5U)                                           /**< (RTC_SR) Time and/or Date Free Running Error Position */
#define RTC_SR_TDERR_Msk                      (0x1U << RTC_SR_TDERR_Pos)                     /**< (RTC_SR) Time and/or Date Free Running Error Mask */
#define RTC_SR_TDERR(value)                   (RTC_SR_TDERR_Msk & ((value) << RTC_SR_TDERR_Pos))
#define   RTC_SR_TDERR_CORRECT_Val            (0U)                                           /**< (RTC_SR) The internal free running counters are carrying valid values since the last read of the Status Register (RTC_SR).  */
#define   RTC_SR_TDERR_ERR_TIMEDATE_Val       (1U)                                           /**< (RTC_SR) The internal free running counters have been corrupted (invalid date or time, non-BCD values) since the last read and/or they are still invalid.  */
#define RTC_SR_TDERR_CORRECT                  (RTC_SR_TDERR_CORRECT_Val << RTC_SR_TDERR_Pos) /**< (RTC_SR) The internal free running counters are carrying valid values since the last read of the Status Register (RTC_SR). Position  */
#define RTC_SR_TDERR_ERR_TIMEDATE             (RTC_SR_TDERR_ERR_TIMEDATE_Val << RTC_SR_TDERR_Pos) /**< (RTC_SR) The internal free running counters have been corrupted (invalid date or time, non-BCD values) since the last read and/or they are still invalid. Position  */
#define RTC_SR_Msk                            (0x0000003FUL)                                 /**< (RTC_SR) Register Mask  */

/* -------- RTC_SCCR : (RTC Offset: 0x1C) ( /W 32) Status Clear Command Register -------- */
#define RTC_SCCR_ACKCLR_Pos                   (0U)                                           /**< (RTC_SCCR) Acknowledge Clear Position */
#define RTC_SCCR_ACKCLR_Msk                   (0x1U << RTC_SCCR_ACKCLR_Pos)                  /**< (RTC_SCCR) Acknowledge Clear Mask */
#define RTC_SCCR_ACKCLR(value)                (RTC_SCCR_ACKCLR_Msk & ((value) << RTC_SCCR_ACKCLR_Pos))
#define RTC_SCCR_ALRCLR_Pos                   (1U)                                           /**< (RTC_SCCR) Alarm Clear Position */
#define RTC_SCCR_ALRCLR_Msk                   (0x1U << RTC_SCCR_ALRCLR_Pos)                  /**< (RTC_SCCR) Alarm Clear Mask */
#define RTC_SCCR_ALRCLR(value)                (RTC_SCCR_ALRCLR_Msk & ((value) << RTC_SCCR_ALRCLR_Pos))
#define RTC_SCCR_SECCLR_Pos                   (2U)                                           /**< (RTC_SCCR) Second Clear Position */
#define RTC_SCCR_SECCLR_Msk                   (0x1U << RTC_SCCR_SECCLR_Pos)                  /**< (RTC_SCCR) Second Clear Mask */
#define RTC_SCCR_SECCLR(value)                (RTC_SCCR_SECCLR_Msk & ((value) << RTC_SCCR_SECCLR_Pos))
#define RTC_SCCR_TIMCLR_Pos                   (3U)                                           /**< (RTC_SCCR) Time Clear Position */
#define RTC_SCCR_TIMCLR_Msk                   (0x1U << RTC_SCCR_TIMCLR_Pos)                  /**< (RTC_SCCR) Time Clear Mask */
#define RTC_SCCR_TIMCLR(value)                (RTC_SCCR_TIMCLR_Msk & ((value) << RTC_SCCR_TIMCLR_Pos))
#define RTC_SCCR_CALCLR_Pos                   (4U)                                           /**< (RTC_SCCR) Calendar Clear Position */
#define RTC_SCCR_CALCLR_Msk                   (0x1U << RTC_SCCR_CALCLR_Pos)                  /**< (RTC_SCCR) Calendar Clear Mask */
#define RTC_SCCR_CALCLR(value)                (RTC_SCCR_CALCLR_Msk & ((value) << RTC_SCCR_CALCLR_Pos))
#define RTC_SCCR_TDERRCLR_Pos                 (5U)                                           /**< (RTC_SCCR) Time and/or Date Free Running Error Clear Position */
#define RTC_SCCR_TDERRCLR_Msk                 (0x1U << RTC_SCCR_TDERRCLR_Pos)                /**< (RTC_SCCR) Time and/or Date Free Running Error Clear Mask */
#define RTC_SCCR_TDERRCLR(value)              (RTC_SCCR_TDERRCLR_Msk & ((value) << RTC_SCCR_TDERRCLR_Pos))
#define RTC_SCCR_Msk                          (0x0000003FUL)                                 /**< (RTC_SCCR) Register Mask  */

/* -------- RTC_IER : (RTC Offset: 0x20) ( /W 32) Interrupt Enable Register -------- */
#define RTC_IER_ACKEN_Pos                     (0U)                                           /**< (RTC_IER) Acknowledge Update Interrupt Enable Position */
#define RTC_IER_ACKEN_Msk                     (0x1U << RTC_IER_ACKEN_Pos)                    /**< (RTC_IER) Acknowledge Update Interrupt Enable Mask */
#define RTC_IER_ACKEN(value)                  (RTC_IER_ACKEN_Msk & ((value) << RTC_IER_ACKEN_Pos))
#define RTC_IER_ALREN_Pos                     (1U)                                           /**< (RTC_IER) Alarm Interrupt Enable Position */
#define RTC_IER_ALREN_Msk                     (0x1U << RTC_IER_ALREN_Pos)                    /**< (RTC_IER) Alarm Interrupt Enable Mask */
#define RTC_IER_ALREN(value)                  (RTC_IER_ALREN_Msk & ((value) << RTC_IER_ALREN_Pos))
#define RTC_IER_SECEN_Pos                     (2U)                                           /**< (RTC_IER) Second Event Interrupt Enable Position */
#define RTC_IER_SECEN_Msk                     (0x1U << RTC_IER_SECEN_Pos)                    /**< (RTC_IER) Second Event Interrupt Enable Mask */
#define RTC_IER_SECEN(value)                  (RTC_IER_SECEN_Msk & ((value) << RTC_IER_SECEN_Pos))
#define RTC_IER_TIMEN_Pos                     (3U)                                           /**< (RTC_IER) Time Event Interrupt Enable Position */
#define RTC_IER_TIMEN_Msk                     (0x1U << RTC_IER_TIMEN_Pos)                    /**< (RTC_IER) Time Event Interrupt Enable Mask */
#define RTC_IER_TIMEN(value)                  (RTC_IER_TIMEN_Msk & ((value) << RTC_IER_TIMEN_Pos))
#define RTC_IER_CALEN_Pos                     (4U)                                           /**< (RTC_IER) Calendar Event Interrupt Enable Position */
#define RTC_IER_CALEN_Msk                     (0x1U << RTC_IER_CALEN_Pos)                    /**< (RTC_IER) Calendar Event Interrupt Enable Mask */
#define RTC_IER_CALEN(value)                  (RTC_IER_CALEN_Msk & ((value) << RTC_IER_CALEN_Pos))
#define RTC_IER_TDERREN_Pos                   (5U)                                           /**< (RTC_IER) Time and/or Date Error Interrupt Enable Position */
#define RTC_IER_TDERREN_Msk                   (0x1U << RTC_IER_TDERREN_Pos)                  /**< (RTC_IER) Time and/or Date Error Interrupt Enable Mask */
#define RTC_IER_TDERREN(value)                (RTC_IER_TDERREN_Msk & ((value) << RTC_IER_TDERREN_Pos))
#define RTC_IER_Msk                           (0x0000003FUL)                                 /**< (RTC_IER) Register Mask  */

/* -------- RTC_IDR : (RTC Offset: 0x24) ( /W 32) Interrupt Disable Register -------- */
#define RTC_IDR_ACKDIS_Pos                    (0U)                                           /**< (RTC_IDR) Acknowledge Update Interrupt Disable Position */
#define RTC_IDR_ACKDIS_Msk                    (0x1U << RTC_IDR_ACKDIS_Pos)                   /**< (RTC_IDR) Acknowledge Update Interrupt Disable Mask */
#define RTC_IDR_ACKDIS(value)                 (RTC_IDR_ACKDIS_Msk & ((value) << RTC_IDR_ACKDIS_Pos))
#define RTC_IDR_ALRDIS_Pos                    (1U)                                           /**< (RTC_IDR) Alarm Interrupt Disable Position */
#define RTC_IDR_ALRDIS_Msk                    (0x1U << RTC_IDR_ALRDIS_Pos)                   /**< (RTC_IDR) Alarm Interrupt Disable Mask */
#define RTC_IDR_ALRDIS(value)                 (RTC_IDR_ALRDIS_Msk & ((value) << RTC_IDR_ALRDIS_Pos))
#define RTC_IDR_SECDIS_Pos                    (2U)                                           /**< (RTC_IDR) Second Event Interrupt Disable Position */
#define RTC_IDR_SECDIS_Msk                    (0x1U << RTC_IDR_SECDIS_Pos)                   /**< (RTC_IDR) Second Event Interrupt Disable Mask */
#define RTC_IDR_SECDIS(value)                 (RTC_IDR_SECDIS_Msk & ((value) << RTC_IDR_SECDIS_Pos))
#define RTC_IDR_TIMDIS_Pos                    (3U)                                           /**< (RTC_IDR) Time Event Interrupt Disable Position */
#define RTC_IDR_TIMDIS_Msk                    (0x1U << RTC_IDR_TIMDIS_Pos)                   /**< (RTC_IDR) Time Event Interrupt Disable Mask */
#define RTC_IDR_TIMDIS(value)                 (RTC_IDR_TIMDIS_Msk & ((value) << RTC_IDR_TIMDIS_Pos))
#define RTC_IDR_CALDIS_Pos                    (4U)                                           /**< (RTC_IDR) Calendar Event Interrupt Disable Position */
#define RTC_IDR_CALDIS_Msk                    (0x1U << RTC_IDR_CALDIS_Pos)                   /**< (RTC_IDR) Calendar Event Interrupt Disable Mask */
#define RTC_IDR_CALDIS(value)                 (RTC_IDR_CALDIS_Msk & ((value) << RTC_IDR_CALDIS_Pos))
#define RTC_IDR_TDERRDIS_Pos                  (5U)                                           /**< (RTC_IDR) Time and/or Date Error Interrupt Disable Position */
#define RTC_IDR_TDERRDIS_Msk                  (0x1U << RTC_IDR_TDERRDIS_Pos)                 /**< (RTC_IDR) Time and/or Date Error Interrupt Disable Mask */
#define RTC_IDR_TDERRDIS(value)               (RTC_IDR_TDERRDIS_Msk & ((value) << RTC_IDR_TDERRDIS_Pos))
#define RTC_IDR_Msk                           (0x0000003FUL)                                 /**< (RTC_IDR) Register Mask  */

/* -------- RTC_IMR : (RTC Offset: 0x28) (R/  32) Interrupt Mask Register -------- */
#define RTC_IMR_ACK_Pos                       (0U)                                           /**< (RTC_IMR) Acknowledge Update Interrupt Mask Position */
#define RTC_IMR_ACK_Msk                       (0x1U << RTC_IMR_ACK_Pos)                      /**< (RTC_IMR) Acknowledge Update Interrupt Mask Mask */
#define RTC_IMR_ACK(value)                    (RTC_IMR_ACK_Msk & ((value) << RTC_IMR_ACK_Pos))
#define RTC_IMR_ALR_Pos                       (1U)                                           /**< (RTC_IMR) Alarm Interrupt Mask Position */
#define RTC_IMR_ALR_Msk                       (0x1U << RTC_IMR_ALR_Pos)                      /**< (RTC_IMR) Alarm Interrupt Mask Mask */
#define RTC_IMR_ALR(value)                    (RTC_IMR_ALR_Msk & ((value) << RTC_IMR_ALR_Pos))
#define RTC_IMR_SEC_Pos                       (2U)                                           /**< (RTC_IMR) Second Event Interrupt Mask Position */
#define RTC_IMR_SEC_Msk                       (0x1U << RTC_IMR_SEC_Pos)                      /**< (RTC_IMR) Second Event Interrupt Mask Mask */
#define RTC_IMR_SEC(value)                    (RTC_IMR_SEC_Msk & ((value) << RTC_IMR_SEC_Pos))
#define RTC_IMR_TIM_Pos                       (3U)                                           /**< (RTC_IMR) Time Event Interrupt Mask Position */
#define RTC_IMR_TIM_Msk                       (0x1U << RTC_IMR_TIM_Pos)                      /**< (RTC_IMR) Time Event Interrupt Mask Mask */
#define RTC_IMR_TIM(value)                    (RTC_IMR_TIM_Msk & ((value) << RTC_IMR_TIM_Pos))
#define RTC_IMR_CAL_Pos                       (4U)                                           /**< (RTC_IMR) Calendar Event Interrupt Mask Position */
#define RTC_IMR_CAL_Msk                       (0x1U << RTC_IMR_CAL_Pos)                      /**< (RTC_IMR) Calendar Event Interrupt Mask Mask */
#define RTC_IMR_CAL(value)                    (RTC_IMR_CAL_Msk & ((value) << RTC_IMR_CAL_Pos))
#define RTC_IMR_TDERR_Pos                     (5U)                                           /**< (RTC_IMR) Time and/or Date Error Mask Position */
#define RTC_IMR_TDERR_Msk                     (0x1U << RTC_IMR_TDERR_Pos)                    /**< (RTC_IMR) Time and/or Date Error Mask Mask */
#define RTC_IMR_TDERR(value)                  (RTC_IMR_TDERR_Msk & ((value) << RTC_IMR_TDERR_Pos))
#define RTC_IMR_Msk                           (0x0000003FUL)                                 /**< (RTC_IMR) Register Mask  */

/* -------- RTC_VER : (RTC Offset: 0x2C) (R/  32) Valid Entry Register -------- */
#define RTC_VER_NVTIM_Pos                     (0U)                                           /**< (RTC_VER) Non-valid Time Position */
#define RTC_VER_NVTIM_Msk                     (0x1U << RTC_VER_NVTIM_Pos)                    /**< (RTC_VER) Non-valid Time Mask */
#define RTC_VER_NVTIM(value)                  (RTC_VER_NVTIM_Msk & ((value) << RTC_VER_NVTIM_Pos))
#define RTC_VER_NVCAL_Pos                     (1U)                                           /**< (RTC_VER) Non-valid Calendar Position */
#define RTC_VER_NVCAL_Msk                     (0x1U << RTC_VER_NVCAL_Pos)                    /**< (RTC_VER) Non-valid Calendar Mask */
#define RTC_VER_NVCAL(value)                  (RTC_VER_NVCAL_Msk & ((value) << RTC_VER_NVCAL_Pos))
#define RTC_VER_NVTIMALR_Pos                  (2U)                                           /**< (RTC_VER) Non-valid Time Alarm Position */
#define RTC_VER_NVTIMALR_Msk                  (0x1U << RTC_VER_NVTIMALR_Pos)                 /**< (RTC_VER) Non-valid Time Alarm Mask */
#define RTC_VER_NVTIMALR(value)               (RTC_VER_NVTIMALR_Msk & ((value) << RTC_VER_NVTIMALR_Pos))
#define RTC_VER_NVCALALR_Pos                  (3U)                                           /**< (RTC_VER) Non-valid Calendar Alarm Position */
#define RTC_VER_NVCALALR_Msk                  (0x1U << RTC_VER_NVCALALR_Pos)                 /**< (RTC_VER) Non-valid Calendar Alarm Mask */
#define RTC_VER_NVCALALR(value)               (RTC_VER_NVCALALR_Msk & ((value) << RTC_VER_NVCALALR_Pos))
#define RTC_VER_Msk                           (0x0000000FUL)                                 /**< (RTC_VER) Register Mask  */

/** \brief RTC register offsets definitions */
#define RTC_CR_OFFSET                  (0x00)         /**< (RTC_CR) Control Register Offset */
#define RTC_MR_OFFSET                  (0x04)         /**< (RTC_MR) Mode Register Offset */
#define RTC_TIMR_OFFSET                (0x08)         /**< (RTC_TIMR) Time Register Offset */
#define RTC_CALR_OFFSET                (0x0C)         /**< (RTC_CALR) Calendar Register Offset */
#define RTC_TIMALR_OFFSET              (0x10)         /**< (RTC_TIMALR) Time Alarm Register Offset */
#define RTC_CALALR_OFFSET              (0x14)         /**< (RTC_CALALR) Calendar Alarm Register Offset */
#define RTC_SR_OFFSET                  (0x18)         /**< (RTC_SR) Status Register Offset */
#define RTC_SCCR_OFFSET                (0x1C)         /**< (RTC_SCCR) Status Clear Command Register Offset */
#define RTC_IER_OFFSET                 (0x20)         /**< (RTC_IER) Interrupt Enable Register Offset */
#define RTC_IDR_OFFSET                 (0x24)         /**< (RTC_IDR) Interrupt Disable Register Offset */
#define RTC_IMR_OFFSET                 (0x28)         /**< (RTC_IMR) Interrupt Mask Register Offset */
#define RTC_VER_OFFSET                 (0x2C)         /**< (RTC_VER) Valid Entry Register Offset */

/** \brief RTC register API structure */
typedef struct
{
  __IO  uint32_t                       RTC_CR;          /**< Offset: 0x00 (R/W  32) Control Register */
  __IO  uint32_t                       RTC_MR;          /**< Offset: 0x04 (R/W  32) Mode Register */
  __IO  uint32_t                       RTC_TIMR;        /**< Offset: 0x08 (R/W  32) Time Register */
  __IO  uint32_t                       RTC_CALR;        /**< Offset: 0x0c (R/W  32) Calendar Register */
  __IO  uint32_t                       RTC_TIMALR;      /**< Offset: 0x10 (R/W  32) Time Alarm Register */
  __IO  uint32_t                       RTC_CALALR;      /**< Offset: 0x14 (R/W  32) Calendar Alarm Register */
  __I   uint32_t                       RTC_SR;          /**< Offset: 0x18 (R/   32) Status Register */
  __O   uint32_t                       RTC_SCCR;        /**< Offset: 0x1c ( /W  32) Status Clear Command Register */
  __O   uint32_t                       RTC_IER;         /**< Offset: 0x20 ( /W  32) Interrupt Enable Register */
  __O   uint32_t                       RTC_IDR;         /**< Offset: 0x24 ( /W  32) Interrupt Disable Register */
  __I   uint32_t                       RTC_IMR;         /**< Offset: 0x28 (R/   32) Interrupt Mask Register */
  __I   uint32_t                       RTC_VER;         /**< Offset: 0x2c (R/   32) Valid Entry Register */
} rtc_registers_t;
/** @}  end of Real-time Clock */

#endif /* SAME_SAME70_RTC_MODULE_H */

