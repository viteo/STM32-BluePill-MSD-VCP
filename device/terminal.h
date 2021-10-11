/*
 * terminal.h
 *
 *  Created on: 11 oct 2021
 *      Author: v.simonenko
 */

#ifndef TERMINAL_H_
#define TERMINAL_H_

#include <stddef.h>
#include "term-srv.h"

extern term_srv_cmd_t cmd_list[];
extern size_t cmd_count;

void term_send_data(const char *data, int16_t size);

#endif /* TERMINAL_H_ */
