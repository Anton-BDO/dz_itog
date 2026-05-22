#ifndef ELEKTRONIKA_H
#define ELEKTRONIKA_H

#include "Tovar.h"

class Elektronika : public Tovar {
    std::string model;
    std::string proizvoditel;
public:
    Elektronika(std::string artikul, std::string naimenovanie,
                std::string model, std::string proizvoditel,
                double cena, int kolichestvo)
        : Tovar(artikul, naimenovanie, cena, kolichestvo),
          model(model), proizvoditel(proizvoditel) {}

    std::string get_model() const { return model; }
    std::string get_proizvoditel() const { return proizvoditel; }

    std::string get_kategoriya() const override { return "Электроника"; }

    std::string get_info() const override {
        std::ostringstream out;
        out << "[Электроника] " << artikul << "; " << naimenovanie
            << "; модель=" << model << "; производитель=" << proizvoditel
            << "; цена=" << cena << "; на складе=" << kolichestvo;
        return out.str();
    }
};

#endif
