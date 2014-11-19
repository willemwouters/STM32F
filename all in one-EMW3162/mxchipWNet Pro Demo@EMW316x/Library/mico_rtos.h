#ifndef __MICO_RTOS_H__
#define __MICO_RTOS_H__

#include "mxchipWNET.h"

#define MICO_NEVER_TIMEOUT   (0xFFFFFFFF)
#define MICO_WAIT_FOREVER    (0xFFFFFFFF)
#define MICO_NO_WAIT         (0)

/************************************************************************
 *   MICO thread priority table
 *
 * +----------+-----------------+
 * | Priority |      Thread     |
 * |----------|-----------------|
 * |     0    |      MICO      |   Highest priority
 * |     1    |     Network     |
 * |     2    |                 |
 * |     3    | Network worker  |
 * |     4    |                 |
 * |     5    | Default Library |
 * |          | Default worker  |
 * |     6    |                 |
 * |     7    |   Application   |
 * |     8    |                 |
 * |     9    |      Idle       |   Lowest priority
 * +----------+-----------------+
 */
#define MICO_NETWORK_WORKER_PRIORITY      (3)
#define MICO_DEFAULT_WORKER_PRIORITY      (5)
#define MICO_DEFAULT_LIBRARY_PRIORITY     (5)
#define MICO_APPLICATION_PRIORITY         (7)

/* RTOS APIs*/
typedef void (*mico_thread_function_t)( void* arg );
typedef void* mico_semaphore_t;
typedef void* mico_mutex_t;
typedef void* mico_thread_t;
typedef void* mico_queue_t;

/** Creates and starts a new thread
 *
 * Creates and starts a new thread
 *
 * @param thread     : Pointer to variable that will receive the thread handle
 * @param priority   : A priority number.
 * @param name       : a text name for the thread (can be null)
 * @param function   : the main thread function
 * @param stack_size : stack size for this thread
 * @param arg        : argument which will be passed to thread function
 *
 * @return    MXCHIP_SUCCESS : on success.
 * @return    MXCHIP_FAILED   : if an error occurred
 */
MxchipStatus mico_rtos_create_thread( mico_thread_t* thread, uint8_t priority, const char* name, mico_thread_function_t function, u32 stack_size, void* arg );


/** Deletes a terminated thread
 *
 * @param thread     : the handle of the thread to delete, , NULL is the current thread
 *
 * @return    MXCHIP_SUCCESS : on success.
 * @return    MXCHIP_FAILED   : if an error occurred
 */
MxchipStatus mico_rtos_delete_thread( mico_thread_t* thread );

/** Suspend a thread
 *
 * @param thread     : the handle of the thread to suspend, NULL is the current thread
 *
 * @return    MXCHIP_SUCCESS : on success.
 * @return    MXCHIP_FAILED   : if an error occurred
 */
void mico_rtos_suspend_thread(mico_thread_t* thread);


/** Sleeps until another thread has terminated
 *
 * Causes the current thread to sleep until the specified other thread
 * has terminated. If the processor is heavily loaded
 * with higher priority tasks, this thread may not wake until significantly
 * after the thread termination.
 * Causes the specified thread to wake from suspension. This will usually
 * cause an error or timeout in that thread, since the task it was waiting on
 * is not complete.
 *
 * @param thread : the handle of the other thread which will terminate
 *
 * @return    MXCHIP_SUCCESS : on success.
 * @return    MXCHIP_FAILED   : if an error occurred
 */
MxchipStatus mico_rtos_thread_join( mico_thread_t* thread );


/** Forcibly wakes another thread
 *
 * Causes the specified thread to wake from suspension. This will usually
 * cause an error or timeout in that thread, since the task it was waiting on
 * is not complete.
 *
 * @param thread : the handle of the other thread which will be woken
 *
 * @return    MXCHIP_SUCCESS : on success.
 * @return    MXCHIP_FAILED   : if an error occurred
 */
MxchipStatus mico_rtos_thread_force_awake( mico_thread_t* thread );


/** Checks if a thread is the current thread
 *
 * Checks if a specified thread is the currently running thread
 *
 * @param thread : the handle of the other thread against which the current thread will be compared
 *
 * @return    MXCHIP_SUCCESS : specified thread is the current thread
 * @return    MXCHIP_FAILED   : specified thread is not currently running
 */
MxchipStatus mico_rtos_is_current_thread( mico_thread_t* thread );


/** Initialises a semaphore
 *
 * Initialises a counting semaphore
 *
 * @param semaphore : a pointer to the semaphore handle to be initialised
 * @param count         : the max count number of this semaphore
 *
 * @return    MXCHIP_SUCCESS : on success.
 * @return    MXCHIP_FAILED   : if an error occurred
 */
MxchipStatus mico_rtos_init_semaphore( mico_semaphore_t* semaphore, int count );


/** Set (post/put/increment) a semaphore
 *
 * Set (post/put/increment) a semaphore
 *
 * @param semaphore : a pointer to the semaphore handle to be set
 *
 * @return    MXCHIP_SUCCESS : on success.
 * @return    MXCHIP_FAILED   : if an error occurred
 */
MxchipStatus mico_rtos_set_semaphore( mico_semaphore_t* semaphore );


/** Get (wait/decrement) a semaphore
 *
 * Attempts to get (wait/decrement) a semaphore. If semaphore is at zero already,
 * then the calling thread will be suspended until another thread sets the
 * semaphore with @ref mico_rtos_set_semaphore
 *
 * @param semaphore : a pointer to the semaphore handle
 * @param timeout_ms: the number of milliseconds to wait before returning
 *
 * @return    MXCHIP_SUCCESS : on success.
 * @return    MXCHIP_FAILED   : if an error occurred
 */
