#include "Bitboard.h"

int main() {
    // Creazione di due oggetti Bitboard
    Bitboard b1, b2;

    // Impostiamo alcuni bit nelle bitboard
    b1.setBit(5).setBit(10).setBit(20);
    b2.setBit(10).setBit(20).setBit(30);

    // Stampa iniziale delle bitboard
    std::cout << "Bitboard b1 (prima delle operazioni):" << std::endl;
    b1.print();
    std::cout << "Bitboard b2 (prima delle operazioni):" << std::endl;
    b2.print();

    // Eseguiamo una serie di operazioni bit a bit
    std::cout << "\nEseguiamo NOT su b1:" << std::endl;
    b1.notOp().print();

    std::cout << "\nEseguiamo AND tra b1 e b2:" << std::endl;
    b1.andOp(b2).print();

    std::cout << "\nEseguiamo OR tra b1 e b2:" << std::endl;
    b1.orOp(b2).print();

    std::cout << "\nEseguiamo XOR tra b1 e b2:" << std::endl;
    b1.xorOp(b2).print();

    std::cout << "\nEseguiamo shiftLeft su b1 di 3 posizioni:" << std::endl;
    b1.shiftLeft(3).print();

    std::cout << "\nEseguiamo shiftRight su b1 di 2 posizioni:" << std::endl;
    b1.shiftRight(2).print();

    return 0;
}
