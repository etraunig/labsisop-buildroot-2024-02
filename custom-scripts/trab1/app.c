#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/sysinfo.h> 
#include <sys/utsname.h>
#include <sys/types.h>

#define BUFLEN 4096
#define PORT 8080
#define MAX_PID  32768 

void getSystemKernelInfo(FILE *htmlFile) {
    char kernelVersion[BUFLEN];
    FILE *kernelFile = fopen("/proc/version", "r");

    if (kernelFile) {
        // Lê a versão do kernel
        if (fgets(kernelVersion, sizeof(kernelVersion), kernelFile)) {
            // Remove o caractere de nova linha do final da string
            kernelVersion[strcspn(kernelVersion, "\n")] = '\0';
            fprintf(htmlFile, "<p><strong>Kernel version:</strong> %s </p>\n", kernelVersion);

        }
        fclose(kernelFile);
    } else {
        // Imprime uma mensagem de erro se não for possível abrir o arquivo
        fprintf(htmlFile, "<p><strong>Kernel version: Not avaliable</strong></p>\n");
    }
}

void getUptime(FILE *htmlFile) {
    struct sysinfo sys_info;
    sysinfo(&sys_info);

    int uptime_seconds = sys_info.uptime;

    // Calcula dias, horas, minutos e segundos
    unsigned int days = uptime_seconds / (24 * 3600);
    unsigned int hours = (uptime_seconds % (24 * 3600)) / 3600;
    unsigned int minutes = (uptime_seconds % 3600) / 60;
    unsigned int seconds = uptime_seconds % 60;

    fprintf(htmlFile, "<p><strong>Uptime:</strong> %lu days, %lu hours, %lu minutes, %lu seconds</p>\n", days, hours, minutes, seconds);
}


void getCurrentDateTime(FILE *htmlFile) {
    time_t segundos;
    struct tm *data_hora_atual;

    time(&segundos);
    data_hora_atual = localtime(&segundos);

    fprintf(htmlFile, "<p><strong>Data:</strong> %d/%d/%d </p>\n", data_hora_atual->tm_mday, data_hora_atual->tm_mon + 1, data_hora_atual->tm_year + 1900);
    fprintf(htmlFile, "<p><strong>Hora:</strong> %d:%d:%d </p>\n", data_hora_atual->tm_hour, data_hora_atual->tm_min, data_hora_atual->tm_sec);
}

void getProcessorInfo(FILE *htmlFile) {
    FILE *cpuinfo = fopen("/proc/cpuinfo", "r");
    if (cpuinfo == NULL) {
        perror("Error opening /proc/cpuinfo");
        return;
    }

    char line[BUFLEN];
    char processorModel[BUFLEN] = "";
    unsigned long int processorSpeed = 0;
    unsigned numCores = 0;

    while (fgets(line, sizeof(line), cpuinfo)) {
        if (strstr(line, "model name")) {
            sscanf(line, "model name : %s", &processorModel);
        } else if (strstr(line, "cpu MHz")) {
            sscanf(line, "cpu MHz : %lu", &processorSpeed);
        } else if (strstr(line, "cpu cores")) {
            sscanf(line, "cpu cores : %d", &numCores);
        }
    }

    fclose(cpuinfo);

    fprintf(htmlFile, "<p><strong>Processor Model:</strong> %s </p>\n", processorModel);
    fprintf(htmlFile, "<p><strong>Processor Speed:</strong> %lu </p>\n", processorSpeed);
    fprintf(htmlFile, "<p><strong>Number of Cores:</strong> %d </p>\n", numCores);


}

