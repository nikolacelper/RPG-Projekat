#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <time.h>
#include <windows.h>

/* Dimenzije mape i osnovna ogranicenja igre */
#define MAX_ROWS 10
#define MAX_COLS 100
#define MAX_INVENTAR 7
#define UKUPNO_NIVOA 5

/* Konstante koje oznacavaju rezultat borbe */
#define BORBA_POBJEDA 1
#define BORBA_BEKSTVO_IGRACA 2
#define BORBA_BEKSTVO_NEPRIJATELJA 3

/* Prototipovi funkcija koje se koriste prije njihove definicije */
void sacuvajIgru();
void pomjeriNeprijatelje();
int ucitajSacuvanuIgru();
void novaIgra(int ucitaj_save);

/* TipPredmeta oznacava sve moguce vrste predmeta u igri */
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

/* Struktura jednog predmeta: naziv, tip i bonus koji daje */
typedef struct {
    char naziv[40];
    TipPredmeta tip;
    int bonus;
} Predmet;

/* Struktura igraca: cuva HP, napad, odbranu, opremu i inventar */
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

/* Struktura neprijatelja: naziv i osnovne borbene statistike */
typedef struct {
    char naziv[40];
    int hp;
    int napad;
    int odbrana;
} Neprijatelj;

int pokreniBorbu(Neprijatelj *vanjski);

/* Mapa se cuva kao dvodimenzionalni niz karaktera: map[y][x] */
char map[MAX_ROWS][MAX_COLS];

/* rows je broj redova mape, a px/py su trenutne koordinate igraca */
int rows = 0;
int px = 0;
int py = 0;
int trenutni_nivo = 1;

/* brojac ubijenih neprijatelja tokom cijele partije */
int ukupno_ubijenih = 0;

/* Tabela koja pamti cijelog neprijatelja na mapi po poziciji [y][x].
   aktivan[y][x] == 0 znaci da na toj celiji nema neprijatelja. */
Neprijatelj neprijatelj_tabela[MAX_ROWS][MAX_COLS];
int neprijatelj_aktivan[MAX_ROWS][MAX_COLS];

/* Pozicija garantovanog kljuca na zadnjem nivou.
   -1 znaci da nema garantovanog kljuca na ovom nivou. */
int garantovani_kljuc_y = -1;
int garantovani_kljuc_x = -1;

/* Pocetne vrijednosti igraca na startu programa */
Igrac heroj = {
    100, 100,
    10, 10,
    10, 10,
    0, 0, 0,
    {{0}}, 0
};

/* Zaustavlja program dok igrac ne pritisne bilo koji taster */
void pauza() {
    printf("\nPritisni bilo koji taster...");
    _getch();
}

/* Racuna trenutni napad i odbranu na osnovu osnovnih vrijednosti i opreme */
void izracunajStatistiku() {
    heroj.napad = heroj.osnovni_napad + heroj.bonus_maca;
    heroj.odbrana = heroj.osnovna_odbrana + heroj.bonus_stita;

    if (heroj.napad < 1) {
        heroj.napad = 1;
    }

    if (heroj.odbrana < 0) {
        heroj.odbrana = 0;
    }
}

/* Crta jednostavnu HP traku od 10 karaktera */
void iscrtajHPBar(int hp, int max_hp) {
    int popunjeno = (hp * 10) / max_hp;

    if (popunjeno < 0) {
        popunjeno = 0;
    }

    if (popunjeno > 10) {
        popunjeno = 10;
    }

    printf("HP: [");

    for (int i = 0; i < 10; i++) {
        if (i < popunjeno) {
            printf("#");
        }
        else {
            printf(".");
        }
    }

    printf("] %d/%d\n", hp, max_hp);
}

/* Prikazuje inventar u jednom redu iznad mape */
void prikaziKratakInventar() {
    printf("Inventar: ");

    for (int i = 0; i < MAX_INVENTAR; i++) {
        if (i < heroj.broj_predmeta) {
            printf("[%s] ", heroj.inventar[i].naziv);
        }
        else {
            printf("[ ] ");
        }
    }

    printf("\n");
}

/* Uklanja predmet na zadatom indeksu i pomjera ostale predmete lijevo */
void ukloniPredmet(int indeks) {
    if (indeks < 0 || indeks >= heroj.broj_predmeta) {
        return;
    }

    /*
       Ako izbacujemo Vatreni mac iz inventara,
       a on je trenutno aktivan, ukidamo njegovu kaznu.
    */
    if (heroj.inventar[indeks].tip == MAC_VATRENI &&
        heroj.kazna_maca_po_potezu == 10 &&
        heroj.bonus_maca == heroj.inventar[indeks].bonus) {

        heroj.bonus_maca = 0;
        heroj.kazna_maca_po_potezu = 0;
        izracunajStatistiku();
    }

    /*
       Ako izbacujemo Vitezov mac iz inventara,
       a on je trenutno aktivan, ukidamo njegovu kaznu.
       Ovo sam dodao da i on radi ispravno, jer i on skida HP.
    */
    if (heroj.inventar[indeks].tip == MAC_VITEZOV &&
        heroj.kazna_maca_po_potezu == 5 &&
        heroj.bonus_maca == heroj.inventar[indeks].bonus) {

        heroj.bonus_maca = 0;
        heroj.kazna_maca_po_potezu = 0;
        izracunajStatistiku();
    }

    /*
       Ako izbacujemo Obican mac koji je opremljen,
       skidamo njegov bonus napada.
    */
    if (heroj.inventar[indeks].tip == MAC_OBICAN &&
        heroj.bonus_maca == heroj.inventar[indeks].bonus) {

        heroj.bonus_maca = 0;
        heroj.kazna_maca_po_potezu = 0;
        izracunajStatistiku();
    }

    for (int i = indeks; i < heroj.broj_predmeta - 1; i++) {
        heroj.inventar[i] = heroj.inventar[i + 1];
    }

    heroj.broj_predmeta--;
}
/* Vraca indeks eliksira u inventaru, ili -1 ako ga nema */
int pronadjiEliksir() {
    for (int i = 0; i < heroj.broj_predmeta; i++) {
        if (heroj.inventar[i].tip == ELIKSIR_ZIVOTA) {
            return i;
        }
    }

    return -1;
}

