#include "AIG.h"
#include "binaryDS.h"
using namespace std;



int posY_max,posX_max;
int main(int argc, char** argv) {
    if(MNIST_DS == 1)
        {posY_max=28; posX_max=224;}
    else
        {posY_max=32*3; posX_max=32*8;}//3*8*32;
    
    struct rusage buf;
    int start,stop,start_simplf,stop_simplf,start_app,stop_app,iterations=0;
    if(getrusage(RUSAGE_SELF,&buf)==-1)
        cout<<"GETRUSAGE FAILURE!"<<endl;
    start=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
    string file_name,new_name,abc_name;
#if TEST == 0
//    file_name="A1.aig";;
//    file_name="exemploBrunno.aig";
    file_name="inputAigsV2/out_forest_numTrees_75_maxDepth_13_bitsPrecision_5_nosynth.aig";
#elif TEST == 1
    file_name="exemploBrunno.aig";;
#endif
//    file_name="ABC_andre_NO_CONSTANTS_ANDs_removed_FIXED_.aig";
    ofstream dump_append("dumps/dump_append.txt"),exec_times("exec_times.csv"),script("abc_stuff/script.scr"),log("abc_stuff/log.txt"),abc_info("dumps/abc_info.txt"),nodes_file("Nodes_in_level.csv");
    log.close(); dump_append.close(); nodes_file.close();
    ifstream read_aig,read_mnist;
    read_aig.open(file_name.c_str(),ifstream::binary);
    binaryDS mnist_obj;
    aigraph graph_obj;
    int option,alpha=2,LEAVE_CONSTANTS=0;
    float min_th=0.9999;
//    stringstream ss(argv[1]);  ss>>option;
    option=RUN_OPTION;
    cout<<"Setting option to:"<<option<<endl;
          
    //1->linear, 2->root, 3->exp, 4->sigmod, 51->#nodes_linear, 52->#nodes_root,53->#nodes_exp
#if APPLY_MNIST == 1
    exec_times<<"Otption:"<<option<<", Circuit:"<<file_name<<endl<<"Min_th, Set Constants, Train Set ABC, Test Set ABC, My Simplification, Train Set, Test Set"<<endl;
#else
    exec_times<<"Otption:"<<option<<", Circuit:"<<file_name<<endl<<"Min_th, Set Constants, My Simplification"<<endl;
#endif
    
#if EXECUTE_ONCE ==1
//     min_th=0.999999;
    min_th=0.55;
//    for(min_th;min_th>0.89;min_th-=0.02)
//    {
    ///////////////////////////////Generating file WITH CONSTANTS to go trhough ABC/////////////////////////////////////////////////
            cout<<"//////////////////////////"<<endl<<"/////////"<<min_th<<"///////////"<<endl<<"//////////////////////////"<<endl;
            mnist_obj.clearMnist();
            read_mnist.open("mnist/train-images.idx3-ubyte",ifstream::binary);
            mnist_obj.readIdx(read_mnist,"mnist/train-images.idx3-ubyte");
            mnist_obj.setPIsBitsProbabilities(read_mnist);
            read_mnist.close();

            graph_obj.clearCircuit(); read_aig.close(); read_aig.open(file_name.c_str(),ifstream::binary);
            graph_obj.setThrehsold(min_th);        
            graph_obj.readAIG(read_aig,file_name);
            cout<<"Starting evaluation!"<<endl;
            graph_obj.evaluateScorseAbcCommLine21(4,4);
            cout<<"-----------SCORE BEFORE:"<<graph_obj.getScore()<<endl;
            LEAVE_CONSTANTS=1;  
            getrusage(RUSAGE_SELF,&buf); start_simplf=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            if(option>=0) 
                graph_obj.propagateAndDeleteAll(mnist_obj,option,min_th,alpha,LEAVE_CONSTANTS);
            else if(option==-1) 
                graph_obj.propagateAndDeletePIBased(mnist_obj,min_th,LEAVE_CONSTANTS);
            getrusage(RUSAGE_SELF,&buf); stop_simplf=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            exec_times<<min_th<<","<<((stop_simplf-start_simplf)/(float)3600)<<",";
            graph_obj.evaluateScorseAbcCommLine21(4,4);
            cout<<"-----------SCORE AFTER:"<<graph_obj.getScore()<<endl;

            graph_obj.writeAIG();

            new_name=graph_obj.getName();
            abc_name="ABC_"+new_name+".aig";
            abcWrite(new_name,abc_name);  

            graph_obj.clearCircuit(); read_aig.close(); read_aig.open(abc_name.c_str(),ifstream::binary);
            graph_obj.setThrehsold(min_th);        
            graph_obj.readAIG(read_aig,abc_name); graph_obj.setDepthsInToOut();
            abc_info<<graph_obj.getName()<<","<<min_th<<",option:"<<option<<","<<graph_obj.getDepth()<<","<<graph_obj.getANDS()->size()<<endl;
//#if APPLY_MNIST >0    
//            graph_obj.clearCircuit(); read_aig.close(); read_aig.open(abc_name.c_str(),ifstream::binary);
//            graph_obj.setThrehsold(min_th);        
//            graph_obj.readAIG(read_aig,abc_name);
//#endif
//#if APPLY_MNIST == 2
//            cout<<"APPLYING TRAIN MNIST WITH ABC's SIMPLIFICATION -> "<<endl;
//            getrusage(RUSAGE_SELF,&buf);  start_app=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
//            graph_obj.applyMnistRecursive(mnist_obj);
//            getrusage(RUSAGE_SELF,&buf);  stop_app=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
//            exec_times<<((stop_app-start_app)/(float)3600)<<","; cout<<"TRAIN DONE, ";
//#endif 
//#if APPLY_MNIST >= 1
//            cout<<"APPLYING TEST MNIST WITH ABC's SIMPLIFICATION -> ";
//            mnist_obj.clearMnist();
//            read_mnist.open("mnist/t10k-images.idx3-ubyte",ifstream::binary);
//            mnist_obj.readIdx(read_mnist,"mnist/t10k-images.idx3-ubyte");
//            mnist_obj.setBitsProbabilities(read_mnist);
//            read_mnist.close();
//            getrusage(RUSAGE_SELF,&buf); start_app=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
//            graph_obj.applyMnistRecursive(mnist_obj);
//            getrusage(RUSAGE_SELF,&buf); stop_app=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
//            exec_times<<((stop_app-start_app)/(float)3600)<<","; cout<<"TEST DONE, "<<endl;
//#endif
//    #if CEC == 1
//    /////////////////////////////Generating file with my own simplification/////////////////////////////////////////////////
//            mnist_obj.clearMnist();
//            read_mnist.open("mnist/train-images.idx3-ubyte",ifstream::binary);
//            mnist_obj.readIdx(read_mnist,"mnist/train-images.idx3-ubyte");
//            mnist_obj.setBitsProbabilities(read_mnist);
//            read_mnist.close();
//
//            graph_obj.clearCircuit(); read_aig.close(); read_aig.open(file_name.c_str(),ifstream::binary);
//            graph_obj.setThrehsold(min_th);
//            graph_obj.readAIG(read_aig,file_name);
//
//            LEAVE_CONSTANTS=0;
//            getrusage(RUSAGE_SELF,&buf); start_simplf=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
//            if(option>=0) graph_obj.propagateAndDeleteAll(mnist_obj,option,min_th,alpha,LEAVE_CONSTANTS);
//            else if(option==-1) graph_obj.propagateAndDeletePIBased(mnist_obj,min_th,LEAVE_CONSTANTS);
//            getrusage(RUSAGE_SELF,&buf); stop_simplf=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
//            exec_times<<((stop_simplf-start_simplf)/(float)3600)<<",";
//            graph_obj.writeAIG();
//
//            new_name=graph_obj.getName(); 
//            abcCeC(new_name,abc_name,min_th,option);
//            ofstream csv_final; csv_final.open("todos_scores.csv",ios::app); csv_final<<endl;
//    #endif


#elif EXECUTE_ONCE == 0
    if(option>=0)
    {
        if(option==0)
            min_th=1.0;
        else
            min_th=0.9999;
        for(min_th;min_th>0.999;min_th-=0.0002)
        {
    ///////////////////////////////Generating file WITH CONSTANTS to go trhough ABC/////////////////////////////////////////////////
            cout<<"//////////////////////////"<<endl<<"/////////"<<min_th<<"///////////"<<endl<<"//////////////////////////"<<endl;
            mnist_obj.clearMnist();
            read_mnist.open("mnist/train-images.idx3-ubyte",ifstream::binary);
            mnist_obj.readIdx(read_mnist,"mnist/train-images.idx3-ubyte");
            mnist_obj.setPIsBitsProbabilities(read_mnist);
            read_mnist.close();

            graph_obj.clearCircuit(); read_aig.close(); read_aig.open(file_name.c_str(),ifstream::binary);
            graph_obj.setThrehsold(min_th);        
            graph_obj.readAIG(read_aig,file_name);

//            LEAVE_CONSTANTS=1;  
            getrusage(RUSAGE_SELF,&buf); start_simplf=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            if(option>=0) graph_obj.propagateAndDeleteAll(mnist_obj,option,min_th,alpha,LEAVE_CONSTANTS);
            else if(option==-1) graph_obj.propagateAndDeletePIBased(mnist_obj,min_th,LEAVE_CONSTANTS);
            getrusage(RUSAGE_SELF,&buf); stop_simplf=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            exec_times<<min_th<<","<<((stop_simplf-start_simplf)/(float)3600)<<",";
            graph_obj.writeAIG();

#if USE_ABC == 1
            new_name=graph_obj.getName();
            abc_name="ABC_"+new_name+".aig";
            abcWrite(new_name,abc_name);  

            graph_obj.clearCircuit(); read_aig.close(); read_aig.open(abc_name.c_str(),ifstream::binary);
            graph_obj.setThrehsold(min_th);        
            graph_obj.readAIG(read_aig,abc_name); graph_obj.setDepthsInToOut();
            abc_info<<graph_obj.getName()<<","<<min_th<<",option:"<<option<<","<<graph_obj.getDepth()<<","<<graph_obj.getANDS()->size()<<endl;
#endif
#if APPLY_MNIST >0    
            graph_obj.clearCircuit(); read_aig.close(); read_aig.open(abc_name.c_str(),ifstream::binary);
            graph_obj.setThrehsold(min_th);        
            graph_obj.readAIG(read_aig,abc_name);
#endif
#if APPLY_MNIST == 2
            cout<<"APPLYING TRAIN MNIST WITH ABC's SIMPLIFICATION -> "<<endl;
            getrusage(RUSAGE_SELF,&buf);  start_app=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            graph_obj.applyMnistRecursive(mnist_obj);
            getrusage(RUSAGE_SELF,&buf);  stop_app=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            exec_times<<((stop_app-start_app)/(float)3600)<<","; cout<<"TRAIN DONE, ";
#endif 
#if APPLY_MNIST >= 1
            cout<<"APPLYING TEST MNIST WITH ABC's SIMPLIFICATION -> ";
            mnist_obj.clearMnist();
            read_mnist.open("mnist/t10k-images.idx3-ubyte",ifstream::binary);
            mnist_obj.readIdx(read_mnist,"mnist/t10k-images.idx3-ubyte");
            mnist_obj.setPIsBitsProbabilities(read_mnist);
            read_mnist.close();
            getrusage(RUSAGE_SELF,&buf); start_app=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            graph_obj.applyMnistRecursive(mnist_obj);
            getrusage(RUSAGE_SELF,&buf); stop_app=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            exec_times<<((stop_app-start_app)/(float)3600)<<","; cout<<"TEST DONE, "<<endl;
#endif
    #if CEC == 1
    /////////////////////////////Generating file with my own simplification/////////////////////////////////////////////////
            mnist_obj.clearMnist();
            read_mnist.open("mnist/train-images.idx3-ubyte",ifstream::binary);
            mnist_obj.readIdx(read_mnist,"mnist/train-images.idx3-ubyte");
            mnist_obj.setPIsBitsProbabilities(read_mnist);
            read_mnist.close();

            graph_obj.clearCircuit(); read_aig.close(); read_aig.open(file_name.c_str(),ifstream::binary);
            graph_obj.setThrehsold(min_th);
            graph_obj.readAIG(read_aig,file_name);

            LEAVE_CONSTANTS=0;
            getrusage(RUSAGE_SELF,&buf); start_simplf=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            if(option>=0) graph_obj.propagateAndDeleteAll(mnist_obj,option,min_th,alpha,LEAVE_CONSTANTS);
            else if(option==-1) graph_obj.propagateAndDeletePIBased(mnist_obj,min_th,LEAVE_CONSTANTS);
            getrusage(RUSAGE_SELF,&buf); stop_simplf=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            exec_times<<((stop_simplf-start_simplf)/(float)3600)<<",";
            graph_obj.writeAIG();

            new_name=graph_obj.getName(); 
            abcCeC(new_name,abc_name,min_th,option);
            ofstream csv_final; csv_final.open("todos_scores.csv",ios::app); csv_final<<endl;
    #endif
            exec_times<<endl;
            iterations++;
        }

        for(min_th=0.999;min_th>0.99;min_th-=0.002)
        {
    ///////////////////////////////Generating file WITH CONSTANTS to go trhough ABC/////////////////////////////////////////////////
            cout<<"//////////////////////////"<<endl<<"/////////"<<min_th<<"///////////"<<endl<<"//////////////////////////"<<endl;
            mnist_obj.clearMnist();
            read_mnist.open("mnist/train-images.idx3-ubyte",ifstream::binary);
            mnist_obj.readIdx(read_mnist,"mnist/train-images.idx3-ubyte");
            mnist_obj.setPIsBitsProbabilities(read_mnist);
            read_mnist.close();

            graph_obj.clearCircuit(); read_aig.close(); read_aig.open(file_name.c_str(),ifstream::binary);
            graph_obj.setThrehsold(min_th);        
            graph_obj.readAIG(read_aig,file_name);

//            LEAVE_CONSTANTS=1;  
            getrusage(RUSAGE_SELF,&buf); start_simplf=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            if(option>=0) graph_obj.propagateAndDeleteAll(mnist_obj,option,min_th,alpha,LEAVE_CONSTANTS);
            else if(option==-1) graph_obj.propagateAndDeletePIBased(mnist_obj,min_th,LEAVE_CONSTANTS);
            getrusage(RUSAGE_SELF,&buf); stop_simplf=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            exec_times<<min_th<<","<<((stop_simplf-start_simplf)/(float)3600)<<",";
            graph_obj.writeAIG();

#if USE_ABC == 1
            new_name=graph_obj.getName();
            abc_name="ABC_"+new_name+".aig";
            abcWrite(new_name,abc_name);  

            graph_obj.clearCircuit(); read_aig.close(); read_aig.open(abc_name.c_str(),ifstream::binary);
            graph_obj.setThrehsold(min_th);        
            graph_obj.readAIG(read_aig,abc_name); graph_obj.setDepthsInToOut();
            abc_info<<graph_obj.getName()<<","<<min_th<<",option:"<<option<<","<<graph_obj.getDepth()<<","<<graph_obj.getANDS()->size()<<endl;
#endif
#if APPLY_MNIST >0    
            graph_obj.clearCircuit(); read_aig.close(); read_aig.open(abc_name.c_str(),ifstream::binary);
            graph_obj.setThrehsold(min_th);        
            graph_obj.readAIG(read_aig,abc_name);
#endif
#if APPLY_MNIST == 2
            cout<<"APPLYING TRAIN MNIST WITH ABC's SIMPLIFICATION -> "<<endl;
            getrusage(RUSAGE_SELF,&buf);  start_app=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            graph_obj.applyMnistRecursive(mnist_obj);
            getrusage(RUSAGE_SELF,&buf);  stop_app=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            exec_times<<((stop_app-start_app)/(float)3600)<<","; cout<<"TRAIN DONE, ";
#endif 
#if APPLY_MNIST >= 1
            cout<<"APPLYING TEST MNIST WITH ABC's SIMPLIFICATION -> ";
            mnist_obj.clearMnist();
            read_mnist.open("mnist/t10k-images.idx3-ubyte",ifstream::binary);
            mnist_obj.readIdx(read_mnist,"mnist/t10k-images.idx3-ubyte");
            mnist_obj.setPIsBitsProbabilities(read_mnist);
            read_mnist.close();
            getrusage(RUSAGE_SELF,&buf); start_app=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            graph_obj.applyMnistRecursive(mnist_obj);
            getrusage(RUSAGE_SELF,&buf); stop_app=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            exec_times<<((stop_app-start_app)/(float)3600)<<","; cout<<"TEST DONE, "<<endl;
#endif
#if CEC == 1
    /////////////////////////////Generating file with my own simplification/////////////////////////////////////////////////
            mnist_obj.clearMnist();
            read_mnist.open("mnist/train-images.idx3-ubyte",ifstream::binary);
            mnist_obj.readIdx(read_mnist,"mnist/train-images.idx3-ubyte");
            mnist_obj.setPIsBitsProbabilities(read_mnist);
            read_mnist.close();

            graph_obj.clearCircuit(); read_aig.close(); read_aig.open(file_name.c_str(),ifstream::binary);
            graph_obj.setThrehsold(min_th);
            graph_obj.readAIG(read_aig,file_name);

            LEAVE_CONSTANTS=0;
            getrusage(RUSAGE_SELF,&buf); start_simplf=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            if(option>=0) graph_obj.propagateAndDeleteAll(mnist_obj,option,min_th,alpha,LEAVE_CONSTANTS);
            else if(option==-1) graph_obj.propagateAndDeletePIBased(mnist_obj,min_th,LEAVE_CONSTANTS);
            getrusage(RUSAGE_SELF,&buf); stop_simplf=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            exec_times<<((stop_simplf-start_simplf)/(float)3600)<<",";
            graph_obj.writeAIG();

            new_name=graph_obj.getName(); 
            abcCeC(new_name,abc_name,min_th,option);
            ofstream csv_final; csv_final.open("todos_scores.csv",ios::app); csv_final<<endl;
    #endif
            exec_times<<endl;
            iterations++;
        }

        for(min_th=0.98;min_th>=0.9;min_th-=0.02)
        {
    ///////////////////////////////Generating file WITH CONSTANTS to go trhough ABC/////////////////////////////////////////////////
            cout<<"//////////////////////////"<<endl<<"/////////"<<min_th<<"///////////"<<endl<<"//////////////////////////"<<endl;
            mnist_obj.clearMnist();
            read_mnist.open("mnist/train-images.idx3-ubyte",ifstream::binary);
            mnist_obj.readIdx(read_mnist,"mnist/train-images.idx3-ubyte");
            mnist_obj.setPIsBitsProbabilities(read_mnist);
            read_mnist.close();

            graph_obj.clearCircuit(); read_aig.close(); read_aig.open(file_name.c_str(),ifstream::binary);
            graph_obj.setThrehsold(min_th);        
            graph_obj.readAIG(read_aig,file_name);

//            LEAVE_CONSTANTS=1;  
            getrusage(RUSAGE_SELF,&buf); start_simplf=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            if(option>=0) graph_obj.propagateAndDeleteAll(mnist_obj,option,min_th,alpha,LEAVE_CONSTANTS);
            else if(option==-1) graph_obj.propagateAndDeletePIBased(mnist_obj,min_th,LEAVE_CONSTANTS);
            getrusage(RUSAGE_SELF,&buf); stop_simplf=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            exec_times<<min_th<<","<<((stop_simplf-start_simplf)/(float)3600)<<",";
            graph_obj.writeAIG();

#if USE_ABC == 1
            new_name=graph_obj.getName();
            abc_name="ABC_"+new_name+".aig";
            abcWrite(new_name,abc_name);  

            graph_obj.clearCircuit(); read_aig.close(); read_aig.open(abc_name.c_str(),ifstream::binary);
            graph_obj.setThrehsold(min_th);        
            graph_obj.readAIG(read_aig,abc_name); graph_obj.setDepthsInToOut();
            abc_info<<graph_obj.getName()<<","<<min_th<<",option:"<<option<<","<<graph_obj.getDepth()<<","<<graph_obj.getANDS()->size()<<endl;
#endif
#if APPLY_MNIST >0    
            graph_obj.clearCircuit(); read_aig.close(); read_aig.open(abc_name.c_str(),ifstream::binary);
            graph_obj.setThrehsold(min_th);        
            graph_obj.readAIG(read_aig,abc_name);
#endif
#if APPLY_MNIST == 2
            cout<<"APPLYING TRAIN MNIST WITH ABC's SIMPLIFICATION -> "<<endl;
            getrusage(RUSAGE_SELF,&buf);  start_app=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            graph_obj.applyMnistRecursive(mnist_obj);
            getrusage(RUSAGE_SELF,&buf);  stop_app=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            exec_times<<((stop_app-start_app)/(float)3600)<<","; cout<<"TRAIN DONE, ";
#endif 
#if APPLY_MNIST >= 1
            cout<<"APPLYING TEST MNIST WITH ABC's SIMPLIFICATION -> ";
            mnist_obj.clearMnist();
            read_mnist.open("mnist/t10k-images.idx3-ubyte",ifstream::binary);
            mnist_obj.readIdx(read_mnist,"mnist/t10k-images.idx3-ubyte");
            mnist_obj.setPIsBitsProbabilities(read_mnist);
            read_mnist.close();
            getrusage(RUSAGE_SELF,&buf); start_app=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            graph_obj.applyMnistRecursive(mnist_obj);
            getrusage(RUSAGE_SELF,&buf); stop_app=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            exec_times<<((stop_app-start_app)/(float)3600)<<","; cout<<"TEST DONE, "<<endl;
#endif 
    #if CEC == 1
    /////////////////////////////Generating file with my own simplification/////////////////////////////////////////////////
            mnist_obj.clearMnist();
            read_mnist.open("mnist/train-images.idx3-ubyte",ifstream::binary);
            mnist_obj.readIdx(read_mnist,"mnist/train-images.idx3-ubyte");
            mnist_obj.setPIsBitsProbabilities(read_mnist);
            read_mnist.close();

            graph_obj.clearCircuit(); read_aig.close(); read_aig.open(file_name.c_str(),ifstream::binary);
            graph_obj.setThrehsold(min_th);
            graph_obj.readAIG(read_aig,file_name);

            LEAVE_CONSTANTS=0;
            getrusage(RUSAGE_SELF,&buf); start_simplf=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            if(option>=0) graph_obj.propagateAndDeleteAll(mnist_obj,option,min_th,alpha,LEAVE_CONSTANTS);
            else if(option==-1) graph_obj.propagateAndDeletePIBased(mnist_obj,min_th,LEAVE_CONSTANTS);
            getrusage(RUSAGE_SELF,&buf); stop_simplf=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            exec_times<<((stop_simplf-start_simplf)/(float)3600)<<",";
            graph_obj.writeAIG();

            new_name=graph_obj.getName(); 
            abcCeC(new_name,abc_name,min_th,option);
            ofstream csv_final; csv_final.open("todos_scores.csv",ios::app); csv_final<<endl;
    #endif
            exec_times<<endl;
            iterations++;
        }
    }
    else if (option==-1)
    {
        float higher_than=0.90, minus=0.02;
        min_th=1;
        for(min_th;min_th>=higher_than;min_th=min_th-minus)
        {
//            cout<<"minus:"<<minus<<endl;
//            cout<<min_th<<endl;
            
    ///////////////////////////////Generating file WITH CONSTANTS to go trhough ABC/////////////////////////////////////////////////
            cout<<"//////////////////////////"<<endl<<"/////////"<<min_th<<"///////////"<<endl<<"//////////////////////////"<<endl;
            mnist_obj.clearMnist();
            read_mnist.open("mnist/train-images.idx3-ubyte",ifstream::binary);
            mnist_obj.readIdx(read_mnist,"mnist/train-images.idx3-ubyte");
            mnist_obj.setPIsBitsProbabilities(read_mnist);
            read_mnist.close();

            graph_obj.clearCircuit(); read_aig.close(); read_aig.open(file_name.c_str(),ifstream::binary);
            graph_obj.setThrehsold(min_th);        
            graph_obj.readAIG(read_aig,file_name);

//            LEAVE_CONSTANTS=1;  
            getrusage(RUSAGE_SELF,&buf); start_simplf=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            if(option>=0) graph_obj.propagateAndDeleteAll(mnist_obj,option,min_th,alpha,LEAVE_CONSTANTS);
            else if(option==-1) graph_obj.propagateAndDeletePIBased(mnist_obj,min_th,LEAVE_CONSTANTS);
            getrusage(RUSAGE_SELF,&buf); stop_simplf=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            exec_times<<min_th<<","<<((stop_simplf-start_simplf)/(float)3600)<<",";
            graph_obj.writeAIG();

#if USE_ABC == 1
            new_name=graph_obj.getName();
            abc_name="ABC_"+new_name+".aig";
            abcWrite(new_name,abc_name);  

            graph_obj.clearCircuit(); read_aig.close(); read_aig.open(abc_name.c_str(),ifstream::binary);
            graph_obj.setThrehsold(min_th);        
            graph_obj.readAIG(read_aig,abc_name); graph_obj.setDepthsInToOut();
            abc_info<<graph_obj.getName()<<","<<min_th<<",option:"<<option<<","<<graph_obj.getDepth()<<","<<graph_obj.getANDS()->size()<<endl;
#endif
#if APPLY_MNIST >0    
            graph_obj.clearCircuit(); read_aig.close(); read_aig.open(abc_name.c_str(),ifstream::binary);
            graph_obj.setThrehsold(min_th);        
            graph_obj.readAIG(read_aig,abc_name);
#endif
#if APPLY_MNIST == 2
            cout<<"APPLYING TRAIN MNIST WITH ABC's SIMPLIFICATION -> "<<endl;
            getrusage(RUSAGE_SELF,&buf);  start_app=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            graph_obj.applyMnistRecursive(mnist_obj);
            getrusage(RUSAGE_SELF,&buf);  stop_app=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            exec_times<<((stop_app-start_app)/(float)3600)<<","; cout<<"TRAIN DONE, ";
#endif 
#if APPLY_MNIST >= 1
            cout<<"APPLYING TEST MNIST WITH ABC's SIMPLIFICATION -> ";
            mnist_obj.clearMnist();
            read_mnist.open("mnist/t10k-images.idx3-ubyte",ifstream::binary);
            mnist_obj.readIdx(read_mnist,"mnist/t10k-images.idx3-ubyte");
            mnist_obj.setPIsBitsProbabilities(read_mnist);
            read_mnist.close();
            getrusage(RUSAGE_SELF,&buf); start_app=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            graph_obj.applyMnistRecursive(mnist_obj);
            getrusage(RUSAGE_SELF,&buf); stop_app=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            exec_times<<((stop_app-start_app)/(float)3600)<<","; cout<<"TEST DONE, "<<endl;
#endif
    #if CEC == 1
    /////////////////////////////Generating file with my own simplification/////////////////////////////////////////////////
            mnist_obj.clearMnist();
            read_mnist.open("mnist/train-images.idx3-ubyte",ifstream::binary);
            mnist_obj.readIdx(read_mnist,"mnist/train-images.idx3-ubyte");
            mnist_obj.setPIsBitsProbabilities(read_mnist);
            read_mnist.close();

            graph_obj.clearCircuit(); read_aig.close(); read_aig.open(file_name.c_str(),ifstream::binary);
            graph_obj.setThrehsold(min_th);
            graph_obj.readAIG(read_aig,file_name);

            LEAVE_CONSTANTS=0;
            getrusage(RUSAGE_SELF,&buf); start_simplf=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            if(option>=0) graph_obj.propagateAndDeleteAll(mnist_obj,option,min_th,alpha,LEAVE_CONSTANTS);
            else if(option==-1) graph_obj.propagateAndDeletePIBased(mnist_obj,min_th,LEAVE_CONSTANTS);
            getrusage(RUSAGE_SELF,&buf); stop_simplf=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
            exec_times<<((stop_simplf-start_simplf)/(float)3600)<<",";
            graph_obj.writeAIG();

            new_name=graph_obj.getName(); 
            abcCeC(new_name,abc_name,min_th,option);
            ofstream csv_final; csv_final.open("todos_scores.csv",ios::app); csv_final<<endl;
    #endif
            exec_times<<endl;
            iterations++;
            if(min_th==higher_than)
               min_th=0.85;higher_than=0.55,minus=0.05;
            if(min_th==higher_than==0.55)
                break;
        }
    }
#endif
    ifstream check_log("abc_stuff/log.txt"); string line; int equivalent_count=0;
    while(getline(check_log,line))
    {
        if(line.find("Networks are equivalent")!=string::npos)
            equivalent_count++;
    }
    cout<<"AIGs processed:"<<iterations<<", CECs passed:"<<equivalent_count<<endl;
    
    if(getrusage(RUSAGE_SELF,&buf)==-1)
        cout<<"GETRUSAGE FAILURE!"<<endl;
    stop=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
    cout<<"Time for whole process:"<<((stop-start)/(float)3600)<<endl;
    return 0;
}

