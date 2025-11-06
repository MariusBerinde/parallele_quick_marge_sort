# Dettagli 
Questo repository contiene lo sviluppo degli algoritmi **quick sort** e **merge sort**.
Degli algoritmi usati esistono due versioni un che funziona con i tipi long e una per i tipi int , i file che hanno la sigla **_int** contengono questa versione.
La cartella sequenziale lo sviluppo della versione sequeziale dei suddetti algoritmi, è stata utilizzata principalmente per esporare come questi algoritmi possano essere scritti in C 
e come usare i make file.
La cartella parallelo contiene le implementazioni parallele dei sudddetti algoritmi, in particolare 
La cartella scripts contiene gli scripts usati per il lancio dei test usado sbatch.
**main** contiene i benckmark della versione implementata con openmpi mentre merge contiene implementazione e benckmark della versione implementata con **mpi**.

Il make file presente serve per i test in locale .


Il file omp.c contiene i tentativi per lo sviluppo della versione che utilizza la libreria ompi.
Il file mpi.c contiene i tentativi per lo sviluppo della versione che utilizza la libreria mpi.

## Descrizione comandi Makefile 
- compila_run : serve per compilare main.c 
- run: serve per eseguire i benckmark della versione finale dei programmi di sorting.
- compila_omp: serve per eseguire test durante lo sviluppo della versione openmpi.
- omp: serve per eseguire il test mpi
- compila_mpi: serve per compilare merge.c 
- mpi:serve per eseguire i benckmark contenuti in merge.c 