void getSystemLoad(FILE *htmlFile) {
    FILE *loadavgFile = fopen("/proc/loadavg", "r");
    if (loadavgFile == NULL) {
        perror("Error opening /proc/loadavg");
        return;
    }

    float load1, load5, load15;
    unsigned int procsRunning, procsBlocked;
    unsigned int lastPid;

    // Lê as informações de carga do sistema
    fscanf(loadavgFile, "%f %f %f %d/%d %d", &load1, &load5, &load15, &procsRunning, &procsBlocked, &lastPid);
    fclose(loadavgFile);

    fprintf(htmlFile, "<p><strong>System Load:</strong></p>\n");
    fprintf(htmlFile, "<p><strong>1 min:</strong> %.2f </p>\n", load1);
    fprintf(htmlFile, "<p><strong>5 min:</strong> %.2f </p>\n", load5);
    fprintf(htmlFile, "<p><strong>15 min:</strong> %.2f </p>\n", load15);
    fprintf(htmlFile, "<p><strong>Processes Running:</strong> %d </p>\n", procsRunning);
    fprintf(htmlFile, "<p><strong>Processes Blocked:</strong> %d </p>\n", procsBlocked);
    fprintf(htmlFile, "<p><strong>Last PID:</strong> %d </p>\n", lastPid);


}

void getCpuOccupancy(FILE *htmlFile) {
    FILE *cpuStat = fopen("/proc/stat", "r");
    if (cpuStat == NULL) {
        perror("Error opening /proc/stat");
        exit(1);
    }

    char line[BUFLEN];
    fgets(line, sizeof(line), cpuStat);  // Skip the first line
    fgets(line, sizeof(line), cpuStat);  // Read the second line for CPU stats

    unsigned long long int user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
           &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);

    fclose(cpuStat);

    unsigned long  int totalTicks = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
    unsigned long  int totalIdle = idle + iowait;
    unsigned long  int totalNonIdle = totalTicks - totalIdle;

    float cpuOccupancy = (float)(totalNonIdle) / totalTicks * 100;

    // Converte o tempo ocioso de jiffies para segundos
    unsigned long int idleTimeSeconds = totalIdle / sysconf(_SC_CLK_TCK);

    // Converte os segundos para dias, horas e minutos
    unsigned int days = idleTimeSeconds / (60 * 60 * 24);
    idleTimeSeconds %= (60 * 60 * 24);
    unsigned int hours = idleTimeSeconds / (60 * 60);
    idleTimeSeconds %= (60 * 60);
    unsigned int minutes = idleTimeSeconds / 60;
    unsigned int seconds = idleTimeSeconds % 60;

    fprintf(htmlFile, "<p><strong>CPU occupancy:</strong> %.2f </p>\n", cpuOccupancy);
    fprintf(htmlFile, "<p><strong>Idle time:</strong> %u days, %u hours, %u minutes, %u seconds </p>\n", days, hours, minutes, seconds);  
}

void getMemoryInfo(FILE *htmlFile) {
    FILE *meminfo = fopen("/proc/meminfo", "r");
    if (meminfo) {
        char line[BUFLEN];
        while (fgets(line, sizeof(line), meminfo)) {
            if (strstr(line, "MemTotal")) {
                unsigned long memTotal = 0;
                sscanf(line, "MemTotal: %llu kB", &memTotal);
                float memTotalMb = (float)memTotal / 1024;

                fprintf(htmlFile, "<p><strong>Total Memory:</strong> %.2f </p>\n", memTotalMb);
            } else if (strstr(line, "MemFree")) {
                unsigned long int memFree = 0;
                sscanf(line, "MemFree: %llu kB", &memFree);
                float memFreeMb = (float)memFree / 1024;
                fprintf(htmlFile, "<p><strong>Free Memory:</strong> %.2f </p>\n", memFreeMb);
                break;
            }
        }
        fclose(meminfo);
    } else {
        fprintf(htmlFile, "<p><strong>Error: Unable to open /proc/meminfo</strong> </p>\n");
    }
}

