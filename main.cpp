#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <map>
#include <time.h>
#include <dirent.h>
#include <sstream>
#include <math.h>
#include <stdio.h>

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
    char mbr_fecha_creacion[80];
    int mbr_dsk_signature;
    char dsk_fit[1];
    Partition mbr_partition_1;
    Partition mbr_partition_2;
    Partition mbr_partition_3;
    Partition mbr_partition_4;
};
//Estructura EBR
struct EBR{
    char part_status[1];
    char part_fit[1];
    int part_start;
    int part_s;
    int part_next;
    char part_name[16];
};

//Estructura particion montada
struct particionMontada{
    int numero;
    int estado=0;//0=vacía,1=montada
    int type=-1;//2=ext2, 3=ext3
    string nombre;
    string path;
    string id;
};

//Estructura SuperBloque
struct SB{
    int s_filesystem_type;
    int s_inodes_count;
    int s_blocks_count;
    int s_free_blocks_count;
    int s_free_inodes_count;
    char s_mtime[80];
    char s_umtime[80];
    int s_mnt_count;
    int s_magic;
    int s_inode_s;
    int s_block_s;
    int s_first_ino;
    int s_first_blo;
    int s_bm_inode_start;
    int s_bm_block_start;
    int s_inode_start;
    int s_block_start;
};

//Estructura Inodos
struct Inodo{
    int i_uid;
    int i_gid;
    int i_s;
    char i_atime[80];
    char i_ctime[80];
    char i_mtime[80];
    int i_block[15];
    char i_type[1];
    int i_perm;
};

//Estructura contenido Bloque
struct Content{
    char b_name[12];
    int b_inodo;
};

//Estructura Bloque
struct Bloque{
    Content b_content[4];
};

//Estructura Bloque Archivo
struct Archivo{
    char b_content[64];
};

//Estructura Bloque Apuntadores
struct Apuntadores{
    int b_pointers[16];
};

//Estructura Journaling
struct Journaling{
    char j_accion[20];
    char j_nombre[20];
    char j_destino[20];
    char j_contenido[20];
    char j_fecha[80];
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
void makePrimary(string path,Partition ptn);
bool partitionFull(string path);
bool namePartition(string path, string name);
void makeExtended(string path, Partition ptn);
bool findingExtended(string path);
void makeLogic(string path, EBR ebr);
template<typename Map>
void commandMount(Map& parametros);
template<typename Map>
void commandUnmount(Map& parametros);
void commandPause();
int findinID(string id);
template<typename Map>
void commandMkfs(Map& parametros);
void mostrarMount();
template<typename Map>
void commandRep(Map& parametros);
void reporteMBR(string path, string ruta);
void reportEBR(EBR *ebr,FILE *myFile,int index);
EBR* leerEBR(int next,string path);
void reporteDisk(string path, string ruta);

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
        cout<<"-> ";
        getline(cin,entrada);
        if(strcmp(entrada.c_str(),"exit")==0){
            break;
        }else if(strcmp(entrada.c_str(),"cls")==0){
            system("clear");
            continue;
        }else if(strcmp(entrada.c_str(),"montadas")==0){
            mostrarMount();
            continue;
        }
        comandos(entrada);
    }
}

