/*
 * This file contains functions declarations for manipulating mailboxes.
 */

#ifndef MAILBOX_H
#define MAILBOX_H

#include <pthread.h>
#include <stdlib.h>

#include "queue.h"

#define NEW_MSG	malloc(sizeof(message));

typedef struct mailbox_s {
	queue q;
} mailbox;


typedef struct sort_probe
{
	int probe_id;
	int next_id;
	mailbox* next_mb;
	mailbox* f_mb;
	int count;
	int thr_count;
} sort_probe;

enum mtypes {
	ID,
	MAILBOX,
	PING,
	PRINT,
	SHUTDOWN,
	PROBE,
	SORT
};

typedef union {
	int integer;
	mailbox* mb;
	sort_probe sp;
} msg_data;

typedef struct {
	enum mtypes type;
	msg_data payload;
} message;

void mailbox_init(mailbox* mb);
message* mailbox_receive(mailbox* mb);
void mailbox_send(mailbox* mb, message* msg);

#endif
