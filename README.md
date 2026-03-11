# Parallel Sorting Algorithms — QuickSort & MergeSort

Implementazione sequenziale e parallela di **QuickSort** e **MergeSort** in C, con approccio ibrido **MPI + OpenMP** e deployment su cluster tramite **SLURM/sbatch**.

---

## Tecnologie

| Tecnologia | Versione | Utilizzo |
|---|---|---|
| `GCC` | 9.1 | Compilatore |
| `OpenMP` | 5.0 | Parallelismo shared-memory (intra-nodo) |
| `MPI` | 4.1 | Parallelismo distributed-memory (inter-nodo) |
| `SLURM / sbatch` | — | Scheduling job su cluster HPC |
| `Make` | — | Build system |

---

## Struttura del progetto

```
├── sequenziale/              # Versioni sequenziali (baseline)
├── parallelo/
│   ├── main.c                # Implementazione e benchmark QuickSort (OpenMP+MPI)
│   ├── merge.c               # Implementazione e benchmark MergeSort (OpenMP+MPI)
│   ├── omp.c                 # Sviluppo sperimentale OpenMP
│   ├── mpi.c                 # Sviluppo sperimentale MPI
│   └── scripts/              # Script sbatch per test su cluster
└── Makefile
```

> I file con suffisso `_int` operano su tipo `int`, gli altri su tipo `long`.

---

## Algoritmi

### MergeSort
- Complessità temporale: **O(n log n)** in ogni caso
- Complessità spaziale: **O(n)** per l'array ausiliario
- Parallelizzazione: task OpenMP ricorsivi + merge distribuito via MPI con albero binario (`binary_merge_tree_alt`)
- Frazione parallelizzabile stimata (Amdahl): **~67%** → speedup teorico massimo ≈ 3×

### QuickSort
- Complessità temporale media: **O(n log n)**, caso peggiore O(n²)
- Complessità spaziale: **O(log n)** (stack ricorsivo)
- Pivoting: schema di **Hoare + median-of-three**
- Parallelizzazione: task OpenMP ricorsivi + stessa fase MPI di MergeSort
- Frazione parallelizzabile stimata (Amdahl): **~70%** → speedup teorico massimo ≈ 3.3×

---

## Risultati sperimentali

Test eseguiti su cluster **Broadwell** (Intel Xeon E5-2697 v4, 36 core/nodo, 128 GB RAM, rete Intel OmniPath). Array da ~1 miliardo di elementi (`2³⁰`), valori random in `[0, 2×N]`.

### Strong Scaling (2³⁰ elementi)

| Nodi MPI | QuickSort int speedup | MergeSort int speedup |
|---|---|---|
| 8 | 6.09 | 7.42 |
| 16 | 11.76 | 11.29 |
| 32 | 14.11 | 14.45 |
| 64 | 14.72 | 15.40 |
| 128 | **14.92** | **16.55** |

> MergeSort scala meglio di QuickSort grazie alla struttura ricorsiva più stabile. La versione `long` mostra speedup superlineare a basso numero di nodi per effetti di cache.

### Weak Scaling
Buona efficienza fino a 8 nodi. Crolla oltre 32 nodi per entrambi gli algoritmi a causa dell'overhead del merge distribuito `O(N log P)`.

---

## Ambiente di test

```
CPU:       Intel Xeon E5-2697 v4
Core/nodo: 36
RAM:       128 GB
Rete:      Intel OmniPath
Compiler:  GCC 9.1  (-Wall -g -fopenmp -O3)
MPI:       OpenMPI 4.1
OpenMP:    5.0
```

---

## Build & Esecuzione

### In locale

```bash
# QuickSort (OpenMP+MPI)
make run

# MergeSort (OpenMP+MPI)
make mpi
```

### Target Makefile

| Target | Descrizione |
|---|---|
| `compila_run` | Compila `main.c` (QuickSort) |
| `run` | Esegue benchmark QuickSort |
| `compila_omp` | Compila `omp.c` (sviluppo OpenMP) |
| `omp` | Esegue test OpenMP |
| `compila_mpi` | Compila `merge.c` (MergeSort) |
| `mpi` | Esegue benchmark MergeSort |

### Su cluster (SLURM)

```bash
cd parallelo/scripts
sbatch <script>.sh
```

---

## Conclusioni

- **QuickSort** è più veloce in esecuzione sequenziale ma scala peggio in parallelo
- **MergeSort** sfrutta meglio la parallelizzazione grazie alla struttura divide-et-impera più prevedibile
- L'approccio ibrido **MPI+OpenMP** supera le versioni solo-MPI e solo-OpenMP (speedup > 7× su entrambi gli algoritmi)
- Il collo di bottiglia principale è il **merge finale distribuito** (`O(N log P)`), che diventa dominante oltre i 32 nodi
