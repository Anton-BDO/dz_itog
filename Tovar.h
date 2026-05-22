#ifndef TOVAR_H
#define TOVAR_H

#include <string>
#include <sstream>
#include <stdexcept>

// Базовый класс товара. Содержит общие поля и виртуальные методы, которые переопределяются в наследниках Odezhda, Elektronika, Kniga
class Tovar {
protected:
    std::string artikul;
    std::string naimenovanie;
    double cena;
    int kolichestvo;
public:
    Tovar(std::string artikul, std::string naimenovanie, double cena, int kolichestvo)
        : artikul(artikul), naimenovanie(naimenovanie),
          cena(cena), kolichestvo(kolichestvo)
    {
        if (cena < 0)
            throw std::invalid_argument("Отрицательная цена у товара " + this->artikul);
        if (kolichestvo < 0)
            throw std::invalid_argument("Отрицательное количество у товара " + this->artikul);
    }

    virtual ~Tovar() {}

    std::string get_artikul() const { return artikul; }
    std::string get_naimenovanie() const { return naimenovanie; }
    double get_cena() const { return cena; }
    int get_kolichestvo() const { return kolichestvo; }

    bool est_v_nalichii(int n) const { return kolichestvo >= n; }
    void zarezervirovat(int n) { kolichestvo -= n; }

    virtual std::string get_kategoriya() const = 0;

    virtual std::string get_info() const {
        std::ostringstream out;
        out << artikul << "; " << naimenovanie
            << "; цена=" << cena
            << "; на складе=" << kolichestvo;
        return out.str();
    }
};

#endif
