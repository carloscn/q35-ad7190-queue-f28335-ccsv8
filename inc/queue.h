/*
 * queue.h
 *
 *  Created on: 2018Äê10ÔÂ28ÈÕ
 *      Author: delvis
 */

#ifndef INC_QUEUE_H_
#define INC_QUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct queue_t QUEUE;

struct queue_t {

    uint32_t data_len;
    uint16_t *data;     // malloc
    uint32_t front, tail;
    uint32_t size;
    uint32_t capacity;

    bool (*is_empty)(QUEUE* self);
    void (*init)(QUEUE* self,uint32_t capacity);
    void (*queue)(QUEUE* self,uint16_t e);
    uint16_t (*dequeue)(QUEUE* self);
    uint16_t (*get_front)(QUEUE* self);
    uint32_t (*get_size)(QUEUE* self);
    uint32_t (*get_capacity)(QUEUE* self);
    uint16_t (*get_max_value)(QUEUE* self);
    uint16_t (*get_min_value)(QUEUE* self);
    bool (*dequeue_all)(QUEUE* self, uint16_t *des);
    int (*dequeue_len)(QUEUE* self, uint16_t *des, uint32_t length);
    void (*append)(QUEUE* self, uint16_t *src, uint32_t length);
};



void __init(QUEUE* self, uint32_t capacity);
void __resize(QUEUE* self, uint32_t new_capacity);

extern QUEUE* new_queue(uint32_t capacity);



#endif /* INC_QUEUE_H_ */
