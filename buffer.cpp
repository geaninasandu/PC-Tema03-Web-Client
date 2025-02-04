#include "buffer.h"

buffer buffer_init()
{
    buffer buffer;

    buffer.data = nullptr;
    buffer.size = 0;

    return buffer;
}

void buffer_add(buffer *buffer, const char *data, size_t data_size)
{
    if (buffer->data != nullptr) {
        buffer->data = static_cast<char *>(realloc(buffer->data,
                                                   (buffer->size + data_size) *
                                                   sizeof(char)));
    } else {
        buffer->data = static_cast<char *>(calloc(data_size, sizeof(char)));
    }

    memcpy(buffer->data + buffer->size, data, data_size);

    buffer->size += data_size;
}

int buffer_find(buffer *buffer, const char *data, size_t data_size)
{
    if (data_size > buffer->size)
        return -1;

    size_t last_pos = buffer->size - data_size + 1;

    for (size_t i = 0; i < last_pos; ++i) {
        size_t j;

        for (j = 0; j < data_size; ++j) {
            if (buffer->data[i + j] != data[j]) {
                break;
            }
        }

        if (j == data_size)
            return i;
    }

    return -1;
}

int buffer_find_insensitive(buffer *buffer, const char *data, size_t data_size)
{
    if (data_size > buffer->size)
        return -1;

    size_t last_pos = buffer->size - data_size + 1;

    for (size_t i = 0; i < last_pos; ++i) {
        size_t j;

        for (j = 0; j < data_size; ++j) {
            if (tolower(buffer->data[i + j]) != tolower(data[j])) {
                break;
            }
        }

        if (j == data_size)
            return i;
    }

    return -1;
}
