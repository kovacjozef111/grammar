/*-----------------2.12.2004 6:55-------------------
 *  Program pro vyhodnocování aritmetických výrazů s využitím gramatiky z přednášek.
 *  Autor: Tomáš Dulík, 2004
 *  Popis: Metodu převodu gramatiky na kód programovacího jazyka máte uvedenu
 *          v přednáškách. Následující kód je hustě komentován, snad bude pochopitelný.
 * --------------------------------------------------*/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef enum {start=1, isNum=2, num=4, plus=8, minus=16,  /* definice stavů kon. automatu*/
    krat=32, deleno=64, lzavorka=128, pzavorka=256,
    strednik=512, ef=1024, error=2048,
    sinus = 4096, cosinus = 8192
} tStav;
/*-----------------2.12.2004 6:41-------------------
 * Pozn: Jednotlivé stavy mají přiřazené ordinální hodnoty 2^N, aby bylo
 *       v programu možno jednoduše testovat, zda automat skončil v
 *       nějaké množině stavů - k takovému testu pak můžeme s výhodou využít
 *       bitové operace &,|,^. Např. pokud chceme otestovat, zda výsledný
 *       stav je některý ze stavům num, plus, minus, krat, deleno, napíšeme
 *       podmínku "if (stav&(num|plus|minus|krat|deleno))..."
 *       Je to stejný princip, na kterém funguje vaše "Množina s libovolnou mocností".
 *
 *       V podstatě byste mohli stejně dobře stavy definovat obyčejným výčtem, a k testům,
 *       zda je výsledný stav v nějaké množině stavů, použít tento váš ADT "Množina s libovolnou mocností".
 *       Nechtěl jsem ale tento program prodlužovat zahrnutím dalších procedur, doufám,
 *       že i takto bude kód dostatečně čitelný. Každopádně je to takto efektivnější,
 *       protože výše uvedená podmínka se přeloží do 2 instrukcí ASM, kdežto operace
 *       s ADT množina by vyžadovali mnohem větší úsilí procesoru (zkuste to promyslet)
 * --------------------------------------------------*/

/*-----------------2.12.2004 6:58-------------------
 * Funkce "index" prepocte hodnotu symbolu (=výsledného stavu konečného automatu)
 * na index do pole "symbols", obsahujícího řetězec s popisem stavu.
 * V tomto programu není funkce index() využita, ale můžete ji využít při ladění pro výpisy
 * stavů tak, aby byly pro člověka čitelné.
 * Pozn.: Pokud byste implementovali stavy obyčejným výčtem a k testování, zde stav
 *        patří do množiny stavů, využívali
 * --------------------------------------------------*/
char* symbols[]={
    "start","isNum","num","plus","minus","krat","deleno",
        "lzavorka","pzavorka","eof","error"
};
int index(long sym) {
    int ind=0;
    long mask=1;
    while (!(mask&sym)) { mask<<=1; ind++; }
    return ind;
}


char numbuf[50];                 /* numbuf slouží pro ukládání číslic čísel int */
char linebuf[200];               /* v linebuf se postupně ukládá celý řádek ze vstupu. Použití=výpis chyb*/
int numval, linenr, column;      /* numval=číselná hodnota čísla int na vstupu. Linenr=aktuální č. řádku, column=akt.č. sloupce */
FILE * soubor;                   /* vstupní soubor */

int lexAnalyzer() {              /* Implementace konečného automatu */
    tStav stav=start;            /* na začátku nastavíme stav na start */
    char * ptrnum=numbuf;        /* ukazatel do bufferu pro ukládání řetězce s číslem */
    int c;
    while (isspace(c=fgetc(soubor)) && c!=EOF)	/* vypust mezery ze vstupu */
        if (c=='\n') { linenr++; column=0; }	/* pocitej cisla radku a sloupcu */
        else linebuf[column++]=c;				/* a uschovej soucasne pisemnko */

    if (c!=EOF) {                       /* pokud ještě není konec souboru */
        ungetc(c, soubor);              /* vrať poslední přečtený znak zpět na vstup */
        while ((c=linebuf[column++]=fgetc(soubor))!=EOF)  /* a spusť konečný automat */
            switch (stav)  {
            case start:
                if (isdigit(c)) {       /* je na vstupu číslice? */
                    stav=isNum;
                    *ptrnum++=c;
                } else {
                    switch (c) {        /* je na vstupu jeden z uvedených znaků? */
                        case '*': return krat; break;
                        case '/': return deleno; break;
                        case '+': return plus; break;
                        case '-': return minus; break;
                        case '(': return lzavorka; break;
                        case ')': return pzavorka; break;
                        case ';': return strednik; break;

                        case 's':
                        if(fgetc(soubor) == 'i'){
                            if(fgetc(soubor) == 'n'){
                                return sinus;
                            }
                            else{
                                return error;
                            }
                        }
                        else{
                            return error;
                        }
                        break;

                        default: return error; break;
                    }
                }
                break;

            case isNum:                       /* jsme ve stavu hledání a ukládání dalších číslic čísla int */
                if (isdigit(c)) *ptrnum++=c;  /* takže ukládáme číslice do bufferu */
                else {                        /* a jakmile najdeme první nečíslici */
                    *ptrnum='\0';             /* uložíme na konec řetězce znak ukončující znak '\0' (řetězce v C...)*/
                    numval=strtol(numbuf, &ptrnum, 10);  /* a převedeme řetězec na číslo */
                    if ((unsigned int)(ptrnum-numbuf)<strlen(numbuf)) printf("Error in integer constant.");
                    ungetc(c, soubor);        /* poslední přečtený znak vrátíme na vstup, může to být totiž např. znaménko které nás bude zajímat v příštím průchodu */
                    return num;
                }
            }
    }
    return ef;                          /* c==EOF = došli jsme na konec souboru */
}
/*-----------------2.12.2004 7:17-------------------
 * nyní si nadeklarujeme všechny funkce, které budou implementovat analýzu nonterminálů.
 * Protože funkce s vzájemně rekurzivně provolávají, musíme si hned na začátku nadeklarovat
 * jejich hlavičky, aby byly v době překladu těl všech funkcí již známé: */
