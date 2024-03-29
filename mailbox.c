/*
 * This file contains functions for manipulating mailboxes.  Students will
 * need to make these functions threadsafe.
 */

#include "mailbox.h"
#include "queue.h"
 //#include "tring_thread.h"
 static pthread_mutex_t maillock;

/*
 * void mailbox_init(mailbox* mb)
 *
 * This function is used to initialize the resources used by a mailbox (mb).
 *
 * FIXME
 * You will need to change this function to make the other two threadsafe.
 *
 */
void mailbox_init(mailbox* mb) {
	//Initialize the queue.
	pthread_mutex_lock(&maillock);
	queue_init(&(mb->q));
	pthread_mutex_unlock(&maillock);
}

/*
 * message* mailbox_receive(mailbox* mb)
 *
 * This function is used to retreive a message from a mailbox (mb).
 *
 * FIXME
 * This function should be made threadsafe.
 *
 */
message* mailbox_receive(mailbox* mb) {
	pthread_mutex_lock(&maillock);
	message* msg = queue_pop(&(mb->q));
	pthread_mutex_unlock(&maillock);
	return msg;
}

/*
 * void mailbox_send(mailbox* mb, message* msg)
 *
 * This function is use to send a message (msg) to a mailbox (mb).
 *
 * FIXME
 * This function should be made threadsafe.
 *
 */
void mailbox_send(mailbox* mb, message* msg) {
	pthread_mutex_lock(&maillock);
	queue_push(&(mb->q), msg);
	pthread_mutex_unlock(&maillock);
}