/* Vraca indeks kljuca u inventaru, ili -1 ako ga nema */
int imaKljuc() {
    for (int i = 0; i < heroj.broj_predmeta; i++) {
        if (heroj.inventar[i].tip == KLJUC) {
            return i;
        }
    }

    return -1;
}

/* Automatski koristi eliksir ako igrac umre, ako eliksir postoji u inventaru */
int upotrebiEliksirAkoPostoji() {
    int indeks = pronadjiEliksir();

    if (indeks == -1) {
        return 0;
    }

    printf("\nAktiviran je Eliksir zivota!");
    printf("\nVracen si u zivot sa %d HP.", heroj.max_hp / 2);

    heroj.hp = heroj.max_hp / 2;
    ukloniPredmet(indeks);

    pauza();

    return 1;
}

/* Popunjava podatke predmeta na osnovu njegovog tipa */
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

/* Nasumicno bira jedan predmet koji ce se pojaviti u kutiji */
Predmet generisiNasumicanPredmet() {
    Predmet p;
    int r = rand() % 10;

    /* r odredjuje koji tip predmeta ce biti napravljen */
    if (r == 0) {
        napraviPredmet(&p, LEK_NARODNI);
    }
    else if (r == 1) {
        napraviPredmet(&p, LEK_VILINSKI);
    }
    else if (r == 2) {
        napraviPredmet(&p, MAC_OBICAN);
    }
    else if (r == 3) {
        napraviPredmet(&p, MAC_VITEZOV);
    }
    else if (r == 4) {
        napraviPredmet(&p, MAC_VATRENI);
    }
    else if (r == 5) {
        napraviPredmet(&p, STIT_DRVENI);
    }
    else if (r == 6) {
        napraviPredmet(&p, STIT_GVOZDENI);
    }
    else if (r == 7) {
        napraviPredmet(&p, STIT_ZMAJSKI);
    }
    else if (r == 8) {
        napraviPredmet(&p, KLJUC);
    }
    else {
        napraviPredmet(&p, ELIKSIR_ZIVOTA);
    }

    return p;
}

/* Postavlja mac kao aktivnu opremu i odredjuje kaznu po potezu */
void opremiMac(Predmet p) {
    heroj.bonus_maca = p.bonus;

    if (p.tip == MAC_OBICAN) {
        heroj.kazna_maca_po_potezu = 0;
    }
    else if (p.tip == MAC_VITEZOV) {
        heroj.kazna_maca_po_potezu = 5;
    }
    else if (p.tip == MAC_VATRENI) {
        heroj.kazna_maca_po_potezu = 10;
    }

    izracunajStatistiku();
}

/* Postavlja stit kao aktivnu opremu */
void opremiStit(Predmet p) {
    heroj.bonus_stita = p.bonus;
    izracunajStatistiku();
}

/* Nakon pronalaska maca ili stita, pita igraca da li zeli odmah da ga opremi */
void pitajZaOpremanje(Predmet p) {
    if (p.tip == MAC_OBICAN || p.tip == MAC_VITEZOV || p.tip == MAC_VATRENI) {
        printf("\n\nZelis li da koristis ovaj mac? (d/n): ");
        char izbor = _getch();

        if (izbor == 'd' || izbor == 'D') {
            opremiMac(p);

            printf("\nOpremio si: %s", p.naziv);
            printf("\nNapad je sada: %d", heroj.napad);
            printf("\nGubitak HP po potezu: %d", heroj.kazna_maca_po_potezu);
        }
    }
    else if (p.tip == STIT_DRVENI || p.tip == STIT_GVOZDENI || p.tip == STIT_ZMAJSKI) {
        printf("\n\nZelis li da koristis ovaj stit? (d/n): ");
        char izbor = _getch();

        if (izbor == 'd' || izbor == 'D') {
            opremiStit(p);

            printf("\nOpremio si: %s", p.naziv);
            printf("\nOdbrana je sada: %d", heroj.odbrana);
        }
    }
}

/* Dodaje predmet u inventar; ako je inventar pun, trazi odbacivanje */
void dodajPredmet(Predmet p) {
    system("cls");

    printf("Pronadjen predmet: %s\n", p.naziv);

    if (heroj.broj_predmeta >= MAX_INVENTAR) {
        printf("\nInventar je pun. Moras odbaciti jedan predmet.\n\n");

        for (int i = 0; i < heroj.broj_predmeta; i++) {
            printf("%d. %s\n", i + 1, heroj.inventar[i].naziv);
        }

        printf("\n0 - Ne uzimaj novi predmet");
        printf("\nKoji predmet odbacujes? ");

        char izbor = _getch();

        if (izbor == '0') {
            printf("\nNisi pokupio novi predmet.");
            pauza();
            return;
        }

        int indeks = izbor - '1';

        if (indeks < 0 || indeks >= heroj.broj_predmeta) {
            printf("\nPogresan izbor. Novi predmet nije pokupljen.");
            pauza();
            return;
        }

        printf("\nOdbacio si: %s", heroj.inventar[indeks].naziv);
        ukloniPredmet(indeks);
    }

    heroj.inventar[heroj.broj_predmeta] = p;
    heroj.broj_predmeta++;

    printf("\nPokupio si: %s", p.naziv);

    pitajZaOpremanje(p);

    pauza();
}

