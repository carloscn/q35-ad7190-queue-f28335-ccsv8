
#include "queue.h"


void __resize(QUEUE* self, uint32_t new_capacity)
{
    uint32_t i = 0;
    uint16_t* new_data = (uint16_t *)malloc( (sizeof(uint16_t) * (new_capacity + 1)));
    for (i = 0; i < self->size; i ++) {
        new_data[i] = self->data[(i + self->front) % self->capacity];
    }
    free(self->data);
    self->data = new_data;
    self->front = 0;
    self->tail = self->size;
    self->capacity = new_capacity;
}

void loop_queue_queue(QUEUE* self, uint16_t e)
{
    if ((self->tail + 1) % self->capacity == self->front)
        __resize(self, self->capacity * 2);
    self->data[self->tail] = e;
    self->tail = (self->tail + 1) % self->capacity;
    self->size ++;

}

uint16_t loop_queue_dequeue(QUEUE* self)
{
    uint16_t ret = 0;

    if (self->is_empty(self))
        return NULL;
    ret = self->data[self->front];
    self->data[self->front] = NULL;
    self->front = (self->front + 1) % self->capacity;
    self->size --;
    if ((self->size == self->capacity/4) && (self->capacity / 2 != 0))
        __resize(self, self->capacity / 2);

    return ret;
}

uint16_t loop_queue_get_front(QUEUE* self)
{
    if (self->is_empty(self))
        return NULL;

    return self->data[self->front];
}

uint32_t loop_queue_get_size(QUEUE* self)
{
    return self->size;
}

bool loop_queue_is_empty(QUEUE* self)
{

    return (self->front == self->tail)?true:false;
}

uint32_t loop_queue_get_capacity(QUEUE *self)
{
    return self->capacity;
}

void loop_queue_init(QUEUE* self, uint32_t capacity)
{
    __init(self, capacity);
}

uint16_t loop_queue_get_max_value(QUEUE* self)
{
    uint16_t i = 0;
    uint16_t max = NULL;

    if (self->is_empty(self))
        return max;
    max = self->data[self->front];
    for (i = 0; i < self->size; i ++ ) {
        if (max < self->data[(i + self->front) % self->capacity])
            max = self->data[(i + self->front) % self->capacity];
    }
    return max;
}

uint16_t loop_queue_get_min_value(QUEUE* self)
{
    uint16_t i = 0;
    uint16_t min = NULL;

    if (self->is_empty(self))
        return min;
    min = self->data[self->front];
    for (i = 0; i < self->size; i ++ ) {
        if (min > self->data[(i + self->front) % self->capacity])
            min = self->data[(i + self->front) % self->capacity];
    }

    return min;
}

uint32_t loop_queue_dequeue_all(QUEUE* self, uint16_t *des)
{
    uint32_t i = 0;

    while (self->is_empty(self) == false) {
        *(des + i) = self->dequeue(self);
        i ++;
    }

    return i;
}

bool loop_queue_dequeue_len(QUEUE* self, uint16_t *des, uint32_t length)
{
    bool flag;
    uint32_t i;
    if (length > self->size)
        flag = false;
    else if (length == self->size){
        self->dequeue_all(self, des);
        flag = true;
    } else {
        for (i = 0 ; i < length; i++)
            *(des + i) = self->dequeue(self);
        flag = true;
    }

    return flag;
}

void loop_queue_append(QUEUE* self, uint16_t *src, uint32_t length)
{
    uint32_t i;
    for (i = 0; i < length; i ++) {
        self->queue(self, *(src + i));
    }
}

void __init(QUEUE* self, uint32_t capacity)
{
    self->capacity = capacity;
    self->size = 0;
    self->front = 0;
    self->tail = 0;
    self->data = (uint16_t*)malloc( (sizeof(uint16_t)*(capacity + 1)));
    self->init = &loop_queue_init;
    self->queue = &loop_queue_queue;
    self->dequeue = &loop_queue_dequeue;
    self->get_front = &loop_queue_get_front;
    self->get_size = &loop_queue_get_size;
    self->is_empty = &loop_queue_is_empty;
    self->get_capacity = &loop_queue_get_capacity;
    self->get_max_value = &loop_queue_get_max_value;
    self->get_min_value = &loop_queue_get_min_value;
    self->append = &loop_queue_append;
    self->dequeue_all = &loop_queue_dequeue_all;
    self->dequeue_len = &loop_queue_dequeue_len;
}

QUEUE* new_queue(uint32_t capacity)
{

    QUEUE* queue = (QUEUE*)malloc( sizeof(QUEUE));
    __init(queue, capacity);

    return queue;
}




