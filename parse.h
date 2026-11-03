#ifndef PARSE_H
#define PARSE_H

#include <QObject>

class Parse : public QObject
{
    Q_OBJECT
public:
    explicit Parse(QObject *parent = nullptr);

signals:
};

#endif // PARSE_H