/* Glavni ekran inventara: koriscenje, opremanje i odbacivanje predmeta */
void prikaziInventar() {
    while (1) {
        system("cls");

        printf("=== INVENTAR (%d/%d) ===\n", heroj.broj_predmeta, MAX_INVENTAR);
        iscrtajHPBar(heroj.hp, heroj.max_hp);
        printf("Napad: %d | Odbrana: %d\n", heroj.napad, heroj.odbrana);
        printf("----------------------------------\n");

        if (heroj.broj_predmeta == 0) {
            printf("Inventar je prazan.\n");
        }
        else {
            for (int i = 0; i < heroj.broj_predmeta; i++) {
                printf("%d. %s\n", i + 1, heroj.inventar[i].naziv);
            }
        }

        printf("----------------------------------\n");
        printf("Izaberi broj predmeta za koriscenje/opremanje.\n");
        printf("0 - Povratak u igru\n");
        printf("O - Odbaci predmet\n");
        printf("Izbor: ");

        char izbor = _getch();

        if (izbor == '0') {
            return;
        }

        if (izbor == 'o' || izbor == 'O') {
            if (heroj.broj_predmeta == 0) {
                printf("\nInventar je prazan, nema sta da odbacis.");
                pauza();
                continue;
            }

            printf("\nKoji predmet odbacujes? (1-%d, 0 - odustani): ", heroj.broj_predmeta);
            char izbor_odbaci = _getch();

            if (izbor_odbaci == '0') {
                continue;
            }

            int indeks_odbaci = izbor_odbaci - '1';

            if (indeks_odbaci < 0 || indeks_odbaci >= heroj.broj_predmeta) {
                printf("\nPogresan izbor.");
                pauza();
                continue;
            }

            printf("\nSigurno zelis da odbacis: %s? (d/n): ", heroj.inventar[indeks_odbaci].naziv);
            char potvrda = _getch();

            if (potvrda == 'd' || potvrda == 'D') {
                printf("\nOdbacio si: %s", heroj.inventar[indeks_odbaci].naziv);
                ukloniPredmet(indeks_odbaci);
            }
            else {
                printf("\nOdustao si od odbacivanja.");
            }

            pauza();
            continue;
        }

        int indeks = izbor - '1';

        if (indeks < 0 || indeks >= heroj.broj_predmeta) {
            printf("\nPogresan izbor.");
            pauza();
            continue;
        }

        Predmet p = heroj.inventar[indeks];

        if (p.tip == LEK_NARODNI || p.tip == LEK_VILINSKI) {
            heroj.hp += p.bonus;

            if (heroj.hp > heroj.max_hp) {
                heroj.hp = heroj.max_hp;
            }

            printf("\nIskoristio si: %s", p.naziv);
            printf("\nHP je sada: %d/%d", heroj.hp, heroj.max_hp);

            ukloniPredmet(indeks);
        }
        else if (p.tip == MAC_OBICAN || p.tip == MAC_VITEZOV || p.tip == MAC_VATRENI) {
            opremiMac(p);

            printf("\nOpremio si: %s", p.naziv);
            printf("\nNapad je sada: %d", heroj.napad);
            printf("\nGubitak HP po potezu: %d", heroj.kazna_maca_po_potezu);
        }
        else if (p.tip == STIT_DRVENI || p.tip == STIT_GVOZDENI || p.tip == STIT_ZMAJSKI) {
            opremiStit(p);

            printf("\nOpremio si: %s", p.naziv);
            printf("\nOdbrana je sada: %d", heroj.odbrana);
        }
        else if (p.tip == KLJUC) {
            printf("\nKljuc se ne koristi rucno.");
            printf("\nOn se automatski koristi kod zakljucanog izlaza.");
        }
        else if (p.tip == ELIKSIR_ZIVOTA) {
            printf("\nEliksir zivota se koristi automatski ako pogines u borbi.");
        }

        pauza();
    }
}

