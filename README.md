# Dettagli 
Questo repository contiene lo sviluppo degli algoritmi **quick sort** e **merge sort**.
La cartella sequenziale lo sviluppo della versione sequeziale dei suddetti algoritmi, è stata utilizzata principalmente per esporare come questi algoritmi possano essere scritti in C 
e come usare i make file.
La cartella parallelo contiene le implementazioni parallele dei sudddetti algoritmi, in particolare 
**main** contiene i benckmark della versione implementata con openmpi mentre merge contiene implementazione e benckmark della versione implementata con **mpi**.

Il make file presente serve per i test in locale .
Il file parallelo/Makefile_server contiene la versione dei comandi da eseguire su un server HPC.

## Descrizione comandi Makefile 
- compila_run : serve per compilare main.c 
- run: serve per eseguire i benckmark contenuti in main.c 
- compila_merge: serve per compilare merge.c 
- merge:serve per eseguire i benckmark contenuti in merge.c

