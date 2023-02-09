#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <map>
#include <time.h>
#include <dirent.h>

using namespace std;

//Mapa
template<typename Map>
void printMap(Map& parametros){
    for(auto& p: parametros)
        count<<p.first<<":"<<p.second<<";"<<endl;
    cout<<endl;
}

//Estructura Partition
struct Partition{
    char part_status[1];
    char part_type[1];
    char part_fit[1];
    int part_start;
    int part_s;
    char part_name[16];
};
//Estructura MBR
struct MBR{
    int mbr_tamano;
    char mbr_fecha_creacion;
    int mbr_dsk_signature;
    char dsk_fit[1];
    Partition mbr_partition_1;
    Partition mbr_partition_2;
    Partition mbr_partition_3;
    Partition mbr_partition_4;
};
//Estructura EBR
struct EBR{
    char part_status;
    char part_fit;
    int part_start;
    int part_s;
    int part_next;
    int part_name[16];
};

//Prototipo de funciones
void menu();
void comandos(string entrada);

int main(){
    cout<<"\n\n";
    cout<<" # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #"<<endl;
    cout<<" # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #"<<endl;
    cout<<" # # #                                                                 # # #"<<endl;
    cout<<" # # #                                                                 # # #"<<endl;
    cout<<" # # #    S I S T E M A   D E   A R C H I V O S   E X T 2 / E X T 3    # # #"<<endl;
    cout<<" # # #                                                                 # # #"<<endl;
    cout<<" # # #                                                                 # # #"<<endl;
    cout<<" # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #"<<endl;
    cout<<" # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #"<<endl;
    cout<<"\n\n\n";
    menu();
    return 0;
}

//Función para ingresar comandos
void menu(){
    string entrada;
    cout<<"Ingrese comando:"<<endl;
    getline(cin,entrada);
    comandos(entrada);
    menu();
}

//Función para redireccionar comandos
void comandos(string entrada){
    string piv = "";
    string param = "";
    string valor = "";
    string comment  = "";
    int estado = 0;
    entrada += ' ';
    bool com = false;
}