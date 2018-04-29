#include "seedindexvalidator.hpp"

QValidator::State SeedIndValidator::validate(QString& input, int& pos) const
{
    Q_UNUSED(pos);

    bool ok = false;
    qlonglong seedInd = input.toLongLong(&ok);

    if (input.isEmpty()) {
        return QValidator::Intermediate;
    }
    else if ((!ok) || (seedInd < 0) || (seedInd > 4294967295)) {
        return QValidator::Invalid;
    }
    else {
        return QValidator::Acceptable;
    }
}
