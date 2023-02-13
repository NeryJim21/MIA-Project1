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
        cout<<p.first<<":"<<p.second<<";"<<endl;
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
string mayusculas(string entrada);
template<typename Map>
void ejecutar(string entrada, Map& parametros);
template<typename Map>
void commandExecute(Map& parametros);
template<typename Map>
void commandMkdisk(Map& parametros);
bool carpeta(string pathC);
template<typename Map>
void commandRmdisk(Map& parametros);
template<typename Map>
void commandFdisk(Map& parametros);


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
    bool recursive = true;
    while(recursive){
        string entrada;
        cout<<"Ingrese comando:"<<endl;
        getline(cin,entrada);
        comandos(entrada);
    }
    //menu();
}

//Función para redireccionar comandos
void comandos(string entrada){
    string piv = "";
    string param = "";
    string valor = "";
    string comment  = "";
    int estado = 0;
    entrada += ' ';
    //bool com = false;
    map<string, string> parametros; //Mapa para almacenar parametros y su valor

    //Recirre entrada y separa los comandos
    for(int i = 0; i<entrada.length();i++){
        if(estado == 0){ //Comando
            if(entrada[i] != ' ' && entrada[i] != '#'){
                piv += entrada[i];
            }
            else if(entrada[i] == ' '){
                estado = 1;
            }
            else if(entrada[i] == '#'){
                //com = true;
                estado = 6;
            }
        }
        if(estado == 1){ //Mayor
            if(entrada[i] == '>'){
                estado = 2;
                continue;
            }
            if(entrada[i] == '#'){
                estado = 6;
            }
            if(entrada[i] == ' '){
                continue;
            }
        }
        if(estado == 2){ //Parametro
            if(entrada[i] != '=' && entrada[i] != ' '){
                param += entrada[i];
            }
            if(entrada[i] == ' '){ //Parametro sin valores
                parametros[param] = "vacio";
                param = "";
                estado = 1;
                continue;
            }
            if(entrada[i] == '='){ //Salta
                estado = 3;
                continue;
            }
        }
        if(estado == 3){ //Valor parametro
            if(entrada[i] == '\"'){ //comienza cadena con comilla
                estado = 4;
                continue;
            }
            else{
                estado = 5;
            }
        }
        if(estado == 4){ //Cadena con espacios
            if(entrada[i] != '\"'){  //concatena cadena con espacios
                valor += entrada[i];
            }else{
                estado = 5;
                continue;
            }
        }
        if(estado == 5){
            if(entrada[i] == ' '){
                parametros[param]= valor;
                param  = "";
                valor = "";
                estado = 1;
            }
            if(entrada[i] != ' '){
                valor += entrada[i];
            }
        }
        if(estado == 6){
            comment += entrada[i];
        }
    }
    //Imprime mapa
    printMap(parametros);

    //Ejecuta el comando reconocido
    ejecutar(piv,parametros);
}

//Selecciona comando a ejecutarse
template<typename Map>
void ejecutar(string entrada, Map& parametros){
    //Pasa el comando a mayusculas
    entrada = mayusculas(entrada);

    //Opciones de comandos
    if(entrada == "MKDISK"){
        cout<<"MKDISK"<<endl;
        //commandMkdisk(parametros);
    }else if(entrada == "RMDISK"){
        //commandRmdisk(parametros);
    }else if(entrada == "FDISK"){
        cout<<"FDISK"<<endl;
    }else if(entrada == "MOUNT"){
        cout<<"MOUNT"<<endl;
    }else if(entrada == "UNMOUNT"){
        cout<<"UNMOUNT"<<endl;
    }else if(entrada == "MKFS"){
        cout<<"MKFS"<<endl;
    }else if(entrada == "LOGIN"){
        cout<<"LOGIN"<<endl;
    }else if(entrada == "LOGOUT"){
        cout<<"LOGOUT"<<endl;
    }else if(entrada == "MKGRP"){
        cout<<"MKGRP"<<endl;
    }else if(entrada == "RMGRP"){
        cout<<"RMGRP"<<endl;
    }else if(entrada == "MKUSR"){
        cout<<"MKUSR"<<endl;
    }else if(entrada == "RMUSR"){
        cout<<"RMUSR"<<endl;
    }else if(entrada == "CHMOD"){
        cout<<"CHMOD"<<endl;
    }else if(entrada == "MKFILE"){
        cout<<"MKFILE"<<endl;
    }else if(entrada == "CAT"){
        cout<<"CAT"<<endl;
    }else if(entrada == "REMOVE"){
        cout<<"REMOVE"<<endl;
    }else if(entrada == "EDIT"){
        cout<<"EDIT"<<endl;;
    }else if(entrada == "RENAME"){
        cout<<"RENAME"<<endl;
    }else if(entrada == "MKDIR"){
        cout<<"MKDIR"<<endl;
    }else if(entrada == "COPY"){
        cout<<"COPY"<<endl;
    }else if(entrada == "MOVE"){
        cout<<"MOVE"<<endl;
    }else if(entrada == "FIND"){
        cout<<"FIND"<<endl;
    }else if(entrada == "CHOWN"){
        cout<<"CHOWN"<<endl;
    }else if(entrada == "CHGRP"){
        cout<<"CHGRP"<<endl;
    }else if(entrada == "PAUSE"){
        cout<<"PAUSE"<<endl;
    }else if(entrada == "RECOVERY"){
        cout<<"RECOVERY"<<endl;
    }else if(entrada == "LOSS"){
        cout<<"LOSS"<<endl;
    }else if(entrada == "EXECUTE"){
        cout<<"EXECUTE"<<endl;
        commandExecute(parametros);
    }else if(entrada == "REP"){
        cout<<"REP"<<endl;
    }
}

//Convierte entrada a mayusculas
string mayusculas(string entrada){
    for(int i = 0; i<entrada.size();i++){
        entrada[i] = toupper(entrada[i]);
    }    
    return entrada;
}

//Comando para ejecutar script
template<typename Map>
void commandExecute(Map& parametros){
    string piv;
    //Valida que los parametros estén correctos
    for (auto& p: parametros){
        if(p.first == "path"){
            cout<<"Archivo ubicado "<<p.second<<endl;
            piv= p.second;
        }else{
            cout<<"parametro incorrecto"<<endl;
        }
    }
    //Variables para manejo del archivo
    char path[piv.length()+1];
    strcpy(path,piv.c_str());
    long medida;
    char *txt;
    
    FILE * script;
    script = fopen(path,"r");
    fseek(script,0,SEEK_END);
    medida = ftell(script);
    rewind(script);
    txt = (char*) malloc(sizeof(char)*medida);
    fread(txt,medida+1,1,script);
    fclose(script);

    //Mandar comandos al analizador
    string pivEntrada;
    int i = 0;

    while(i<=medida){
        if(txt[i] != '\n'){
            pivEntrada += txt[i];
        }
        if(txt[i] == '\n' && pivEntrada != ""){
            cout<<pivEntrada<<endl;
            comandos(pivEntrada);
            pivEntrada = "";
        }
        i++;
    }
}