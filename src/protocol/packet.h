#ifndef PROTOCOL_PACKET_H_
#define PROTOCOL_PACKET_H_

#include "sqpkv/status.h"
#include "rocksdb/slice.h"
#include "rocksdb/status.h"

#include <memory>
#include <vector>

#include <cstring>

namespace sqpkv {

enum OpCode {
  kGet    = 0,
  kPut    = 1,
  kDelete = 2,
  kGetAll = 3,
  kEnd    = 4,
};

/**
 * General packet format:
 * +--------+--------+--------------------------------+
 * | offset |  size  |         description            |
 * +--------+--------+--------------------------------+
 * |   0    |    4   |      size of the payload       |
 * |   4    |   var  |           payload              |
 * +--------+--------+--------------------------------+
 */
class Packet {
public:
  Packet(const char *buf);
  Packet(uint32_t payload_size, char *buf);
  rocksdb::Slice ToBinary() const;

protected:
  uint32_t HeaderSize();
  virtual char *Payload();
  template <typename T>
  char *AddValue(char *buf, T value) {
    memcpy(buf, reinterpret_cast<const char *>(&value), sizeof(value));
    return buf + sizeof(T);
  }
  template <typename T>
  const char *ReadValue(const char *buf, T &value) {
    value = *(reinterpret_cast<const T *>(buf));
    return buf + sizeof(T);
  }
  char *AddSlice(char *buf, rocksdb::Slice data);
  char *AddString(char *buf, const std::string &data);
  const char *ReadSlice(const char *buf, rocksdb::Slice &data);

  void Release();

