#ifndef SEEDINDEXVALIDATOR_HPP
#define SEEDINDEXVALIDATOR_HPP

#include <QValidator>

class SeedIndValidator : public QValidator {
    Q_OBJECT
public:
    explicit SeedIndValidator(QObject* parent = nullptr)
        : QValidator(parent) {}
    State validate(QString&, int&) const;
};

#endif
