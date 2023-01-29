# Tirocinio

## Obiettivo
Valutazione delle prestazioni di un servizio microcloud di task offloading in vehicular edge computing

## Tools utilizzati
- OMNeT++ ([documentation](https://omnetpp.org/documentation/))
- SUMO ([documentation](https://sumo.dlr.de/docs/))

## Framework utilizzati
- Veins ([documentation](https://veins.car2x.org/documentation/))
- INET ([documentation](https://inet.omnetpp.org/Introduction.html))

### Riproduzione ambiente di lavoro (in locale)
- Installare/compilare OMNeT++
- Installare/importare il progetto INET in OMNeT++
- Installare/importare il progetto Veins in OMNeT++
- Eseguire build di tutti i progetti
- Importare questo progetto all'interno del workspace
- Eseguire build del progetto

### Riproduzione ambiente di lavoro (con Docker)
È stata resa disponibile una versione containerizzata con Docker del progetto, per poterla eseguire:
- Clonare la repo e spostarsi sul branch `main-docker`
- `docker compose build`
- `docker compose up`

Questi comandi permettono di ricreare l'ambiente di lavoro in Docker ed eseguire la simulazione automaticamente. <br>
I risultati verranno salvati nella cartella locale `simulations/v5_0_Final/results` all'interno del progetto.

---

***NOTE*** 
- I branch `main` e `main-docker` contengono le versioni stabili delle corrispettive versioni in locale e su docker.
- I branch `develop` e `develop-docker` contengono gli ultimi aggiornamenti e le ultime modifiche al progetto.
- Il branch `refactoring` non contiene nuove funzionalità, ma è utilizzato per la manutenzione e rifattorizzazione del codice.

@author Luca Parenti <br>
@supervisor Christian Quadri
