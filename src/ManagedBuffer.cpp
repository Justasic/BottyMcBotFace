/*
 * Copyright (c) 2017, Justin Crawford <Justin@stacksmash.net>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright notice
 * and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <new>
#include <algorithm>
#include "ManagedBuffer.h"

ManagedBuffer::ManagedBuffer()
{
        this->data = new internaldata_t;
        memset(this->data, 0, sizeof(internaldata_t));
        this->data->refs++;
        this->data->allocatedsz = 1024;
        this->data->size = 0;
        this->data->data = malloc(this->data->allocatedsz);
        if (!this->data->data)
                throw std::bad_alloc();

        // Initialize the buffer.
        memset(this->data->data, 0, this->data->allocatedsz);
}

ManagedBuffer::ManagedBuffer(const ManagedBuffer &other)
{
        this->data = other.data;
        this->data->refs++;
}

ManagedBuffer &ManagedBuffer::operator= (const ManagedBuffer &other)
{
        this->data = other.data;
        this->data->refs++;
        return *this;
}

ManagedBuffer::~ManagedBuffer()
{
        this->data->refs--;
        if (!this->data->refs)
        {
                // condition should never happen.
                assert(this->data && this->data->data);
                free(this->data->data);
                delete this->data;
                this->data = nullptr;
        }
}


void ManagedBuffer::Write(const void *ddata, size_t size)
{
        if (this->data->size + size > this->data->allocatedsz)
        {
                size_t newsz = std::max(this->data->size + size, this->data->size + 1024UL);
                void *ptr = realloc(this->data->data, newsz);
                if (!ptr)
                        throw std::bad_alloc();

                this->data->data = ptr;
                this->data->allocatedsz = newsz;
                // Initialize the new area
                memset(reinterpret_cast<uint8_t*>(this->data->data) + this->data->size, 0, newsz - this->data->size);
        }

        // Copy our data.
        memcpy(reinterpret_cast<uint8_t*>(this->data->data) + this->data->size, ddata, size);
        this->data->size += size;
}

void ManagedBuffer::AllocateAhead(size_t sz)
{
        size_t newsz = this->data->allocatedsz + sz;
        void *ptr = realloc(this->data->data, newsz);
        if (!ptr)
                throw std::bad_alloc();

        this->data->data = ptr;
        this->data->allocatedsz = newsz;
        memset(reinterpret_cast<uint8_t*>(this->data->data) + this->data->size, 0, newsz - this->data->size);
}
