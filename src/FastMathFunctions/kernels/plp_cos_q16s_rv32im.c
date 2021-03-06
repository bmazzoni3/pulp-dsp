/* =====================================================================
 * Project:      PULP DSP Library
 * Title:        plp_cos_q16s_rv32im.c
 * Description:  Calculates cosine of a q16 scaled input for RV32IM
 *
 * $Date:        30.07.2020
 *
 * Target Processor: PULP cores
 * ===================================================================== */
/*
 * Copyright (C) 2020 ETH Zurich and University of Bologna.
 *
 * Author: Michael Rogenmoser, ETH Zurich
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Notice: project inspired by ARM CMSIS DSP and parts of source code
 * ported and adopted for RISC-V PULP platform from ARM CMSIS DSP
 * released under Copyright (C) 2010-2019 ARM Limited or its affiliates
 * with Apache-2.0.
 */

#include "plp_common_tables.h"
#include "plp_math.h"

/**
 * @brief      q16 cosine function for RV32IM
 *
 * @param[in]  x     Scaled input value: Q1.15 value in range [0, +0.9999] and is mapped to [0,
 * 2*PI)
 *
 * @return     cos(x)
 */

int16_t plp_cos_q16s_rv32im(int16_t x) {
    int16_t cosVal; /* Temporary input, output variables */
    int32_t index;  /* Index variable */
    int16_t a, b;   /* Two nearest output values */
    int16_t fract;  /* Temporary values for fractional values */

    /* add 0.25 (pi/2) to read sine table */
    x = (uint16_t)x + 0x2000;
    if (x < 0) { /* convert negative numbers to corresponding positive ones */
        x = (uint16_t)x + 0x8000;
    }

    /* Calculate the nearest index */
    index = (uint32_t)x >> FAST_MATH_Q16_SHIFT;

    /* Calculation of fractional value */
    fract = (x - (index << FAST_MATH_Q16_SHIFT)) << 9;

    /* Read two nearest values of input value from the sin table */
    a = sinTable_q16[index];
    b = sinTable_q16[index + 1];

    /* Linear interpolation process */
    cosVal = (int32_t)(0x8000 - fract) * a >> 16;
    cosVal = (int16_t)((((int32_t)cosVal << 16) + ((int32_t)fract * b)) >> 16);

    /* Return output value */
    return (cosVal << 1);
}
