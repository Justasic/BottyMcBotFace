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
#include <cstdint>

// This is intended to be used so that you can write to it
// as if it were a file of infinite size but it's a memory
// block. This memory block is self-expanding and self-freeing.
class ManagedBuffer
{
        typedef struct
        {
            void *data;
            size_t size;
            size_t allocatedsz;
            int refs;
        } internaldata_t;
        internaldata_t *data;
public:
        ManagedBuffer();
        ~ManagedBuffer();

        void Write(const void *data, size_t size);
        void AllocateAhead(size_t sz);
        inline size_t length() const { return this->data->size; }
        inline size_t size() const { return this->data->size; }
        inline void *GetPointer() const { return this->data->data; }
        inline void *GetEndPointer() const { return reinterpret_cast<void*>((reinterpret_cast<uint8_t*>(this->data->data) + this->data->size)); }

        inline void operator +=(size_t sz) { this->data->size += sz; }
        inline void operator  =(size_t sz) { this->data->size = sz; }
        // Conveninece function
        inline void* operator*() const { return this->data->data; }

        // Used for reference counting.
        // See http://www.linuxprogrammingblog.com/cpp-objects-reference-counting
        ManagedBuffer(const ManagedBuffer &other);
        ManagedBuffer &operator= (const ManagedBuffer &other);
};
