// From libweaver: https://github.com/itsmattkc/libweaver

#include "file.h"

namespace f {

bool File::Open(const char *c, Mode mode)
{
  std::ios::openmode m = std::ios::binary;

  if (mode == Read) {
    m |= std::ios::in;
  } else {
    m |= std::ios::out;
  }

  m_Stream.open(c, m);
  if (m_Stream.good() && m_Stream.is_open()) {
    m_Mode = mode;
    return true;
  }

  return false;
}

#ifdef _WIN32
bool File::Open(const wchar_t *c, Mode mode)
{
  std::ios::openmode m = std::ios::binary;

  if (mode == Read) {
    m |= std::ios::in;
  } else {
    m |= std::ios::out;
  }

  m_Stream.open(c, m);
  if (m_Stream.good() && m_Stream.is_open()) {
    m_Mode = mode;
    return true;
  }

  return false;
}
#endif

size_t File::pos()
{
  if (m_Mode == Read) {
    return m_Stream.tellg();
  } else {
    return m_Stream.tellp();
  }
}

size_t File::size()
{
  size_t before = pos();
  seek(0, SeekEnd);
  size_t sz = pos();
  seek(before, SeekStart);
  return sz;
}

void File::seek(size_t p, SeekMode s)
{
  std::ios::seekdir d;

  switch (s) {
  case SeekStart:
    d = std::ios::beg;
    break;
  case SeekCurrent:
    d = std::ios::cur;
    break;
  case SeekEnd:
    d = std::ios::end;
    break;
  }

  if (m_Mode == Read) {
    m_Stream.seekg(p, d);
  } else {
    m_Stream.seekp(p, d);
  }
}

bool File::atEnd()
{
  return !m_Stream.good();
}

void File::Close()
{
  m_Stream.close();
}

size_t File::ReadData(void *data, size_t size)
{
  size_t before = this->pos();
  m_Stream.read((char *) data, size);
  return this->pos() - before;
}

size_t File::WriteData(const void *data, size_t size)
{
  size_t before = this->pos();
  m_Stream.write((const char *) data, size);
  return this->pos() - before;
}

uint8_t FileBase::ReadU8()
{
  uint8_t u;
  ReadData(&u, sizeof(u));
  return u;
}

void FileBase::WriteU8(uint8_t u)
{
  WriteData(&u, sizeof(u));
}

uint16_t FileBase::ReadU16()
{
  uint16_t u;
  ReadData(&u, sizeof(u));
  return u;
}

void FileBase::WriteU16(uint16_t u)
{
  WriteData(&u, sizeof(u));
}

uint32_t FileBase::ReadU32()
{
  uint32_t u;
  ReadData(&u, sizeof(u));
  return u;
}

float FileBase::ReadFloat()
{
    float u;
    ReadData(&u, sizeof(u));
    return u;
}

void FileBase::WriteU32(uint32_t u)
{
  WriteData(&u, sizeof(u));
}

Vector3 FileBase::ReadVector3()
{
  Vector3 u;
  ReadData(&u, sizeof(u));
  return u;
}

void FileBase::WriteVector3(const Vector3 &v)
{
  WriteData(&v, sizeof(v));
}

std::string FileBase::ReadString()
{
  std::string d;

  while (true) {
    char c;
    ReadData(&c, 1);
    if (c == 0) {
      break;
    }
    d.push_back(c);
  }

  return d;
}

void FileBase::WriteString(const std::string &d)
{
  WriteData(d.c_str(), d.size());

  // Ensure null terminator
  WriteU8(0);
}

bytearray FileBase::ReadBytes(size_t size)
{
  bytearray d;

  d.resize(size);
  ReadData(d.data(), size);

  return d;
}

void FileBase::WriteBytes(const bytearray &ba)
{
  WriteData(ba.data(), ba.size());
}

MemoryBuffer::MemoryBuffer()
{
  m_Position = 0;
}

MemoryBuffer::MemoryBuffer(const bytearray &data)
{
  m_Internal = data;
  m_Position = 0;
}

size_t MemoryBuffer::pos()
{
  return m_Position;
}

size_t MemoryBuffer::size()
{
  return m_Internal.size();
}

void MemoryBuffer::seek(size_t p, SeekMode s)
{
  switch (s) {
  case SeekStart:
    m_Position = p;
    break;
  case SeekCurrent:
    m_Position += p;
    break;
  case SeekEnd:
    m_Position = std::max(size_t(0), m_Internal.size() - p);
    break;
  }
}

bool MemoryBuffer::atEnd()
{
  return m_Position == m_Internal.size();
}

size_t MemoryBuffer::ReadData(void *data, size_t size)
{
  size = std::min(size, m_Internal.size() - m_Position);
  memcpy(data, m_Internal.data() + m_Position, size);
  m_Position += size;
  return size;
}

size_t MemoryBuffer::WriteData(const void *data, size_t size)
{
  size_t end = m_Position + size;
  if (end > m_Internal.size()) {
    m_Internal.resize(end);
  }
  memcpy(m_Internal.data() + m_Position, data, size);
  m_Position += size;
  return size;
}

}
