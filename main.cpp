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
    int estado;
    int type=-1;
    string nombre;
};

//Discos montados
struct Discos{
    string path;
    int estado;
    particionMontada particiones[10];
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
    cout<<"Fit: "<<fit<<endl;
    
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
        cout<<"pivStart:"<<pivStart<<endl;
        if(dataMBR.mbr_partition_1.part_type[0] == 'E'){
            sizeExtend = dataMBR.mbr_partition_1.part_s;
            pivStart = dataMBR.mbr_partition_1.part_start;
            cout<<"uno"<<endl;
        }else if(dataMBR.mbr_partition_2.part_type[0] == 'E'){
            sizeExtend = dataMBR.mbr_partition_2.part_s;
            pivStart = dataMBR.mbr_partition_2.part_start;
            cout<<"dos"<<endl;
        }else if(dataMBR.mbr_partition_3.part_type[0] == 'E'){
            sizeExtend = dataMBR.mbr_partition_3.part_s;
            pivStart = dataMBR.mbr_partition_3.part_start;
            cout<<"tres"<<endl;
        }else if(dataMBR.mbr_partition_4.part_type[0] == 'E'){
            sizeExtend = dataMBR.mbr_partition_4.part_s;
            pivStart = dataMBR.mbr_partition_4.part_start;
            cout<<"cuatro"<<endl;
        }
        //Buscando EBR inicial
        EBR dataEBR;
        int sizeEBR = sizeof(EBR);
        int pivSize = 0, acumulado = 0;
        bool alerta = false;
        cout<<"pivStart:"<<pivStart<<endl;
        fseek(disk,pivStart+1,SEEK_SET);
        fread(&dataEBR,sizeof(EBR),1,disk);
        cout<<"part_next:"<<dataEBR.part_next<<"<-"<<endl;
        while(!feof(disk)){
            if(dataEBR.part_next == -1){
                //cout<<"hola"<<endl;
                start = dataEBR.part_start;
                pivSize = dataEBR.part_s;
                //Validando que la partición lógica quepa en la extendida
                if(ebr.part_s <= (sizeExtend-acumulado)){
                    start += sizeEBR+pivSize;
                    dataEBR.part_next = start;   
                }else{
                    alerta = true;
                    cout<<"No hay espacio suficiente para crear la partición lógica..."<<endl;
                }
                cout<<"ebr:"<<ebr.part_s<<endl;
                cout<<"ext:"<<sizeExtend<<endl;
                cout<<"str:"<<pivStart<<endl;
                break;
            }else{
                acumulado += dataEBR.part_s;
            }
        }
        //Escribe partición lógica
        if(alerta == false){
            //Terminando de llenar valores de EBR
            ebr.part_start = start;
            //Ubicandonos en la posición de nueva partición
            fseek(disk,start,SEEK_SET);
            fwrite(&ebr,sizeof(EBR),1,disk);
            cout<<"Partición lógica creada"<<endl;
            cout<<"Start:"<<start<<endl;
        }
        fclose(disk);
    }
}

//Comando Mount
Discos discos[5]; //Array para los discos montados
template <typename Map>
void commandMount(Map& parametros){
    //Variables para parametros
    string path, name, pivP;
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
    for(i = 0; i<5;i++){
        if(discos[i].estado==0||discos[i].path==path){
            break;
        }
    }
    if(i==5){
        cout<<"No hay espacio para montar otro disco"<<endl;
        alerta = true;
    }
    //Procedemos a montar la partición
    if(alerta != true){
        //ID para la partición
        id = id + to_string(partition) + name;
        cout<<"Montando la partición: "<<id<<endl;
        //Escribiendo struct
        discos[i].estado = 1;
        discos[i].path = path;
        int j =0;
        for(j = 0; j<10;j++){
            particionMontada par = discos[i].particiones[j];
            if(par.estado == 0){ //Para montar la nueva partición
                discos[i].particiones[j].estado = 1;
                discos[i].particiones[j].nombre = id;
                discos[i].particiones[j].numero = j+1;
                break;
            }

        }
        cout<<"Disco Montado..."<<endl;
        cout<<"ID:"<<discos[i].particiones[j].nombre<<endl;
        cout<<"Estado:"<<discos[i].particiones[j].estado<<endl;
        cout<<"Número:"<<discos[i].particiones[j].numero<<endl;
    }
}