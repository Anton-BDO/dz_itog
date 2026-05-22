#include "Ploshchadka.h"

#include <iostream>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

int main() {
#ifdef _WIN32
    // переключаем страницу консоли на utf-8 для корректного вывода кириллицы
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    cout << "___ Торговая площадка ___\n";

    double S, p;
    cout << "Введите пороговую сумму заказа S (для предоставления скидки): ";
    cin >> S;
    cout << "Введите размер скидки p (в процентах): ";
    cin >> p;

    Ploshchadka ploshchadka(S, p);

    cout << fixed << setprecision(2);

    try {
        ploshchadka.zagruzit_tovary("tovary.csv");
        ploshchadka.zagruzit_pokupatelej("pokupateli.csv");
        ploshchadka.zagruzit_pokupki("pokupki.csv");
    }
    catch (const exception& e) {
        cerr << "Ошибка загрузки данных: " << e.what() << endl;
        return 1;
    }

    cout << "\nДанные загружены. Начинаем обработку заказов...\n";

    ploshchadka.obrabotat_pokupki();

    ploshchadka.vyvesti_vyruchku();
    ploshchadka.vyvesti_ochered_pokupatelej();
    ploshchadka.vyvesti_rezerv();
    ploshchadka.otpravit_povtornye_napominaniya();

    cout << "\nРабота завершена.\n";
    return 0;
}
