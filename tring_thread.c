/*
 * This file contains the code for a extra credit assignment worker thread.
 * The main thread of execution will start a thread by using tring_thread_start.
 * Most of a student's work will go into this file.
 *
 */

#include <stdio.h>

#include "mailbox.h"
#include "tring_thread.h"
#include "tring.h"
#include "util.h"


 void tring_signal(void);
 void increment_sleep_count(void);


/*
 * void* tring_thread_start(void* arg)
 *
 * This function is used in the creating of worker threads for this assignment.
 * Its parameter 'arg' is ;a pointer to a thread's designated mailbox.  The
 * thread should exit with a NULL pointer.
 *
 */
void* tring_thread_start(void* arg) {
	int id=-1;
	mailbox* mb, * next_mb=NULL, *sorted_mb=NULL;
	message* msg;
	int next_id=MAX;


	mb = (mailbox*) arg;

	while(1)
	{
	 	msg = mailbox_receive(mb);
	 	if(msg == NULL)
	 	{
	 		continue;
	 	}
		//SORT message, initially sent by the main thread after the PROBE message to allow threads 
		//to assign their new sorted mailboxes.
	 	if(msg->type == SORT) 
	 	{
	 		printf("SORT\n");	 		
	 		if(msg->payload.sp.probe_id != id)
	 		{
				//-1 means sent by the main thread to the first mailbox.
	 			if(msg->payload.sp.probe_id == -1)
	 			{
	 				msg->payload.sp.probe_id = id;
	 			}
	 			if(id<msg->payload.sp.next_id) //Simultaneously find the thread with the lowest id and store it in the message.
	 			{
	 				msg->payload.sp.next_id = id;
	 				msg->payload.sp.next_mb = mb;	
	 			}
				
	 			mailbox_send(next_mb,msg); //Send to the next mailbox.
	 			next_mb = sorted_mb;
	 		}
	 		else
	 		{
	 			//printf("Signalling main thread\n");
				//All the threads have finished updating their mailbox. Point the first_mb to the mailbox
				// of the thread with the lowest id.
	 			
				//free(msg);
	 			tring_signal();
	 		}


	 	}

		//PROBE - The initial message to allow threads to find its next neighbour.
	 	else if(msg->type == PROBE)
	 	{
	 		//printf("PROBE %d\n",msg->payload.sp.probe_id);
			//-1 means the initial message sent from the main thread.
	 		if(msg->payload.sp.probe_id == -1)
	 		{
	 			// start the probe
				//printf("Probe start\n");
				
				//Create a message to broadcast to all the other threads.
	 			msg->payload.sp.probe_id = id;
	 			msg->payload.sp.next_id = MAX;
	 			msg->payload.sp.next_mb = NULL;
				msg->payload.sp.count = msg->payload.sp.count + 1;
	 			mailbox_send(next_mb,msg); 
				
				//printf("Probe sent from %d\n",id);
	 		}
	 		else if(msg->payload.sp.probe_id == id)
	 		{
	 			//Stop forwarding and note the next mb.
				
	 			sorted_mb = msg->payload.sp.next_mb;
	 			next_id = msg->payload.sp.next_id;
				
				
				if(msg->payload.sp.count >= msg->payload.sp.thr_count)
				{
					msg->type = SORT;
					msg->payload.sp.probe_id = -1;
					msg->payload.sp.next_id = MAX;
					msg->payload.sp.next_mb = NULL;
				}
				else
				{
									
					msg->payload.sp.probe_id = -1;
					msg->payload.sp.next_id = MAX;
					msg->payload.sp.next_mb = NULL;
				}
				
				printf("PROBE_END for %d. Next ID = %d\n",id, next_id);
				mailbox_send(next_mb,msg); 
				
				
	 			//increment_sleep_count();
	 			//free(msg);
	 			

	 		}
	 		else
	 		{
				//printf("probe forward\n");
				//Check if the current thread can be the neighbour of the calling thread. If yes, update the message and pass it on.
				//Else just pass it on to the next thread.
	 			if(id>msg->payload.sp.probe_id)
	 			{
	 				if(id<msg->payload.sp.next_id)
	 				{
	 					msg->payload.sp.next_id = id;
	 					msg->payload.sp.next_mb = mb;
	 				}
	 			}
	 			mailbox_send(next_mb,msg);

	 			/*if(probe_done == 0)
	 			{
	 				probe_done = 1;	
	 				msg = NEW_MSG;
	 				msg->type = PROBE;
	 				msg->payload.sp.probe_id = id;
	 				msg->payload.sp.next_id = MAX;
	 				msg->payload.sp.next_mb = NULL;
	 				
	 				mailbox_send(next_mb,msg);	
	 			}*/
	 		}
	 	}
	 	
	 	else if(msg->type == ID)
	 	{
			//Set the ID of the thread.
	 		id = msg->payload.integer;
	 			//printf("ID set to %d\n",id);
			free(msg);
	 	}

	 	else if(msg->type == MAILBOX)
	 	{
			//Mailbox of the next thread.
	 		next_mb = msg->payload.mb;
	 		//printf("Next mailbox updated\n");
	 		if(next_mb == NULL)
	 			tring_signal();
			free(msg);

	 	}
	 	else if(msg->type == PING)
	 	{
			//Handle ping message.
	 		//printf("Ping %d\n",id); 
	 		pong(id);
	 		if(next_mb != NULL)
	 			mailbox_send(next_mb,msg);
	 	}
	 	else if(msg->type == PRINT)
	 	{
			
			//Handle print message.

	 		if(next_mb != NULL)
	 		{
	 			printf("Print %d -> %d\n",id,next_id); 
	 			tring_print(id,next_id);
	 			mailbox_send(next_mb,msg);
	 		}
	 		else
	 		{
	 			printf("Print %d -> NULL\n",id); 
	 			tring_print_end(id);
	 			tring_signal();
	 		}
	 	}
	 	else if(msg->type == SHUTDOWN)
	 	{
			
			//Free the resources and exit.
	 		if(next_mb != NULL) 
	 		{
				//Forward the shut down message to the next thread.
	 			mailbox_send(next_mb,msg);
	 			
	 		}
	 			free(mb);
	 			sorted_mb = NULL;
	 			break;
	 		
	 	}
	 	
	 	
	}
	 
	return NULL;
}
