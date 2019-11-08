 /*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: gudeh
 *
 * Created on 3 de Julho de 2019, 01:16
 */

#include "gde.h"
#include "mnist.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    struct rusage buf;
    int start,stop;
    if(getrusage(RUSAGE_SELF,&buf)==-1)
        cout<<"GETRUSAGE FAILURE!"<<endl;
    start=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
    string file_name;
    file_name="../A1.aig";
//    file_name="andre.aig";
    ofstream dump_append("dump_append.txt");
    dump_append.close();
    ifstream read,read_mnist;
    read.open(file_name.c_str(),ifstream::binary);
    mnist mnist_obj;
    graph graph_obj;
    int option=53,alpha=2;
    float min_th=0.9999;
    //1->linear, 2->sqrt, 3->exp, 4->sigmod, 51->#nodes_linear, 52->#nodes_sqrt,53->#nodes_exp
   
//    cout<<"threhsold:"<<min_th<<", 1-threshold:"<<(float)1-min_th<<endl;
//    cout<<"Calculating th_inverted:"<<endl;
//    float th_inverted=min_th;
//    int aux;
//    cout<<"th_inverted=min_th:"<<th_inverted<<endl;
//    aux=th_inverted*10000;
//    cout<<"aux=th_inverted*10000:"<<aux<<endl;
//    aux=10000-aux;
//    cout<<"aux=10000-aux:"<<aux<<endl;
//    th_inverted=(aux/(float)10000);
//    cout<<"th_inverted=aux/10000:"<<th_inverted<<endl;
//    
//    vector<float> new_ths(5479,0);
//    ofstream dump("dump.txt");
//    for(int k=0;k<3000;k++)
//        new_ths[k]=-(((0.1)*k)/3000)+1;
//    
//    for(int k=0;k<new_ths.size();k++)
//        dump<<k<<":"<<new_ths[k]<<endl;
//    unsigned long long int bits1,bits2,result;
//    unsigned int id1=UINT_MAX,id2=1024;
//    
//    bits2=(unsigned long long int)id2;
//    bits1=(unsigned long long int)id1;
//    bitset<64> x(bits1),y(bits2),z(id2);
//    
//    result=(bits1 << 32) | bits2;
//    cout<<result<<endl;
//    bitset<64> w(result);
//    cout<<w<<endl;
//     cout<<id1<<","<<(unsigned long long int)id1<<","<<bits1<<endl;
//    cout<<x<<","<<y<<","<<z<<endl;
    
//    vector<int> depth_counter<0,5479>;
//    
//    int biggest=3000;
//    
//    {
//        for(int k=0;k<new_ths.size();k++)
//            new_ths[k]=(th_inverted*(-pow((((float)depth_counter[k])/(biggest-1)),alpha)+((float)biggest/(biggest-1))))+min_th;
//    }


#if EXECUTE_ONCE ==1
    graph_obj.clearCircuit();
    graph_obj.setThrehsold(min_th);

    mnist_obj.clearMnist();
    read_mnist.close();
    read_mnist.open("../train-images.idx3-ubyte",ifstream::binary);
    mnist_obj.readIdx(read_mnist,"../train-images.idx3-ubyte");
    mnist_obj.setBitsProbabilities(read_mnist);
    
    graph_obj.readAIG(read,file_name);;
//    graph_obj.printCircuit();
//    graph_obj.readAAG(read,file_name);
    
//    graph_obj.setName("andre_PI_Fix");
//    graph_obj.writeAIG();;
    
    
    graph_obj.propagateAndDeleteAll(mnist_obj,option,min_th,alpha);;
//    graph_obj.propagateAndDeletePIBased(mnist_obj);
//    graph_obj.setDepthsInToOut();

#if ONLY_REDUCE == 0
    graph_obj.applyMnistRecursive(mnist_obj);



    mnist_obj.clearMnist();
    read_mnist.close();
    read_mnist.open("../t10k-images.idx3-ubyte",ifstream::binary);
    mnist_obj.readIdx(read_mnist,"../t10k-images.idx3-ubyte");
    mnist_obj.setBitsProbabilities(read_mnist);
    graph_obj.applyMnistRecursive(mnist_obj);
#endif

