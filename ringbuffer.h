#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <QByteArray>
#include <algorithm>
#include <cstring>

class RingBuffer
{
public:
    explicit RingBuffer(qsizetype capacity = 4096);

public:
    qsizetype write(const void *data, qsizetype len);
    qsizetype write(const QByteArray &data);
    qsizetype peek(void *data, qsizetype len, qsizetype offset = 0) const;
    QByteArray peekBytes(qsizetype len, qsizetype offset = 0) const;
    qsizetype read(void *data, qsizetype len);
    QByteArray readBytes(qsizetype len);
    void discard(qsizetype len);
    qsizetype size();
    qsizetype capacity();
    qsizetype freeSpace();
    bool isEmpty();
    bool isFull();
    void clear();


private:
    qsizetype  m_capacity;   //缓冲区空间大小
    QByteArray m_buffer;
    qsizetype  m_head = 0;   // 写索引
    qsizetype  m_tail = 0;   // 读索引
    qsizetype  m_size = 0;   // 有效数据长度（解决空满判定）


    // ==================== 状态 ====================
    qsizetype size()      const { return m_size; }
    qsizetype capacity()  const { return m_capacity; }
    qsizetype freeSpace() const { return m_capacity - m_size; }
    bool isEmpty()        const { return m_size == 0; }
    bool isFull()         const { return m_size == m_capacity; }

    // void clear() { m_head = m_tail = m_size = 0; }

};

#endif // RINGBUFFER_H
