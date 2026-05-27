#!/usr/bin/env python3

import random
import argparse
import gzip

# ----------------------------
# Argumentos de línea de comandos
# ----------------------------

parser = argparse.ArgumentParser(
    description="Generar archivos FASTQ Illumina paired-end comprimidos"
)

parser.add_argument(
    "-o", "--output_prefix",
    required=True,
    help="Prefijo de salida (ejemplo: sample)"
)

parser.add_argument(
    "-n", "--num_sequences",
    type=int,
    required=True,
    help="Número de reads paired-end"
)

parser.add_argument(
    "-l", "--length",
    type=int,
    required=True,
    help="Longitud de cada read"
)

args = parser.parse_args()

# ----------------------------
# Configuración
# ----------------------------

bases = ['A', 'T', 'G', 'C']

# Calidad Illumina-like Q20-Q40
quality_chars = [chr(q) for q in range(30, 74)]

# Archivos comprimidos
r1_file = f"{args.output_prefix}_R1.fastq.gz"
r2_file = f"{args.output_prefix}_R2.fastq.gz"

# ----------------------------
# Reverse complement
# ----------------------------

def reverse_complement(seq):

    complement = {
        'A': 'T',
        'T': 'A',
        'G': 'C',
        'C': 'G'
    }

    return ''.join(complement[b] for b in reversed(seq))

# ----------------------------
# Generar FASTQ PE comprimidos
# ----------------------------

with gzip.open(r1_file, "wt") as r1, gzip.open(r2_file, "wt") as r2:

    for i in range(1, args.num_sequences + 1):

        # Fragmento original
        fragment_length = args.length * 2

        fragment = ''.join(
            random.choices(bases, k=fragment_length)
        )

        # Reads PE
        read1 = fragment[:args.length]

        read2 = reverse_complement(
            fragment[-args.length:]
        )

        # Calidades aleatorias
        qual1 = ''.join(
            random.choices(quality_chars, k=args.length)
        )

        qual2 = ''.join(
            random.choices(quality_chars, k=args.length)
        )

        # Header Illumina-like
        header = f"@SIM:{i}:FCX:1:1101:1000:{i}"

        # Write R1
        r1.write(f"{header} 1:N:0:1\n")
        r1.write(f"{read1}\n")
        r1.write("+\n")
        r1.write(f"{qual1}\n")

        # Write R2
        r2.write(f"{header} 2:N:0:1\n")
        r2.write(f"{read2}\n")
        r2.write("+\n")
        r2.write(f"{qual2}\n")

# ----------------------------
# Resumen
# ----------------------------

print("FASTQ paired-end comprimidos generados")
print(f"R1: {r1_file}")
print(f"R2: {r2_file}")
print(f"Reads: {args.num_sequences}")
print(f"Longitud: {args.length} bp")
