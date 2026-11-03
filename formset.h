#ifndef FORMSET_H
#define FORMSET_H

#include <QWidget>
namespace Ui { class Form; }
class FormSet : public QWidget
{
    Q_OBJECT
public:
    explicit FormSet(QWidget *parent = nullptr);
    ~FormSet() override;


signals:

};

#endif // FORMSET_H