MxchipStatus mico_rtos_get_semaphore( mico_semaphore_t* semaphore, u32 timeout_ms );



/** De-initialise a semaphore
 *
 * Deletes a semaphore created with @ref mico_rtos_init_semaphore
 *
 * @param semaphore : a pointer to the semaphore handle
 *
 * @return    MXCHIP_SUCCESS : on success.
 * @return    MXCHIP_FAILED   : if an error occurred
 */
MxchipStatus mico_rtos_deinit_semaphore( mico_semaphore_t* semaphore );


/** Initialises a mutex
 *
 * Initialises a mutex
 * A mutex is different to a semaphore in that a thread that already holds
 * the lock on the mutex can request the lock again (nested) without causing
 * it to be suspended.
 *
 * @param mutex : a pointer to the mutex handle to be initialised
 *
 * @return    MXCHIP_SUCCESS : on success.
 * @return    MXCHIP_FAILED   : if an error occurred
 */
MxchipStatus mico_rtos_init_mutex( mico_mutex_t* mutex );


/** Obtains the lock on a mutex
 *
 * Attempts to obtain the lock on a mutex. If the lock is already held
 * by another thead, the calling thread will be suspended until
 * the mutex lock is released by the other thread.
 *
 * @param mutex : a pointer to the mutex handle to be locked
 *
 * @return    MXCHIP_SUCCESS : on success.
 * @return    MXCHIP_FAILED   : if an error occurred
 */
MxchipStatus mico_rtos_lock_mutex( mico_mutex_t* mutex );


/** Releases the lock on a mutex
 *
 * Releases a currently held lock on a mutex. If another thread
 * is waiting on the mutex lock, then it will be resumed.
 *
 * @param mutex : a pointer to the mutex handle to be unlocked
 *
 * @return    MXCHIP_SUCCESS : on success.
 * @return    MXCHIP_FAILED   : if an error occurred
 */
MxchipStatus mico_rtos_unlock_mutex( mico_mutex_t* mutex );


/** De-initialise a mutex
 *
 * Deletes a mutex created with @ref mico_rtos_init_mutex
 *
 * @param mutex : a pointer to the mutex handle
 *
 * @return    MXCHIP_SUCCESS : on success.
 * @return    MXCHIP_FAILED   : if an error occurred
 */
MxchipStatus mico_rtos_deinit_mutex( mico_mutex_t* mutex );


/** Initialises a queue
 *
 * Initialises a FIFO queue
 *
 * @param queue : a pointer to the queue handle to be initialised
 * @param name  : a text string name for the queue (NULL is allowed)
 * @param message_size : size in bytes of objects that will be held in the queue
 * @param number_of_messages : depth of the queue - i.e. max number of objects in the queue
 *
 * @return    MXCHIP_SUCCESS : on success.
 * @return    MXCHIP_FAILED   : if an error occurred
 */
MxchipStatus mico_rtos_init_queue( mico_queue_t* queue, const char* name, u32 message_size, u32 number_of_messages );


/** Pushes an object onto a queue
 *
 * Pushes an object onto a queue
 *
 * @param queue : a pointer to the queue handle
 * @param message : the object to be added to the queue. Size is assumed to be
 *                  the size specified in @ref mico_rtos_init_queue
 * @param timeout_ms: the number of milliseconds to wait before returning
 *
 * @return    MXCHIP_SUCCESS : on success.
 * @return    MXCHIP_FAILED   : if an error or timeout occurred
 */
MxchipStatus mico_rtos_push_to_queue( mico_queue_t* queue, void* message, u32 timeout_ms );


/** Pops an object off a queue
 *
 * Pops an object off a queue
 *
 * @param queue : a pointer to the queue handle
 * @param message : pointer to a buffer that will receive the object being
 *                  popped off the queue. Size is assumed to be
 *                  the size specified in @ref mico_rtos_init_queue , hence
 *                  you must ensure the buffer is long enough or memory
 *                  corruption will result
 * @param timeout_ms: the number of milliseconds to wait before returning
 *
 * @return    MXCHIP_SUCCESS : on success.
 * @return    MXCHIP_FAILED   : if an error or timeout occurred
 */
MxchipStatus mico_rtos_pop_from_queue( mico_queue_t* queue, void* message, u32 timeout_ms );


/** De-initialise a queue
 *
 * Deletes a queue created with @ref mico_rtos_init_queue
 *
 * @param queue : a pointer to the queue handle
 *
 * @return    MXCHIP_SUCCESS : on success.
 * @return    MXCHIP_FAILED   : if an error occurred
 */
MxchipStatus mico_rtos_deinit_queue( mico_queue_t* queue );


/** Check if a queue is empty
 *
 * @param queue : a pointer to the queue handle
 *
 * @return    MXCHIP_SUCCESS : queue is empty.
 * @return    MXCHIP_FAILED   : queue is not empty.
 */
MxchipStatus mico_rtos_is_queue_empty( mico_queue_t* queue );


/** Check if a queue is full
 *
 * @param queue : a pointer to the queue handle
 *
 * @return    MXCHIP_SUCCESS : queue is full.
 * @return    MXCHIP_FAILED   : queue is not full.
 */
MxchipStatus mico_rtos_is_queue_full( mico_queue_t* queue );

/** Enables the MCU to enter powersave mode.
*
* @param enable : 1=enable MCU powersave, 0=disable MCU powersave
* @return    void
*/
void mico_mcu_powersave_config( int enable );


/**
 * Gets time in miiliseconds since RTOS start
 *
 * @Note: since this is only 32 bits, it will roll over every 49 days, 17 hours.
 *
 * @returns Time in milliseconds since RTOS started.
 */
u32 mico_get_time(void);

/** Enables the C library thread safe.
*
* @param      void
* @return    void
*/
void mico_clib_thread_safe_init(void);

#endif

