#include "ringbuffer.h"

RingBuffer::RingBuffer(qsizetype capacity)
    : m_capacity(capacity > 0 ? capacity : 1)
    , m_buffer(m_capacity, Qt::Uninitialized)
{

}


qsizetype RingBuffer::write(const void *data, qsizetype len)
{
    if (!data || len <= 0 || m_size == m_capacity)
        return 0;

    const qsizetype toWrite = std::min(len, m_capacity - m_size);
    const char *src = static_cast<const char *>(data);

    // 第一段：写到缓冲区尾部为止
    const qsizetype firstPart = std::min(toWrite, m_capacity - m_head);
    std::memcpy(m_buffer.data() + m_head, src, firstPart);
    // 第二段：环绕回头部继续写
    if (toWrite > firstPart)
        std::memcpy(m_buffer.data(), src + firstPart, toWrite - firstPart);

    m_head = (m_head + toWrite) % m_capacity;
    m_size += toWrite;
    return toWrite;   // 小于 len 说明发生了溢出由上层处理
}



qsizetype RingBuffer::write(const QByteArray &data)
{
    //constData()拿到数组的指针，只读形式，任何情况下都可调用
    return write(data.constData(), data.size());
}



qsizetype RingBuffer::peek(void *data, qsizetype len, qsizetype offset) const
{
    if (!data || len <= 0 || offset >= m_size)
        return 0;

    const qsizetype toRead = std::min(len, m_size - offset);
    char *dst = static_cast<char *>(data);

    qsizetype pos = (m_tail + offset) % m_capacity;
    const qsizetype firstPart = std::min(toRead, m_capacity - pos);
    std::memcpy(dst, m_buffer.constData() + pos, firstPart);
    if (toRead > firstPart)   // 跨越尾部的环绕段
        std::memcpy(dst + firstPart, m_buffer.constData(), toRead - firstPart);

    return toRead;
}



QByteArray RingBuffer::peekBytes(qsizetype len, qsizetype offset) const
{
    const qsizetype available = m_size - offset;
    if (available <= 0)
        return {};
    const qsizetype got = std::min(len, available);
    QByteArray out(got, Qt::Uninitialized);
    peek(out.data(), got, offset);
    return out;
}



qsizetype RingBuffer::read(void *data, qsizetype len)
{
    const qsizetype got = peek(data, len, 0);
    if (got > 0)
        discard(got);
    return got;
}



QByteArray RingBuffer::readBytes(qsizetype len)
{
    QByteArray out = peekBytes(len);
    discard(out.size());
    return out;
}

void RingBuffer::discard(qsizetype len)
{
    const qsizetype n = std::min(len, m_size);
    m_tail = (m_tail + n) % m_capacity;
    m_size -= n;
}

qsizetype RingBuffer::size()
{
    return m_size;
}

qsizetype RingBuffer::capacity()
{
    return m_capacity;
}

qsizetype RingBuffer::freeSpace()
{
    return m_capacity - m_size;
}

bool RingBuffer::isEmpty()
{
    return m_size == 0;
}

bool RingBuffer::isFull()
{
    return m_size == m_capacity;
}

void RingBuffer::clear()
{
     m_head = m_tail = m_size = 0;
}