float ex();
float m();
float m2(float v);
float ex2(float v);
float t();

int symbol;
/*-----------------2.12.2004 7:19-------------------
 * Funkce expect(long s) očekává na vstupu symbol (=výsledný stav konečného automatu),
 * daný v parametru s. Pokud tento symbol odpovídá, zavolá funkci lexAnalyzer, v níž
 * konečný automat najde další symbol na vstupu.
 * Pozn.: Expect znamená anglicky "očekávat"
 * --------------------------------------------------*/
void expect(long s) {
 if (symbol==s) symbol=lexAnalyzer();
 else {
   printf("symbol nr. %d expected !", symbol);
   exit(-1);
    }
}
/*-----------------2.12.2004 7:22-------------------
 * Funkce check(int s) kontroluje, zda je symbol (konečný stav konečného automatu)
 * v množině možných symbolů, daných parametrem s.
 * Parametr s obsahuje množinu zadanou bitovým součtem možných symbolů.
 * --------------------------------------------------*/
void check(int s) {
    if (!(symbol&s)) {
        printf("unexpected symbol nr. %d !", symbol );
        exit(-1);
    }
}
/*-----------------2.12.2004 7:25-------------------
 * A zde už následují jednotlivé funkce, vyhodnocující nonterminální symboly gramatiky.
 * Jak se k nim došlo - viz přednášky...
 * --------------------------------------------------*/
void s() {
    /* S  -> EX; S | e */
    float val=0;
    check (plus|minus|lzavorka|num|ef|sinus);
    if (symbol!=ef) {
        val=ex();
        printf("Vysledek=%f\n", val);
        expect(strednik);
        s();
    }
}

float ex() {
    /* EX  -> M EX2 | e */
    float val=0;
    check (plus|minus|lzavorka|num|pzavorka|ef|sinus);
    if (symbol!=ef) {
        val=m();
        val=ex2(val);
    }
    return val;
}

float ex2(float v) {
    /* EX2 -> + M EX2 | - M EX2 | e */
    float val=0;

    check(plus|minus|strednik|pzavorka|ef|sinus);
    if (symbol==plus || symbol==minus) {
        if (symbol==plus) {
            symbol=lexAnalyzer();
            val=m();
            val+=v;
        } else if (symbol==minus) {
            symbol=lexAnalyzer();
            val=m();
            val=v-val;
        }
        val=ex2(val);
        return val;
    } else return v; /* pro prázdné pravidlo (EX2 -> e) */
}


float m() {
    /*  M   -> T M2 */
    float val;
    val=t();
    val=m2(val);
    return val;
}

float m2(float v) {
    /*  M2  -> * T M2| / T M2| e */
    float val;
    check(krat|deleno|plus|minus|strednik|pzavorka|sinus);
    if (symbol==krat || symbol==deleno) {
        if (symbol==krat) {
            symbol=lexAnalyzer();
            val=t();
            val*=v;
        }
        else if (symbol==deleno) {
            symbol=lexAnalyzer();
            val=t();
            if(val != 0){
                val=v/val;
            }
            else{
                printf("!!! DELENIE NULOU !!!\n");
                exit(-1);
            }
        }
        val=m2(val);
        return val;
    } else return v;	/* pro prázdné pravidlo (M2 -> e) */
}

float t() {
    /*   T-> num | + num | -num | ( EX ) | sin(EX) | cos(EX) */
    float val;
    check(num|plus|minus|lzavorka|sinus);
    if (symbol==num) {
        val=(float)numval;
        symbol=lexAnalyzer();
        return val;
    } else {
        switch (symbol) {
            case plus:
                symbol=lexAnalyzer();
                expect(num);
                return (float)numval;
            case minus:
                symbol=lexAnalyzer();
                expect(num);
                return (float)-numval;
            case lzavorka:
                symbol=lexAnalyzer();
                val=ex();
                expect(pzavorka);
                return val;
            case sinus:
                symbol = lexAnalyzer();
                val = (float) sin(ex());
                return val;
            /*case cosinus:
                symbol = lexAnalyzer();
                val = (float) cos(ex());
                return val;*/
            default:
                printf("Error: unexpected symbol...");
                return 0;
        }
    }
}

int main() {
    soubor=stdin;
    symbol=lexAnalyzer();

    float val=ex();
    printf("\nVysledek=%f\n", val);

    /* Pro vypocet nekolika vyrazu oddelenych strednikem: */
    s();
    return 0;
}
