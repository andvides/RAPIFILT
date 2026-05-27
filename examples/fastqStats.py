#!/usr/bin/env python3

import argparse
import gzip

# --------------------------------------------------
# Argumentos por línea de comandos
# --------------------------------------------------

parser = argparse.ArgumentParser(
    description="Tabular calidad mínima y máxima de cada read"
)

parser.add_argument(
    "-i", "--input",
    required=True,
    help="Archivo FASTQ (.fastq o .fastq.gz)"
)

parser.add_argument(
    "-o", "--output",
    required=True,
    help="Archivo TSV de salida"
)

args = parser.parse_args()

# --------------------------------------------------
# Abrir FASTQ
# --------------------------------------------------

if args.input.endswith(".gz"):

    fastq = gzip.open(args.input, "rt")

else:

    fastq = open(args.input, "r")

# --------------------------------------------------
# Procesar reads
# --------------------------------------------------

with fastq as f, open(args.output, "w") as out:

    out.write(
        "Read_ID\tLength\tMin_Q\tMax_Q\tMean_Q\n"
    )

    while True:

        header = f.readline().strip()

        if not header:
            break

        seq = f.readline().strip()
        plus = f.readline().strip()
        qual = f.readline().strip()

        # ASCII -> Phred+33
        q_scores = [
            ord(q) - 33
            for q in qual
        ]

        min_q = min(q_scores)
        max_q = max(q_scores)

        mean_q = round(
            sum(q_scores) / len(q_scores),
            2
        )

        read_id = header.replace("@", "").split()[0]

        out.write(
            f"{read_id}\t"
            f"{len(seq)}\t"
            f"{min_q}\t"
            f"{max_q}\t"
            f"{mean_q}\n"
        )

print("Archivo generado:")
print(args.output)
