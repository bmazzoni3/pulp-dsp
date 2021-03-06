#!/usr/bin/env python3

import numpy as np


##################
# compute_result #
##################


def compute_result(result_parameter, inputs, env, fix_point):
    """
    Funciton to generate the expected result of the testcase.

    Arguments
    ---------
    result_parameter: Either OutputArgument or ReturnValue (see pulp_dsp_test.py)
    inputs: Dict mapping name to the Argument, with arg.value, arg.ctype (and arg.length)
    env: Dict mapping the variable (SweepVariable or DynamicVariable) names to their value.
    fix_point: None (if no fixpoint is used) or decimal point
    """
    stride_src = env['strideSrc']
    stride_dst = env['strideDst']
    M = env['len_m']
    N = env['len_n']

    src = inputs['pSrc'].value.reshape((M, stride_src))

    if result_parameter.ctype == 'float':
        # not necessary to make cast src from a vector to a matrix
        src = src.astype(np.float32)
        result = np.zeros((env['len_dst']))
        scaleFactor = np.float32(inputs['scaleFactor'].value)
        result = np.zeros((M, stride_dst), dtype=np.float32)
        result[:M, :N] = (src[:M, :N] * scaleFactor).astype(np.float32)
        result = result.reshape((env['len_dst'], ))
    else:
        if result_parameter.ctype == 'int8_t':
            dtype = np.int8
        elif result_parameter.ctype == 'int16_t':
            dtype = np.int16
        elif result_parameter.ctype == 'int32_t':
            dtype = np.int32
        elif result_parameter.ctype == 'float':
            dtype = np.float32
        else:
            raise RuntimeError("Unrecognized result type: %s" % result_parameter.ctype)

        # not necessary to make cast src from a vector to a matrix
        src = src.astype(dtype)
        scaleFactor = np.int32(inputs['scaleFactor'].value)
        shift = np.int32(inputs['shift'].value)
        result = np.zeros((M, stride_dst), dtype=dtype)
        for m in range(M):
            for n in range(N):
                result[m, n] = dtype((src[m, n] * scaleFactor) >> shift)
        result = result.reshape((env['len_dst'], ))

    return result


######################
# Fixpoint Functions #
######################


def q_sat(x):
    if x > 2**31 - 1:
        return x - 2**32
    elif x < -2**31:
        return x + 2**32
    else:
        return x


def q_add(a, b):
    return q_sat(a + b)


def q_sub(a, b):
    return q_sat(a - b)


def q_mul(a, b, p):
    return q_roundnorm(a * b, p)


def q_roundnorm(a, p):
    rounding = 1 << (p - 1)
    return q_sat((a + rounding) >> p)
