#ifndef KNIGA_H
#define KNIGA_H

#include "Tovar.h"

class Kniga : public Tovar {
    std::string avtor;
    std::string izdatelstvo;
public:
    // у книги порядок полей в csv: артикул, автор, название, цена, издательство, кол-во
    Kniga(std::string artikul, std::string avtor, std::string nazvanie,
          double cena, std::string izdatelstvo, int kolichestvo)
        : Tovar(artikul, nazvanie, cena, kolichestvo),
          avtor(avtor), izdatelstvo(izdatelstvo) {}

    std::string get_avtor() const { return avtor; }
    std::string get_izdatelstvo() const { return izdatelstvo; }

    std::string get_kategoriya() const override { return "Книги"; }

    std::string get_info() const override {
        std::ostringstream out;
        out << "[Книга] " << artikul << "; автор=" << avtor
            << "; название=" << naimenovanie << "; цена=" << cena
            << "; издательство=" << izdatelstvo << "; на складе=" << kolichestvo;
        return out.str();
    }
};

#endif
