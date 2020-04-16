#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>

#include "circular_buffer/circular_buffer.h"
// The hidden definition of our circular buffer structure
struct circular_buf_t {
	uint8_t * buffer;
	size_t head;
	size_t tail;
	size_t max; //of the buffer
	bool full;
};
#define EXAMPLE_BUFFER_SIZE 10

void print_buffer_status(cbuf_handle_t cbuf);
/// Pass in a storage buffer and size 
/// Returns a circular buffer handle
cbuf_handle_t circular_buf_init(uint8_t* buffer, size_t size);

/// Free a circular buffer structure.
/// Does not free data buffer; owner is responsible for that
void circular_buf_free(cbuf_handle_t cbuf);

/// Reset the circular buffer to empty, head == tail
void circular_buf_reset(cbuf_handle_t cbuf);

/// Put version 1 continues to add data if the buffer is full
/// Old data is overwritten
void circular_buf_put(cbuf_handle_t cbuf, uint8_t data);

/// Put Version 2 rejects new data if the buffer is full
/// Returns 0 on success, -1 if buffer is full
int circular_buf_put2(cbuf_handle_t cbuf, uint8_t data);

/// Retrieve a value from the buffer
/// Returns 0 on success, -1 if the buffer is empty
int circular_buf_get(cbuf_handle_t cbuf, uint8_t * data);

/// Returns true if the buffer is empty
bool circular_buf_empty(cbuf_handle_t cbuf);

/// Returns true if the buffer is full
bool circular_buf_full(cbuf_handle_t cbuf);

/// Returns the maximum capacity of the buffer
size_t circular_buf_capacity(cbuf_handle_t cbuf);

/// Returns the current number of elements in the buffer
size_t circular_buf_size(cbuf_handle_t cbuf);


cbuf_handle_t circular_buf_init(uint8_t* buffer, size_t size)
{
	assert(buffer && size);

	cbuf_handle_t cbuf = malloc(sizeof(circular_buf_t));
	assert(cbuf);

	cbuf->buffer = buffer;
	cbuf->max = size;
	circular_buf_reset(cbuf);

	assert(circular_buf_empty(cbuf));

	return cbuf;
}
void circular_buf_free(cbuf_handle_t cbuf)
{
	assert(cbuf);
	free(cbuf);
}
void circular_buf_reset(cbuf_handle_t cbuf)
{
    assert(cbuf);

    cbuf->head = 0;
    cbuf->tail = 0;
    cbuf->full = false;
}
bool circular_buf_full(cbuf_handle_t cbuf)
{
	assert(cbuf);

    return cbuf->full;
}
size_t circular_buf_capacity(cbuf_handle_t cbuf)
{
	assert(cbuf);

	return cbuf->max;
}
size_t circular_buf_size(cbuf_handle_t cbuf)
{
	assert(cbuf);

	size_t size = cbuf->max;

	if(!cbuf->full)
	{
		if(cbuf->head >= cbuf->tail)
		{
			size = (cbuf->head - cbuf->tail);
		}
		else
		{
			size = (cbuf->max + cbuf->head - cbuf->tail);
		}
	}

	return size;
}
bool circular_buf_empty(cbuf_handle_t cbuf)
{
	assert(cbuf);

    return (!cbuf->full && (cbuf->head == cbuf->tail));
}
static void advance_pointer(cbuf_handle_t cbuf)
{
	assert(cbuf);

	if(cbuf->full)
   	{
		cbuf->tail = (cbuf->tail + 1) % cbuf->max;
	}

	cbuf->head = (cbuf->head + 1) % cbuf->max;
	cbuf->full = (cbuf->head == cbuf->tail);
}
static void retreat_pointer(cbuf_handle_t cbuf)
{
	assert(cbuf);

	cbuf->full = false;
	cbuf->tail = (cbuf->tail + 1) % cbuf->max;
}
void circular_buf_put(cbuf_handle_t cbuf, uint8_t data)
{
	assert(cbuf && cbuf->buffer);

    cbuf->buffer[cbuf->head] = data;

    advance_pointer(cbuf);
}
int circular_buf_get(cbuf_handle_t cbuf, uint8_t * data)
{
    assert(cbuf && data && cbuf->buffer);

    int r = -1;

    if(!circular_buf_empty(cbuf))
    {
        *data = cbuf->buffer[cbuf->tail];
        retreat_pointer(cbuf);

        r = 0;
    }

    return r;
}

uint8_t * buffer  = malloc(EXAMPLE_BUFFER_SIZE * sizeof(uint8_t));
cbuf_handle_t cbuf = circular_buf_init(buffer, 
	EXAMPLE_BUFFER_SIZE);

int main(void)
{
	uint8_t * buffer  = malloc(EXAMPLE_BUFFER_SIZE * sizeof(uint8_t));

	cbuf_handle_t cbuf = circular_buf_init(buffer, EXAMPLE_BUFFER_SIZE);
	print_buffer_status(cbuf);

	for(uint8_t i = 0; i < (EXAMPLE_BUFFER_SIZE - 1); i++)
	{
		circular_buf_put(cbuf, i);
		printf("Added %u, Size now: %zu\n", i, circular_buf_size(cbuf));
	}

	printf("\n******\nAdding %d values\n", EXAMPLE_BUFFER_SIZE);
	for(uint8_t i = 0; i < EXAMPLE_BUFFER_SIZE; i++)
	{
		circular_buf_put(cbuf, i);
		printf("Added %u, Size now: %zu\n", i, circular_buf_size(cbuf));
	}

	print_buffer_status(cbuf);

	printf("\n******\nReading back values: ");
	while(!circular_buf_empty(cbuf))
	{
		uint8_t data;
		circular_buf_get(cbuf, &data);
		printf("%u ", data);
	}
	printf("\n");

	print_buffer_status(cbuf);

	printf("\n******\nAdding %d values\n", EXAMPLE_BUFFER_SIZE + 5);
	for(uint8_t i = 0; i < EXAMPLE_BUFFER_SIZE + 5; i++)
	{
		circular_buf_put(cbuf, i);
		printf("Added %u, Size now: %zu\n", i, circular_buf_size(cbuf));
	}

	print_buffer_status(cbuf);

	printf("\n******\nReading back values: ");
	while(!circular_buf_empty(cbuf))
	{
		uint8_t data;
		circular_buf_get(cbuf, &data);
		printf("%u ", data);
	}
	printf("\n");

	printf("\n******\nAdding %d values using non-overwrite version\n", EXAMPLE_BUFFER_SIZE + 5);
	for(uint8_t i = 0; i < EXAMPLE_BUFFER_SIZE + 5; i++)
	{
		circular_buf_put2(cbuf, i);
	}

	print_buffer_status(cbuf);

	printf("\n******\nReading back values: ");
	while(!circular_buf_empty(cbuf))
	{
		uint8_t data;
		circular_buf_get(cbuf, &data);
		printf("%u ", data);
	}
	printf("\n");

	free(buffer);
	circular_buf_free(cbuf);

	return 0;
}

void print_buffer_status(cbuf_handle_t cbuf)
{
	printf("Full: %d, empty: %d, size: %zu\n",
		circular_buf_full(cbuf),
		circular_buf_empty(cbuf),
		circular_buf_size(cbuf));
}