  static const uint32_t kHeaderSize = 4;
  uint32_t payload_size_;
  std::unique_ptr<char[]> payload_;
};

/**
 *
 * General client(command) payload format:
 * +--------+--------+--------------------------------+
 * | offset |  size  |         description            |
 * +--------+--------+--------------------------------+
 * |    0   |    1   |            op code             |
 * |    0   |   var  |       command extra data       |
 * +--------+--------+--------------------------------+
 */
class CommandPacket : public Packet {
public:
  CommandPacket(const char *buf);
  CommandPacket(uint32_t payload_size, char *buf);
  virtual OpCode GetOp() = 0;
};

/**
 * Payload format:
 * +--------+--------+--------------------------------+
 * | offset |  size  |         description            |
 * +--------+--------+--------------------------------+
 * |    0   |    1   |            op kGet             |
 * |    1   |    4   |           key size             |
 * |    5   |   var  |           key data             |
 * +--------+--------+--------------------------------+
 */
class GetPacket : public CommandPacket {
public:
  GetPacket(const char *buf);
  GetPacket(const std::string &key, char *buf=nullptr);
  rocksdb::Slice key();
  OpCode GetOp();

protected:
  uint32_t HeaderSize();

private:
  rocksdb::Slice key_;
};

/**
 * Payload format:
 * +--------+--------+--------------------------------+
 * | offset |  size  |         description            |
 * +--------+--------+--------------------------------+
 * |    0   |    1   |            op kPut             |
 * |    1   |    4   |           key size             |
 * |    5   |   var  |           key data             |
 * |  var+5 |    4   |          value size            |
 * |  var+9 |   var  |          value data            |
 * +--------+--------+--------------------------------+
 */
class PutPacket : public CommandPacket {
public:
  PutPacket(const char *buf);
  PutPacket(const std::string &key, const std::string &value, char *buf=nullptr);
  rocksdb::Slice key();
  rocksdb::Slice value();
  OpCode GetOp();

protected:
  uint32_t HeaderSize();

private:
  rocksdb::Slice key_;
  rocksdb::Slice value_;
};

/**
 * Payload format:
 * +--------+--------+--------------------------------+
 * | offset |  size  |         description            |
 * +--------+--------+--------------------------------+
 * |    0   |    1   |          op kDelete            |
 * |    1   |    4   |           key size             |
 * |    5   |   var  |           key data             |
 * +--------+--------+--------------------------------+
 */
class DeletePacket : public CommandPacket {
public:
  DeletePacket(const char *buf);
  DeletePacket(const std::string &key, char *buf=nullptr);
  rocksdb::Slice key();
  OpCode GetOp();

protected:
  uint32_t HeaderSize();

private:
  rocksdb::Slice key_;
};

/**
 * Payload format:
 * +--------+--------+--------------------------------+
 * | offset |  size  |         description            |
 * +--------+--------+--------------------------------+
 * |    0   |    1   |           op kGetAll           |
 * |    1   |    4   |          prefix size           |
 * |    5   |   var  |          prefix data           |
 * +--------+--------+--------------------------------+
 */
class GetAllPacket : public CommandPacket {
public:
  GetAllPacket(const char *buf);
  GetAllPacket(const std::string &key, char *buf=nullptr);
  rocksdb::Slice prefix();
  OpCode GetOp();

protected:
  uint32_t HeaderSize();

private:
  rocksdb::Slice prefix_;
};

/**
 * Payload format:
 * +--------+--------+--------------------------------+
 * | offset |  size  |         description            |
 * +--------+--------+--------------------------------+
 * |    0   |    1   |            op kEnd             |
 * +--------+--------+--------------------------------+
 */
class EndPacket : public CommandPacket {
public:
  EndPacket(const char *buf);
  EndPacket();
  OpCode GetOp();
  
protected:
  uint32_t HeaderSize();
};

/**
 * General server(response) payload format:
 * +--------+--------+--------------------------------+
 * | offset |  size  |         description            |
 * +--------+--------+--------------------------------+
 * |    0   |    1   |            op code             |
 * |    1   |    1   |          status_code           |
 * |    2   |    4   |          message size          |
 * |    6   |   var  |         status message         |
 * +--------+--------+--------------------------------+
 * | var+6  |   var  |       response payload         |
 * +--------+--------+--------------------------------+
 */
class ResponsePacket : public Packet {
public:
  ResponsePacket(const char *buf);
  ResponsePacket(uint32_t payload_size, OpCode op, Status status, char *buf);
  virtual Status status();

protected:
  uint32_t HeaderSize();
  void ParseStatus();
  virtual char *Payload() override;

private:
    Status status_;
    uint32_t extra_header_size_;
};

/**
 * Get response payload format:
 * +--------+--------+--------------------------------+
 * | offset |  size  |         description            |
 * +--------+--------+--------------------------------+
 * |    0   |    4   |          value size            |
 * |    4   |   var  |          value data            |
 * +--------+--------+--------------------------------+
 */
class GetResponsePacket : public ResponsePacket {
public:
  GetResponsePacket(const char *buf);
  GetResponsePacket(rocksdb::Status status, const std::string &value, char *buf=nullptr);
  GetResponsePacket(Status status, const std::string &value, char *buf);
  rocksdb::Slice value();

protected:
  uint32_t HeaderSize();

private:
  rocksdb::Slice value_;
};

/**
 * Put response payload format:
 *      Zero (no payload)
 */
class PutResponsePacket : public ResponsePacket {
public:
  PutResponsePacket(const char *buf);
  PutResponsePacket(rocksdb::Status status, char *buf=nullptr);
  PutResponsePacket(Status status, char *buf);
};

/**
 * Delete response payload format:
 *      Zero (no payload)
 */
class DeleteResponsePacket : public ResponsePacket {
public:
  DeleteResponsePacket(const char *buf);
  DeleteResponsePacket(rocksdb::Status status, char *buf=nullptr);
  DeleteResponsePacket(Status status, char *buf);
};

/**
 * GetAll response payload format:
 * +--------+--------+--------------------------------+
 * | offset |  size  |         description            |
 * +--------+--------+--------------------------------+
 * |    0   |    4   |        number of keys          |
 * |    1   |    4   |          value size            |
 * |    5   |   var  |          value data            |
 * |                      ...                         |
 * +--------+--------+--------------------------------+
 */
class GetAllResponsePacket : public ResponsePacket {
public:
  GetAllResponsePacket(const char *buf);
  GetAllResponsePacket(rocksdb::Status status, const std::vector<std::string> &keys, char *buf=nullptr);
  GetAllResponsePacket(Status status, const std::vector<std::string> &keys, char *buf=nullptr);
  std::vector<rocksdb::Slice> keys();
  void AddKeys(std::vector<std::string> &keys);

protected:
  uint32_t HeaderSize();

private:
  std::vector<rocksdb::Slice> keys_;
};

/**
 * Get response payload format:
 * +--------+--------+--------------------------------+
 * | offset |  size  |         description            |
 * +--------+--------+--------------------------------+
 * |    0   |    4   |         message size           |
 * |    4   |   var  |            message             |
 * +--------+--------+--------------------------------+
 */
class EndResponsePacket : public ResponsePacket {
public:
  EndResponsePacket(const char *buf);
  EndResponsePacket(const std::string &message, char *buf=nullptr);
  EndResponsePacket(Status status, const std::string &message, char *buf);
  rocksdb::Slice message();

protected:
  uint32_t HeaderSize();

private:
  rocksdb::Slice message_;
};

class CommandPacketFactory {
public:
  static std::unique_ptr<CommandPacket> CreateCommandPacket(const char *buf);
};

class ResponsePacketFactory {
public:
  static std::unique_ptr<ResponsePacket> CreateResponsePacket(const char *buf);
};

} // namespace sqpkv

#endif // PROTOCOL_PACKET_H_
