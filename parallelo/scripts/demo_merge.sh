#!/bin/bash
#SBATCH --job-name=demo_mergesort_long_30          # Nome del job
#SBATCH --output=../risultati_test/dmerge_sort30_%j.out  # File output (%j = job ID)
#SBATCH --error=../risultati_test/dmerge_sort30_%j.err    # File errori
#SBATCH --nodes=4                    # Numero di nodi
#SBATCH --ntasks-per-node=32          # Task (processi MPI) per nodo
#SBATCH --cpus-per-task=1            # CPU per task
#SBATCH --partition=broadwell           # Partizione da usare

# Carica i moduli necessari (adatta al tuo cluster)
module load gcc openmpi

# Mostra i moduli caricati
echo "=== Moduli Caricati ==="
module list
echo ""

# Informazioni sul job
echo "=== Informazioni Job ==="
echo "Job ID: $SLURM_JOB_ID"
echo "Nodi allocati: $SLURM_JOB_NODELIST"
echo "Numero totale di task: $SLURM_NTASKS"
echo "Directory lavoro: $(pwd)"
echo "Data inizio: $(date)"
echo ""
cd ..

# Definisci il percorso del file sorgente
SOURCE_DIR="src"
SOURCE_FILE="mpi.c"
EXECUTABLE="out/mpi"

# Verifica che il file sorgente esista
if [ ! -f "$SOURCE_DIR/$SOURCE_FILE" ]; then
    echo "ERRORE: File $SOURCE_DIR/$SOURCE_FILE non trovato!"
    exit 1
fi

# Compilazione
echo "=== Compilazione ==="
echo "Compilando $SOURCE_DIR/$SOURCE_FILE..."
make compila_mpi

# Controlla se la compilazione è avvenuta con successo
if [ $? -eq 0 ]; then
    echo " Compilazione completata con successo!"
    echo ""
else
    echo " ERRORE: Compilazione fallita!"
    exit 1
fi

# Test 1: 128 processi (come nell'esempio del codice)
echo "--- Test con 64 processi ---"
srun -n 128 out/mpi
echo ""

# Test 2: 64 processi (come nell'esempio del codice)
echo "--- Test con 64 processi ---"
srun -n 64 out/mpi
echo ""

# Test 3: 32 processi (come nell'esempio del codice)
echo "--- Test con 32 processi ---"
srun -n 32 out/mpi
echo ""

# Salva il codice di uscita dell'ultimo test
EXIT_CODE=$?

echo "=== Fine Esecuzione ==="
echo "Codice di uscita: $EXIT_CODE"
echo "Data fine: $(date)"

# Pulizia (opzionale - commenta se vuoi mantenere l'eseguibile)
# rm -f $EXECUTABLE

exit $EXIT_CODE
