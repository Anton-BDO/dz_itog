#ifndef POKUPATEL_H
#define POKUPATEL_H

#include <string>

class Pokupatel {
    std::string fio;
    std::string nomer_karty;
    double schet;
public:
    Pokupatel() : fio(""), nomer_karty(""), schet(0) {}

    Pokupatel(std::string fio, std::string nomer_karty, double schet)
        : fio(fio), nomer_karty(nomer_karty), schet(schet) {}

    std::string get_fio() const { return fio; }
    std::string get_nomer_karty() const { return nomer_karty; }
    double get_schet() const { return schet; }

    bool mozhet_oplatit(double summa) const { return schet >= summa; }
    void spisat(double summa) { schet -= summa; }
};

#endif