/* Obrada misterioznog napitka: moze biti koristan ili otrovan */
void misteriozniNapitak() {
    system("cls");

    printf("Pronasao si misteriozni napitak!");
    printf("\nNe znas da li je carobni napitak ili otrov.");
    printf("\nDa li ces ga popiti? [d/n]: ");

    char izbor = _getch();

    int caroban = rand() % 2;
    int napad_ili_odbrana = rand() % 2;
    int bonus = (rand() % 11) + 5;

    if (izbor == 'd' || izbor == 'D') {
        if (caroban) {
            if (napad_ili_odbrana == 0) {
                heroj.osnovni_napad += bonus;
                printf("\n\nCarobni napitak!");
                printf("\nNapad +%d", bonus);
            }
            else {
                heroj.osnovna_odbrana += bonus;
                printf("\n\nCarobni napitak!");
                printf("\nOdbrana +%d", bonus);
            }
        }
        else {
            if (napad_ili_odbrana == 0) {
                heroj.osnovni_napad -= bonus;

                if (heroj.osnovni_napad < 1) {
                    heroj.osnovni_napad = 1;
                }

                printf("\n\nOtrov!");
                printf("\nNapad -%d", bonus);
            }
            else {
                heroj.osnovna_odbrana -= bonus;

                if (heroj.osnovna_odbrana < 0) {
                    heroj.osnovna_odbrana = 0;
                }

                printf("\n\nOtrov!");
                printf("\nOdbrana -%d", bonus);
            }
        }
    }
    else {
        printf("\n\nProsuo si napitak.");

        if (caroban) {
            if (napad_ili_odbrana == 0) {
                printf("\nNjegovo dejstvo je bilo: Napad +%d", bonus);
            }
            else {
                printf("\nNjegovo dejstvo je bilo: Odbrana +%d", bonus);
            }
        }
        else {
            if (napad_ili_odbrana == 0) {
                printf("\nNjegovo dejstvo je bilo: Napad -%d", bonus);
            }
            else {
                printf("\nNjegovo dejstvo je bilo: Odbrana -%d", bonus);
            }
        }
    }

    izracunajStatistiku();

    printf("\n\nTrenutni napad: %d", heroj.napad);
    printf("\nTrenutna odbrana: %d", heroj.odbrana);

    pauza();
}

/* Generise neprijatelja zavisno od trenutnog nivoa */
Neprijatelj generisiNeprijatelja() {
    Neprijatelj n;

    int r = rand() % 3;

    if (trenutni_nivo == 1) {
        if (r == 0)      strcpy(n.naziv, "Goblin");
        else if (r == 1) strcpy(n.naziv, "Vuk");
        else             strcpy(n.naziv, "Lopov");
    }
    else if (trenutni_nivo == 2) {
        if (r == 0)      strcpy(n.naziv, "Ork");
        else if (r == 1) strcpy(n.naziv, "Pecinski pauk");
        else             strcpy(n.naziv, "Mracni strazar");
    }
    else if (trenutni_nivo == 3) {
        if (r == 0)      strcpy(n.naziv, "Trol");
        else if (r == 1) strcpy(n.naziv, "Crni vitez");
        else             strcpy(n.naziv, "Otrovan duh");
    }
    else if (trenutni_nivo == 4) {
        if (r == 0)      strcpy(n.naziv, "Vatreni demon");
        else if (r == 1) strcpy(n.naziv, "Kameni cuvar");
        else             strcpy(n.naziv, "Sjena");
    }
    else {
        if (r == 0)      strcpy(n.naziv, "Zmajev sluga");
        else if (r == 1) strcpy(n.naziv, "Gospodar lavirinta");
        else             strcpy(n.naziv, "Drevni demon");
    }

    n.hp      = (rand() % 50) + 1;
    n.napad   = (rand() % 12) + 1;
    n.odbrana = (rand() % 8);

    return n;
}

/* Ispisuje trenutne statistike igraca i neprijatelja u borbi */
void ispisiStanjeBorbe(Neprijatelj n) {
    printf("=== BORBA ===\n");
    printf("Neprijatelj: %s\n", n.naziv);
    printf("----------------------------------\n");

    printf("IGRAC:\n");
    printf("HP: %d/%d | Napad: %d | Odbrana: %d\n",
           heroj.hp, heroj.max_hp, heroj.napad, heroj.odbrana);

    printf("\nNEPRIJATELJ:\n");
    printf("HP: %d | Napad: %d | Odbrana: %d\n",
           n.hp, n.napad, n.odbrana);

    printf("----------------------------------\n");
}

/*  pomocna funkcija za ispis zavrsnog ekrana */
void ispisiZavrsniEkran(int pobjeda) {
    system("cls");

    if (pobjeda) {
        printf("==========================================\n");
        printf("           *** CESTITAM! ***             \n");
        printf("        Pobijedio si sve neprijatelje!   \n");
        printf("==========================================\n\n");
    }
    else {
        printf("==========================================\n");
        printf("             *** GAME OVER ***           \n");
        printf("          Izgubio si borbu...            \n");
        printf("==========================================\n\n");
    }

    printf("--- STATISTIKE PARTIJE ---\n\n");
    printf("Dostignut nivo      : %d / %d\n", trenutni_nivo, UKUPNO_NIVOA);
    printf("Ubijenih neprijatelja: %d\n", ukupno_ubijenih);  
    printf("\n--- STANJE IGRACA ---\n\n");
    printf("HP                  : %d / %d\n", heroj.hp, heroj.max_hp);
    printf("Napad               : %d\n", heroj.napad);
    printf("Odbrana             : %d\n", heroj.odbrana);
    printf("Predmeta u inventaru: %d / %d\n", heroj.broj_predmeta, MAX_INVENTAR);

    if (heroj.broj_predmeta > 0) {
        printf("\nInventar:\n");
        for (int i = 0; i < heroj.broj_predmeta; i++) {
            printf("  %d. %s\n", i + 1, heroj.inventar[i].naziv);
        }
    }

    printf("\n==========================================\n");
    pauza();
}

