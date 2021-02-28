/****************************************************************************
 * libs/libdsp/lib_pid.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <dsp.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: pid_controller_init
 *
 * Description:
 *   Initialize PID controller. This function does not initialize saturation
 *   limits.
 *
 * Input Parameters:
 *   pid - (out) pointer to the PID controller data
 *   KP  - (in) proportional gain
 *   KI  - (in) integral gain
 *   KD  - (in) derivative gain
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void pid_controller_init(FAR pid_controller_f32_t *pid, float KP, float KI,
                         float KD)
{
  DEBUGASSERT(pid != NULL);

  /* Reset controller data */

  memset(pid, 0, sizeof(pid_controller_f32_t));

  /* Copy controller parameters */

  pid->KP = KP;
  pid->KI = KI;
  pid->KD = KD;
}

/****************************************************************************
 * Name: pi_controller_init
 *
 * Description:
 *   Initialize PI controller. This function does not initialize saturation
 *   limits.
 *
 * Input Parameters:
 *   pid - (out) pointer to the PID controller data
 *   KP  - (in) proportional gain
 *   KI  - (in) integral gain
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void pi_controller_init(FAR pid_controller_f32_t *pid, float KP, float KI)
{
  DEBUGASSERT(pid != NULL);

  /* Reset controller data */

  memset(pid, 0, sizeof(pid_controller_f32_t));

  /* Copy controller parameters */

  pid->KP = KP;
  pid->KI = KI;
  pid->KD = 0.0f;
}

/****************************************************************************
 * Name: pid_saturation_set
 *
 * Description:
 *   Set controller saturation limits. Sometimes we need change saturation
 *   configuration in the run-time, so this function is separate from
 *   pid_controller_init().
 *
 * Input Parameters:
 *   pid - (out) pointer to the PID controller data
 *   min - (in) lower limit
 *   max - (in) upper limit
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void pid_saturation_set(FAR pid_controller_f32_t *pid, float min, float max)
{
  DEBUGASSERT(pid != NULL);
  DEBUGASSERT(min < max);

  pid->sat.max = max;
  pid->sat.min = min;
}

/****************************************************************************
 * Name: pi_saturation_set
 *
 * Description:
 *
 * Input Parameters:
 *   pid - (out) pointer to the PID controller data
 *   min - (in) lower limit
 *   max - (in) upper limit
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

void pi_saturation_set(FAR pid_controller_f32_t *pid, float min, float max)
{
  DEBUGASSERT(pid != NULL);
  DEBUGASSERT(min < max);

  pid_saturation_set(pid, min, max);
}

/****************************************************************************
 * Name: pid_integral_reset
 ****************************************************************************/

void pid_integral_reset(FAR pid_controller_f32_t *pid)
{
  pid->part[1] = 0.0f;
}

/****************************************************************************
 * Name: pi_integral_reset
 ****************************************************************************/

void pi_integral_reset(FAR pid_controller_f32_t *pid)
{
  pid_integral_reset(pid);
}

/****************************************************************************
 * Name: pi_controller
 *
 * Description:
 *   PI controller with output saturation and windup protection
 *
 * Input Parameters:
 *   pid - (in/out) pointer to the PI controller data
 *   err - (in) current controller error
 *
 * Returned Value:
 *   Return controller output.
 *
 ****************************************************************************/

float pi_controller(FAR pid_controller_f32_t *pid, float err)
{
  DEBUGASSERT(pid != NULL);

  /* Store error in controller structure */

  pid->err = err;

  /* Get proportional part */

  pid->part[0] = pid->KP * err;

  /* Get intergral part */

  pid->part[1] += pid->KI * err;

  /* Add proportional, integral */

  pid->out = pid->part[0] + pid->part[1];

  /* Saturate output only if we are not in a PID calculation and only
   * if some limits are set. Saturation for a PID controller are done later
   * in PID routine.
   */

  if (pid->sat.max != pid->sat.min && pid->KD == 0.0f)
    {
      if (pid->out > pid->sat.max)
        {
          /* Limit output to the upper limit */

          pid->out = pid->sat.max;

          /* Integral anti-windup - reset integral part */

          if (err > 0.0f)
            {
              pi_integral_reset(pid);
            }
        }
      else if (pid->out < pid->sat.min)
        {
          /* Limit output to the lower limit */

          pid->out = pid->sat.min;

          /* Integral anti-windup - reset integral part */

          if (err < 0.0f)
            {
              pi_integral_reset(pid);
            }
        }
    }

  /* Return regulator output */

  return pid->out;
}

/****************************************************************************
 * Name: pid_controller
 *
 * Description:
 *   PID controller with output saturation and windup protection
 *
 * Input Parameters:
 *   pid - (in/out) pointer to the PID controller data
 *   err - (in) current controller error
 *
 * Returned Value:
 *   Return controller output.
 *
 ****************************************************************************/

float pid_controller(FAR pid_controller_f32_t *pid, float err)
{
  DEBUGASSERT(pid != NULL);

  /* Get PI output */

  pi_controller(pid, err);

  /* Get derivative part */

  pid->part[2] = pid->KD * (err - pid->err_prev);

  /* Add derivative part to the PI part */

  pid->out +=  pid->part[2];

  /* Store current error */

  pid->err_prev = err;

  /* Saturate output if limits are set */

  if (pid->sat.max != pid->sat.min)
    {
      if (pid->out > pid->sat.max)
        {
          /* Limit output to the upper limit */

          pid->out = pid->sat.max;
        }
      else if (pid->out < pid->sat.min)
        {
          /* Limit output to the lower limit */

          pid->out = pid->sat.min;
        }
    }

  /* Return regulator output */

  return pid->out;
}
