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
  p->tip = tip;

    if (tip == LEK_NARODNI) {
        strcpy(p->naziv, "Narodni lek");
        p->bonus = 10;
    }
    else if (tip == LEK_VILINSKI) {
        strcpy(p->naziv, "Vilinski lek");
        p->bonus = 20;
    }
    else if (tip == MAC_OBICAN) {
        strcpy(p->naziv, "Obican mac");
        p->bonus = 5;
    }
    else if (tip == MAC_VITEZOV) {
        strcpy(p->naziv, "Vitezov mac");
        p->bonus = 15;
    }
    else if (tip == MAC_VATRENI) {
        strcpy(p->naziv, "Vatreni mac");
        p->bonus = 25;
    }
    else if (tip == STIT_DRVENI) {
        strcpy(p->naziv, "Drveni stit");
        p->bonus = 5;
    }
    else if (tip == STIT_GVOZDENI) {
        strcpy(p->naziv, "Gvozdeni stit");
        p->bonus = 10;
    }
    else if (tip == STIT_ZMAJSKI) {
        strcpy(p->naziv, "Zmajski stit");
        p->bonus = 15;
    }
    else if (tip == KLJUC) {
        strcpy(p->naziv, "Kljuc");
        p->bonus = 0;
    }
    else if (tip == ELIKSIR_ZIVOTA) {
        strcpy(p->naziv, "Eliksir zivota");
        p->bonus = 0;
    }
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
 char ime_fajla[30];

    sprintf(ime_fajla, "nivo%d.txt", broj_nivoa);

    FILE *f = fopen(ime_fajla, "r");

    if (f == NULL) {
        printf("Greska pri otvaranju fajla: %s\n", ime_fajla);
        printf("Fajl mora biti u istom folderu gdje se pokrece .exe.\n");
        return 0;
    }

    rows = 0;

    while (rows < MAX_ROWS && fgets(map[rows], MAX_COLS, f)) {
        for (int i = 0; i < MAX_COLS; i++) {
            if (map[rows][i] == '\n' || map[rows][i] == '\r') {
                map[rows][i] = '\0';
            }
        }

        rows++;
    }

    fclose(f);

    int pronadjen_igrac = 0;

    for (int y = 0; y < rows; y++) {
        for (int x = 0; map[y][x] != '\0'; x++) {
            if (map[y][x] == '@') {
                px = x;
                py = y;
                pronadjen_igrac = 1;
            }
        }
    }

    if (!pronadjen_igrac) {
        printf("Greska: u fajlu %s ne postoji znak @ za igraca.\n", ime_fajla);
        return 0;
    }

    for (int y = 0; y < MAX_ROWS; y++)
        for (int x = 0; x < MAX_COLS; x++)
            neprijatelj_aktivan[y][x] = 0;

    garantovani_kljuc_y = -1;
    garantovani_kljuc_x = -1;

    int ima_zakljucana_vrata = 0;
    for (int y = 0; y < rows; y++) {
        for (int x = 0; map[y][x] != '\0'; x++) {
            if (map[y][x] == 'L') {
                ima_zakljucana_vrata = 1;
            }
        }
    }

    if (ima_zakljucana_vrata) {
        int kutija_y[MAX_ROWS * MAX_COLS];
        int kutija_x[MAX_ROWS * MAX_COLS];
        int broj_kutija = 0;

        for (int y = 0; y < rows; y++) {
            for (int x = 0; map[y][x] != '\0'; x++) {
                if (map[y][x] == '$') {
                    kutija_y[broj_kutija] = y;
                    kutija_x[broj_kutija] = x;
                    broj_kutija++;
                }
            }
        }

        if (broj_kutija > 0) {
            int izbor = rand() % broj_kutija;
        
            garantovani_kljuc_y = kutija_y[izbor];
            garantovani_kljuc_x = kutija_x[izbor];
        }
    }

    trenutni_nivo = broj_nivoa;

    return 1;

    
}

void postaviBoju(int boja) {

     SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), boja);

}

void crtajPolje(char polje) {
    if (polje == '#') {
        postaviBoju(8);
        printf("%c", 219);
    }
    else if (polje == '@') {
        postaviBoju(10);
        printf("@");
    }
    else if (polje == 'E') {
        postaviBoju(12);
        printf("E");
    }
    else if (polje == '$') {
        postaviBoju(14);
        printf("$");
    }
    else if (polje == '?') {
        postaviBoju(13);
        printf("?");
    }
    else if (polje == '>') {
        postaviBoju(11);
        printf(">");
    }
    else if (polje == 'L') {
        postaviBoju(9);
        printf("L");
    }
    else {
        postaviBoju(7);
        printf(" ");
    }

    postaviBoju(7);

}

void crtaj() {
system("cls");

    printf("NIVO %d/%d\n", trenutni_nivo, UKUPNO_NIVOA);

    iscrtajHPBar(heroj.hp, heroj.max_hp);

    printf("Napad: %d  Odbrana: %d\n", heroj.napad, heroj.odbrana);
    printf("Kazna maca po potezu: %d HP\n", heroj.kazna_maca_po_potezu);

    prikaziKratakInventar();

    printf("----------------------------------\n");

    for (int i = 0; i < rows; i++) {
    for (int j = 0; map[i][j] != '\0'; j++) {
        crtajPolje(map[i][j]);
    }
    printf("\n");
}

    printf("----------------------------------\n");
    printf("WASD - kretanje | I - inventar | Q - izlaz\n");
    printf("# zid | E neprijatelj | $ predmet | ? napitak | > izlaz | L zakljucan izlaz\n");
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


void sacuvajIgru() {

}

int ucitajSacuvanuIgru() {

}

void meni() {
    while (1) {
        system("cls");

        printf("=== RPG LAVIRINT ===\n\n");

        // Provera da li postoji save fajl i pravilno zatvaranje fajla
        FILE *f = fopen("save.dat", "r");
        int ima_save = (f != NULL);
        if (ima_save) {
            fclose(f); 
        }

        // Prikaz menija
        if (ima_save) {
            printf("1 - Nova igra\n");
            printf("2 - Nastavi sacuvanu igru\n");
            printf("3 - Uputstvo\n");
            printf("0 - Izlaz\n\n");
        } else {
            printf("1 - Nova igra\n");
            printf("2 - Uputstvo\n");
            printf("0 - Izlaz\n\n");
        }

        printf("Izbor: ");
        char izbor = _getch();

        // Glavna logika menija preko switch-a
        switch (izbor) {
            case '1':
                if (ima_save) {
                    remove("save.dat");
                }
                novaIgra(0);
                break;

            case '2':
                if (ima_save) {
                    novaIgra(1); // Ako ima save, 2 je "Nastavi"
                } else {
                    uputstvo();  // Ako nema save, 2 je "Uputstvo"
                }
                break;

            case '3':
                if (ima_save) {
                    uputstvo(); // Opcija 3 postoji samo ako ima save fajla
                } else {
                    printf("\nPogresan izbor.");
                    pauza();
                }
                break;

            case '0':
                return; // Izlaz iz funkcije i prekid while petlje

            default:
                printf("\nPogresan izbor.");
                pauza();
                break;
        }
    }
}

int main() {

}