/* Glavna logika borbe izmedju igraca i neprijatelja */
int pokreniBorbu(Neprijatelj *vanjski) {
    Neprijatelj lokalni;
    if (vanjski == NULL) {
        lokalni = generisiNeprijatelja();
        vanjski = &lokalni;
    }

    system("cls");

    printf("!!! POCELA JE BORBA !!!\n\n");
    printf("Parametri na pocetku borbe:\n\n");

    ispisiStanjeBorbe(*vanjski);

    pauza();

    while (heroj.hp > 0 && vanjski->hp > 0) {
        system("cls");

        ispisiStanjeBorbe(*vanjski);

        printf("Izaberi akciju:\n");
        printf("N - Napad\n");
        printf("O - Odbrana\n");
        printf("B - Bekstvo\n");
        printf("Izbor: ");

        char izbor = _getch();

        /* Akcija igraca se prevodi u broj radi lakse obrade */
        int akcija_igraca = 0;

        if (izbor == 'n' || izbor == 'N') {
            akcija_igraca = 1;
        }
        else if (izbor == 'o' || izbor == 'O') {
            akcija_igraca = 2;
        }
        else if (izbor == 'b' || izbor == 'B') {
            akcija_igraca = 3;
        }
        else {
            printf("\nPogresan taster. Koristi N, O ili B.");
            pauza();
            continue;
        }

        /* Neprijatelj nasumicno bira jednu od tri akcije */
        int akcija_neprijatelja = (rand() % 3) + 1;

        int igrac_pobjegao = 0;
        int neprijatelj_pobjegao = 0;

        printf("\n\nTvoja akcija: ");

        if (akcija_igraca == 1) {
            printf("NAPAD");
        }
        else if (akcija_igraca == 2) {
            printf("ODBRANA");
        }
        else {
            printf("BEKSTVO");
        }

        printf("\nAkcija neprijatelja: ");

        if (akcija_neprijatelja == 1) {
            printf("NAPAD");
        }
        else if (akcija_neprijatelja == 2) {
            printf("ODBRANA");
        }
        else {
            printf("BEKSTVO");
        }

        if (akcija_igraca == 3) {
            if ((rand() % 100) < 50) {
                printf("\n\nUspio si da pobjegnes iz borbe!");
                igrac_pobjegao = 1;
            }
            else {
                printf("\n\nPokusao si da pobjegnes, ali nisi uspio.");
            }
        }

        if (akcija_neprijatelja == 3) {
            if ((rand() % 100) < 50) {
                printf("\nNeprijatelj je pobjegao iz borbe!");
                neprijatelj_pobjegao = 1;
            }
            else {
                printf("\nNeprijatelj je pokusao da pobjegne, ali nije uspio.");
            }
        }

        if (igrac_pobjegao) {
            pauza();
            return BORBA_BEKSTVO_IGRACA;
        }

        if (neprijatelj_pobjegao) {
            pauza();
            return BORBA_BEKSTVO_NEPRIJATELJA;
        }

        if (akcija_igraca == 1) {
            int steta = heroj.napad;

            if (akcija_neprijatelja == 2) {
                steta -= vanjski->odbrana;

                if (steta < 0) {
                    steta = 0;
                }
            }

            vanjski->hp -= steta;

            printf("\n\nNanio si neprijatelju %d stete.", steta);
        }

        if (akcija_neprijatelja == 1) {
            int steta = vanjski->napad;

            if (akcija_igraca == 2) {
                steta -= heroj.odbrana;

                if (steta < 0) {
                    steta = 0;
                }
            }

            heroj.hp -= steta;

            printf("\nNeprijatelj ti je nanio %d stete.", steta);
        }

        if (vanjski->hp <= 0) {
            /* povecaj brojac kada neprijatelj pogine */
            ukupno_ubijenih++;
            printf("\n\nPobijedio si neprijatelja!");
            pauza();
            return BORBA_POBJEDA;
        }

        if (heroj.hp <= 0) {
            if (upotrebiEliksirAkoPostoji()) {
                continue;
            }

            /* ispisi zavrsni ekran pri porazu */
            ispisiZavrsniEkran(0);
            exit(0);
        }

        printf("\n\nStanje poslije akcija:");
        printf("\nTvoj HP: %d/%d", heroj.hp, heroj.max_hp);
        printf("\nNeprijatelj HP: %d", vanjski->hp);

        pauza();
    }

    return BORBA_POBJEDA;
}

/* Ucitava mapu iz tekstualnog fajla nivoX.txt */
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

/* Mijenja boju teksta u Windows konzoli */
void postaviBoju(int boja) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), boja);
}

/* Crta jedan znak mape odgovarajucom bojom */
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

/* Iscrtava kompletan ekran igre: nivo, HP, inventar i mapu */
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

/* Nakon poteza provjerava da li opremljeni mac skida HP */
void provjeriKaznuMaca() {
    if (heroj.kazna_maca_po_potezu > 0) {
        heroj.hp -= heroj.kazna_maca_po_potezu;

        if (heroj.hp <= 0) {
            system("cls");

            printf("Izgubio si previse HP zbog teskog maca.");

            if (upotrebiEliksirAkoPostoji()) {
                return;
            }

            ispisiZavrsniEkran(0);
            exit(0);
        }
    }
}

/* Prelazak na sledeci nivo ili zavrsetak igre ako je zadnji nivo */
void predjiNaSljedeciNivo() {
    if (trenutni_nivo < UKUPNO_NIVOA) {
        int sljedeci_nivo = trenutni_nivo + 1;

        system("cls");

        printf("Pronasao si izlaz!");
        printf("\nPrelazak na nivo %d.", sljedeci_nivo);

        pauza();

        if (!ucitajNivo(sljedeci_nivo)) {
            printf("\nNe mogu da ucitam nivo %d.", sljedeci_nivo);
            pauza();
            exit(1);
        }
    }
    else {
        /* ispisi zavrsni ekran pri pobjedi */
        ispisiZavrsniEkran(1);
        exit(0);
    }
}

