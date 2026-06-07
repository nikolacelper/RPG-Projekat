#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#define MAX_ROWS 10
#define MAX_COLS 100
#define MAX_INVENTAR 7
#define UKUPNO_NIVOA 5

#define BORBA_POBJEDA 1
#define BORBA_BEKSTVO_IGRACA 2
#define BORBA_BEKSTVO_NEPRIJATELJA 3

void sacuvajIgru();
void pomjeriNeprijatelje();
int ucitajSacuvanuIgru();
void novaIgra(int ucitaj_save);

typedef enum {
    LEK_NARODNI,
    LEK_VILINSKI,
    MAC_OBICAN,
    MAC_VITEZOV,
    MAC_VATRENI,
    STIT_DRVENI,
    STIT_GVOZDENI,
    STIT_ZMAJSKI,
    KLJUC,
    ELIKSIR_ZIVOTA
} TipPredmeta;

typedef struct {
    char naziv[40];
    TipPredmeta tip;
    int bonus;
} Predmet;

typedef struct {
    int hp;
    int max_hp;

    int osnovni_napad;
    int osnovna_odbrana;

    int napad;
    int odbrana;

    int bonus_maca;
    int kazna_maca_po_potezu;
    int bonus_stita;

    Predmet inventar[MAX_INVENTAR];
    int broj_predmeta;
} Igrac;

typedef struct {
    char naziv[40];
    int hp;
    int napad;
    int odbrana;
} Neprijatelj;

int pokreniBorbu(Neprijatelj *vanjski);

char map[MAX_ROWS][MAX_COLS];

int rows = 0;
int px = 0;
int py = 0;
int trenutni_nivo = 1;

/* NOVO - linija 70: brojac ubijenih neprijatelja tokom cijele partije */
int ukupno_ubijenih = 0;

/* Tabela koja pamti cijelog neprijatelja na mapi po poziciji [y][x].
   aktivan[y][x] == 0 znaci da na toj celiji nema neprijatelja. */
Neprijatelj neprijatelj_tabela[MAX_ROWS][MAX_COLS];
int neprijatelj_aktivan[MAX_ROWS][MAX_COLS];

/* Pozicija garantovanog kljuca na zadnjem nivou.
   -1 znaci da nema garantovanog kljuca na ovom nivou. */
int garantovani_kljuc_y = -1;
int garantovani_kljuc_x = -1;

Igrac heroj = {

};

void pauza() {

}

void izracunajStatistiku() {

}

void iscrtajHPBar(int hp, int max_hp) {

}

void prikaziKratakInventar() {

}

void ukloniPredmet(int indeks) {

}

int pronadjiEliksir() {

}

int imaKljuc() {

}

int upotrebiEliksirAkoPostoji() {

}

void napraviPredmet(Predmet *p, TipPredmeta tip) {

}

Predmet generisiNasumicanPredmet() {

}

void opremiMac(Predmet p) {

}

void opremiStit(Predmet p) {

}

void pitajZaOpremanje(Predmet p) {

}

void dodajPredmet(Predmet p) {

}

void prikaziInventar() {

}

void misteriozniNapitak() {

}

Neprijatelj generisiNeprijatelja() {

}

/* NOVO - linija 546: pomocna funkcija za ispis zavrsnog ekrana */
void ispisiZavrsniEkran(int pobjeda) {

}

int pokreniBorbu(Neprijatelj *vanjski) {

}

int ucitajNivo(int broj_nivoa) {

}

void postaviBoju(int boja) {

}

void crtajPolje(char polje) {

}

void crtaj() {

}

void provjeriKaznuMaca() {

}

void predjiNaSljedeciNivo() {

}

void pomjeriNeprijatelje() {

}

void novaIgra(int ucitaj_save) {

}

void uputstvo() {
void uputstvo() {
    system("cls");

    printf("=== UPUTSTVO ===\n\n");

    printf("Cilj igre je da prodjes kroz 5 nivoa lavirinta.\n\n");

    printf("Kontrole:\n");
    printf("W - gore\n");
    printf("A - lijevo\n");
    printf("S - dole\n");
    printf("D - desno\n");
    printf("I - inventar\n");
    printf("Q - izlaz iz igre\n\n");

    printf("Simboli:\n");
    printf("@ - igrac\n");
    printf("# - zid\n");
    printf(". - prazan prostor\n");
    printf("E - neprijatelj\n");
    printf("$ - predmet\n");
    printf("? - misteriozni napitak\n");
    printf("> - izlaz\n");
    printf("L - zakljucan izlaz, potreban je kljuc\n\n");

    printf("Borba:\n");
    printf("N - napad\n");
    printf("O - odbrana\n");
    printf("B - bekstvo, ima 50%% sanse za uspeh\n\n");

    pauza();
}
}

void sacuvajIgru() {

}

int ucitajSacuvanuIgru() {

}

void meni() {

}

int main() {

}
