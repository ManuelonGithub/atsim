/*
 * File: queue.c
 * Author: Manuel Burnay
 * Date: May 20, 2019
 * Purpose:
 *      This file contains the function bodies that initialize and manage the
 *      flight queue used in the simulation.
 *
 */

#include "queue.h"

/*
 * brief: Initializes a flight queue.
 *
 * param: [in] queue: flight_queue_t *
 *                  > Pointer to a flight queue data type.
 *
 * details: This flight queue has been implemented as a circular buffer,
 *          which requires the head and tail pointers to be initializes to 0.
 */
void init_queue (flight_queue_t * queue)
{
    queue->tail = 0;
    queue->head = 0;
}

/*
 * brief: dequeues a flight from a flight queue.
 *
 *  param: [in] queue: flight_queue_t *
 *                   > Pointer to a flight queue data type.
 *
 * details: Because the size of this circular buffer is static and a power of 2,
 *          dequeueing can be managed simply incrementing the head and
 *          masking it with the size of the queue, which prevents the head from
 *          ever pointing outside of the queue.
 */
void dequeue(flight_queue_t *queue)
{
    queue->head++;
    queue->head &= FLIGHT_QUEUE_SIZE_MASK;
}

/*
 * brief: enqueues a flight from a flight queue.
 *
 *  param: [in] queue: flight_queue_t *
 *                   > Pointer to a flight queue data type.
 *
*          [in] flight: flight_t *
 *                    > pointer to a flight data type.
 *
 * details: Because the size of this circular buffer is static and a power of 2,
 *          enqueueing can be managed simply incrementing the tail and
 *          masking it with the size of the queue, which prevents the tail from
 *          ever pointing outside of the queue.
 */
void enqueue(flight_queue_t *queue, flight_t *flight)
{
    queue->buffer[queue->tail] = flight;
    queue->tail++;
    queue->tail &= FLIGHT_QUEUE_SIZE_MASK;
}

/*
 * brief: peeks at the element in from of a flight queue.
 *
 *  param: [in] queue: flight_queue_t *
 *                   > Pointer to a flight queue data type.
 *
 *         [out] flight_t *
 *               > pointer to the flight element in front of the queue.
 */
flight_t* peek(flight_queue_t *queue)
{
    return queue->buffer[queue->head];
}

/*
 * brief: finds the size of a flight queue.
 *
 *  param: [in] queue: flight_queue_t *
 *                   > Pointer to a flight queue data type.
 *
 *         [out] uint32_t
 *               > size of the queue.
 *
 * details: Finding the length of the queue is done by finding the distance
 *          between the head and the tail, and masking it with the size of
 *          the buffer, which accounts for when the tail has circled through
 *          the buffer.
 */
uint32_t size (flight_queue_t * queue)
{
    return ((queue->tail - queue->head) & FLIGHT_QUEUE_SIZE_MASK);
}