/* Pomjera neprijatelje na nivoima 3 i vise */
void pomjeriNeprijatelje() {
    if (trenutni_nivo < 3) {
        return;
    }

    int dx[] = {0, 0, 1, -1};
    int dy[] = {-1, 1, 0, 0};

    char stara_mapa[MAX_ROWS][MAX_COLS];
    for (int i = 0; i < rows; i++) {
        strcpy(stara_mapa[i], map[i]);
    }

    for (int y = 0; y < rows; y++) {
        for (int x = 0; stara_mapa[y][x] != '\0'; x++) {
            if (stara_mapa[y][x] != 'E') {
                continue;
            }

            int redosljed[4] = {0, 1, 2, 3};
            for (int k = 3; k > 0; k--) {
                int j = rand() % (k + 1);
                int tmp = redosljed[k];
                redosljed[k] = redosljed[j];
                redosljed[j] = tmp;
            }

            for (int k = 0; k < 4; k++) {
                int smjer = redosljed[k];
                int enx = x + dx[smjer];
                int eny = y + dy[smjer];

                if (eny < 0 || eny >= rows) continue;
                if (enx < 0 || enx >= (int)strlen(map[eny])) continue;

                char ciljno = map[eny][enx];

                if (ciljno == '@') {
                    system("cls");
                    printf("Neprijatelj te je napao!\n");
                    pauza();

                    if (!neprijatelj_aktivan[y][x]) {
                        neprijatelj_tabela[y][x] = generisiNeprijatelja();
                        neprijatelj_aktivan[y][x] = 1;
                    }

                    int rezultat = pokreniBorbu(&neprijatelj_tabela[y][x]);

                    if (rezultat == BORBA_POBJEDA) {
                        neprijatelj_aktivan[y][x] = 0;
                        map[y][x] = '.';
                    }
                    else if (rezultat == BORBA_BEKSTVO_NEPRIJATELJA) {
                        int premjesten = 0;
                        for (int m = 0; m < 4; m++) {
                            int bnx = x + dx[m];
                            int bny = y + dy[m];
                            if (bny < 0 || bny >= rows) continue;
                            if (bnx < 0 || bnx >= (int)strlen(map[bny])) continue;
                            if (map[bny][bnx] != '.') continue;
                            neprijatelj_tabela[bny][bnx] = neprijatelj_tabela[y][x];
                            neprijatelj_aktivan[bny][bnx] = 1;
                            neprijatelj_aktivan[y][x] = 0;
                            map[y][x] = '.';
                            map[bny][bnx] = 'E';
                            premjesten = 1;
                            break;
                        }
                        if (!premjesten) { }
                    }
                    else { }
                    break;
                }

                if (ciljno == '.') {
                    neprijatelj_tabela[eny][enx] = neprijatelj_tabela[y][x];
                    neprijatelj_aktivan[eny][enx] = neprijatelj_aktivan[y][x];
                    neprijatelj_aktivan[y][x] = 0;
                    map[y][x] = '.';
                    map[eny][enx] = 'E';
                    break;
                }
            }
        }
    }
}

