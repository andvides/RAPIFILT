# RAPIFILT
RAPId FILTer (RAPIFILT) is a C program to filter DNA sequences. It removes lowquality
bases and removes short sequences.

--------------------------------------------------------------
Installing 
--------------------------------------------------------------
cd RAPIFILT_v1.1<br />
make<br />
export PATH=$PATH:$(pwd)<br />

---------------------------------------------------------------
Running DATMA
--------------------------------------------------------------
rapifilt -fastq bmini.fq -l 20 -r 20 -o bmini_out

---------------------------------------------------------------
Output
---------------------------------------------------------------
RAPIFILT generates the following files:
1. bmini_out.fastq: Filtered sequences.
2. bmini_out_bad.fastq: Removed sequences.
3. bmini_out_stat.txt: Statistics per base.
---------------------------------------------------------------
