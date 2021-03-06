/*---------------------------------------------------------------
 *
 *  RAPIFILT:"RAPId FILTer"
 *	
 *  RAPIFILT is a fast computational tool designed to trim sequences 
 *  using the quality scores of bases within individual read.
 *  
 *  Copyright (C) 2017 Benavides A. Cabarcas F, Alzate J.F.
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see http://www.gnu.org/licenses/ .
 *   
 *  Version 1.0
 ---------------------------------------------------------------*/

#include "process_fastqFile.h"
#include "generic.h"

void process_fastqFile(struct Args * args, struct Names * names)
{
    //cout<<"working in fastq";
    FILE *Outfastq_fp1, *bad_fp1, *fp1, *stat_fp;
    string ext=".fastq"; 
    char firstC='@';
    if(args->enableFasta)
        firstC='>',ext=".fasta";

    if ( args->outputFile ) 
    {
        string name5=names->outputFile+ext;
        string bad1=names->outputFile+"_bad"+ext;
        string stat=names->outputFile+"_stat.txt";

        char myArray1[name5.size()+1];
        char myArray5[bad1.size()+1];
        char myArray6[stat.size()+1];

        strcpy(myArray1, name5.c_str());
        strcpy(myArray5, bad1.c_str());
        strcpy(myArray6, stat.c_str());
        
        if ((Outfastq_fp1 = fopen(myArray1, "w")) == NULL ||  (bad_fp1 = fopen(myArray5, "w")) == NULL  || (stat_fp = fopen(myArray6, "w")) == NULL) 	
            cout<<"Could not open files for writing "<<names->outputFile<<endl, exit(1);
    }
    
    char * file1 = strdup(names->fastq.c_str());
	
    // START read gz compressed file or not compresed
    bool gzFiles = false;
    if (names->fastq.compare(names->fastq.size()-3,3,".gz") == 0) {
        gzFiles = true;
    }

    //filebuf fb;
    filebuf infile1;
    ifstream gzFile1;

    istream instream1(NULL),instream2(NULL);
    boost::iostreams::filtering_streambuf<boost::iostreams::input> inbuf1;

    if (gzFiles) {

        //Read from the first command line argument, assume it's gzipped
        gzFile1.open(file1, std::ios_base::in | std::ios_base::binary);
        inbuf1.push(boost::iostreams::gzip_decompressor());
        inbuf1.push(gzFile1);
        //Convert streambuf to istream
        instream1.rdbuf(&inbuf1);
    } else {
        //fb.open(names.fastq.c_str(),std::ios::in);
        infile1.open(file1,ios::in);
        instream1.rdbuf(&infile1);
    }
    // END read gz compressed file or not compresed

    typedef enum {s0, s1,s2, s3, s4,s5,s6,s7} STATES; //to read multifasta files
    STATES state=s0;
	
    bool next=true, save=true;
    //char word1='\0', word2='\0'; 
    string name1="", aux1=""; 
    vector<char> bases1;
    vector<char> quality1;
    vector<unsigned int> quality1_int;

    unsigned int sum=0, k=0;
    unsigned int bouderLeft1=0, bouderRight1=0, finalSize1=0, size1=0;
    std::vector<unsigned int>::iterator it1b, it1e;

    //statistics
    //int Gbase=0, Cbase=0;
    //int Gbasef=0, Cbasef=0;
    //int totalBases=0, totalBasesf=0;
    int qual_position[maxReadLen]={0};  //maxReadLen max read lenght
    int qual_positionf[maxReadLen]={0};  //maxReadLen max read lenght
    unsigned int maxQ[maxReadLen]={0}, minQ[maxReadLen];
    unsigned int maxQF[maxReadLen]={0}, minQF[maxReadLen];   
    int total_reads[maxReadLen]={0}, total_readsf[maxReadLen]={0};   

    int index=0, indexF=0; // total_reads=0, total_readsf=0, 
    int max_read_len=0, max_read_lenf=0;
    
    int binSize=args->binSize;


    for (int j=0; j<maxReadLen;j++)
        minQ[j]=1000, minQF[j]=1000;
    
    while (next)
    {
        char word1='\0'; 
        int tb=args->tb;
        int te=args->te;
        //int countbases=0;
        //cout<<state<<endl;
        switch ( state ) 
        {
            case s0:
                next=false;//total_reads++;
                while(word1!='\n')
                {
                    if(instream1.get(word1)){
                        next=true;
                        if(int(word1)!=32 && save)//32=space 
                            name1+=word1;
                        else
                            aux1+=word1, save=false;}
                    else
                        break;
                }
                    state=s1;
                    save=true;
                    break;
                case s1:
                    
                    while(instream1.get(word1) && word1!='\n')
                        bases1.push_back(word1);
                    state=s2;
                    break;
                case s2:
                    while(word1!='\n')
                        instream1.get(word1);
                    state=s3;
                    break;
                case s3:
                    while(instream1.get(word1) && word1!='\n')
                        quality1.push_back(word1),quality1_int.push_back((unsigned int)word1-33) ;
                    state=s4;
                    break;
                case s4:
                    //cut by quality
                    //left
                    //remove the first or the last n bases;
                    size1=quality1_int.size();
                    it1b = quality1_int.begin();
                    it1e = quality1_int.end();
                    bouderLeft1=tb;
                    
                   
                    while (it1b!=it1e) 
                    {
                        sum=0;k=0;
                        while((bouderLeft1+k)<size1 && k<args->windows_size && *(it1b+k+tb)>=args->left_cut )
                            sum++,k++;

                        if(sum==args->windows_size)
                            break;
                        bouderLeft1++;it1b++;
                    }	
                    //right and remove the first &/or the last n bases;
                    it1b = quality1_int.begin();
                    it1e = quality1_int.end();
                    bouderRight1=te;
                   
                    while (it1e!=it1b) 
                    {
                        it1e--;sum=0;k=0;
                        while((size1-bouderRight1-k)>0 && k<args->windows_size && *(it1e-k-te)>=args->right_cut)
                        sum++,k++;

                        if(sum==args->windows_size)
                            break;
                        bouderRight1++;
                    }

                    finalSize1=quality1_int.size()-	bouderLeft1-bouderRight1;
                    state=s5;
                    break;
                case s5:
                    if ( !args->outputFile ) 
                        fp1 = stdout;
                    else
                    {
                        if((finalSize1>=args->min_len) && (finalSize1<=args->max_len) ) //11
                            fp1=Outfastq_fp1;//total_readsf++;
                        else //to bad files (next version)
                            bouderLeft1=0,bouderRight1=0,fp1=bad_fp1;//,bouderLeft2=0,bouderRight2=0,fp2=bad_fp2;
                    }
                    state=s6;
                    break;
                case s6:
                    name1.erase (0,1);
                    //print name
                    fprintf(fp1,"%c%s%s",firstC,name1.c_str(),aux1.c_str());//cout<<firstC<<name<<aux1;
                    //print bases
                    for (std::vector<char>::iterator it = bases1.begin()+bouderLeft1; it != bases1.end()-bouderRight1; ++it)
                        fprintf(fp1, "%c", *it);//std::cout << *it;
                    fprintf(fp1, "\n");//cout<<endl;
                    //print qualities
                    if(args->enableFasta==0)
                    {
                        fprintf(fp1,"+\n");//cout<<"+"<<name1<<aux1;
                        for (std::vector<char>::iterator it = quality1.begin()+bouderLeft1; it != quality1.end()-bouderRight1; ++it)
                            fprintf(fp1, "%c", *it);//std::cout << *it;
                        fprintf(fp1, "\n");//cout<<endl;
                    }
                    state=s7;
                    break;
                case s7: //GC and statistics process  for original read
                    
                    //original
                    //GC_content(bases1.data(),size1,&Gbase,&Cbase,0,0);
                    statistic_mean (quality1_int.data(), binSize, size1, &max_read_len, &maxQ[0], &minQ[0], &total_reads[0], &qual_position[0], 0, 0);

                    //trimer
                    //GC_content(bases1.data(),finalSize1,&Gbase,&Cbase,bouderLeft1,bouderRight1);
                    statistic_mean (quality1_int.data(), binSize, finalSize1, &max_read_lenf, &maxQF[0], &minQF[0], &total_readsf[0], &qual_positionf[0], bouderLeft1, bouderRight1);
                    
                    name1="",  aux1=""; 
                    bases1.clear();
                    quality1.clear();
                    quality1_int.clear();                
                    state=s0;
                    break;
        }
    }
    
    //statistics process  for original reads
    fprintf(stat_fp,">Original Reads: %d%s%d%s", total_reads[0],"\tLargest read: ",max_read_len,"\n");
    index=max_read_len/binSize;
    for(int i=0;i<index;i++)
        if (total_reads[i] > 0)
            fprintf(stat_fp, "%d%s", qual_position[i]/total_reads[i]," ");
        else
            fprintf(stat_fp, "%d%s", qual_position[i]," ");
    fprintf(stat_fp, "\n");

    for(int i=0;i<index;i++)
        fprintf(stat_fp, "%d%s", maxQ[i]," ");
    fprintf(stat_fp, "\n");

    for(int i=0;i<index;i++)
        fprintf(stat_fp, "%d%s", minQ[i]," ");
    fprintf(stat_fp, "\n");
    
    fprintf(stat_fp,">Final Reads: %d%s%d%s", total_readsf[0],"\tLargest read: ",max_read_lenf,"\n");
    indexF=max_read_lenf/binSize;
    for(int i=0;i<indexF;i++) {
        if (total_readsf[i] > 0)
            fprintf(stat_fp, "%d%s", qual_positionf[i]/total_readsf[i]," ");
        else
            fprintf(stat_fp, "%d%s", qual_positionf[i]," ");
    }
    fprintf(stat_fp, "\n");

    for(int i=0;i<indexF;i++)
        fprintf(stat_fp, "%d%s", maxQF[i]," ");
    fprintf(stat_fp, "\n");

    for(int i=0;i<indexF;i++)
        fprintf(stat_fp, "%d%s", minQF[i]," ");
    fprintf(stat_fp, "\n");
    
    if(gzFiles)
      gzFile1.close();
    else
      infile1.close();
    if ( args->outputFile ) 
    {
        //fclose(Outfastq_fp1);
        fclose(fp1);
    }
    
}