/* Pokrece novu igru ili nastavlja sacuvanu, zatim ulazi u glavnu petlju */
void novaIgra(int ucitaj_save) {
    /* Ako je ucitaj_save == 1, igrac zeli nastaviti sacuvanu igru */
    if (ucitaj_save) {

        /* Pokusava se ucitati save.dat */
        if (!ucitajSacuvanuIgru()) {
            printf("\nNije moguce ucitati sacuvanu igru.");
            pauza();
            return;   /* Ako save ne moze da se ucita, izlazi se iz funkcije */
        }
    }
    else {
        /* Ako nije save, pocinje nova igra i resetuju se osnovne vrijednosti */

        heroj.hp = 100;          /* Trenutni HP igraca */
        heroj.max_hp = 100;      /* Maksimalni HP igraca */

        heroj.osnovni_napad = 10;    /* Napad bez maca */
        heroj.osnovna_odbrana = 10;  /* Odbrana bez stita */

        heroj.napad = 10;       /* Ukupan napad koji se koristi u borbi */
        heroj.odbrana = 10;     /* Ukupna odbrana koja se koristi u borbi */

        heroj.bonus_maca = 0;              /* Na pocetku nema bonus od maca */
        heroj.kazna_maca_po_potezu = 0;    /* Na pocetku mac ne skida HP */
        heroj.bonus_stita = 0;             /* Na pocetku nema bonus od stita */

        heroj.broj_predmeta = 0;           /* Inventar je prazan */

        ukupno_ubijenih = 0;               /* Reset broja ubijenih neprijatelja */

        izracunajStatistiku();             /* Racuna napad i odbranu nakon reseta */

        /* Ucitava prvi nivo iz fajla nivo1.txt */
        if (!ucitajNivo(1)) {
            pauza();
            return;
        }
    }

    /* Glavna petlja igre - vrti se dok igrac ne izadje ili ne zavrsi igru */
    while (1) {
        crtaj();               /* Iscrtava mapu, HP, inventar i podatke */

        char c = _getch();     /* Cita pritisnut taster bez Enter-a */

        /* Ako igrac pritisne Q, pita ga da li stvarno zeli izaci */
        if (c == 'q' || c == 'Q') {
            printf("\nStvarno zelis da izadjes iz igre? (d/n): ");

            char provera = _getch();

            /* Ako potvrdi izlaz, igra se sacuva */
            if (provera == 'd' || provera == 'D') {
                sacuvajIgru();
                pauza();
                break;         /* Izlazi iz while petlje */
            }
            else {
                continue;      /* Ako ne potvrdi, nastavlja igru */
            }
        }

        /* Ako igrac pritisne I, otvara se inventar */
        if (c == 'i' || c == 'I') {
            prikaziInventar();
            continue;          /* Nakon inventara vraca se na pocetak petlje */
        }

        /* nx i ny su nova pozicija na koju igrac pokusava da se pomjeri */
        int nx = px;
        int ny = py;

        /* Obrada kretanja */
        if (c == 'w' || c == 'W') {
            ny--;              /* Gore: y se smanjuje */
        }
        else if (c == 's' || c == 'S') {
            ny++;              /* Dole: y se povecava */
        }
        else if (c == 'a' || c == 'A') {
            nx--;              /* Lijevo: x se smanjuje */
        }
        else if (c == 'd' || c == 'D') {
            nx++;              /* Desno: x se povecava */
        }
        else {
            continue;          /* Ako nije WASD/I/Q, ignorise se */
        }

        /* Provjera da igrac ne izadje van granica mape */
        if (ny < 0 || ny >= rows || nx < 0 || nx >= strlen(map[ny])) {
            continue;
        }

        /* Uzimamo znak sa polja na koje igrac pokusava da stane */
        char polje = map[ny][nx];

        /* Zid - igrac ne moze proci */
        if (polje == '#') {
            continue;
        }

        /* E znaci neprijatelj */
        if (polje == 'E') {

            /* Ako neprijatelj na toj poziciji jos nije napravljen, napravi ga */
            if (!neprijatelj_aktivan[ny][nx]) {
                neprijatelj_tabela[ny][nx] = generisiNeprijatelja();
                neprijatelj_aktivan[ny][nx] = 1;
            }

            /* Pokrece borbu sa neprijateljem sa te pozicije */
            int rezultat = pokreniBorbu(&neprijatelj_tabela[ny][nx]);

            /* Ako je igrac pobijedio */
            if (rezultat == BORBA_POBJEDA) {
                neprijatelj_aktivan[ny][nx] = 0;  /* Neprijatelj vise nije aktivan */
                map[ny][nx] = '.';                /* Brise se sa mape */

                map[py][px] = '.';                /* Stara pozicija igraca postaje prazna */
                px = nx;                          /* Azurira se x pozicija igraca */
                py = ny;                          /* Azurira se y pozicija igraca */
                map[py][px] = '@';                /* Nova pozicija dobija znak igraca */

                provjeriKaznuMaca();              /* Ako ima tezak mac, skida HP */
                pomjeriNeprijatelje();            /* Na nivoima 3+ pomjera neprijatelje */
            }

            /* Ako je neprijatelj pobjegao */
            else if (rezultat == BORBA_BEKSTVO_NEPRIJATELJA) {
                int dx[] = {0, 0, 1, -1};          /* Pomjeranje po x osi */
                int dy[] = {-1, 1, 0, 0};          /* Pomjeranje po y osi */

                /* Mijesa smjerove da bjekstvo bude nasumicno */
                for (int k = 3; k > 0; k--) {
                    int j = rand() % (k + 1);
                    int tmp;

                    tmp = dx[k]; 
                    dx[k] = dx[j]; 
                    dx[j] = tmp;

                    tmp = dy[k]; 
                    dy[k] = dy[j]; 
                    dy[j] = tmp;
                }

                int premjesten = 0;                /* Da znamo da li je neprijatelj uspio pobjeci */

                /* Pokusava naci prazno susjedno polje za neprijatelja */
                for (int k = 0; k < 4; k++) {
                    int enx = nx + dx[k];
                    int eny = ny + dy[k];

                    if (eny < 0 || eny >= rows) continue;
                    if (enx < 0 || enx >= (int)strlen(map[eny])) continue;
                    if (map[eny][enx] != '.') continue;

                    /* Prenosimo neprijatelja na novu poziciju */
                    neprijatelj_tabela[eny][enx] = neprijatelj_tabela[ny][nx];
                    neprijatelj_aktivan[eny][enx] = 1;
                    neprijatelj_aktivan[ny][nx] = 0;

                    map[ny][nx] = '.';             /* Staro polje neprijatelja se prazni */
                    map[eny][enx] = 'E';           /* Novo polje dobija neprijatelja */

                    premjesten = 1;
                    break;
                }

                /* Ako nije premjesten, ostaje gdje jeste */
                if (!premjesten) { }

                /* Igrac ulazi na polje gdje je neprijatelj bio */
                map[py][px] = '.';
                px = nx;
                py = ny;
                map[py][px] = '@';

                provjeriKaznuMaca();
                pomjeriNeprijatelje();
            }

            /* Ako je igrac pobjegao, ostaje na staroj poziciji */
            else if (rezultat == BORBA_BEKSTVO_IGRACA) {
                continue;
            }
        }

        /* $ znaci kutija/predmet */
        else if (polje == '$') {
            Predmet novi_predmet;

            /* Ako je ova kutija garantovani kljuc, pravi se kljuc */
            if (ny == garantovani_kljuc_y && nx == garantovani_kljuc_x) {
                napraviPredmet(&novi_predmet, KLJUC);
                garantovani_kljuc_y = -1;
                garantovani_kljuc_x = -1;
            }
            else {
                /* Ako nije garantovani kljuc, pravi se nasumican predmet */
                novi_predmet = generisiNasumicanPredmet();
            }

            dodajPredmet(novi_predmet);            /* Dodaje predmet u inventar */

            map[ny][nx] = '.';                     /* Kutija nestaje */

            map[py][px] = '.';                     /* Stara pozicija igraca */
            px = nx;
            py = ny;
            map[py][px] = '@';                     /* Nova pozicija igraca */

            provjeriKaznuMaca();
            pomjeriNeprijatelje();
        }

        /* ? znaci misteriozni napitak */
        else if (polje == '?') {
            misteriozniNapitak();                  /* Aktivira logiku napitka */

            map[ny][nx] = '.';                     /* Napitak nestaje sa mape */

            map[py][px] = '.';
            px = nx;
            py = ny;
            map[py][px] = '@';

            provjeriKaznuMaca();
            pomjeriNeprijatelje();
        }

        /* L znaci zakljucan izlaz */
        else if (polje == 'L') {
            int indeks_kljuca = imaKljuc();         /* Trazimo kljuc u inventaru */

            if (indeks_kljuca == -1) {
                printf("\nOvaj izlaz je zakljucan. Potreban ti je kljuc.");
                pauza();
                continue;
            }

            /* Ako ima kljuc, trosi se i prelazi se nivo */
            printf("\nOtkljucao si izlaz pomocu kljuca!");
            ukloniPredmet(indeks_kljuca);
            pauza();

            predjiNaSljedeciNivo();
            continue;
        }

        /* > znaci obican izlaz */
        else if (polje == '>') {

            /* Na zadnjem nivou i obican izlaz trazi kljuc */
            if (trenutni_nivo == UKUPNO_NIVOA && imaKljuc() == -1) {
                printf("\nOva vrata su zakljucana. Potreban ti je kljuc da izadjes!");
                pauza();
                continue;
            }

            /* Ako je zadnji nivo i ima kljuc, kljuc se uklanja */
            if (trenutni_nivo == UKUPNO_NIVOA) {
                ukloniPredmet(imaKljuc());
            }

            predjiNaSljedeciNivo();
            continue;
        }

        /* Ako je obicno prazno polje, igrac se samo pomjera */
        else {
            map[py][px] = '.';
            px = nx;
            py = ny;
            map[py][px] = '@';

            provjeriKaznuMaca();
            pomjeriNeprijatelje();
        }
    }
}

