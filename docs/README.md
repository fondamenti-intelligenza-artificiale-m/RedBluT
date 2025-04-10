# Documentazione di *RedBluT*

Questa documentazione raccoglie e approfondisce tutti gli aspetti tecnici e progettuali del motore di gioco *RedBluT*, spiegando le scelte implementative e le tecnologie.  
È pensata sia come riferimento che come guida per chi vuole comprendere il funzionamento interno del sistema o alcuni suoi dettagli.

## 📚 Indice

- [🧩 Architettura del Sistema](#architettura-del-sistema)
- [🧵 Utilizzo del Parallelismo](#utilizzo-del-parallelismo)
- [🧠 Rappresentazione dello Spazio degli Stati](#rappresentazione-dello-spazio-degli-stati)
- [🎯 Generazione delle Mosse](#generazione-delle-mosse)
- [🔍 Strategie di Ricerca](#strategie-di-ricerca)
- [⚖️ Valutazioni ed Euristiche](#valutazioni-ed-euristiche)


## Architettura del Sistema

Il motore *RedBluT* sfrutta un'architettura **SMP (Symmetric MultiProcessing)** per massimizzare l'efficienza nella ricerca delle mosse in ambienti multi-core.

## Utilizzo del Parallelismo

### Lazy SMP (Lazy Symmetric MultiProcessing)

E' stato adottato un modello noto come **Lazy SMP**, che permette ai thread di lavorare in modo indipendente su diverse parti dell'albero delle mosse, condividendo una [transposition table centrale](#trasnsposition-table).

Nella configurazione attuale, con **4 core disponibili**, i thread sono assegnati come segue:  
- 🕒 **1 thread** è dedicato esclusivamente alla gestione del tempo.  
    Esso segnala quando la ricerca deve interrompersi per consegnare una [mossa valida](#mossa-valida) entro il tempo limite.
- 🚀 **3 thread** si occupano della ricerca vera e propria.  
    Lavorando ciascuno su una tra le **prime tre mosse** ordinate secondo la [funzione euristica](#euristica).

### LMR (Late Move Reductions)

A partire dalla **quarta mossa** in ordine euristico, entra in gioco la tecnica chiamata **Late Move Reductions (LMR)** 😴: questa tecnica riduce la profondità di ricerca per le mosse considerate meno promettenti (cioè quelle che appaiono più tardi nella lista ordinata).  
L’idea è che se una mossa non è tra le prime valutate, è probabilmente meno interessante, e quindi può essere esplorata con una profondità minore e, solo se essa supera comunque un certo valore soglia, detto fail-high 📈, verrà riesaminata a piena profondità.

## Rappresentazione dello Spazio degli Stati

L'intero stato di gioco è rappresentato in maniera compatta ed efficiente attraverso l'utilizzo di maschere dette **bitboard**: una rappresentazione compatta di 81 bit per ciascun tipo di pezzo. 🧠

```python
00  01  02  03  04  05  06  07  08
09  10  11  12  13  14  15  16  17
18  19  20  21  22  23  24  25  26
27  28  29  30  31  32  33  34  35
36  37  38  39  40  41  42  43  44
45  46  47  48  49  50  51  52  53
54  55  56  57  58  59  60  61  62
63  64  65  66  67  68  69  70  71
72  73  74  75  76  77  78  79  80
```

Ogni indice rappresenta una casella sulla scacchiera 9x9 e assume `0` se il cavaliere è assente, `1` se è presente.

Nel dettaglio, la struttura dello stato comprende:
- 🟤⚪ **2 Bitboard principali** - rispettivamente per i per cavalieri neri e per i cavalieri bianchi.
- 👑 **Bitboard del re** – con un singolo bit attivo, perchè c'è sempre e solo un re.
- 🔄 **Bit di turno** – che indica a quale giocatore tocca muovere.
    - `0` -> bianchi.
    - `1` -> neri.

Complessivamente sono utilizzati 244 bit per rappresentare una posizione. Questa rappresentazione è facilmente manipolabile da un motore di gioco ottimizzato: le operazioni bitwise permettono una gestione rapida delle mosse, dei controlli e delle interazioni tra pezzi.

## Generazione delle Mosse

La generazione delle mosse di un gioco su scacchiera modellato con **bitboard** è un processo complesso ma altamente ottimizzabile, e il nostro motore approfitta di questa possibilità.

### Maschere di Sollitudine

Inizialmente, si precomputano le **maschere di solitudine**.  
Queste indicano le possibili mosse, specifiche per un pezzo di un preciso schieramento, che si muove da una precisa posizione.  
**Non entrambi gli schieramenti possono occupare ogni casella**: è il caso degli accampamenti e del castello.  
Ovviamente non ha senso pensare ad una maschera di solitudine che indica i movimenti a partire da una casella che non si può raggiungere in primo luogo; il numero delle maschere di movimento è quindi (81 - 16) + (81 - 1) = 145.

### Maschere d'Intralcio

E' necessario considerare gli altri pezzi presenti sulla scacchiera, e per questo si utilizzano le **maschere d'intralcio**.
Le maschere di intralcio sono bitboard di soli 16 bit, perchè ogni pezzo, al massimo, può essere capace di 16 mosse; e servono a indicare dove si trovano i pezzi che intralciano il movimento.
Anche le maschere di intralcio si possono precomputare; il numero di maschere di intralcio sembra essere 145 * 2^16 = 9.502.720, mentre in realtà la presenza di molte caselle vietate limita il numero di maschere di intralcio a 1161600, dal momento che no considero d'intralcio i pezzi sulla stessa riga o colonna, ma che si trovano su una casella non prevista dalla maschera di soliitudine.

### Pezzi Scorrevoli

I pezzi di tablut si dicono pezzi scorrevoli, come anche lo sono regina, torre, ed alfiere a schacchi; diversamente dai pezzi della dama.  
Se non si presta attenzione si richia di far saltare i pezzi oltre gli ostacoli, e non sarebbe corretto:

```python
allowed_moves_bitboard = loneliness_bitboard & ~snag_bitboard # The pieces are not equipped with Red Bull
```

### Mosse Lette

Si deve evitare di considerare una mossa per volta con un ciclo in ogni direzione!
Di conseguenza le mosse non sono davvero generate durante la partita ma sono in realtà semplicemente lette.
Per ogni schieramento e per ogni posizione sono previste tutte maschere d'intralcio possibili.
L'indice utilizzato per accedere alla struttura è molto compatto:

```python
(color << 23) | (position << 16) | snag_mask
```

Questa soluzione è buona ma non ideale perchè richiede di normalizzare la maschera d'intralcio da 81 a 16 bit.

## Strategie di Ricerca

### Zobrist Hash

Zobrist Hashing è una tecnica utilizzata per rappresentare in modo efficiente le posizioni di gioco, associando a ciascuna configurazione un valore hash unico.  
Ogni caratteristica del gioco, ovvero la posizione dei pezzi e il turno del giocatore, ha un valore hash casuale, e lo stato complessivo è ottenuto facendo lo XOR tra i valori delle caratteristiche.  
Ogni mossa modifica poche delle caratteristiche della posizione e questo permette di aggiornare rapidamente l'hash quando cambia una parte della posizione, rendendo l'algoritmo veloce ed efficiente.

Lo Zobrist Hashing utilizza valori di hash per rappresentare le posizioni di gioco.  
La probabilità di collisione di due posizioni con lo stesso hash dipende dalla lunghezza dell'hash e dal numero di posizioni esplorate:

$$
P_{\text{collisione}} \approx 1 - \exp\left( -\frac{m^2}{2 \cdot 2^b} \right)
$$

- ( m = 240,000 ) è il numero di posizioni esplorate (ogni thread parallelo 1000 al secondo).
- ( b = 64 ) è il numero di bit del valore hash.

La probabilità di collisione è praticamente nulla, rendendo questo valore adeguato per garantire unicità nelle configurazioni di gioco.

## Valutazioni ed Euristiche

Lavori in corso... Luca e Francesco stanno cucinando.
