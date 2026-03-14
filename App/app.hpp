// Klasyczny Include Guard - zapobiega wielokrotnemu dołączaniu pliku
#ifndef APP_HPP
#define APP_HPP

/* * Sekcja dla języka C 
 * Jeśli ten nagłówek jest dołączany do pliku .c (np. main.c), 
 * wyłączamy "miksowanie" nazw (name mangling) typowe dla C++.
 */
#ifdef __cplusplus
extern "C" {
#endif

// Deklaracja głównej funkcji, którą wywołamy z main.c
void app();

#ifdef __cplusplus
}
#endif

/*
 * Sekcja czysto dla C++
 * Tutaj możesz deklarować swoje klasy, obiekty i funkcje,
 * które będą używane tylko wewnątrz plików .cpp.
 */
#ifdef __cplusplus

// np. class RobotArm { ... };

#endif // __cplusplus

#endif // APP_HPP