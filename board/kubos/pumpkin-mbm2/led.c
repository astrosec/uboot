/*
* Copyright (C) 2017 Kubos Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#include <common.h>
#include <asm/io.h>
#include <asm/arch/gpio.h>

void coloured_LED_init(void)
{
	gpio_direction_output(CONFIG_RED_LED, 1);
	gpio_direction_output(CONFIG_GREEN_LED, 1);
	gpio_direction_output(CONFIG_YELLOW_LED, 1);

	gpio_set_value(CONFIG_RED_LED, 1);
	gpio_set_value(CONFIG_GREEN_LED, 1);
	gpio_set_value(CONFIG_YELLOW_LED, 1);
}
