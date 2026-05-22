#ifndef ODEZHDA_H
#define ODEZHDA_H

#include "Tovar.h"

class Odezhda : public Tovar {
    std::string razmer;
    std::string cvet;
public:
    Odezhda(std::string artikul, std::string naimenovanie,
            std::string razmer, std::string cvet,
            double cena, int kolichestvo)
        : Tovar(artikul, naimenovanie, cena, kolichestvo),
          razmer(razmer), cvet(cvet) {}

    std::string get_razmer() const { return razmer; }
    std::string get_cvet() const { return cvet; }

    std::string get_kategoriya() const override { return "Одежда"; }

    std::string get_info() const override {
        std::ostringstream out;
        out << "[Одежда] " << artikul << "; " << naimenovanie
            << "; размер=" << razmer << "; цвет=" << cvet
            << "; цена=" << cena << "; на складе=" << kolichestvo;
        return out.str();
    }
};

#endif
