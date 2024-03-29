#ifndef PARQUET_PLAIN_ENCODING_H
#define PARQUET_PLAIN_ENCODING_H

#include "encodings.h"

namespace parquet_cpp {

class PlainDecoder : public Decoder {
 public:
  PlainDecoder(const parquet::SchemaElement* schema)
    : Decoder(schema, parquet::Encoding::PLAIN), data_(NULL), len_(0) {
  }

  virtual void SetData(int num_values, const uint8_t* data, int len) {
    num_values_ = num_values;
    data_ = data;
    len_ = len;
  }

  int GetValues(void* buffer, int max_values, int byte_size) {
    max_values = std::min(max_values, num_values_);
    int size = max_values * byte_size;
    if (len_ < size)  ParquetException::EofException();
    memcpy(buffer, data_, size);
    data_ += size;
    len_ -= size;
    num_values_ -= max_values;
    return max_values;
  }

  virtual int GetInt32(int32_t* buffer, int max_values) {
    return GetValues(buffer, max_values, sizeof(int32_t));
  }

  virtual int GetInt64(int64_t* buffer, int max_values) {
    return GetValues(buffer, max_values, sizeof(int64_t));
  }

  virtual int GetFloat(float* buffer, int max_values) {
    return GetValues(buffer, max_values, sizeof(float));
  }

  virtual int GetDouble(double* buffer, int max_values) {
    return GetValues(buffer, max_values, sizeof(double));
  }

  virtual int GetByteArray(ByteArray* buffer, int max_values) {
    max_values = std::min(max_values, num_values_);
    for (int i = 0; i < max_values; ++i) {
      buffer[i].len = *reinterpret_cast<const uint32_t*>(data_);
      if (len_ < sizeof(uint32_t) + buffer[i].len) ParquetException::EofException();
      buffer[i].ptr = data_ + sizeof(uint32_t);
      data_ += sizeof(uint32_t) + buffer[i].len;
      len_ -= sizeof(uint32_t) + buffer[i].len;
    }
    num_values_ -= max_values;
    return max_values;
  }

 private:
  const uint8_t* data_;
  int len_;
};

}

#endif