void getIOOperationsInfo(FILE *htmlFile) {
    FILE *diskStatsFile = fopen("/proc/diskstats", "r");
    if (diskStatsFile == NULL) {
        fprintf(htmlFile, "<p><strong>Error: Unable to open /proc/diskstats</strong> </p>\n");
        return;
    }
        
    

    char line[BUFLEN];
    unsigned long totalReads = 0, totalWrites = 0, totalSectorsRead = 0, totalSectorsWritten = 0;

    while (fgets(line, sizeof(line), diskStatsFile)) {
        unsigned long readsCompleted, readsMerged, sectorsRead, readTime;
        unsigned long writesCompleted, writesMerged, sectorsWritten, writeTime;
        
        // Capture informações de todos os discos
        if (sscanf(line, "%*d %*d %*s %lu %lu %lu %lu %*lu %lu %lu %lu %lu",
                   &readsCompleted, &readsMerged, &sectorsRead, &readTime,
                   &writesCompleted, &writesMerged, &sectorsWritten, &writeTime) == 8) {
            totalReads += readsCompleted;
            totalWrites += writesCompleted;
            totalSectorsRead += sectorsRead;
            totalSectorsWritten += sectorsWritten;
        }
    }

    fclose(diskStatsFile);

    // Cálculo da velocidade em MB/s (considerando um intervalo de tempo)
    float readSpeed = (float)totalSectorsRead / 2048;  // Convert sectors to megabytes (assuming sector size is 512 bytes)
    float writeSpeed = (float)totalSectorsWritten / 2048;

    fprintf(htmlFile, "<p><strong>Disk I/O Info:</strong></p>\n");
    fprintf(htmlFile, "<p><strong>Read Speed:</strong> %.2f </p>\n", readSpeed);
    fprintf(htmlFile, "<p><strong>Write Speed:</strong> %.2f </p>\n", writeSpeed);

}

void getFileSystemInfo(FILE *htmlFile) {
    FILE *fsFile = fopen("/proc/filesystems", "r");
    if (fsFile) {
        char line[BUFLEN];
        fprintf(htmlFile, "<p><strong>Filesystems:</strong></p>\n");
        while (fgets(line, sizeof(line), fsFile)) {
            fprintf(htmlFile, "<p><strong></strong> %s </p>\n", line);
        }
        fclose(fsFile);
    } else {
        fprintf(htmlFile, "<p><strong>Failed to open /proc/filesystems</strong> %.2f </p>\n");
    }
}

int is_pid_dir(const struct dirent *entry) {
    const char *p;

    for (p = entry->d_name; *p; p++) {
        if (!isdigit(*p))
            return 0;
    }

    return 1;
}

void processList(FILE *htmlFile){
    DIR *procdir;
    FILE *fp;
    struct dirent *entry;
    char path[256 + 5 + 5]; 
    int pid;
    int maj_faults;

    procdir = opendir("/proc");
    if (!procdir) {
        perror("opendir failed");
        return;
    }
    fprintf(htmlFile, "<p><strong>PID | Process Name</strong> %s </p>");
    
    while ((entry = readdir(procdir))) {
        if (!is_pid_dir(entry))
            continue;

        
        snprintf(path, sizeof(path), "/proc/%s/stat", entry->d_name);
        fp = fopen(path, "r");

        if (!fp) {
            perror(path);
            continue;
        }

        fscanf(fp, "%d %s %*c %*d %*d %*d %*d %*d ",
            &pid, &path
        );

        fprintf(htmlFile, "<p><strong></strong> %5d %-20s </p>", pid, path);
        fclose(fp);
    }

    closedir(procdir);
    return;
}
    
void getDevicesAndGroups(FILE *htmlFile) {
    char buffer[BUFLEN];
    int isCharacterDevices = 0, isBlockDevices = 0;

    // Listando dispositivos de caractere
    fprintf(htmlFile, "<p><strong>Character devices:</strong> </p>\n");
    FILE *devices_file = fopen("/proc/devices", "r");
    if (devices_file == NULL) {
        perror("Erro ao abrir /proc/devices");
        return;
    }

    // Lendo o arquivo e identificando os dispositivos de caractere
    while (fgets(buffer, sizeof(buffer), devices_file) != NULL) {
        if (strstr(buffer, "Character devices:")) {
            isCharacterDevices = 1;
            continue;
        }
        if (strstr(buffer, "Block devices:")) {
            isCharacterDevices = 0;
            isBlockDevices = 1;
            fprintf(htmlFile, "<p><strong>Block devices:</strong> </p>\n");
            continue;
        }

        if (isCharacterDevices && buffer[0] != '\n') {
            fprintf(htmlFile, "<p><strong></strong> %s </p>", buffer);
        }
        
        if (isBlockDevices && buffer[0] != '\n') {
            fprintf(htmlFile, "<p><strong></strong> %s </p>", buffer);
        }
    }
    fclose(devices_file);

}