#else

    for(min_th=0.9999;min_th>0.999;min_th-=0.0001)
    {
      
        graph_obj.clearCircuit();
        graph_obj.setThrehsold(min_th);
        
        mnist_obj.clearMnist();
        read_mnist.close();
        read_mnist.open("../train-images.idx3-ubyte",ifstream::binary);
        mnist_obj.readIdx(read_mnist,"../train-images.idx3-ubyte");
        mnist_obj.setBitsProbabilities(read_mnist);
        
        graph_obj.readAIG(read,file_name);

//        graph_obj.setANDsProbabilities(mnist_obj);
        graph_obj.propagateAndDeleteAll(mnist_obj,option,min_th,alpha);
//        graph_obj.propagateAndDeletePIBased(mnist_obj);
//        graph_obj.setDepthsInToOut();
#if ONLY_REDUCE == 0
        graph_obj.applyMnistRecursive(mnist_obj);

        
        mnist_obj.clearMnist();
        read_mnist.close();
        read_mnist.open("../t10k-images.idx3-ubyte",ifstream::binary);
        mnist_obj.readIdx(read_mnist,"../t10k-images.idx3-ubyte");
        mnist_obj.setBitsProbabilities(read_mnist);
        graph_obj.applyMnistRecursive(mnist_obj);
#endif
    }


    for(min_th=0.999;min_th>0.99;min_th-=0.001)
    {
        graph_obj.clearCircuit();
        graph_obj.setThrehsold(min_th);
        
        mnist_obj.clearMnist();
        read_mnist.close();
        read_mnist.open("../train-images.idx3-ubyte",ifstream::binary);
        mnist_obj.readIdx(read_mnist,"../train-images.idx3-ubyte");
        mnist_obj.setBitsProbabilities(read_mnist);
        
        graph_obj.readAIG(read,file_name);
        
//        graph_obj.setANDsProbabilities(mnist_obj);
        graph_obj.propagateAndDeleteAll(mnist_obj,option,min_th,alpha);
//        graph_obj.propagateAndDeletePIBased(mnist_obj);
//        graph_obj.setDepthsInToOut();
#if ONLY_REDUCE == 0
        graph_obj.applyMnistRecursive(mnist_obj);

        
        mnist_obj.clearMnist();
        read_mnist.close();
        read_mnist.open("../t10k-images.idx3-ubyte",ifstream::binary);
        mnist_obj.readIdx(read_mnist,"../t10k-images.idx3-ubyte");
        mnist_obj.setBitsProbabilities(read_mnist);
        graph_obj.applyMnistRecursive(mnist_obj);
#endif
    }
    
    for(min_th=0.99;min_th>0.9;min_th-=0.01)
    {
        graph_obj.clearCircuit();
        graph_obj.setThrehsold(min_th);
        
        mnist_obj.clearMnist();
        read_mnist.close();
        read_mnist.open("../train-images.idx3-ubyte",ifstream::binary);
        mnist_obj.readIdx(read_mnist,"../train-images.idx3-ubyte");
        mnist_obj.setBitsProbabilities(read_mnist);
        
        graph_obj.readAIG(read,file_name);
        
//        graph_obj.setANDsProbabilities(mnist_obj);
        graph_obj.propagateAndDeleteAll(mnist_obj,option,min_th,alpha);
//        graph_obj.propagateAndDeletePIBased(mnist_obj);
//        graph_obj.setDepthsInToOut();
#if ONLY_REDUCE == 0
        graph_obj.applyMnistRecursive(mnist_obj);

        
        mnist_obj.clearMnist();
        read_mnist.close();
        read_mnist.open("../t10k-images.idx3-ubyte",ifstream::binary);
        mnist_obj.readIdx(read_mnist,"../t10k-images.idx3-ubyte");
        mnist_obj.setBitsProbabilities(read_mnist);
        graph_obj.applyMnistRecursive(mnist_obj);
#endif
    }
#endif
    
    if(getrusage(RUSAGE_SELF,&buf)==-1)
        cout<<"GETRUSAGE FAILURE!"<<endl;
    stop=buf.ru_stime.tv_sec+buf.ru_utime.tv_sec;
    cout<<"Time for whole process:"<<stop-start<<endl;
    return 0;
}