//Función para redireccionar comandos
void comandos(string entrada){
    string piv = "";
    string param = "";
    string valor = "";
    string comment  = "";
    int estado = 0;
    entrada += ' ';
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
        commandMkdisk(parametros);
    }else if(entrada == "RMDISK"){
        commandRmdisk(parametros);
    }else if(entrada == "FDISK"){
        cout<<"FDISK"<<endl;
        commandFdisk(parametros);
    }else if(entrada == "MOUNT"){
        cout<<"MOUNT"<<endl;
        commandMount(parametros);
    }else if(entrada == "UNMOUNT"){
        cout<<"UNMOUNT"<<endl;
        commandUnmount(parametros);
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
        commandPause();
    }else if(entrada == "RECOVERY"){
        cout<<"RECOVERY"<<endl;
    }else if(entrada == "LOSS"){
        cout<<"LOSS"<<endl;
    }else if(entrada == "EXECUTE"){
        cout<<"EXECUTE"<<endl;
        commandExecute(parametros);
    }else if(entrada == "REP"){
        cout<<"REP"<<endl;
        commandRep(parametros);
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

//Comando para crear discos binarios
template<typename Map>
void commandMkdisk(Map& parametros){
    //Variables para parametros
    string path, pivP,pivF,fit;
    char unit;
    int size;
    bool alerta, dk = false;
    MBR disco;

    //Valida que los parametros estén correctos
    for(auto& p: parametros){
        pivP = mayusculas(p.first);
        if(pivP == "PATH"){
            cout<<"Archivo ubicado: "<<p.second<<endl;
            path = p.second;
        }
        else if(pivP == "SIZE"){
            cout<<"Tamano disco: "<<p.second<<endl;
            size = stoi(p.second);
        }
        else if(pivP == "UNIT"){
            cout<<"Unidades: "<<p.second<<endl;
            pivF = mayusculas(p.second);
            unit = pivF[0];
        }
        else if(pivP == "FIT"){
            cout<<"Ajuste: "<<p.second<<endl;
            pivF = mayusculas(p.second);
            fit = pivF[0];
        }
        else{
            cout<<"parametro incorrecto"<<pivP<<endl;
            alerta = true;
        }
    }

    //Tomando hora del sistema
    time_t now = time(NULL);
    struct tm *ts;
    char buf[80];
    ts = localtime(&now);
    strftime(disco.mbr_fecha_creacion,sizeof(disco.mbr_fecha_creacion), "%a %Y-%m-%d %H:%M:%S %Z", ts);

    //Tamaño disco
    if(unit == 'K'){//Opcionales
        size = size *1024;
    }
    else if(unit == 'M' || unit == NULL){
        size = size *1024*1024;
    }
    else{
        cout<<"unidades erroneas"<<endl;
        alerta = true;
    }
    disco.mbr_tamano = size;
    disco.mbr_dsk_signature = rand(); //Numero random
    
    //Ajuste disco
    if(fit == ""){
        fit = "F";
    }
    if(fit != "W" && fit != "B" && fit != "F"){
        cout<<"Ajuste incorrecto"<<endl;
        alerta = true;
    }
    else{
        strcpy(disco.dsk_fit,fit.c_str());
    }
    
    //Inicializando particiones en -1
    string partStatus = "-";
    strcpy(disco.mbr_partition_1.part_status,partStatus.c_str());
    strcpy(disco.mbr_partition_2.part_status,partStatus.c_str());
    strcpy(disco.mbr_partition_3.part_status,partStatus.c_str());
    strcpy(disco.mbr_partition_4.part_status,partStatus.c_str());
    
    //Creando disco
    FILE * disk;
    if(disk = fopen(path.c_str(),"r")){
        fclose(disk);
    }
    else{
        if(path != "" && size > 0 && alerta != true){
            disk = fopen(path.c_str(),"wb");
            cout<<"Disco creado con exito"<<endl;
            cout<<"\n--------------------------- M B R -----------------------\n"<<endl;
            cout<<"Tamaño: "<<disco.mbr_tamano<<endl;
            cout<<"Fecha: "<<disco.mbr_fecha_creacion<<endl;
            cout<<"Ajuste: "<<disco.dsk_fit<<endl;
            cout<<"\n----------------------------------------------------------\n"<<endl;
            dk = true;
            if(disk == NULL){
                cout<<"Error en la creación de disco"<<endl;
            }
            fclose(disk);
        }
        else{
            if(alerta != true){
                cout<<"Parametros insuficientes"<<endl;
            }
        }
    }

    //Llenando disco
    if(dk != false){
        disk = fopen(path.c_str(),"a+");
        char buffer = '0';
        fwrite(&disco,sizeof(disco),1,disk);
        for(int i = 0; i<disco.mbr_tamano;i++){
            fwrite(&buffer,sizeof(char),1,disk);
        }
        fclose(disk);
    }
}

//Comando RMDISK
template<typename Map>
void commandRmdisk(Map& parametros){
    string path,pivP;
    bool bandera = false;

    for (auto& p: parametros){
        pivP = mayusculas(p.first);
        if(pivP == "PATH"){
            cout<<"Archivo ubicado: "<<p.second<<endl;
            path= p.second;
        }else{
            cout<<"Parametros incorrectos"<<endl;
            bandera = true;
        }
    }

    if(path != ""){
        FILE *dsk = fopen(path.c_str(),"rb");
        if(dsk != NULL){
            char conf;
            int rm = 0;
            cout<<"¿Desea eliminar el disco?"<<endl;
            cout<<"S/N"<<endl;
            cin>>conf;

            if(conf == 'N' || conf == 'n'){
                bandera = true;
            }
            if(conf == 'S' || conf == 's'){
                rm = remove(path.c_str());
                cout<<"Borrando el disco"<<endl;
                bandera = false;
            }
            else{
                cout<<"Opción inválida"<<endl;
                bandera = true;
            }

            if(rm != 0 && bandera != false){
                cout<<"\n Error al borrar disco"<<endl;
            }
        }
        else{
            cout<<"Disco no encontrado"<<endl;
        }  
    }
    else{
        cout<<"Parametros incorrectos"<<endl;
    }
}

//Comando FDISK
template<typename Map>
void commandFdisk(Map& parametros){
    //Variables parametros
    string unit, path, tipo, fit, name, eliminaP, pivP, pivF;
    int size = 0, add = 0;
    bool alerta = false;

    //Obtiene valor de parámetros y verifica que sean los correspondientes al comando
    for(auto& p: parametros){
        pivP = mayusculas(p.first);
        if(pivP == "PATH"){
            cout<<"Archivo ubicado: "<<p.second<<endl;
            path = p.second;
        }
        else if(pivP == "SIZE"){
            cout<<"Tamano disco: "<<p.second<<endl;
            size = stoi(p.second);
        }
        else if(pivP == "UNIT"){
            cout<<"Unidades: "<<p.second<<endl;
            pivF = mayusculas(p.second);
            unit = pivF[0];
        }
        else if(pivP == "FIT"){
            cout<<"Ajuste: "<<p.second<<endl;
            pivF = mayusculas(p.second);
            fit = pivF[0];
        }
        else if(pivP == "TYPE"){
            cout<<"Tipo: "<<p.second<<endl;
            pivF = mayusculas(p.second);
            tipo = pivF[0];
        }
        else if(pivP == "DELETE"){
            cout<<"Eliminar partición:  "<<p.second<<endl;
            pivF = mayusculas(p.second);
            eliminaP = pivF; //FULL
        }
        else if(pivP == "NAME"){
            cout<<"Nombre de partición: "<<p.second<<endl;
            name = p.second;
        }
        else if(pivP == "ADD"){
            cout<<"Modificar tamaño: "<<p.second<<endl;
            add = stoi(p.second); //Pendiente valores negativos
        }
        else{
            cout<<"Parametro incorrecto"<<pivP<<endl;
            alerta = true;
        }
    }

    //Validamos que los valores de los parámetros sean correctos
    //Validamos que el size sea positivo y mayor a cero
    if(size<0){
        cout<<"Parametro SIZE debe ser mayor a cero"<<endl;
        alerta = true;
    }
    if(size == 0){
        cout<<"Size = 0"<<endl;
    }

    //Validamos el tamaño de la particion
    if(unit == "B"){
    }
    if(unit == "K" || unit == ""){//Opcionales
        size = size*1024;
    }
    else if(unit == "M"){
        size = size*1024*1024;
    }
    else{
        cout<<"Unidades erroneas"<<endl;
        alerta = true;
    }

    //Tipo partition
    if(tipo == ""){
        tipo = "P";
    }
    if(tipo != "P" && tipo != "L" && tipo != "E"){
        cout<<"Tipo de partición Erróneo"<<endl;
        alerta = true;
    }

    //Validamos que no exista partition extendida
    if(tipo == "E"){
        if(findingExtended(path) == true){
            alerta = true;
            cout<<"Ya existe una partición extendida..."<<endl;
        }
    }

    //Si viene type L, validamos que sí exista partición extendida
    if(tipo == "L"){
        if(findingExtended(path) != true){
            alerta = true;
            cout<<"No existe partición extendida para escribir particiones lógicas..."<<endl;
        }
    }

    //Ajuste partition
    if(fit == ""){
        fit = "W";
    }
    if(fit != "W" && fit != "B" && fit != "F"){
        cout<<"Ajuste incorrecto"<<endl;
        alerta = true;
    }

    //Verificamos que el disco a modificar exista
    FILE * disk;
    if(disk = fopen(path.c_str(),"r")){
        fclose(disk);
    }
    else{
        cout<<"Disco no existente"<<endl;
        alerta = true;
    }

    //Veliricamos si las particiones aún no están llenas
    if(partitionFull(path) == true){
        alerta = true;
        cout<<"Particiones llenas"<<endl;
    }

    //Verificamos que el nombre no esté repetido
    if(namePartition(path,name) == true){
        alerta = true;
        cout<<"El nombre:"<<name<<" ya ha sido ingresado a las particiones."<<endl;
    }

    //Creando partition
    if(alerta != true){ //No hay ningún tipo de error en parametros
        Partition ptn;  
        string status = "0";
        //Agregando valores a struct partition
        strcpy(ptn.part_status,status.c_str()); //-:libre, 0:ocupado, 1: montado
        strcpy(ptn.part_type,tipo.c_str());
        strcpy(ptn.part_fit,fit.c_str());
        strcpy(ptn.part_name,name.c_str());
        if(size >0){
            ptn.part_s = size;
        }
        //Enviamos la información hacia el tipo de particion
        if(tipo=="P"){
            makePrimary(path, ptn);
        }else if(tipo == "E"){
            makeExtended(path,ptn);
        } else if(tipo == "L"){
            EBR ebr;
            strcpy(ebr.part_status,status.c_str());
            strcpy(ebr.part_fit,fit.c_str());
            strcpy(ebr.part_name,name.c_str());
            if(size > 0){
                ebr.part_s = size;
            }
            ebr.part_next = -1;
            makeLogic(path, ebr);
        }

        
    }else{
        cout<<"Error: La partición no ha sido creada"<<endl;
    }
}

//Función para crear particiones primarias
void makePrimary(string path, Partition ptn){
    MBR dataMBR;
    int pivSize = 0;
    //Obteniendo tamaño del MBR
    pivSize = sizeof(MBR)+1;
    //Buscando partición libre
    FILE * disk;
    disk = fopen(path.c_str(),"rb+");
    if(disk == NULL){
        cout<<"Error al abrir el disco..."<<endl;
    }
    else{
        //Nos ponemos al principio del disco
        fseek(disk,0,SEEK_SET);
        fread(&dataMBR,sizeof(MBR),1,disk);
        //Buscando partición libre
        int partition = 0;
        if(dataMBR.mbr_partition_1.part_status[0] == '-'){
            partition = 1;
        }else if(dataMBR.mbr_partition_2.part_status[0] == '-'){
            partition = 2;
        }else if(dataMBR.mbr_partition_3.part_status[0] == '-'){
            partition = 3;
        }else if(dataMBR.mbr_partition_4.part_status[0] == '-'){
            partition = 4;
        }
        cout<<"Particion libre"<<partition<<endl;
        //Primera partición en el disco
        if(partition == 1){
            pivSize += ptn.part_s;
            ptn.part_start = pivSize;
            dataMBR.mbr_partition_1 = ptn;
        }else if(partition == 2){
            pivSize += dataMBR.mbr_partition_1.part_s;
            ptn.part_start = pivSize;
            dataMBR.mbr_partition_2 = ptn;
        }else if(partition == 3){
            pivSize += dataMBR.mbr_partition_1.part_s + dataMBR.mbr_partition_2.part_s;
            ptn.part_start = pivSize;
            dataMBR.mbr_partition_3 = ptn;
        }else if(partition == 4){
            pivSize += dataMBR.mbr_partition_1.part_s + dataMBR.mbr_partition_2.part_s + dataMBR.mbr_partition_3.part_s;
            ptn.part_start = pivSize;
            dataMBR.mbr_partition_4 = ptn;
        }
        //Actualizando MBR
        fseek(disk,0,SEEK_SET);
        fwrite(&dataMBR,sizeof(dataMBR),1,disk);
        fclose(disk);
        cout<<"MBR actualizado..."<<endl;
    }

}

//Función para validar si las particiones están llenas
bool partitionFull(string path){
    FILE * disk;
    if(disk = fopen(path.c_str(),"r")){
        MBR dataMbr;
        fseek(disk,0,SEEK_SET);
        fread(&dataMbr,sizeof(MBR),1,disk);
        if(dataMbr.mbr_partition_1.part_status[0] == '-' || dataMbr.mbr_partition_2.part_status[0] == '-'
        || dataMbr.mbr_partition_3.part_status[0] == '-' || dataMbr.mbr_partition_4.part_status[0] == '-'){
            fclose(disk);
            return false;
        }else{
            fclose(disk);
            return true;
        }
    }
}

//Función para validar que el nombre de partición no se repita
bool namePartition(string path, string name){
    FILE * disk;
    if(disk = fopen(path.c_str(),"r")){
        MBR dataMBR;
        fseek(disk,0,SEEK_SET);
        fread(&dataMBR,sizeof(MBR),1,disk);
        if(dataMBR.mbr_partition_1.part_name != name && dataMBR.mbr_partition_2.part_name != name
        && dataMBR.mbr_partition_3.part_name != name && dataMBR.mbr_partition_4.part_name != name){
            fclose(disk);
            return false;
        }else{
            fclose(disk);
            return true;
        }
    }
}

//Función para crear particiones extendidas
void makeExtended(string path, Partition ptn){
    MBR dataMBR;
    int pivSize = 0;
    int contPartition = 0;
    bool extendida = false;
    //Obteniendo tamaño del MBR
    pivSize = sizeof(MBR)+1;
    //Buscando partición libre
    FILE * disk;
    disk = fopen(path.c_str(),"rb+");
    if(disk == NULL){
        cout<<"Error al abrir el disco..."<<endl;
    }
    else{
        //Nos ponemos al principio del disco
        fseek(disk,0,SEEK_SET);
        fread(&dataMBR,sizeof(MBR),1,disk);
        //Buscando partición libre
        int partition = 0;
        if(dataMBR.mbr_partition_1.part_status[0] == '-'){
            partition = 1;
        }else if(dataMBR.mbr_partition_2.part_status[0] == '-'){
            partition = 2;
        }else if(dataMBR.mbr_partition_3.part_status[0] == '-'){
            partition = 3;
        }else if(dataMBR.mbr_partition_4.part_status[0] == '-'){
            partition = 4;
        }
        cout<<"Particion libre"<<partition<<endl;
        //Primera partición en el disco
        if(partition == 1){
            ptn.part_start = pivSize;
            dataMBR.mbr_partition_1 = ptn;
        }else if(partition == 2){
            pivSize += dataMBR.mbr_partition_1.part_s;
            ptn.part_start = pivSize;
            dataMBR.mbr_partition_2 = ptn;
        }else if(partition == 3){
            pivSize += dataMBR.mbr_partition_1.part_s + dataMBR.mbr_partition_2.part_s;
            ptn.part_start = pivSize;
            dataMBR.mbr_partition_3 = ptn;
        }else if(partition == 4){
            pivSize += dataMBR.mbr_partition_1.part_s + dataMBR.mbr_partition_2.part_s + dataMBR.mbr_partition_3.part_s;
            ptn.part_start = pivSize;
            dataMBR.mbr_partition_4 = ptn;
        }
        
        //Actualizando MBR
        fseek(disk,0,SEEK_SET);
        fwrite(&dataMBR,sizeof(dataMBR),1,disk);
        cout<<"MBR actualizado..."<<endl;
        
        //Creando EBR inicial
        EBR dataEBR;
        string statusEbr = "-";
        string fit = ptn.part_fit;
        int size = ptn.part_s;
        string name = ptn.part_name;
        strcpy(dataEBR.part_status,statusEbr.c_str());
        strcpy(dataEBR.part_name,name.c_str());
        strcpy(dataEBR.part_fit,fit.c_str());
        dataEBR.part_start = pivSize;
        dataEBR.part_s = size;
        dataEBR.part_next = -1;
        
        //Escribiendo EBR en el disco
        pivSize ++;
        fseek(disk,pivSize,SEEK_SET);
        fwrite(&dataEBR,sizeof(dataEBR),1,disk);
        cout<<"EBR actualizado..."<<endl;
        fclose(disk);

    }
}

//Función para validar si existe partición extendida
bool findingExtended(string path){
    FILE * disk;
    if(disk = fopen(path.c_str(),"r")){
        MBR dataMBR;
        fseek(disk,0,SEEK_SET);
        fread(&dataMBR,sizeof(MBR),1,disk);
        if(dataMBR.mbr_partition_1.part_type[0] != 'E' && dataMBR.mbr_partition_2.part_type[0] != 'E'
        && dataMBR.mbr_partition_3.part_type[0] != 'E' && dataMBR.mbr_partition_4.part_type[0] != 'E'){
            fclose(disk);
            return false;
        }else{
            fclose(disk);
            return true;
        }
    }
}

//Función para crear particiones lógicas
void makeLogic(string path, EBR ebr){
    //Leer tamaño de partición extendida
    FILE * disk;
    if(disk = fopen(path.c_str(), "rb+")){
        MBR dataMBR;
        int sizeExtend = 0;
        int pivStart = 0, start = 0;
        fseek(disk,0,SEEK_SET);
        fread(&dataMBR,sizeof(MBR),1,disk);
        if(dataMBR.mbr_partition_1.part_type[0] == 'E'){
            sizeExtend = dataMBR.mbr_partition_1.part_s;
            pivStart = sizeof(MBR)+1;
        }else if(dataMBR.mbr_partition_2.part_type[0] == 'E'){
            sizeExtend = dataMBR.mbr_partition_2.part_s;
            pivStart = dataMBR.mbr_partition_2.part_start;
        }else if(dataMBR.mbr_partition_3.part_type[0] == 'E'){
            sizeExtend = dataMBR.mbr_partition_3.part_s;
            pivStart = dataMBR.mbr_partition_3.part_start;
        }else if(dataMBR.mbr_partition_4.part_type[0] == 'E'){
            sizeExtend = dataMBR.mbr_partition_4.part_s;
            pivStart = dataMBR.mbr_partition_4.part_start;
        }
        //Buscando EBR inicial
        int acumulado =  0;
        bool alerta = false;
        EBR *dataEBR = leerEBR(pivStart+1,path);
        while(dataEBR->part_next  != -1){
            //Siguiente EBR
            acumulado = dataEBR->part_s;
            dataEBR = leerEBR(dataEBR->part_next,path);
        }
        //Actualizando EBR y creando uno nuevo
        start = dataEBR->part_start+dataEBR->part_s+sizeof(EBR);
        //Validando que la partición quepa en la extendida
        if(dataEBR->part_s <= (sizeExtend-acumulado)){
            dataEBR->part_next = start;
            fseek(disk,start,SEEK_SET);
            fwrite(dataEBR,sizeof(dataEBR),1,disk);
            //break;
        }else{
            cout<<"No hay espacio suficiente para crear la partición lógica..."<<endl;
            alerta = true;
        }
        //Escribiendo nueva partición logica
        if(alerta != true){
            char pivFit = ebr.part_fit[0];
            char pivStatus = ebr.part_status[0];
            string pivName = ebr.part_name;
            int pivS = ebr.part_s;
            dataEBR->part_s = pivS;
            dataEBR->part_start = start;
            dataEBR->part_fit[0] = pivFit;
            dataEBR->part_status[0] = pivStatus;
            strcpy(dataEBR->part_name,pivName.c_str());
            dataEBR->part_next = -1;
            fseek(disk,start,SEEK_SET);
            fwrite(dataEBR,sizeof(dataEBR),1,disk);
            cout<<"Partición lógica creada..."<<endl;
            cout<<dataEBR->part_name<<endl;
        }
        fclose(disk);
    }
}

//Comando Mount
particionMontada montada[10]; //Array para los discos montados
template <typename Map>
void commandMount(Map& parametros){
    //Variables para parametros
    string path, name, pivP, nameD;
    string id = "81";
    int partition = 0;
    bool alerta = false;

    //Valida que los parametros estén correctos
    for(auto& p: parametros){
        pivP = mayusculas(p.first);
        if(pivP == "PATH"){
            cout<<"Archivo ubicado: "<<p.second<<endl;
            path = p.second;
        }else if(pivP == "NAME"){
            cout<<"Nombre de partición: "<<p.second<<endl;
            name = p.second;
        }else{
            cout<<"Parametro incorrecto"<<pivP<<endl;
            alerta = true;
        }
    }

    //Validando que el disco exista
    MBR dataMBR;
    FILE * disk;
    if(disk = fopen(path.c_str(),"rb")){
        fseek(disk,0,SEEK_SET);
        fread(&dataMBR,sizeof(MBR),1,disk);
        //Buscaando nombre de partición
        if(dataMBR.mbr_partition_1.part_name == name){
            partition = 1;
        }else if(dataMBR.mbr_partition_2.part_name == name){
            partition = 2;
        }else if(dataMBR.mbr_partition_3.part_name == name){
            partition = 3;
        }else if(dataMBR.mbr_partition_4.part_name == name){
            partition = 4;
        }else{
            cout<<"No existe la partición llamada: "<<name<<endl;
            alerta = true;
        }
        fclose(disk);
    }else{
        cout<<"Disco no existente..."<<endl;
        alerta = true;
    }
    //Viendo si existe espacio para montar disco
    int i = 0;
    for(i = 0; i<10;i++){
        if(montada[i].estado==0/*||montada[i].path==path*/){
            break;
        }
    }
    if(i==10){
        cout<<"No hay espacio para montar otra partición"<<endl;
        alerta = true;
    }
    //Procedemos a montar la partición
    if(alerta != true){
        //Nombre del disco
        string lectura;
        stringstream input_stringstream(path);
        while(getline(input_stringstream, lectura, '/')){
            nameD = lectura;
        }
        for(int x = 0; x<4;x++){
            nameD.pop_back();
        }
        
        //ID para la partición
        id = id + to_string(partition) + nameD;
        cout<<"Montando la partición: "<<id<<endl;
        //Escribiendo struct
        montada[i].estado = 1;
        montada[i].path = path;
        montada[i].type = 2;
        montada[i].nombre = name;
        montada[i].numero = i+1;
        montada[i].id = id;

        mostrarMount();
    }
}

//Comando UnMount
template<typename Map>
void commandUnmount(Map& parametros){
    //Variables para parametros
    string id, pivP;
    int pivIndice = -1;
    bool alerta = false;;
    //Valida que los parametros estén correctos
    for(auto& p: parametros){
        pivP = mayusculas(p.first);
        if(pivP == "ID"){
            id = p.second;
        }else{
            cout<<"Parametro incorrecto..."<<pivP<<endl;
        }
    }

    if(id == ""){
        cout<<"Falta el valor del ID"<<endl;
        alerta = true;
    }

    //Buscando partición montada
    pivIndice = findinID(id);
    if(pivIndice == -1){
        alerta = true;
    }
    //Borrando datos de partición montada
    if(alerta != true){
        montada[pivIndice].estado = 0;
        montada[pivIndice].nombre = "";
        montada[pivIndice].numero = 0;
        montada[pivIndice].path = "";
        cout<<"Partición Desmontada: "<<id<<endl;
    }else{
        cout<<"Error, el ID: "<<id<<" no ha sido montado"<<endl;
    }
}

//Buscando partición montada
int findinID(string id){
    int i = 0, pivIndice = -1;
    for(i = 0; i<10;i++){
        if(montada[i].id == id){
            pivIndice = i;
            return pivIndice;
            break;
        }
    }
    if( i == 10 && pivIndice == -1){
        return -1;
    }
}
//Comando Pause
void commandPause(){
    system("read -r -p \"Presiona una tecla para continuar...\" key");
}

//Mostrando particiones montadas
void mostrarMount(){
    
    for(int i = 0; i<10;i++){
        bool isMount = false;
        if(montada[i].type != -1){
            cout<<"\n - - - - - - - - - - P A R T I C I O N E S  M O N T A D A S - - - - - - - - -"<<endl;
            cout<<"Nombre: "<<montada[i].nombre<<endl;
            cout<<"ID: "<<montada[i].id<<endl;
            cout<<"Tipo: "<<montada[i].type<<endl;
            cout<<"Status: "<<montada[i].estado<<endl;
            cout<<"Numero: "<<montada[i].numero<<endl;
            cout<<"Path: "<<montada[i].path<<endl;
            isMount = true;
        }
        if(i==10 && isMount == false){
            cout<<"Aún no hay particiones montadas..."<<endl;
        }
    }
}

template<typename Map>
void commandMkfs(Map& parametros){
    string pivP, id, type, fs;
    int pivID;
    bool alerta = false;
    //Validando parametros
    for(auto& p: parametros){
        pivP = mayusculas(p.first);
        if(pivP == "ID"){
            id = p.second;
        }else if(pivP == "TYPE"){
            type = mayusculas(p.second);
        }else if(pivP == "FS"){
            fs = p.second;
        }else{
            cout<<"Parametro incorrecto..."<<endl;
            alerta = true;
        }
    }

    //Verificando id
    pivID = findinID(id);
    if(pivID == -1){
        cout<<"El ID no ha sido montado..."<<endl;
        alerta = true;
    }

    //Verificando el tipo de formateo
    if(type != "" && type != "FULL"){
        cout<<"Tipo de formateo incorrecto..."<<endl;
        alerta = true;
    }

    //Verificando sistema de archivo
    if(fs == ""){
        fs = "2FS";
    }
    if(fs != "2FS" && fs != "3FS"){
        cout<<"Sistema de archivos incorrecto..."<<endl;
        alerta = true;
    }

    //Realizando formateo
    if(alerta != true){
        string path = montada[pivID].path;
        string nameP = montada[pivID].nombre;
        int inicio=0,fin=0;
        int pivTam=0;
        MBR dataMBR;
        //Abriendo MBR
        FILE * disk;
        if(disk = fopen(path.c_str(),"rb+")){
            //Nos posicionamos al inicio del disco
            fseek(disk,0,SEEK_SET);
            fread(&dataMBR,sizeof(MBR),1,disk);
            //Buscaando nombre de partición
            if(dataMBR.mbr_partition_1.part_name == nameP){
                inicio = dataMBR.mbr_partition_1.part_start;
                pivTam = dataMBR.mbr_partition_1.part_s;
                fin = inicio + pivTam;
            }else if(dataMBR.mbr_partition_2.part_name == nameP){
                inicio = dataMBR.mbr_partition_2.part_start;
                pivTam = dataMBR.mbr_partition_2.part_s;
                fin = inicio + pivTam;
            }else if(dataMBR.mbr_partition_3.part_name == nameP){
                inicio = dataMBR.mbr_partition_3.part_start;
                pivTam = dataMBR.mbr_partition_3.part_s;
                fin = inicio + pivTam;
            }else if(dataMBR.mbr_partition_4.part_name == nameP){
                inicio = dataMBR.mbr_partition_4.part_start;
                pivTam = dataMBR.mbr_partition_4.part_s;
                fin = inicio + pivTam;
            }
            //Escribiendo el formato
            double sizeN;
            int contInode,contBlock;
            SB dataSB;
            if(type == "2FS"){
                //tamaño_particion = sizeOf(superblock) + n + 3 * n + n * sizeOf(inodos) + 3 *
                //n * sizeOf(block)
                sizeN = (pivTam-sizeof(SB))/(4+sizeof(Inodo)+3*sizeof(Bloque));
                dataSB.s_filesystem_type=2; 
            }else if(type =="3FS"){
                sizeN = (pivTam-sizeof(SB))/(4+sizeof(Journaling)+sizeof(Inodo)+3*sizeof(Bloque));
                dataSB.s_filesystem_type=3;
            }
            contInode = floor(sizeN);
            contBlock = 3*contInode;
            //Escribiendo datos de Superbloque SB
            dataSB.s_inodes_count = contInode;
            dataSB.s_blocks_count = contBlock;
            dataSB.s_free_inodes_count=contInode;
            dataSB.s_free_blocks_count = contBlock;
            //Tomando hora del sistema
            time_t now = time(NULL);
            struct tm *ts;
            char buf[80];
            ts = localtime(&now);
            strftime(dataSB.s_mtime,sizeof(dataSB.s_mtime), "%a %Y-%m-%d %H:%M:%S %Z", ts);
            strftime(dataSB.s_umtime,sizeof(dataSB.s_umtime), "%a %Y-%m-%d %H:%M:%S %Z", ts);
            dataSB.s_mnt_count = 0;
            dataSB.s_magic = 0xEF53;
            dataSB.s_inode_s = sizeof(Inodo);
            dataSB.s_block_s = sizeof(Bloque);
            dataSB.s_first_ino = 0;
            dataSB.s_first_blo = 0;
            int piv = inicio + sizeof(SB);
            if(type == "3FS"){
                piv = piv+(sizeof(Journaling)*contInode);    
            }
            dataSB.s_bm_inode_start = piv;
            int piv2 = piv+contInode;
            dataSB.s_bm_block_start = piv2;
            int piv3 = piv2+contBlock;
            dataSB.s_inode_start = piv3;
            dataSB.s_block_start = piv3+sizeof(Inodo)*contInode;
            //Escribiendo datos en el disco
            fseek(disk,inicio,SEEK_SET);
            fwrite(&dataSB,sizeof(SB),1,disk);
            //Escribiendo bitmaps inodos
            int contador = 0;
            fseek(disk,dataSB.s_bm_inode_start,SEEK_SET);
            while(contador<contInode){
                fwrite("0",sizeof(char),1,disk);
                contador++;
            }
            //Escribiendo bitmap bloques
            contador = 0;
            fseek(disk,dataSB.s_bm_block_start,SEEK_SET);
            while(contador<contBlock){
                fwrite("0",sizeof(char),1,disk);
                contador++;
            }
            //Escribiendo carpeta raiz
            Inodo dataInodo;
            dataInodo.i_uid = 1;
            dataInodo.i_gid = 1;
            dataInodo.i_s = 0;
            time_t now1 = time(NULL);
            struct tm *ts1;
            char buf1[80];
            ts1 = localtime(&now1);
            strftime(dataInodo.i_atime,sizeof(dataInodo.i_atime), "%a %Y-%m-%d %H:%M:%S %Z", ts1);
            strftime(dataInodo.i_ctime,sizeof(dataInodo.i_ctime), "%a %Y-%m-%d %H:%M:%S %Z", ts1);
            strftime(dataInodo.i_mtime,sizeof(dataInodo.i_mtime), "%a %Y-%m-%d %H:%M:%S %Z", ts1);
            for(int i = 0; i<1;i++){
                dataInodo.i_block[i] = -1;
            }
            dataInodo.i_perm = 777;
            dataInodo.i_type = '0';
            int indexI = dataSB.s_first_ino;
            //Nuevo bloque de carpeta
            Bloque dataDir;
            for(int i = 0; i<4;i++){
                dataDir.b_content[i].b_inodo = -1;
            }
            dataDir.b_content[0].b_name = '/';
            dataDir.b_content[0].b_inodo = indexI;
            dataDir.b_content[1].b_name =  '/';
            dataDir.b_content[1].b_inodo = 0;
            //Asignando bloque a carpeta
            dataInodo.i_block[0] = dataSB.s_first_blo;

            //Escribiendo Inodo en el disco
            fseek(disk,dataSB.s_inode_start+(dataSB.s_inode_s*indexI),SEEK_SET);
            fwrite(&dataInodo,sizeof(Inodo),1,disk);

            //Escribiendo Bloque en el disco
            fseek(disk,dataSB.s_block_start+(dataSB.s_block_s*dataSB.s_first_blo),SEEK_SET);
            fwrite(&dataDir,sizeof(Bloque),1,disk);

            //Escribiendo datos en el disco
            fseek(disk,inicio,SEEK_SET);
            fwrite(&dataSB,sizeof(SB),1,disk);

            //Creando archivo de usuarios
            char *usuario =  "1,G,root\n,1,U,root,root,123\n";
            int indiceInodoPadre = 0;
            stringstream ss("/users.txt");
            string token,dirPad="/";
            int indiceBloqueActual = 0;
            while(getline(ss,token, '/')){
                if(token != ""){
                    if(ss.tellg() == -1){
                        //Nuevo bloque inodo
                        Inodo dataInodo2;
                        dataInodo2.i_uid = 1;
                        dataInodo2.i_gid = 1;
                        dataInodo2.i_s = 28;
                        time_t now2 = time(NULL);
                        struct tm *ts2;
                        char buf2[80];
                        ts2 = localtime(&now2);
                        strftime(dataInodo2.i_atime,sizeof(dataInodo2.i_atime), "%a %Y-%m-%d %H:%M:%S %Z", ts2);
                        strftime(dataInodo2.i_ctime,sizeof(dataInodo2.i_ctime), "%a %Y-%m-%d %H:%M:%S %Z", ts2);
                        strftime(dataInodo2.i_mtime,sizeof(dataInodo2.i_mtime), "%a %Y-%m-%d %H:%M:%S %Z", ts2);
                        for(int i = 0; i<1;i++){
                            dataInodo2.i_block[i] = -1;
                        }
                        dataInodo.i_perm = 664;
                        dataInodo.i_type = '1';
                        int indexI = dataSB.s_first_ino;
                        //Nuevo bloque de carpeta
                        Archivo dataDir1;
                        for(int i = 0; i<64;i++){
                            dataDir1.b_content[i] = 0;
                        }
                        int indexofInodo = 0;
                        int contadorCaracteres = 0;
                        int indexCaracteres = 0;
                        bool keepSaving = true;
                        while(keepSaving){
                            if(contadorCaracteres >= 64 || indexCaracteres >= 28){
                                if(indexofInodo<12){
                                    int pivInitB = dataSB.s_first_blo;
                                    //dataDir1[indexofInodo] = pivInitB;
                                    pivInitB = pivInitB*dataSB.s_block_s+dataSB.s_block_start;
                                    fseek(disk,pivInitB,SEEK_SET);
                                    fwrite(&dataSB,sizeof(SB),1,disk);
                                    dataSB.s_first_blo = 1;
                                    dataSB.s_free_blocks_count--;
                                }
                            }
                        }
                    }
                }
            }
            
        }
    }
}

//Comando REP
template<typename Map>
void commandRep(Map& parametros){
    //Vaiables para parametros
    string name,path,id,ruta,pivP;
    bool alerta = false;
    //Valida que los parametros estén correctos
    for(auto& p: parametros){
        pivP = mayusculas(p.first);
        if(pivP == "PATH"){
            cout<<"Archivo ubicado: "<<p.second<<endl;
            path = p.second;
        }
        else if(pivP == "NAME"){
            cout<<"Nombre del reporte: "<<p.second<<endl;
            name = mayusculas(p.second);
        }
        else if(pivP == "ID"){
            cout<<"ID: "<<p.second<<endl;
            id = p.second;
        }
        else if(pivP == "RUTA"){
            cout<<"Ruta: "<<p.second<<endl;
            ruta = p.second;
        }
        else{
            cout<<"parametro incorrecto"<<pivP<<endl;
            alerta = true;
        }
    }

    int montID = 0;
    montID = findinID(id);
    if(montID == -1){
        cout<<"La partición: "<<id<<" No ha sido montada"<<endl;
        alerta = true;
    }else{
        ruta = montada[montID].path;
    }

    //Generando los reportes
    if(alerta != true){
        if(name == "MBR"){
            cout<<"Reporte MBR"<<endl;
            reporteMBR(ruta,path);
        }else if(name == "DISK"){
            cout<<"Reporte DISK"<<endl;
            reporteDisk(ruta,path);
        }else if(name == "INODE"){
            cout<<"Reporte INODE"<<endl;
        }else if(name == "JOURNALING"){
            cout<<"Reporte JOURNALING"<<endl;
        }else if(name == "BLOCK"){
            cout<<"Reporte BLOCK"<<endl;
        }else if(name == "BM_INODE"){
            cout<<"Reporte BM_INODE"<<endl;
        }else if(name == "BM_BLOCK"){
            cout<<"Reporte BM_BLOCK"<<endl;
        }else if(name == "TREE"){
            cout<<"Reporte TREE"<<endl;
        }else if(name == "SB"){
            cout<<"Reporte SB"<<endl;
        }else if(name == "FILE"){
            cout<<"Reporte FILE"<<endl;
        }else if(name == "LS"){
            cout<<"Reporte LS"<<endl;
        }else{
            cout<<"Nombre de reporte incorrecto..."<<endl;
        }
    }
}

void reporteMBR(string path, string ruta){
    MBR *dataMBR = (MBR*)malloc(sizeof(MBR));
    FILE *disk = fopen(path.c_str(),"rb+");
    bool alerta = false;
    if(disk != NULL){
        fseek(disk,0,SEEK_SET);
        fread(dataMBR,sizeof(MBR),1,disk);
        fclose(disk);
    }else{
        cout<<"Error al abrir disco"<<endl;
        alerta = true;
    }

    if(alerta != true){
        FILE *report;
        ruta += "reporte_mbr.dot";
        report = fopen(ruta.c_str(),"w+");
        if(report != NULL){
            fputs("digraph {\ntbl [\nshape=plaintext\n label=<\n", report);
            fputs("<table border='0' cellborder='1' cellspacing='0'>\n",report);
            fputs("<tr><td colspan=\"3\">Disk1</td></tr>\n",report);
            fputs("<th><td>Nombre</td><td>Valor</td></th>\n",report);
            //DATOS MBR
            //TAMAÑO
            fputs("<tr><td bgcolor=\"lightblue\">mbr_tamaño</td><td bgcolor=\"lightblue\">",report);
            fputs(&to_string(dataMBR->mbr_tamano)[0],report);
            fputs("</td></tr>\n",report);
            //FECHA
            fputs("<tr><td bgcolor=\"lightblue\">mbr_Fecha_creación</td><td bgcolor=\"lightblue\">",report);
            fputs(dataMBR->mbr_fecha_creacion,report);
            fputs("</td></tr>\n",report);
            //SIGNATURE
            fputs("<tr><td bgcolor=\"lightblue\">mbr_disk_signature</td><td bgcolor=\"lightblue\">",report);
            fprintf(report, "%d", dataMBR->mbr_dsk_signature);
            fputs("</td></tr>\n",report);
            //FIT
            fputs("<tr><td bgcolor=\"lightblue\">mbr_disk_fit</td><td bgcolor=\"lightblue\">",report);
            fprintf(report, "%c", dataMBR->dsk_fit[0]);
            fputs("</td></tr>\n",report);
            
            char colors[4][10];
            for(int i = 0; i<4;i++){
                for(int j = 0; j<10;j++){
                    colors[i][j] = 0;
                }
            }
            strcat(colors[0],"#bcf7c1");
            strcat(colors[1],"#f8fc92");
            strcat(colors[2],"#fcc292");
            strcat(colors[3],"#dfbcf7");
            Partition dataPart;

            for(int i = 0; i<4;i++){
                int partition = 0;
                if(dataMBR->mbr_partition_1.part_status[0] != '-'){
                    partition = 1;
                    dataPart = dataMBR->mbr_partition_1;
                }else if(dataMBR->mbr_partition_2.part_status[0] != '-'){
                    partition = 2;
                    dataPart = dataMBR->mbr_partition_2;
                }else if(dataMBR->mbr_partition_3.part_status[0] != '-'){
                    partition = 3;
                    dataPart = dataMBR->mbr_partition_3;
                }else if(dataMBR->mbr_partition_4.part_status[0] != '-'){
                    partition = 4;
                    dataPart = dataMBR->mbr_partition_4;
                }
                if(partition == i+1){
                    //PART NAME
                    fputs("<tr><td colspan=\"2\" bgcolor=\"",report);
                    fputs(colors[i],report);
                    fputs("\">",report);
                    fputs(dataPart.part_name,report);
                    fputs("</td></tr>\n",report);
                    //PART STATUS
                    fputs("<tr><td bgcolor=\"",report);
                    fputs(colors[i],report);
                    fputs("\">part_status</td><td bgcolor=\"",report);
                    fputs(colors[i],report);
                    fputs("\">",report);
                    fprintf(report, "%d", dataPart.part_status);
                    fputs("</td></tr>\n",report);
                    //PART TYPE
                    fputs("<tr><td bgcolor=\"",report);
                    fputs(colors[i],report);
                    fputs("\">part_type</td><td bgcolor=\"",report);
                    fputs(colors[i],report);
                    fputs("\">",report);
                    fprintf(report, "%c", dataPart.part_type[0]);
                    fputs("</td></tr>\n",report);
                    //PART FIT
                    fputs("<tr><td bgcolor=\"",report);
                    fputs(colors[i],report);
                    fputs("\">part_fit</td><td bgcolor=\"",report);
                    fputs(colors[i],report);
                    fputs("\">",report);
                    fprintf(report, "%c", dataPart.part_fit[0]);
                    fputs("</td></tr>\n",report);
                    //PART START
                    fputs("<tr><td bgcolor=\"",report);
                    fputs(colors[i],report);
                    fputs("\">part_start</td><td bgcolor=\"",report);
                    fputs(colors[i],report);
                    fputs("\">",report);
                    fprintf(report, "%d", dataPart.part_start);
                    fputs("</td></tr>\n",report);
                    //PART SIZE
                    fputs("<tr><td bgcolor=\"",report);
                    fputs(colors[i],report);
                    fputs("\">part_size</td><td bgcolor=\"",report);
                    fputs(colors[i],report);
                    fputs("\">",report);
                    fputs(&to_string(dataPart.part_s)[0],report);
                    fputs("</td></tr>\n",report);
                }
            }
            fputs("</table>\n",report);
            fputs(">];\n\n", report);

            //Reporte EBR
            int i = 0,pivP = 0;
            if(findingExtended(path)==true){
                int inicio=sizeof(MBR)+1;
                if(dataMBR->mbr_partition_1.part_type[0]=='E'){
                    inicio += 1;
                }else if(dataMBR->mbr_partition_2.part_type[0]=='E'){
                    inicio += dataMBR->mbr_partition_1.part_s+1;
                }else if(dataMBR->mbr_partition_3.part_type[0]=='E'){
                    inicio = dataMBR->mbr_partition_1.part_s+dataMBR->mbr_partition_2.part_s+1;
                }else if(dataMBR->mbr_partition_4.part_type[0]=='E'){
                    inicio = dataMBR->mbr_partition_1.part_s+dataMBR->mbr_partition_2.part_s+dataMBR->mbr_partition_3.part_s+1;
                }
                EBR *dataEBR =leerEBR(inicio,path);
                bool bandera = true;
                if(inicio != 0){
                    
                    while(bandera){
                        reportEBR(dataEBR,report,i);
                        if(dataEBR->part_next!=-1){
                            dataEBR = leerEBR(dataEBR->part_next,path);
                        }else{
                            bandera = false;
                        }
                        i++;
                    }
                    fputs("}\n",report);
                    //cerrando stream
                    fclose (report);
                    string pathString(ruta);
                    string command = "dot -Tpng report_mbr.dot -o \""+pathString+"\"";//+"/report_mbr.png";
                    system(command.c_str());
                    cout<<"Reporte de MBR creado...\n";
                    
                }
            }else{
                fputs("}\n",report);
                fclose(report);
            }
        }
    }
}

//Agrega reporte EBR
void reportEBR(EBR *ebr,FILE *myFile,int index){
    string nombreNodo = "tbl"+to_string(index+1);
    fputs(nombreNodo.c_str(),myFile);
    fputs(" [\nshape=plaintext\n label=<\n", myFile);
    fputs("<table border='0' cellborder='1' cellspacing='0'>\n",myFile);
    fputs("<tr><td colspan=\"3\">",myFile);
    fputs(ebr->part_name,myFile);
    fputs("</td></tr>\n",myFile);
    fputs("<th><td>Nombre</td><td>Valor</td></th>\n",myFile);
    //PART STATUS
    fputs("<tr><td bgcolor=\"#fcc8c8\">part_status</td><td bgcolor=\"#fcc8c8\">",myFile);
    fprintf(myFile, "%d", ebr->part_status);
    fputs("</td></tr>\n",myFile);
    //PART FIT
    fputs("<tr><td bgcolor=\"#fcc8c8\">part_fit</td><td bgcolor=\"#fcc8c8\">",myFile);
    fprintf(myFile, "%c", ebr->part_fit[0]);
    fputs("</td></tr>\n",myFile);
    //PART START
    fputs("<tr><td bgcolor=\"#fcc8c8\">part_start</td><td bgcolor=\"#fcc8c8\">",myFile);
    fprintf(myFile, "%d", ebr->part_start);
    fputs("</td></tr>\n",myFile);
    //PART SIZE
    fputs("<tr><td bgcolor=\"#fcc8c8\">part_size</td><td bgcolor=\"#fcc8c8\">",myFile);
    fputs(&to_string(ebr->part_s)[0],myFile);
    fputs("</td></tr>\n",myFile);
    //PART NEXT
    fputs("<tr><td bgcolor=\"#fcc8c8\">part_next</td><td bgcolor=\"#fcc8c8\">",myFile);
    fprintf(myFile, "%d", ebr->part_next);
    fputs("</td></tr>\n",myFile);
    fputs("</table>\n",myFile);
    fputs(">];\n", myFile);
}

//Lee EBR
EBR* leerEBR(int next,string path){
    FILE *myFile = fopen(path.c_str(),"rb+");
    if(myFile==NULL){
        cout<<"Error al abrir el archivo \n";
        return NULL;
    }
    EBR *ebr = (EBR*)malloc(sizeof(EBR));

    fseek(myFile, next, SEEK_SET);
    fread(ebr, sizeof(EBR), 1, myFile);
    fclose(myFile);
    return ebr;
}

void reporteDisk(string path, string ruta){
    MBR *dataMBR = (MBR*)malloc(sizeof(MBR));
    FILE *disk = fopen(path.c_str(),"rb+");
    bool alerta = false;
    if(disk != NULL){
        fseek(disk,0,SEEK_SET);
        fread(dataMBR, sizeof(MBR), 1, disk);
        fclose(disk);
    }else{
        cout<<"Error al abrir disco"<<endl;
        alerta = true;
    }

    if(alerta != true){
        FILE *report;
        ruta += "report_disk.dot";
        report = fopen(ruta.c_str(),"w+");
        if(report != NULL){
            fseek(report,0,SEEK_SET);
            fputs("digraph G {\n", report);
            fputs("parent [\n", report);
            fputs("shape=plaintext\n", report);
            fputs("label=<\n", report);
            fputs("<table border='1' cellborder='1'>\n", report);
            fputs("<tr>\n", report);
            
            //MBR
            fputs("<td rowspan=\"2\" bgcolor =\"#dd8703\" >MBR</td>\n", report);
            int inicio = sizeof(MBR),fin = sizeof(MBR)-2;
            Partition dataPart;
            for(int i = 0; i<4;i++){
                int partition = 0;
                if(dataMBR->mbr_partition_1.part_status[0] != '-'){
                    partition = 1;
                    dataPart = dataMBR->mbr_partition_1;
                }else if(dataMBR->mbr_partition_2.part_status[0] != '-'){
                    partition = 2;
                    dataPart = dataMBR->mbr_partition_2;
                }else if(dataMBR->mbr_partition_3.part_status[0] != '-'){
                    partition = 3;
                    dataPart = dataMBR->mbr_partition_3;
                }else if(dataMBR->mbr_partition_4.part_status[0] != '-'){
                    partition = 4;
                    dataPart = dataMBR->mbr_partition_4;
                }
                if(partition == i+1){
                    if(dataPart.part_type[0] == 'P'){
                        inicio = dataPart.part_start;
                        if(fin-inicio>0){
                            //Espacio libre
                            fputs("<td rowspan=\"2\" bgcolor = \"#3ac9da\">Libre<br/>", report);
                            int decimal = 0;
                            decimal = ((fin-inicio)*100)/dataMBR->mbr_tamano;
                            fprintf(report, "%.2f",(float)decimal);
                            fputs("%</td>\n", report);
                        }
                        fin = dataPart.part_start+dataPart.part_s;
                        //PARTICION PRIMARIA
                        fputs("<td rowspan=\"2\" bgcolor =\"#50b104\" >Primary <br/>",report);
                        int decimal = 0;
                        decimal = (dataPart.part_s*100)/dataMBR->mbr_tamano;
                        fprintf(report, "%.2f",(float)decimal);
                        fputs("%</td>\n", report);
                    }
                    if(dataPart.part_type[0] == 'E'){
                        inicio = dataPart.part_start;
                        if(fin-inicio>0){
                            //Espacio libre
                            fputs("<td rowspan=\"2\" bgcolor = \"#3ac9da\">Libre<br/>\n", report);
                            int decimal = 0;
                            decimal = ((fin-inicio)*100)/dataMBR->mbr_tamano;
                            fprintf(report, "%.2f",(float)decimal);
                            fputs("%</td>\n", report);
                        }
                        fin = dataPart.part_start+dataPart.part_s;
                         //PARTICION EXTENDIDA
                        fputs("<td>\n", report);
                        int pivEBR = sizeof(MBR)+1;
                        if(partition == 1){
                            pivEBR += 1;
                        }else if(partition == 2){
                            pivEBR += dataMBR->mbr_partition_1.part_s+1;
                        }else if(partition == 3){
                            pivEBR = dataMBR->mbr_partition_1.part_s+dataMBR->mbr_partition_2.part_s+1;
                        }else if(partition == 4){
                            pivEBR = dataMBR->mbr_partition_1.part_s+dataMBR->mbr_partition_2.part_s+dataMBR->mbr_partition_3.part_s+1;
                        }
                        EBR *primero = leerEBR(pivEBR,path);
                        fputs("<table border = \"1\" cellborder=\"1\" bgcolor=\"#da3a85\">\n", report);
                        fputs("<tr>\n", report);
                        bool bandera = true;
                        int inicioEBR = primero->part_start;
                        int finalEBR = 0;
                        while(bandera){
                            if(finalEBR-inicioEBR > 0){
                                //ESPACIO LIBRE
                                fputs("<td rowspan=\"2\" bgcolor = \"#3ac9da\">\n", report);
                                fprintf(report, "%.2f",((finalEBR-inicioEBR)*100)/dataMBR->mbr_tamano);
                                fputs("%</td>\n", report);
                            }
                            finalEBR = primero->part_start+primero->part_s;
                            if(primero->part_status[0] == '-'){
                                fputs("<td bgcolor=\"#eeeeee\">EBR</td>\n", report);
                            }else{
                                fputs("<td bgcolor=\"#eeeeee\">", report);
                                if(primero->part_status[0] != '-'){
                                    fputs("Libre1 <br/>",report);
                                }else{
                                    fputs("Lógica <br/>",report);
                                }
                                fprintf(report, "%.2f",(float)(primero->part_s*100)/dataMBR->mbr_tamano);
                                fputs("%</td>\n",report);
                            }
                            if(primero->part_next !=  -1){
                                primero = leerEBR(primero->part_next,path);
                            }else{
                                bandera = false;
                            }
                            if(dataMBR->mbr_tamano-inicioEBR>0){
                                //Espacio libre
                                fputs("<td rowspan=\"2\" bgcolor = \"#3ac9da\">Libre2<br/>\n", report);
                                fprintf(report, "%.2f",(float)((((dataPart.part_start+dataPart.part_s)-inicioEBR)*100)/dataMBR->mbr_tamano));
                                fputs("%</td>\n", report);
                            }
                            fputs("</tr>\n", report);
                            fputs("</table>\n", report);
                        }
                        fputs("</td>\n", report);
                    }
                }
            }
            if(dataMBR->mbr_tamano-dataPart.part_s > 0){
                 //ESPACIO LIBRE
                fputs("<td rowspan=\"2\" bgcolor = \"#3ac9da\">Libre<br/>\n", report);
                fprintf(report, "%.2f", (float)(((dataMBR->mbr_tamano-dataPart.part_s)*100)/dataMBR->mbr_tamano));
                fputs("%</td>\n", report);
            }
            fputs("</tr>\n", report);
            fputs("</table>\n", report);
            fputs(">];\n", report);

            fputs("}\n",report);
            //cerrando stream
            fclose (report);
            string command = "dot -Tpng report_disk.dot -o \""+ruta+"\"";
            system(command.c_str());
            cout<<"Reporte de disco creado...\n";
        }
    }

}