void getNetworkDevices(FILE *htmlFile) {
    char buffer[BUFLEN];
    char formattedNetworkInfo[BUFLEN] = ""; 
   
    struct dirent *de;  
    DIR *dr = opendir("/proc/net/dev_snmp6");
    if (dr == NULL) {
        perror("Erro ao abrir /proc/net/dev_snmp6");
        return;
    }

    fprintf(htmlFile, "<p><strong>Network Devices:</strong>  </p>");
    while ((de = readdir(dr)) != NULL) {
        if (de->d_name[0] == '.' && (de->d_name[1] == '\0' || (de->d_name[1] == '.' && de->d_name[2] == '\0')))
            continue; 
        fprintf(htmlFile, "<p> %s </p>", de->d_name);
    }
    closedir(dr);  
}



int main() {
    // Abre o arquivo HTML para escrita
    FILE *htmlFile = fopen("index.html", "w");
    if (htmlFile == NULL) {
        perror("Erro ao abrir index.html");
        return 1;
    }

   // Escrever o cabeçalho HTML com o estilo CSS para criar quadradinhos
    fputs("<!DOCTYPE html>\n<html>\n<head>\n<title>System Info</title>\n", htmlFile);
    fputs("<style>\n", htmlFile);
    fputs("body { font-family: Arial, sans-serif; }\n", htmlFile);
    fputs(".square { \n", htmlFile);
    fputs("    border: 2px solid #000;\n", htmlFile);
    fputs("    border-radius: 5px;\n", htmlFile);
    fputs("    padding: 10px;\n", htmlFile);
    fputs("    margin: 10px 0;\n", htmlFile);
    fputs("    background-color: #f0f0f0;\n", htmlFile);
    fputs("}\n", htmlFile);
    fputs("p { margin: 0; }\n", htmlFile);
    fputs("strong { color: #333; }\n", htmlFile);
    fputs("</style>\n</head>\n<body>\n", htmlFile);
    fputs("<h1>System Information</h1>\n", htmlFile);

    // Coleta as informações do sistema e escreve no HTML com a classe "square"
    fputs("<div class=\"square\">\n", htmlFile);
    getSystemKernelInfo(htmlFile);
    fputs("</div>\n", htmlFile);
// 
    fputs("<div class=\"square\">\n", htmlFile);
    getUptime(htmlFile);
    fputs("</div>\n", htmlFile);
//
    fputs("<div class=\"square\">\n", htmlFile);
    getCurrentDateTime(htmlFile);
    fputs("</div>\n", htmlFile);
// 

    fputs("<div class=\"square\">\n", htmlFile);
    getProcessorInfo(htmlFile);
    fputs("</div>\n", htmlFile);
// 
    fputs("<div class=\"square\">\n", htmlFile);
    getSystemLoad(htmlFile);
    fputs("</div>\n", htmlFile);
// 
    fputs("<div class=\"square\">\n", htmlFile);
    getCpuOccupancy(htmlFile);
    fputs("</div>\n", htmlFile);
// 
    fputs("<div class=\"square\">\n", htmlFile);
    // getMemoryInfo(htmlFile);
    fputs("</div>\n", htmlFile);
// 

    fputs("<div class=\"square\">\n", htmlFile);
    getIOOperationsInfo(htmlFile);
    fputs("</div>\n", htmlFile);
// 
    fputs("<div class=\"square\">\n", htmlFile);
    getFileSystemInfo(htmlFile);
    fputs("</div>\n", htmlFile);
// 

    fputs("<div class=\"square\">\n", htmlFile);
    //processList(htmlFile);
    fputs("</div>\n", htmlFile);

// 

    fputs("<div class=\"square\">\n", htmlFile);
    getDevicesAndGroups(htmlFile);
    fputs("</div>\n", htmlFile);
// 

    fputs("<div class=\"square\">\n", htmlFile);
    getNetworkDevices(htmlFile);
    fputs("</div>\n", htmlFile);

    // Finaliza o HTML
    fputs("</body>\n</html>", htmlFile);

    // Fecha o arquivo HTML
    fclose(htmlFile);

    printf("Arquivo HTML gerado com sucesso.\n");

    return 0;
}
