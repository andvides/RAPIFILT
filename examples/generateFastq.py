#!/usr/bin/env python3

import random
import argparse
import string

# Argumentos de línea de comandos
parser = argparse.ArgumentParser(
    description="Generar un archivo FASTQ aleatorio"
)

parser.add_argument(
    "-o", "--output",
    required=True,
    help="Nombre del archivo FASTQ de salida"
)

parser.add_argument(
    "-n", "--num_sequences",
    type=int,
    required=True,
    help="Número de secuencias a generar"
)

parser.add_argument(
    "-l", "--length",
    type=int,
    required=True,
    help="Longitud de cada secuencia"
)

args = parser.parse_args()

# Bases nucleotídicas
bases = ['A', 'T', 'G', 'C']

# Caracteres válidos para calidad Phred+33
# Rango típico: ASCII 33 (!) a 73 (I)
quality_chars = [chr(q) for q in range(33, 74)]

# Generar archivo FASTQ
with open(args.output, "w") as f:

    for i in range(1, args.num_sequences + 1):

        # Secuencia aleatoria
        seq = ''.join(random.choices(bases, k=args.length))

        # Calidad aleatoria
        qual = ''.join(random.choices(quality_chars, k=args.length))

        # Escribir FASTQ
        f.write(f"@SEQ_{i}\n")
        f.write(f"{seq}\n")
        f.write("+\n")
        f.write(f"{qual}\n")

print(f"Archivo FASTQ generado: {args.output}")
print(f"Secuencias: {args.num_sequences}")
print(f"Longitud: {args.length} bp")
