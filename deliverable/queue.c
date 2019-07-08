/**
 * @file    queue.c
 * @author  Manuel Burnay
 * @date    May 20, 2019
 * @details This file contains the function bodies that initialize
 *          and manage the flight queue used in the simulation.
 */

#include "queue.h"

/**
 * @brief   Initializes a flight queue.
 * @param   [out] queue: flight_queue_t*
 *                       -- Pointer to a flight queue data type.
 * @details This flight queue has been implemented as a circular buffer,
 *          which requires the head and tail pointers to be initializes to 0.
 */
int init_queue(flight_queue_t * queue)
{
    queue->tail = 0;
    queue->head = 0;
    return (pthread_mutex_init(&queue->mutex, NULL));
}

/**
 * @brief   dequeues a flight from a flight queue.
 * @param   [out] queue: flight_queue_t*
 *                       -- Pointer to a flight queue data type.
 * @details Because the size of this circular buffer is static and a power of 2,
 *          dequeueing can be managed simply incrementing the head and
 *          masking it with the size of the queue, which prevents the head from
 *          ever pointing outside of the queue.
 */
void dequeue(flight_queue_t *queue)
{
    pthread_mutex_lock(&queue->mutex);
    queue->head++;
    queue->head &= FLIGHT_QUEUE_SIZE_MASK;
    pthread_mutex_unlock(&queue->mutex);
}

/**
 * @brief   enqueues a flight from a flight queue.
 * @param   [out] queue: flight_queue_t*
 *                       -- Pointer to a flight queue data type.
 * @param   [in] flight: flight_t*
 *                       -- pointer to a flight data type.
 * @details Because the size of this circular buffer is static and a power of 2,
 *          enqueueing can be managed simply incrementing the tail and
 *          masking it with the size of the queue, which prevents the tail from
 *          ever pointing outside of the queue.
 */
void enqueue(flight_queue_t *queue, flight_t *flight)
{
    pthread_mutex_lock(&queue->mutex);
    queue->buffer[queue->tail] = flight;
    queue->tail++;
    queue->tail &= FLIGHT_QUEUE_SIZE_MASK;
    pthread_mutex_unlock(&queue->mutex);
}

/**
 * @brief   peeks at the element in from of a flight queue.
 * @param   [in] queue: flight_queue_t*
 *                      -- Pointer to a flight queue data type.
 * @return  flight_t*
 *          -- pointer to the flight element in front of the queue.
 */
flight_t* peek(flight_queue_t *queue)
{
    pthread_mutex_lock(&queue->mutex);
    flight_t *flight = queue->buffer[queue->head];
    pthread_mutex_unlock(&queue->mutex);

    return flight;
}

/**
 * @brief   finds the size of a flight queue.
 * @param   [in] queue: flight_queue_t *
 *                      -- Pointer to a flight queue data type.
 * @details Finding the length of the queue is done by finding the distance
 *          between the head and the tail, and masking it with the size of
 *          the buffer, which accounts for when the tail has circled through
 *          the buffer.
 * @return  uint32_t
 *          -- size of the queue.
 */
uint32_t size(flight_queue_t * queue)
{
    pthread_mutex_lock(&queue->mutex);
    uint32_t size = ((queue->tail - queue->head) & FLIGHT_QUEUE_SIZE_MASK);
    pthread_mutex_unlock(&queue->mutex);

    return size;
}