/* Prikazuje osnovna pravila i kontrole igre */
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

/* Cuvanje trenutnog stanja igre u save.dat */
void sacuvajIgru() {
    FILE *f = fopen("save.dat", "w");

    if (f == NULL) {
        printf("\nGreska: nije moguce sacuvati igru.");
        return;
    }

    fprintf(f, "%d %d\n", heroj.hp, heroj.max_hp);
    fprintf(f, "%d %d\n", heroj.osnovni_napad, heroj.osnovna_odbrana);
    fprintf(f, "%d %d\n", heroj.napad, heroj.odbrana);
    fprintf(f, "%d %d %d\n", heroj.bonus_maca, heroj.kazna_maca_po_potezu, heroj.bonus_stita);
    fprintf(f, "%d\n", heroj.broj_predmeta);

    for (int i = 0; i < heroj.broj_predmeta; i++) {
        fprintf(f, "%d %d %s\n", heroj.inventar[i].tip, heroj.inventar[i].bonus, heroj.inventar[i].naziv);
    }

    fprintf(f, "%d %d %d\n", px, py, trenutni_nivo);

    fprintf(f, "%d\n", rows);
    for (int i = 0; i < rows; i++) {
        fprintf(f, "%s\n", map[i]);
    }

    /* sacuvaj i broj ubijenih da se ne izgubi pri nastavku */
    fprintf(f, "%d\n", ukupno_ubijenih);

    fclose(f);

    printf("\nIgra je sacuvana.");
}

/* Ucitavanje sacuvanog stanja iz save.dat */
int ucitajSacuvanuIgru() {
    FILE *f = fopen("save.dat", "r");

    if (f == NULL) {
        return 0;
    }

    fscanf(f, "%d %d\n", &heroj.hp, &heroj.max_hp);
    fscanf(f, "%d %d\n", &heroj.osnovni_napad, &heroj.osnovna_odbrana);
    fscanf(f, "%d %d\n", &heroj.napad, &heroj.odbrana);
    fscanf(f, "%d %d %d\n", &heroj.bonus_maca, &heroj.kazna_maca_po_potezu, &heroj.bonus_stita);
    fscanf(f, "%d\n", &heroj.broj_predmeta);

    for (int i = 0; i < heroj.broj_predmeta; i++) {
        fscanf(f, "%d %d %[^\n]\n", (int*)&heroj.inventar[i].tip, &heroj.inventar[i].bonus, heroj.inventar[i].naziv);
    }

    fscanf(f, "%d %d %d\n", &px, &py, &trenutni_nivo);

    fscanf(f, "%d\n", &rows);
    for (int i = 0; i < rows; i++) {
        fgets(map[i], MAX_COLS, f);
        for (int j = 0; j < MAX_COLS; j++) {
            if (map[i][j] == '\n' || map[i][j] == '\r') {
                map[i][j] = '\0';
            }
        }
    }

    /* ucitaj broj ubijenih iz save fajla */
    fscanf(f, "%d\n", &ukupno_ubijenih);

    fclose(f);

    return 1;
}

/* Glavni meni igre */
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

/* Ulazna tacka programa */
int main() {
    srand(time(NULL));

    for (int y = 0; y < MAX_ROWS; y++)
        for (int x = 0; x < MAX_COLS; x++)
            neprijatelj_aktivan[y][x] = 0;

    meni();

    return 0;
}
